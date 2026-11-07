package de.mbc89.android.core

import de.mbc89.android.transport.CanPacket
import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertNull
import org.junit.Assert.assertTrue
import org.junit.Test

class Mbc89CoreTest {
    @Test
    fun acknowledgesConnect6021StreamEndAndClearsStreamingState() {
        val packet = connectPacket(0x44, byteArrayOf(1, 2, 3, 4, 0xff.toByte(), 0xff.toByte()))
        val core = Mbc89Core()
        val response = core.process(packet)

        assertEquals(packet.withCanId(0x45), response)
        assertFalse(core.connect6021Streaming)
    }

    @Test
    fun tracksConnect6021StreamStartAndEnd() {
        val core = Mbc89Core()
        val start = connectPacket(0x44, byteArrayOf(1, 2, 3, 4, 0, 0))
        val end = connectPacket(0x44, byteArrayOf(1, 2, 3, 4, 0xff.toByte(), 0xff.toByte()))

        assertEquals(start.withCanId(0x45), core.process(start))
        assertTrue(core.connect6021Streaming)
        assertEquals(end.withCanId(0x45), core.process(end))
        assertFalse(core.connect6021Streaming)
    }

    @Test
    fun ignoresConnect6021BlockOfAnotherModuleWithItsAssignments() {
        val core = Mbc89Core(listOf(0x6d, '8'.code, '9'.code, 0x00))
        val other = byteArrayOf(0x6d, '8'.code.toByte(), '9'.code.toByte(), 0x01)
        val own = byteArrayOf(0x6d, '8'.code.toByte(), '9'.code.toByte(), 0x00)

        assertNull(core.process(connectPacket(0x44, other + byteArrayOf(0, 0))))
        assertNull(core.process(connectPacket(0x44, byteArrayOf(0, 0, 0, 0x18, 1))))
        assertNull(core.process(connectPacket(0x44, other + byteArrayOf(0xff.toByte(), 0xff.toByte()))))
        assertFalse(core.connect6021Streaming)
        assertFalse(core.locomotiveMapping(1).assigned)

        val start = connectPacket(0x44, own + byteArrayOf(0, 0))
        assertEquals(start.withCanId(0x45), core.process(start))
        val assignment = connectPacket(0x44, byteArrayOf(0, 0, 0x40, 0x05, 1))
        assertEquals(assignment.withCanId(0x45), core.process(assignment))
        core.process(connectPacket(0x44, own + byteArrayOf(0xff.toByte(), 0xff.toByte())))

        core.process(connectPacket(0x44, other + byteArrayOf(0, 0)))
        core.process(connectPacket(0x44, byteArrayOf(0, 0, 0, 0x18, 1)))
        core.process(connectPacket(0x44, other + byteArrayOf(0xff.toByte(), 0xff.toByte())))
        assertEquals(listOf(0, 0, 0x40, 0x05), core.locomotiveMapping(1).guid)
    }

    @Test
    fun mapsReservedSlot80ToSlotZero() {
        val core = Mbc89Core()
        val packet = connectPacket(0x44, byteArrayOf(0x11, 0x22, 0x33, 0x44, 80))

        assertEquals(packet.withCanId(0x45), core.process(packet))
        assertEquals(listOf(0x11, 0x22, 0x33, 0x44), core.locomotiveMapping(0).guid)
        assertTrue(core.locomotiveMapping(0).assigned)
    }

    @Test
    fun streamStartResetsAddressesButPreservesAssignmentFlags() {
        val core = Mbc89Core()
        core.process(connectPacket(0x44, byteArrayOf(1, 2, 3, 4, 1)))
        val start = connectPacket(0x44, byteArrayOf(0, 0, 0, 0, 0, 0))

        assertEquals(start.withCanId(0x45), core.process(start))
        assertEquals(listOf(0, 0, 0, 1), core.locomotiveMapping(1).guid)
        assertTrue(core.locomotiveMapping(1).assigned)
        assertEquals(listOf(0, 0, 0, 80), core.locomotiveMapping(0).guid)
    }

    @Test
    fun ignoresOutOfRangeSlotAndUnrelatedCanIds() {
        val core = Mbc89Core()

        assertNull(core.process(connectPacket(0x44, byteArrayOf(1, 2, 3, 4, 81))))
        assertNull(core.process(connectPacket(0x44, byteArrayOf(1, 2, 3, 4, 0xff.toByte()))))
        assertNull(core.process(connectPacket(0x16, byteArrayOf(1, 2, 3, 4, 5))))
        assertFalse(core.connect6021Streaming)
    }

    @Test
    fun completesPcRegistrationAndAcknowledgesAliveForAssignedGuid() {
        val core = Mbc89Core()
        core.process(pcPacket(0x00, byteArrayOf(0, 0, 0, 0)))
        core.process(pcPacket(0x01, byteArrayOf(0, 0, 0, 0)))
        core.process(pcPacket(0x02, byteArrayOf(0, 0, 0, 0)))
        core.process(pcPacket(0x03, byteArrayOf(0, 0x22, 0, 0)))

        var response = core.process(pcPacket(0x04, byteArrayOf(0, 0, 0, 0)))
        assertEquals(0x06, response?.data?.get(3)?.toInt()?.and(0xff))
        assertEquals(1, response?.data?.get(6)?.toInt()?.and(0xff))

        for (step in 1..42) {
            val current = response ?: throw AssertionError("Expected registration step $step.")
            val responseData = current.data
            assertEquals(step, responseData[6].toInt() and 0xff)
            val acknowledgement = byteArrayOf(0, 0x55, step.toByte(), responseData[7])
            response = core.process(pcPacket(0x05, acknowledgement))
            if (step < 42) {
                assertEquals(step + 1, response?.data?.get(6)?.toInt()?.and(0xff))
            }
        }

        assertTrue(core.isRegistered)
        assertEquals(listOf(0x22, '8'.code, '9'.code, 0x55), core.registeredGuid)
        val aliveResponse = core.process(
            pcPacket(
                0x09,
                byteArrayOf(0x22, '8'.code.toByte(), '9'.code.toByte(), 0x55),
            ),
        )
        assertEquals(0x0a, aliveResponse?.data?.get(3)?.toInt()?.and(0xff))
        assertEquals(core.registeredGuid, aliveResponse?.data?.slice(4..7)?.map { it.toInt() and 0xff })
    }

    @Test
    fun rejectsRegistrationAcknowledgementWithWrongStepOrValue() {
        val core = Mbc89Core()
        core.process(pcPacket(0x00, byteArrayOf(0, 0, 0, 0)))
        core.process(pcPacket(0x01, byteArrayOf(0, 0, 0, 0)))
        core.process(pcPacket(0x02, byteArrayOf(0, 0, 0, 0)))
        core.process(pcPacket(0x04, byteArrayOf(0, 0, 0, 0)))

        val invalidAcknowledgement = pcPacket(
            0x05,
            byteArrayOf(0, 0, 2, 0),
        )
        assertNull(core.process(invalidAcknowledgement))
        assertFalse(core.isRegistered)
    }

    @Test
    fun doesNotStartPcRegistrationBeforeDatabaseSyncCompletes() {
        val core = Mbc89Core()

        assertNull(core.process(pcPacket(0x04, byteArrayOf(0, 0, 0, 0))))
        assertFalse(core.isRegistered)
    }

    @Test
    fun acceptsParaCenterRegistrationMessagesAcrossTheFull18xxAddressRange() {
        val core = Mbc89Core()
        core.process(pcPacketAtAddress(0x00, byteArrayOf(0, 0, 0, 0), 0x12, 0x34))
        core.process(pcPacketAtAddress(0x01, byteArrayOf(0, 0, 0, 0), 0x56, 0x78))
        core.process(pcPacketAtAddress(0x02, byteArrayOf(0, 0, 0, 0), 0x9a, 0xbc))
        assertTrue(core.isParaCenterDatabaseValid)
        val registration = core.process(
            pcPacketAtAddress(0x04, byteArrayOf(0, 0, 0, 0), 0xde, 0xf0),
        )

        assertEquals(
            CanPacket.create(
                priority = 0,
                canId = 0x16,
                hashHigh = 0x5f,
                hashLow = 0x38,
                data = byteArrayOf(0, 0, 0x18, 0x06, 0, 0, 1, 0),
            ),
            registration,
        )
        assertEquals(0x06, registration?.data?.get(3)?.toInt()?.and(0xff))
        assertEquals(1, registration?.data?.get(6)?.toInt()?.and(0xff))
        assertEquals(1, core.paraCenterRegistrationStep)
        assertFalse(core.isRegistered)
    }

    @Test
    fun exposesConfiguredAndroidArticleVersionsAndSyntheticSerialInInfoBlock() {
        val serial = byteArrayOf(0x12, 0x34, 0x56, 0x78, 0x9a.toByte(), 0xbc.toByte(), 0xde.toByte(), 0xf0.toByte())
        val core = Mbc89Core(syntheticSerial = serial)
        val info = core.systemArraySnapshot

        assertEquals(serial.toList(), info.sliceArray(1..8).toList())
        assertEquals("8980", info.sliceArray(9..12).decodeToString())
        assertEquals("100", info.sliceArray(13..15).decodeToString())
        assertEquals("261103", info.sliceArray(16..21).decodeToString())
        val name = info.sliceArray(22..41)
        assertEquals("0x123456789ABCDEF0", name.sliceArray(0..17).decodeToString())
        assertEquals(listOf(0x81, 0x81), name.sliceArray(18..19).map { it.toInt() and 0xff })
    }

    @Test
    fun initializesSystemArrayWithConfiguredVoltageAndTemperature() {
        val persistedArray = ByteArray(Mbc89SystemArrayLayout.SIZE)
        val core = Mbc89Core(initialSystemArray = persistedArray)
        val systemArray = core.systemArraySnapshot

        assertEquals(
            "187",
            systemArray.sliceArray(
                Mbc89SystemArrayLayout.SUPPLY_VOLTAGE_OFFSET until
                    Mbc89SystemArrayLayout.SUPPLY_VOLTAGE_OFFSET + 3,
            ).decodeToString(),
        )
        assertEquals(
            "221",
            systemArray.sliceArray(
                Mbc89SystemArrayLayout.TEMPERATURE_OFFSET until
                    Mbc89SystemArrayLayout.TEMPERATURE_OFFSET + 3,
            ).decodeToString(),
        )
    }

    @Test
    fun answersCs2PingWithRegisteredGuidAndDerivedHash() {
        val core = registeredCore()
        val ping = CanPacket.create(
            priority = 0,
            canId = 0x30,
            hashHigh = 0,
            hashLow = 0,
            data = ByteArray(8),
        )

        val response = core.process(ping)

        assertEquals(
            CanPacket.create(
                priority = 0,
                canId = 0x31,
                hashHigh = 0x1b,
                hashLow = 0x6d,
                data = byteArrayOf(0x22, '8'.code.toByte(), '9'.code.toByte(), 0x55, 1, 0, 0, 0x20),
            ),
            response,
        )
        assertTrue(core.cs2Connected)
    }

    @Test
    fun detectsCs2PingBeforeRegistrationAndAnswersEmptyPingAfterRegistration() {
        val unregistered = Mbc89Core()
        val ping = CanPacket.create(0, 0x30, 0, 0, ByteArray(8))
        val emptyPing = CanPacket.create(0, 0x30, 0, 0, ByteArray(0))

        assertNull(unregistered.process(ping))
        assertTrue(unregistered.cs2Connected)
        val registered = registeredCore()
        assertEquals(0x31, registered.process(emptyPing)?.canId)
        assertTrue(registered.cs2Connected)
    }

    @Test
    fun servesCs2ConfigurationChannelAsFivePacketsAndAcknowledgement() {
        val core = registeredCore()
        val guid = byteArrayOf(0x22, '8'.code.toByte(), '9'.code.toByte(), 0x55)
        val request = cs2ConfigurationPacket(guid, channel = 12)

        val result = core.processDetailed(request)

        assertTrue(core.cs2Connected)
        assertEquals(6, result.responses.size)
        val row = result.responses.take(5).flatMap { it.data.asIterable() }
        assertEquals(40, row.size)
        assertEquals(listOf(12, 1, 2, 0), row.take(4).map { it.toInt() and 0xff })
        assertEquals(
            "KeyB1 Group1:",
            row.drop(8).take(13).map { it.toInt().toChar() }.joinToString(""),
        )
        result.responses.take(5).forEachIndexed { index, response ->
            assertEquals(0x3b, response.canId)
            assertEquals(0x03, response.hashHigh)
            assertEquals(index + 1, response.hashLow)
            assertEquals(8, response.dataLength)
        }
        assertEquals(
            CanPacket.create(
                priority = 0,
                canId = 0x3b,
                hashHigh = 0x1b,
                hashLow = 0x6d,
                data = guid + byteArrayOf(12, 5),
            ),
            result.responses.last(),
        )
    }

    @Test
    fun servesDeviceInfoChannelAndLastKeyboardConfigurationChannel() {
        val core = registeredCore()
        val guid = byteArrayOf(0x22, '8'.code.toByte(), '9'.code.toByte(), 0x55)
        val deviceInfo = core.processDetailed(cs2ConfigurationPacket(guid, channel = 0))
        val infoRow = deviceInfo.responses.take(5).flatMap { it.data.asIterable() }
        assertEquals(2, infoRow[0].toInt() and 0xff)
        assertEquals(73, infoRow[1].toInt() and 0xff)
        assertEquals(6, deviceInfo.responses.size)

        val lastChannel = core.processDetailed(cs2ConfigurationPacket(guid, channel = 75))
        val lastRow = lastChannel.responses.take(5).flatMap { it.data.asIterable() }
        assertEquals(listOf(75, 1, 2, 0), lastRow.take(4).map { it.toInt() and 0xff })
        assertEquals(
            "KeyB16 Group4:",
            lastRow.drop(8).take(14).map { it.toInt().toChar() }.joinToString(""),
        )
    }

    @Test
    fun servesEveryAdvertisedConfigurationChannelWithReferenceDefaults() {
        val core = registeredCore()
        val guid = byteArrayOf(0x22, '8'.code.toByte(), '9'.code.toByte(), 0x55)

        for (channel in 0..75) {
            val result = core.processDetailed(cs2ConfigurationPacket(guid, channel))
            assertEquals("channel $channel", 6, result.responses.size)
        }

        val softwareMajor = cs2ConfigurationRow(core, guid, channel = 7)
        val softwareMinor = cs2ConfigurationRow(core, guid, channel = 8)
        val mapping = cs2ConfigurationRow(core, guid, channel = 9)
        val keyboardBase = cs2ConfigurationRow(core, guid, channel = 10)
        val i2cFrequency = cs2ConfigurationRow(core, guid, channel = 11)
        assertEquals(1, softwareMajor[7].toInt() and 0xff)
        assertEquals(0, softwareMinor[7].toInt() and 0xff)
        assertEquals(1, softwareMajor[3].toInt())
        assertEquals("1", softwareMajor.sliceArray(18..18).decodeToString())
        assertEquals(0, softwareMinor[3].toInt())
        assertEquals("0", softwareMinor.sliceArray(18..18).decodeToString())
        assertEquals("255", softwareMinor.sliceArray(20..22).decodeToString())
        assertEquals(0, softwareMajor[6].toInt() and 0xff)
        assertEquals(listOf(9, 1, 2, 0), mapping.take(4).map { it.toInt() and 0xff })
        assertEquals("Mapping", mapping.sliceArray(17..23).toString(Charsets.US_ASCII).trimEnd('\u0000'))
        assertEquals(0, mapping[24].toInt() and 0xff)
        assertEquals(
            "Mapping&Direkt",
            mapping.sliceArray(25..38).toString(Charsets.US_ASCII),
        )
        assertEquals('#'.code, keyboardBase[19].toInt() and 0xff)
        assertEquals(1, keyboardBase[7].toInt() and 0xff)
        assertEquals(40, i2cFrequency[7].toInt() and 0xff)
    }

    @Test
    fun servesFirmwareAndHardwareConfigurationRowsAsBaseChannels() {
        val core = registeredCore()
        val guid = byteArrayOf(0x22, '8'.code.toByte(), '9'.code.toByte(), 0x55)

        val firmware = cs2ConfigurationRow(core, guid, channel = 5)
        assertEquals(listOf(5, 1, 1), firmware.take(3).map { it.toInt() and 0xff })
        assertEquals("FW:", firmware.sliceArray(8..10).toString(Charsets.US_ASCII))
        assertEquals("1.00", firmware.sliceArray(13..16).toString(Charsets.US_ASCII))

        val hardware = cs2ConfigurationRow(core, guid, channel = 6)
        assertEquals(listOf(6, 1, 1), hardware.take(3).map { it.toInt() and 0xff })
        assertEquals("HW:", hardware.sliceArray(8..10).toString(Charsets.US_ASCII))
        assertEquals("26.11.03", hardware.sliceArray(13..20).toString(Charsets.US_ASCII))
    }

    @Test
    fun answersMeasurementRequestsForVoltageAndTemperature() {
        val core = registeredCore()
        val guid = byteArrayOf(0x22, '8'.code.toByte(), '9'.code.toByte(), 0x55)

        assertEquals(
            CanPacket.create(
                priority = 0,
                canId = 0x01,
                hashHigh = 0x1b,
                hashLow = 0x6d,
                data = guid + byteArrayOf(0x0b, 1, 0x01, 0xb3.toByte()),
            ),
            core.process(measurementPacket(guid, channel = 1)),
        )
        assertEquals(
            CanPacket.create(
                priority = 0,
                canId = 0x01,
                hashHigh = 0x1b,
                hashLow = 0x6d,
                data = guid + byteArrayOf(0x0b, 2, 0x00, 0x37),
            ),
            core.process(measurementPacket(guid, channel = 2)),
        )
        assertTrue(core.cs2Connected)
    }

    @Test
    fun ignoresMeasurementRequestsForOtherChannelsOrForeignGuid() {
        val core = registeredCore()
        val guid = byteArrayOf(0x22, '8'.code.toByte(), '9'.code.toByte(), 0x55)

        assertNull(core.process(measurementPacket(guid, channel = 3)))
        assertNull(core.process(measurementPacket(byteArrayOf(0, 0, 0, 0), channel = 1)))
        assertNull(Mbc89Core().process(measurementPacket(guid, channel = 1)))
    }

    @Test
    fun answersUniversalStopAndGoWithOwnGuid() {
        val core = registeredCore()
        val guid = byteArrayOf(0x22, '8'.code.toByte(), '9'.code.toByte(), 0x55)

        assertEquals(
            CanPacket.create(0, 0x01, 0x1b, 0x6d, guid + byteArrayOf(0)),
            core.process(stopGoPacket(byteArrayOf(0, 0, 0, 0), go = false)),
        )
        assertTrue(core.systemStopped)
        assertEquals(
            CanPacket.create(0, 0x01, 0x1b, 0x6d, guid + byteArrayOf(1)),
            core.process(stopGoPacket(byteArrayOf(0, 0, 0, 0), go = true)),
        )
        assertFalse(core.systemStopped)
    }

    @Test
    fun matchesModuleStopWithD2ComparedAgainstGuidByte1LikeMbc89Com() {
        val core = registeredCore()
        val referenceAddress = byteArrayOf(0x22, '8'.code.toByte(), '8'.code.toByte(), 0x55)
        val fullGuid = byteArrayOf(0x22, '8'.code.toByte(), '9'.code.toByte(), 0x55)

        assertNull(core.process(stopGoPacket(fullGuid, go = false)))
        assertFalse(core.systemStopped)
        assertEquals(0x01, core.process(stopGoPacket(referenceAddress, go = false))?.canId)
        assertTrue(core.systemStopped)
    }

    @Test
    fun ignoresStopWithoutRegistrationForeignGuidOrUnknownState() {
        assertNull(Mbc89Core().process(stopGoPacket(byteArrayOf(0, 0, 0, 0), go = false)))

        val core = registeredCore()
        assertNull(core.process(stopGoPacket(byteArrayOf(1, 2, 3, 4), go = false)))
        val unknownState = CanPacket.create(0, 0x00, 0, 0, byteArrayOf(0, 0, 0, 0, 2))
        assertNull(core.process(unknownState))
        assertFalse(core.systemStopped)
    }

    @Test
    fun buildsOwnStopGoBroadcastAndKeepsStateUntilSent() {
        assertEquals(
            CanPacket.create(0, 0x00, 0x03, 0x00, byteArrayOf(0, 0, 0, 0, 0)),
            Mbc89Core().buildSystemStopGo(go = false),
        )

        val core = registeredCore()
        assertEquals(
            CanPacket.create(0, 0x00, 0x1b, 0x6d, byteArrayOf(0, 0, 0, 0, 1)),
            core.buildSystemStopGo(go = true),
        )
        core.buildSystemStopGo(go = false)
        assertFalse(core.systemStopped)
        core.markSystemStopGoSent(go = false)
        assertTrue(core.systemStopped)
    }

    @Test
    fun buildsKeyboardAccessoryCommandsFromBaseAddressAndColumn() {
        val core = Mbc89Core(keyboardCount = 2)

        assertEquals(
            CanPacket.create(0, 0x16, 0x03, 0x00, byteArrayOf(0, 0, 0x30, 0x00, 1, 1)),
            core.buildKeyboardSwitch(kbIndex = 0, column = 0, green = true),
        )
        assertEquals(
            CanPacket.create(0, 0x16, 0x03, 0x00, byteArrayOf(0, 0, 0x30, 0x1f, 0, 1)),
            core.buildKeyboardSwitch(kbIndex = 1, column = 15, green = false),
        )
    }

    @Test
    fun usesKeyboardBasisFromSystemArray() {
        val systemArray = ByteArray(Mbc89SystemArrayLayout.SIZE).apply {
            this[Mbc89SystemArrayLayout.KEYBOARD_BASE_OFFSET] = 2
        }
        val core = Mbc89Core(initialSystemArray = systemArray)

        assertEquals(
            listOf(0x31, 0x03),
            core.buildKeyboardSwitch(0, 3, true)!!.data.slice(2..3).map { it.toInt() and 0xff },
        )
    }

    @Test
    fun changesKeyboardStateOnlyAfterSendIsConfirmed() {
        val core = Mbc89Core()

        core.buildKeyboardSwitch(0, 4, green = true)
        assertEquals(0, core.keyboardState(0))
        core.markKeyboardSwitchSent(0, 4, green = true)
        assertEquals(1 shl 4, core.keyboardState(0))
        core.markKeyboardSwitchSent(0, 4, green = false)
        assertEquals(0, core.keyboardState(0))
    }

    @Test
    fun blocksKeyboardCommandsWhileSystemIsStopped() {
        val core = registeredCore()
        core.process(stopGoPacket(byteArrayOf(0, 0, 0, 0), go = false))

        assertNull(core.buildKeyboardSwitch(0, 0, green = true))
        core.process(stopGoPacket(byteArrayOf(0, 0, 0, 0), go = true))
        assertEquals(0x16, core.buildKeyboardSwitch(0, 0, green = true)?.canId)
    }

    @Test
    fun reportsSwitchedAccessoriesIncludingTheFormWithSwitchingTime() {
        val core = Mbc89Core(keyboardCount = 1)

        assertEquals(
            MemoryCommand(0x3005, true),
            core.processDetailed(accessoryPacket(0x3005, green = true)).switchedAccessory,
        )
        val timed = CanPacket.create(
            priority = 0,
            canId = 0x16,
            hashHigh = 0x03,
            hashLow = 0x00,
            data = byteArrayOf(0, 0, 0x30, 0x05, 0, 1, 0, 60),
        )
        assertEquals(MemoryCommand(0x3005, false), core.processDetailed(timed).switchedAccessory)
        val powerOff = CanPacket.create(
            priority = 0,
            canId = 0x16,
            hashHigh = 0x03,
            hashLow = 0x00,
            data = byteArrayOf(0, 0, 0x38, 0x05, 0, 0),
        )
        assertNull(core.processDetailed(powerOff).switchedAccessory)
        assertEquals(0, core.keyboardState(0))
        assertNull(core.processDetailed(pcPacket(0x0d, byteArrayOf(0, 0, 0, 0))).switchedAccessory)
    }

    @Test
    fun reportsS88EventsForTheMemoryAutomaticMode() {
        val core = Mbc89Core(keyboardCount = 1)
        val event = CanPacket.create(
            priority = 0,
            canId = 0x23,
            hashHigh = 0x03,
            hashLow = 0x00,
            data = byteArrayOf(0, 0x05, 0x07, 0xd1.toByte(), 0, 1, 0, 0),
        )

        val result = core.processDetailed(event)

        assertEquals(S88Contact(2001, true), result.s88Contact)
        assertTrue(result.responses.isEmpty())
        assertNull(core.processDetailed(accessoryPacket(0x3005, green = true)).s88Contact)
    }

    @Test
    fun keyboardGroupsSwitchMmOrDccAsSetInTheProtocolArray() {
        val array = ByteArray(Mbc89SystemArrayLayout.SIZE)
        array[Mbc89SystemArrayLayout.KEYBOARD_BASE_OFFSET] = 1
        array[Mbc89SystemArrayLayout.PROTOCOL_SETTINGS_OFFSET + 1 * 4 + 1] = 1
        val core = Mbc89Core(initialSystemArray = array, keyboardCount = 2)

        assertEquals(0x3000 + 16 + 3, core.keyboardAddress(1, 3))
        assertEquals(0x3800 + 16 + 4, core.keyboardAddress(1, 4))
        assertEquals(0x3800 + 16 + 4, core.buildKeyboardSwitch(1, 4, green = true)!!.let {
            ((it.data[2].toInt() and 0xff) shl 8) or (it.data[3].toInt() and 0xff)
        })

        core.process(accessoryPacket(0x3800 + 16 + 4, green = true))
        assertEquals(1 shl 4, core.keyboardState(1))
    }

    @Test
    fun followsAccessoryCommandsFromTheBusWithinOwnKeyboardRange() {
        val core = Mbc89Core(keyboardCount = 2)

        val result = core.processDetailed(accessoryPacket(0x3005, green = true))
        assertTrue(result.responses.isEmpty())
        assertEquals(1 shl 5, core.keyboardState(0))
        core.process(accessoryPacket(0x301f, green = true))
        assertEquals(1 shl 15, core.keyboardState(1))
        core.process(accessoryPacket(0x3005, green = false))
        assertEquals(0, core.keyboardState(0))

        core.process(accessoryPacket(0x3020, green = true))
        core.process(accessoryPacket(0x2fff, green = true))
        assertEquals(0, core.keyboardState(0))
        assertEquals(1 shl 15, core.keyboardState(1))
        assertEquals(0, core.keyboardState(2))
    }

    @Test
    fun detectsCs2ConfigurationWithoutRegistrationOrMatchingGuid() {
        val unregistered = Mbc89Core()
        assertTrue(
            unregistered.processDetailed(
                cs2ConfigurationPacket(byteArrayOf(0, 0, 0, 0), channel = 0),
            ).responses.isEmpty(),
        )
        assertTrue(unregistered.cs2Connected)

        val core = registeredCore()
        val guid = byteArrayOf(0x22, '8'.code.toByte(), '9'.code.toByte(), 0x55)
        assertTrue(
            core.processDetailed(cs2ConfigurationPacket(byteArrayOf(0, 0, 0, 0), channel = 0))
                .responses.isEmpty(),
        )
        assertTrue(core.cs2Connected)
        core.onTransportConnecting()
        assertTrue(core.processDetailed(cs2ConfigurationPacket(guid, channel = 76)).responses.isEmpty())
        assertFalse(core.cs2Connected)
    }

    @Test
    fun restoresPersistedGuidAsRegisteredIdentity() {
        val guid = listOf(0x22, '8'.code, '9'.code, 0x55)
        val core = Mbc89Core(guid)
        val ping = CanPacket.create(0, 0x30, 0, 0, ByteArray(8))

        assertTrue(core.isRegistered)
        assertEquals(guid, core.registeredGuid)
        assertEquals(0x31, core.process(ping)?.canId)
    }

    @Test
    fun acceptsPcArrayTransferAndReadsWritesBoundedSystemArray() {
        val core = registeredCore()
        val guid = byteArrayOf(0x22, '8'.code.toByte(), '9'.code.toByte(), 0x55)
        val arrayRequest = pcPacket(0x0b, guid)
        val arrayResponse = core.process(arrayRequest)
        assertEquals(0x0c, arrayResponse?.data?.get(3)?.toInt()?.and(0xff))
        assertEquals(guid.toList(), arrayResponse?.data?.slice(4..7))

        val writeData = byteArrayOf(1, 0, 100, 0x5a)
        val writeResult = core.processDetailed(pcPacket(0x0d, writeData))
        assertFalse(writeResult.persistSystemArray)
        assertEquals(0x5a, core.systemArraySnapshot[100].toInt() and 0xff)
        assertEquals(registeredPcResponse(0x0e, writeData), writeResult.response)

        val readData = byteArrayOf(0, 0, 100, 0)
        val readResult = core.processDetailed(pcPacket(0x0d, readData))
        assertEquals(0x5a, readResult.response?.data?.get(7)?.toInt()?.and(0xff))

        val lastByteWrite = byteArrayOf(1, 0x04, 0xb7.toByte(), 0xa5.toByte())
        core.process(pcPacket(0x0d, lastByteWrite))
        assertEquals(0xa5, core.systemArraySnapshot.last().toInt() and 0xff)

        val outOfRangeWrite = byteArrayOf(1, 0x04, 0xb8.toByte(), 0x7f)
        val outOfRangeResponse = core.process(pcPacket(0x0d, outOfRangeWrite))
        assertEquals(0x7f, outOfRangeResponse?.data?.get(7)?.toInt()?.and(0xff))
        assertEquals(1208, core.systemArraySnapshot.size)
    }

    @Test
    fun requestsPersistenceOnlyForPcArrayEepromCommand() {
        val core = registeredCore()
        val guid = byteArrayOf(0x22, '8'.code.toByte(), '9'.code.toByte(), 0x55)
        core.process(pcPacket(0x0b, guid))

        val result = core.processDetailed(
            pcPacket(0x0d, byteArrayOf(1, 0xff.toByte(), 0xfd.toByte(), 3)),
        )

        assertTrue(result.persistSystemArray)
        assertEquals(
            registeredPcResponse(
                0x0e,
                byteArrayOf(1, 0xff.toByte(), 0xfd.toByte(), 3),
            ),
            result.response,
        )
    }

    @Test
    fun ignoresPcArrayDataWithoutMatchingGuidOrActiveTransfer() {
        val core = registeredCore()
        val arrayData = pcPacket(0x0d, byteArrayOf(1, 0, 100, 0x5a))
        assertNull(core.process(arrayData))
        assertEquals(0, core.systemArraySnapshot[100].toInt() and 0xff)

        core.process(pcPacket(0x0b, byteArrayOf(0, 0, 0, 0)))
        assertNull(core.process(arrayData))
        assertEquals(0, core.systemArraySnapshot[100].toInt() and 0xff)
    }

    @Test
    fun restoresPersistedSystemArrayAndSynchronizesGuid() {
        val guid = listOf(0x22, '8'.code, '9'.code, 0x55)
        val storedArray = ByteArray(1208).apply { this[1150] = 0x01 }

        val core = Mbc89Core(guid, storedArray)

        assertEquals(0x01, core.systemArraySnapshot[1150].toInt() and 0xff)
        assertEquals(guid.map(Int::toByte), core.systemArraySnapshot.sliceArray(42..45).toList())
    }

    @Test
    fun invalidatesPersistedGuidWhenDatabaseChanges() {
        val guid = listOf(0x22, '8'.code, '9'.code, 0x55)
        val storedArray = ByteArray(1208).apply { this[46] = 1 }
        val core = Mbc89Core(guid, storedArray)

        core.process(pcPacket(0x00, byteArrayOf(0, 0, 0, 0)))
        core.process(pcPacket(0x01, byteArrayOf(0, 0, 0, 0)))
        core.process(pcPacket(0x02, byteArrayOf(0, 0, 0, 0)))

        assertFalse(core.isRegistered)
        assertEquals(
            listOf(0xff, 0xff, 0xff, 0xff),
            core.systemArraySnapshot.sliceArray(42..45).map { it.toInt() and 0xff },
        )
    }

    private fun connectPacket(canId: Int, data: ByteArray): CanPacket =
        CanPacket.create(
            priority = 0,
            canId = canId,
            hashHigh = 0x03,
            hashLow = 0x00,
            data = data,
        )

    private fun pcPacket(command: Int, fields: ByteArray): CanPacket {
        return pcPacketAtAddress(command, fields, 0, 0)
    }

    private fun pcPacketAtAddress(
        command: Int,
        fields: ByteArray,
        addressHigh: Int,
        addressLow: Int,
    ): CanPacket {
        require(fields.size == 4)
        return CanPacket.create(
            priority = 0,
            canId = 0x16,
            hashHigh = 0x5f,
            hashLow = 0x38,
            data = byteArrayOf(
                addressHigh.toByte(),
                addressLow.toByte(),
                0x18,
                command.toByte(),
            ) + fields,
        )
    }

    private fun registeredCore(): Mbc89Core {
        val core = Mbc89Core()
        core.process(pcPacket(0x00, byteArrayOf(0, 0, 0, 0)))
        core.process(pcPacket(0x01, byteArrayOf(0, 0, 0, 0)))
        core.process(pcPacket(0x02, byteArrayOf(0, 0, 0, 0)))
        core.process(pcPacket(0x03, byteArrayOf(0, 0x22, 0, 0)))

        var response = core.process(pcPacket(0x04, byteArrayOf(0, 0, 0, 0)))
        for (step in 1..42) {
            val registrationPacket = response
                ?: throw AssertionError("Expected registration step $step.")
            val data = registrationPacket.data
            response = core.process(
                pcPacket(0x05, byteArrayOf(0, 0x55, step.toByte(), data[7])),
            )
        }
        assertTrue(core.isRegistered)
        return core
    }

    private fun registeredPcResponse(command: Int, fields: ByteArray): CanPacket =
        CanPacket.create(
            priority = 0,
            canId = 0x16,
            hashHigh = 0x1b,
            hashLow = 0x6d,
            data = byteArrayOf(0, 0, 0x18, command.toByte()) + fields,
        )

    private fun cs2ConfigurationPacket(guid: ByteArray, channel: Int): CanPacket =
        CanPacket.create(
            priority = 0,
            canId = 0x3a,
            hashHigh = 0,
            hashLow = 0,
            data = guid + byteArrayOf(channel.toByte()),
        )

    private fun accessoryPacket(address: Int, green: Boolean): CanPacket =
        CanPacket.create(
            priority = 0,
            canId = 0x16,
            hashHigh = 0x03,
            hashLow = 0x00,
            data = byteArrayOf(
                0,
                0,
                (address ushr 8).toByte(),
                address.toByte(),
                if (green) 1 else 0,
                1,
            ),
        )

    private fun stopGoPacket(address: ByteArray, go: Boolean): CanPacket =
        CanPacket.create(
            priority = 0,
            canId = 0x00,
            hashHigh = 0,
            hashLow = 0,
            data = address + byteArrayOf(if (go) 1 else 0),
        )

    private fun measurementPacket(guid: ByteArray, channel: Int): CanPacket =
        CanPacket.create(
            priority = 0,
            canId = 0x00,
            hashHigh = 0,
            hashLow = 0,
            data = guid + byteArrayOf(0x0b, channel.toByte()),
        )

    private fun cs2ConfigurationRow(core: Mbc89Core, guid: ByteArray, channel: Int): ByteArray =
        core.processDetailed(cs2ConfigurationPacket(guid, channel))
            .responses
            .take(5)
            .flatMap { it.data.asIterable() }
            .toByteArray()
}
