package de.mbc89.android.core

import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertNotNull
import org.junit.Assert.assertNull
import org.junit.Assert.assertTrue
import org.junit.Test

class Mbc89MemoryTest {
    private fun record(memory: Mbc89Memory, instance: Int, local: Int, vararg commands: MemoryCommand) {
        memory.press(instance, Mbc89Memory.BUTTON_IN)
        memory.press(instance, Mbc89Memory.FIRST_ROUTE_BUTTON + local)
        commands.forEach { memory.tapKeyboardCommand(it) }
        memory.press(instance, Mbc89Memory.BUTTON_END)
    }

    @Test
    fun recordsAndPlaysRoute() {
        val memory = Mbc89Memory()
        val first = MemoryCommand(0x3000, true)
        val second = MemoryCommand(0x3001, false)
        record(memory, 1, 9, first, second)
        assertEquals(listOf(first, second), memory.route(1, 9))
        assertTrue(memory.isValid(1, 9))
        assertFalse(memory.isActive(1, 9))
        val result = memory.press(1, Mbc89Memory.FIRST_ROUTE_BUTTON + 9)
        assertEquals(listOf(first, second), result.commands)
        assertTrue(memory.isActive(1, 9))
        assertEquals(LedState.OFF, memory.ledState(1, Mbc89Memory.FIRST_ROUTE_BUTTON + 9))
        memory.setPlayLit(1, 9, true)
        assertEquals(LedState.ON, memory.ledState(1, Mbc89Memory.FIRST_ROUTE_BUTTON + 9))
        memory.setPlayLit(1, 9, false)
        assertEquals(LedState.OFF, memory.ledState(1, Mbc89Memory.FIRST_ROUTE_BUTTON + 9))
        memory.finishPlay(1, 9)
        assertEquals(LedState.ON, memory.ledState(1, Mbc89Memory.FIRST_ROUTE_BUTTON + 9))
    }

    @Test
    fun clearInstanceRemovesAllRoutesOfThatInstanceOnly() {
        val memory = Mbc89Memory()
        val command = MemoryCommand(0x3000, true)
        record(memory, 1, 9, command)
        record(memory, 0, 2, command)
        memory.press(1, Mbc89Memory.FIRST_ROUTE_BUTTON + 9)
        memory.clearInstance(1)
        assertFalse(memory.isValid(1, 9))
        assertFalse(memory.isActive(1, 9))
        assertEquals(LedState.OFF, memory.ledState(1, Mbc89Memory.FIRST_ROUTE_BUTTON + 9))
        assertTrue(memory.isValid(0, 2))
    }

    @Test
    fun repeatedSameKeyLengthensSwitchTimeUpToSevenPresses() {
        val memory = Mbc89Memory()
        memory.press(0, Mbc89Memory.BUTTON_IN)
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON)
        repeat(9) { memory.tapKeyboardCommand(MemoryCommand(0x3001, true)) }

        val command = (memory.route(0, 0).single() as MemoryCommand)
        assertEquals(7, command.presses)
        assertEquals(210, command.switchTime)
    }

    @Test
    fun otherDirectionResetsPressCountAndSinglePressUsesDefault() {
        val memory = Mbc89Memory()
        memory.press(0, Mbc89Memory.BUTTON_IN)
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON)
        memory.tapKeyboardCommand(MemoryCommand(0x3001, true))
        memory.tapKeyboardCommand(MemoryCommand(0x3001, true))
        assertEquals(2, (memory.route(0, 0).single() as MemoryCommand).presses)
        assertEquals(60, (memory.route(0, 0).single() as MemoryCommand).switchTime)

        memory.tapKeyboardCommand(MemoryCommand(0x3001, false))
        val command = (memory.route(0, 0).single() as MemoryCommand)
        assertEquals(MemoryCommand(0x3001, false, 1), command)
        assertEquals(0, command.switchTime)
    }

    @Test
    fun extendsExistingRouteByAppendingCommands() {
        val memory = Mbc89Memory()
        memory.press(0, Mbc89Memory.BUTTON_IN)
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON + 1)
        memory.tapKeyboardCommand(MemoryCommand(0x3001, true))
        memory.press(0, Mbc89Memory.BUTTON_END)

        memory.press(0, Mbc89Memory.BUTTON_IN)
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON + 1)
        memory.tapKeyboardCommand(MemoryCommand(0x3002, false))
        memory.press(0, Mbc89Memory.BUTTON_END)

        assertEquals(
            listOf(MemoryCommand(0x3001, true), MemoryCommand(0x3002, false)),
            memory.route(0, 1),
        )
    }

    @Test
    fun decodesStoredRoutesWithAndWithoutPressCount() {
        val text = List(Mbc89Memory.TOTAL_ROUTES) { "" }.toMutableList().apply {
            this[0] = "3001:1,3002:0:4"
        }.joinToString(";")

        val memory = Mbc89Memory.decode(text)

        assertEquals(
            listOf(MemoryCommand(0x3001, true, 1), MemoryCommand(0x3002, false, 4)),
            memory.route(0, 0),
        )
        assertEquals(memory.encode(), Mbc89Memory.decode(memory.encode()).encode())
    }

    @Test
    fun switchTimeSelectsEightByteForm() {
        val core = Mbc89Core()
        val plain = core.buildAccessorySwitch(0x3001, true)!!
        assertEquals(6, plain.dataLength)
        val timed = core.buildAccessorySwitch(0x3001, true, 0x012c)!!
        assertEquals(8, timed.dataLength)
        assertEquals(0x01, timed.data[6].toInt())
        assertEquals(0x2c, timed.data[7].toInt())
    }

    @Test
    fun inputBlinksAndRecordedRouteBlinksWhileWaiting() {
        val memory = Mbc89Memory()
        record(memory, 0, 4, MemoryCommand(0x3000, true))
        assertEquals(LedState.ON, memory.ledState(0, Mbc89Memory.BUTTON_END))
        memory.press(0, Mbc89Memory.BUTTON_IN)
        assertEquals(LedState.OFF, memory.ledState(0, Mbc89Memory.BUTTON_END))
        assertEquals(LedState.BLINK, memory.ledState(0, Mbc89Memory.BUTTON_IN))
        assertEquals(LedState.BLINK, memory.ledState(0, Mbc89Memory.FIRST_ROUTE_BUTTON + 4))
        assertEquals(LedState.OFF, memory.ledState(0, Mbc89Memory.FIRST_ROUTE_BUTTON + 5))
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON + 5)
        assertEquals(LedState.ON, memory.ledState(0, Mbc89Memory.BUTTON_IN))
        assertEquals(LedState.ON, memory.ledState(0, Mbc89Memory.FIRST_ROUTE_BUTTON + 5))
        assertEquals(LedState.OFF, memory.ledState(0, Mbc89Memory.FIRST_ROUTE_BUTTON + 4))
    }

    @Test
    fun clearBlinksWhileWaitingForRoute() {
        val memory = Mbc89Memory()
        memory.press(0, Mbc89Memory.BUTTON_IN)
        memory.press(0, Mbc89Memory.BUTTON_CLR)
        assertEquals(LedState.BLINK, memory.ledState(0, Mbc89Memory.BUTTON_CLR))
        assertEquals(LedState.OFF, memory.ledState(0, Mbc89Memory.BUTTON_IN))
        assertTrue(memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON).deleteShown)
        assertEquals(LedState.OFF, memory.ledState(0, Mbc89Memory.BUTTON_CLR))
        assertEquals(LedState.ON, memory.ledState(0, Mbc89Memory.BUTTON_IN))
        assertEquals(LedState.ON, memory.ledState(0, Mbc89Memory.FIRST_ROUTE_BUTTON))
        assertEquals(LedState.OFF, memory.ledState(0, Mbc89Memory.BUTTON_END))
        assertTrue(memory.finishDeleteDisplay(0))
        assertEquals(LedState.OFF, memory.ledState(0, Mbc89Memory.BUTTON_IN))
        assertEquals(LedState.OFF, memory.ledState(0, Mbc89Memory.FIRST_ROUTE_BUTTON))
        assertEquals(LedState.ON, memory.ledState(0, Mbc89Memory.BUTTON_END))
        assertFalse(memory.finishDeleteDisplay(0))
    }

    @Test
    fun sameAccessoryReplacesEarlierDirection() {
        val memory = Mbc89Memory()
        record(memory, 0, 0, MemoryCommand(0x3000, true), MemoryCommand(0x3000, false))
        assertEquals(listOf(MemoryCommand(0x3000, false)), memory.route(0, 0))
    }

    @Test
    fun recordingLimitsRouteToTwentyCommands() {
        val memory = Mbc89Memory()
        record(memory, 0, 0, *Array(25) { MemoryCommand(0x3000 + it, true) })
        assertEquals(20, memory.route(0, 0).size)
    }

    @Test
    fun clearDuringRecordingDropsLastCommand() {
        val memory = Mbc89Memory()
        memory.press(0, Mbc89Memory.BUTTON_IN)
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON)
        memory.tapKeyboardCommand(MemoryCommand(0x3000, true))
        memory.tapKeyboardCommand(MemoryCommand(0x3001, true))
        val result = memory.press(0, Mbc89Memory.BUTTON_CLR)
        assertTrue(result.routesChanged)
        assertTrue(result.clearFlash)
        assertEquals(listOf(MemoryCommand(0x3000, true)), memory.route(0, 0))
        assertEquals(LedState.ON, memory.ledState(0, Mbc89Memory.BUTTON_CLR))
        assertEquals(LedState.ON, memory.ledState(0, Mbc89Memory.BUTTON_IN))
        assertEquals(LedState.ON, memory.ledState(0, Mbc89Memory.FIRST_ROUTE_BUTTON))
        memory.finishClearFlash(0)
        assertEquals(LedState.OFF, memory.ledState(0, Mbc89Memory.BUTTON_CLR))
    }

    @Test
    fun deletesWholeRoute() {
        val memory = Mbc89Memory()
        record(memory, 0, 2, MemoryCommand(0x3000, true))
        memory.press(0, Mbc89Memory.BUTTON_IN)
        memory.press(0, Mbc89Memory.BUTTON_CLR)
        assertTrue(memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON + 2).routesChanged)
        assertFalse(memory.isValid(0, 2))
    }

    @Test
    fun releaseKeepsContentButClearsActive() {
        val memory = Mbc89Memory()
        record(memory, 0, 0, MemoryCommand(0x3000, true))
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON)
        memory.press(0, Mbc89Memory.BUTTON_CLR)
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON)
        assertFalse(memory.isActive(0, 0))
        assertTrue(memory.isValid(0, 0))
    }

    @Test
    fun routeWithoutContentIsNotPlayed() {
        val memory = Mbc89Memory()
        val result = memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON + 3)
        assertTrue(result.commands.isEmpty())
        assertFalse(memory.isActive(0, 3))
    }

    @Test
    fun playingCrossingRouteEndsOtherActiveRoute() {
        val memory = Mbc89Memory()
        record(memory, 0, 0, MemoryCommand(0x3000, true))
        record(memory, 0, 1, MemoryCommand(0x3000, false))
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON)
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON + 1)
        assertFalse(memory.isActive(0, 0))
        assertTrue(memory.isActive(0, 1))
    }

    @Test
    fun encodeDecodeRoundTrip() {
        val memory = Mbc89Memory()
        record(memory, 2, 5, MemoryCommand(0x3002, true), MemoryCommand(0x3003, false))
        val restored = Mbc89Memory.decode(memory.encode())
        assertEquals(memory.route(2, 5), restored.route(2, 5))
        assertFalse(restored.isValid(0, 0))
    }

    @Test
    fun settingRouteEndsCrossingRouteWithoutInterlock() {
        val memory = Mbc89Memory()
        record(memory, 0, 0, MemoryCommand(0x3000, true))
        record(memory, 0, 1, MemoryCommand(0x3000, false))
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON)
        assertTrue(memory.isActive(0, 0))

        val result = memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON + 1)

        assertEquals(listOf(MemoryCommand(0x3000, false)), result.commands)
        assertNull(result.warningToken)
        assertFalse(memory.isActive(0, 0))
        assertTrue(memory.isActive(0, 1))
    }

    @Test
    fun interlockBlocksCrossingRouteAndBlinksBothUntilSecondPress() {
        val memory = Mbc89Memory()
        memory.interlock.fill(true)
        record(memory, 0, 0, MemoryCommand(0x3000, true))
        record(memory, 0, 1, MemoryCommand(0x3000, false))
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON)

        val blocked = memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON + 1)

        assertTrue(blocked.commands.isEmpty())
        assertNotNull(blocked.warningToken)
        assertTrue(memory.isActive(0, 0))
        assertFalse(memory.isActive(0, 1))
        assertEquals(LedState.BLINK, memory.ledState(0, Mbc89Memory.FIRST_ROUTE_BUTTON))
        assertEquals(LedState.BLINK, memory.ledState(0, Mbc89Memory.FIRST_ROUTE_BUTTON + 1))

        val forced = memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON + 1)

        assertEquals(listOf(MemoryCommand(0x3000, false)), forced.commands)
        assertFalse(memory.isActive(0, 0))
        assertTrue(memory.isActive(0, 1))
    }

    @Test
    fun interlockIgnoresCrossingRouteOfOtherMemory() {
        val memory = Mbc89Memory()
        memory.interlock.fill(true)
        record(memory, 0, 0, MemoryCommand(0x3000, true))
        record(memory, 1, 0, MemoryCommand(0x3000, false))
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON)

        val result = memory.press(1, Mbc89Memory.FIRST_ROUTE_BUTTON)

        assertEquals(listOf(MemoryCommand(0x3000, false)), result.commands)
        assertNull(result.warningToken)
        assertFalse(memory.isActive(0, 0))
    }

    @Test
    fun warningEndsByOtherKeyOrAfterItsTime() {
        val memory = Mbc89Memory()
        memory.interlock.fill(true)
        record(memory, 0, 0, MemoryCommand(0x3000, true))
        record(memory, 0, 1, MemoryCommand(0x3000, false))
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON)
        memory.finishPlay(0, 0)
        val first = memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON + 1).warningToken!!

        memory.press(0, Mbc89Memory.BUTTON_EX)
        assertEquals(LedState.ON, memory.ledState(0, Mbc89Memory.FIRST_ROUTE_BUTTON))
        assertEquals(LedState.OFF, memory.ledState(0, Mbc89Memory.FIRST_ROUTE_BUTTON + 1))
        assertFalse(memory.expireWarning(0, first))

        val second = memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON + 1).warningToken!!
        assertFalse(memory.expireWarning(0, first))
        assertTrue(memory.expireWarning(0, second))
        assertEquals(LedState.OFF, memory.ledState(0, Mbc89Memory.FIRST_ROUTE_BUTTON + 1))
        assertFalse(memory.expireWarning(0, second))
    }

    @Test
    fun followUpRouteIsRecordedAndPlayedInOrder() {
        val memory = Mbc89Memory()
        record(memory, 0, 1, MemoryCommand(0x3001, true))
        memory.press(0, Mbc89Memory.BUTTON_IN)
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON)
        memory.tapKeyboardCommand(MemoryCommand(0x3000, false))
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON + 1)
        memory.tapKeyboardCommand(MemoryCommand(0x3002, true))
        memory.press(0, Mbc89Memory.BUTTON_END)
        assertEquals(
            listOf(MemoryCommand(0x3000, false), MemoryRouteCall(1), MemoryCommand(0x3002, true)),
            memory.route(0, 0),
        )

        val result = memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON)

        assertEquals(
            listOf(MemoryCommand(0x3000, false), MemoryCommand(0x3001, true), MemoryCommand(0x3002, true)),
            result.commands,
        )
        assertTrue(memory.isActive(0, 0))
        assertTrue(memory.isActive(0, 1))
    }

    @Test
    fun followUpRouteMayBeEmptyAndLoopsPlayOnce() {
        val memory = Mbc89Memory()
        memory.press(0, Mbc89Memory.BUTTON_IN)
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON)
        memory.tapKeyboardCommand(MemoryCommand(0x3000, true))
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON + 1)
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON + 2)
        memory.press(0, Mbc89Memory.BUTTON_END)
        memory.press(0, Mbc89Memory.BUTTON_IN)
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON + 1)
        memory.tapKeyboardCommand(MemoryCommand(0x3001, true))
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON)
        memory.press(0, Mbc89Memory.BUTTON_END)

        val result = memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON)

        assertEquals(listOf(MemoryCommand(0x3000, true), MemoryCommand(0x3001, true)), result.commands)
        assertFalse(memory.isActive(0, 2))
    }

    @Test
    fun interlockSkipsCrossingFollowUpRouteSilently() {
        val memory = Mbc89Memory()
        memory.interlock.fill(true)
        record(memory, 0, 2, MemoryCommand(0x3005, true))
        record(memory, 0, 1, MemoryCommand(0x3005, false))
        memory.press(0, Mbc89Memory.BUTTON_IN)
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON)
        memory.tapKeyboardCommand(MemoryCommand(0x3000, true))
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON + 1)
        memory.press(0, Mbc89Memory.BUTTON_END)
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON + 2)

        val result = memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON)

        assertEquals(listOf(MemoryCommand(0x3000, true)), result.commands)
        assertNull(result.warningToken)
        assertTrue(memory.isActive(0, 0))
        assertFalse(memory.isActive(0, 1))
        assertTrue(memory.isActive(0, 2))
    }

    @Test
    fun routeCallAsTwentiethEntryEndsRecording() {
        val memory = Mbc89Memory()
        memory.press(0, Mbc89Memory.BUTTON_IN)
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON)
        repeat(Mbc89Memory.COMMANDS_PER_ROUTE - 1) {
            memory.tapKeyboardCommand(MemoryCommand(0x3000 + it, true))
        }
        assertEquals(LedState.BLINK, memory.ledState(0, Mbc89Memory.FIRST_ROUTE_BUTTON))

        val result = memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON + 3)

        assertTrue(result.routesChanged)
        assertEquals(MemoryRouteCall(3), memory.route(0, 0).last())
        assertEquals(Mbc89Memory.COMMANDS_PER_ROUTE, memory.route(0, 0).size)
        assertEquals(LedState.OFF, memory.ledState(0, Mbc89Memory.BUTTON_IN))
        assertEquals(LedState.ON, memory.ledState(0, Mbc89Memory.BUTTON_END))
    }

    @Test
    fun accessorySwitchedTheOtherWayEndsRoute() {
        val memory = Mbc89Memory()
        record(memory, 0, 0, MemoryCommand(0x3000, true), MemoryCommand(0x3001, false))
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON)

        assertFalse(memory.noteSwitch(MemoryCommand(0x3001, false)))
        assertFalse(memory.noteSwitch(MemoryCommand(0x3002, true)))
        assertTrue(memory.isActive(0, 0))
        assertTrue(memory.noteSwitch(MemoryCommand(0x3001, true)))
        assertFalse(memory.isActive(0, 0))
    }

    @Test
    fun encodesRouteCalls() {
        val memory = Mbc89Memory()
        memory.press(0, Mbc89Memory.BUTTON_IN)
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON)
        memory.tapKeyboardCommand(MemoryCommand(0x3000, true))
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON + 12)
        memory.press(0, Mbc89Memory.BUTTON_END)

        assertTrue(memory.encode().startsWith("3000:1:1,R12;"))
        assertEquals(memory.route(0, 0), Mbc89Memory.decode(memory.encode()).route(0, 0))
    }

    @Test
    fun busCommandsAreRecordedWithStandardSwitchTime() {
        val memory = Mbc89Memory()
        memory.press(0, Mbc89Memory.BUTTON_IN)
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON)
        assertTrue(memory.tapKeyboardCommand(MemoryCommand(0x3001, true), lengthen = false))
        assertFalse(memory.tapKeyboardCommand(MemoryCommand(0x3001, true), lengthen = false))
        assertEquals(listOf(MemoryCommand(0x3001, true, 1)), memory.route(0, 0))
        assertTrue(memory.tapKeyboardCommand(MemoryCommand(0x3001, false), lengthen = false))
        assertEquals(listOf(MemoryCommand(0x3001, false, 1)), memory.route(0, 0))
    }

    @Test
    fun interlockIsSetPerMemory() {
        val memory = Mbc89Memory()
        memory.interlock[1] = true
        record(memory, 0, 0, MemoryCommand(0x3000, true))
        record(memory, 0, 1, MemoryCommand(0x3000, false))
        record(memory, 1, 0, MemoryCommand(0x3001, true))
        record(memory, 1, 1, MemoryCommand(0x3001, false))
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON)
        memory.press(1, Mbc89Memory.FIRST_ROUTE_BUTTON)

        assertNull(memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON + 1).warningToken)
        assertNotNull(memory.press(1, Mbc89Memory.FIRST_ROUTE_BUTTON + 1).warningToken)
        assertTrue(memory.isActive(0, 1))
        assertFalse(memory.isActive(1, 1))
    }

    @Test
    fun hasRoutesReportsOccupiedMemory() {
        val memory = Mbc89Memory()
        assertFalse(memory.hasRoutes(1))
        record(memory, 1, 23, MemoryCommand(0x3000, true))
        assertTrue(memory.hasRoutes(1))
        assertFalse(memory.hasRoutes(0))
        memory.clearInstance(1)
        assertFalse(memory.hasRoutes(1))
    }

    private fun automatic(memory: Mbc89Memory, instance: Int, module: Int) {
        memory.s88Module[instance] = module
        memory.press(instance, Mbc89Memory.BUTTON_EX)
    }

    @Test
    fun s88RequestSetsRouteOnlyInAutomaticMode() {
        val memory = Mbc89Memory()
        record(memory, 0, 10, MemoryCommand(0x3000, true))
        memory.s88Module[0] = 1
        assertFalse(memory.s88Event(1019, true))
        assertTrue(memory.takeAutoPlays().isEmpty())

        memory.press(0, Mbc89Memory.BUTTON_EX)
        assertTrue(memory.s88Event(1019, true))
        val play = memory.takeAutoPlays().single()
        assertEquals(0, play.instance)
        assertEquals(10, play.localRoute)
        assertEquals(listOf(MemoryCommand(0x3000, true)), play.commands)
        assertTrue(memory.isActive(0, 10))
        assertFalse(memory.s88Event(1019, false))
    }

    @Test
    fun s88PinsMapToMemoryRowsAndBuses() {
        val memory = Mbc89Memory()
        record(memory, 1, 23, MemoryCommand(0x3001, true))
        automatic(memory, 0, 1)
        automatic(memory, 1, 32)

        assertFalse(memory.s88Event(1040, true))
        assertFalse(memory.s88Event(1000, true))
        assertFalse(memory.s88Event(1497, true))
        assertFalse(memory.s88Event(4001, true))
        assertTrue(memory.s88Event(2040, true))
        assertEquals(listOf(1 to 23), memory.takeAutoPlays().map { it.instance to it.localRoute })
        assertTrue(memory.isActive(1, 23))
    }

    @Test
    fun interlockStoresRequestAndReleaseInputSetsItByPriority() {
        val memory = Mbc89Memory()
        record(memory, 0, 0, MemoryCommand(0x3000, true), MemoryCommand(0x3001, true))
        record(memory, 0, 1, MemoryCommand(0x3000, false))
        record(memory, 0, 2, MemoryCommand(0x3001, false), MemoryCommand(0x3000, true))
        memory.interlock[0] = true
        automatic(memory, 0, 1)
        memory.s88Event(1001, true)
        memory.takeAutoPlays()

        assertTrue(memory.s88Event(1003, true))
        assertTrue(memory.s88Event(1002, true))
        assertTrue(memory.takeAutoPlays().isEmpty())
        assertTrue(memory.isPending(0, 1))
        assertTrue(memory.isPending(0, 2))
        assertEquals(LedState.OFF, memory.ledState(0, Mbc89Memory.FIRST_ROUTE_BUTTON + 1))

        assertTrue(memory.s88Event(1009, true))
        val plays = memory.takeAutoPlays()
        assertEquals(listOf(1), plays.map { it.localRoute })
        assertTrue(memory.isActive(0, 1))
        assertFalse(memory.isPending(0, 1))
        assertTrue(memory.isPending(0, 2))
    }

    @Test
    fun occupiedReleaseInputBlocksRequestWithInterlock() {
        val memory = Mbc89Memory()
        record(memory, 0, 0, MemoryCommand(0x3000, true))
        memory.interlock[0] = true
        automatic(memory, 0, 1)
        memory.s88Event(1009, true)

        assertFalse(memory.s88Event(1001, true))
        assertFalse(memory.isActive(0, 0))
        assertFalse(memory.isPending(0, 0))

        memory.s88Event(1009, false)
        assertTrue(memory.s88Event(1001, true))
        assertTrue(memory.isActive(0, 0))
    }

    @Test
    fun withoutInterlockRequestIsNotStoredAndReleaseInputDoesNothing() {
        val memory = Mbc89Memory()
        record(memory, 0, 0, MemoryCommand(0x3000, true))
        automatic(memory, 0, 1)
        memory.s88Event(1009, true)
        assertTrue(memory.s88Event(1001, true))
        assertTrue(memory.isActive(0, 0))
        assertFalse(memory.s88Event(1009, true))
        assertTrue(memory.isActive(0, 0))
    }

    @Test
    fun offDropsStoredRequests() {
        val memory = Mbc89Memory()
        record(memory, 0, 0, MemoryCommand(0x3000, true))
        record(memory, 0, 1, MemoryCommand(0x3000, false))
        memory.interlock[0] = true
        automatic(memory, 0, 1)
        memory.s88Event(1001, true)
        memory.s88Event(1002, true)
        assertTrue(memory.isPending(0, 1))

        memory.press(0, Mbc89Memory.BUTTON_OFF)

        assertFalse(memory.isPending(0, 1))
        assertFalse(memory.s88Event(1002, true))
    }

    @Test
    fun manualReleaseAndForeignSwitchSetStoredRequests() {
        val memory = Mbc89Memory()
        record(memory, 0, 0, MemoryCommand(0x3000, true))
        record(memory, 0, 1, MemoryCommand(0x3000, false))
        memory.interlock[0] = true
        automatic(memory, 0, 1)
        memory.s88Event(1001, true)
        memory.s88Event(1002, true)
        memory.takeAutoPlays()

        memory.press(0, Mbc89Memory.BUTTON_CLR)
        memory.press(0, Mbc89Memory.FIRST_ROUTE_BUTTON)
        assertEquals(listOf(1), memory.takeAutoPlays().map { it.localRoute })

        memory.s88Event(1001, true)
        assertTrue(memory.isPending(0, 0))
        assertTrue(memory.noteSwitch(MemoryCommand(0x3000, true)))
        assertEquals(listOf(0), memory.takeAutoPlays().map { it.localRoute })
    }

    @Test
    fun systemStopIgnoresRequestsButKeepsReleaseInputs() {
        val memory = Mbc89Memory()
        record(memory, 0, 0, MemoryCommand(0x3000, true))
        record(memory, 0, 1, MemoryCommand(0x3000, false))
        memory.interlock[0] = true
        automatic(memory, 0, 1)
        memory.s88Event(1001, true)
        memory.s88Event(1002, true)
        memory.takeAutoPlays()
        memory.systemStopped = true

        assertFalse(memory.s88Event(1001, true))
        memory.s88Event(1009, true)
        assertFalse(memory.isActive(0, 0))
        assertTrue(memory.takeAutoPlays().isEmpty())
        assertTrue(memory.isPending(0, 1))

        memory.systemStopped = false
        assertTrue(memory.isPending(0, 1))
        assertTrue(memory.takeAutoPlays().isEmpty())
    }

    @Test
    fun s88ModulesOfDifferentMemoryMustNotOverlap() {
        val modules = listOf(1, 4, 0, 10)
        assertTrue(Mbc89Memory.s88Overlaps(modules, 4, 1, 2))
        assertTrue(Mbc89Memory.s88Overlaps(modules, 4, 1, 3))
        assertFalse(Mbc89Memory.s88Overlaps(modules, 4, 1, 4))
        assertFalse(Mbc89Memory.s88Overlaps(modules, 4, 1, 7))
        assertTrue(Mbc89Memory.s88Overlaps(modules, 4, 2, 8))
        assertFalse(Mbc89Memory.s88Overlaps(modules, 3, 2, 8))
        assertFalse(Mbc89Memory.s88Overlaps(modules, 4, 2, 0))
        assertFalse(Mbc89Memory.s88Overlaps(modules, 4, 0, 1))
    }

    @Test
    fun restoresSetRoutesAndModeButNotStoredRequests() {
        val memory = Mbc89Memory()
        record(memory, 0, 0, MemoryCommand(0x3000, true))
        record(memory, 0, 1, MemoryCommand(0x3000, false))
        record(memory, 2, 5, MemoryCommand(0x3002, true))
        memory.interlock[0] = true
        automatic(memory, 0, 1)
        memory.s88Event(1001, true)
        memory.s88Event(1002, true)
        memory.press(2, Mbc89Memory.FIRST_ROUTE_BUTTON + 5)
        assertTrue(memory.isPending(0, 1))

        val restored = Mbc89Memory.decode(memory.encode())
        restored.restoreState(memory.encodeState())

        assertTrue(restored.isActive(0, 0))
        assertFalse(restored.isActive(0, 1))
        assertTrue(restored.isActive(2, 5))
        assertFalse(restored.isPending(0, 1))
        assertEquals(LedState.ON, restored.ledState(0, Mbc89Memory.BUTTON_EX))
        assertEquals(LedState.ON, restored.ledState(2, Mbc89Memory.BUTTON_OFF))
        assertEquals(LedState.ON, restored.ledState(0, Mbc89Memory.BUTTON_END))
        assertEquals(LedState.ON, restored.ledState(0, Mbc89Memory.FIRST_ROUTE_BUTTON))
    }

    @Test
    fun startsWithOffAndEndWhenNoStateIsStored() {
        val memory = Mbc89Memory()
        memory.restoreState(null)
        assertEquals(LedState.ON, memory.ledState(0, Mbc89Memory.BUTTON_OFF))
        assertEquals(LedState.ON, memory.ledState(0, Mbc89Memory.BUTTON_END))
        assertEquals(LedState.OFF, memory.ledState(0, Mbc89Memory.BUTTON_EX))

        val emptyRoute = Mbc89Memory()
        emptyRoute.restoreState("1" + "0".repeat(Mbc89Memory.TOTAL_ROUTES - 1) + "|0000")
        assertFalse(emptyRoute.isActive(0, 0))
        emptyRoute.restoreState("broken")
        assertEquals(LedState.ON, emptyRoute.ledState(0, Mbc89Memory.BUTTON_END))
    }

    @Test
    fun routeViewsListRecordedRoutesWithState() {
        val memory = Mbc89Memory()
        record(memory, 0, 2, MemoryCommand(0x3000, true))
        record(memory, 1, 9, MemoryCommand(0x3801, false))
        memory.press(1, Mbc89Memory.FIRST_ROUTE_BUTTON + 9)

        assertEquals(
            listOf(
                MemoryRouteView(0, 2, listOf(MemoryCommand(0x3000, true)), false),
                MemoryRouteView(1, 9, listOf(MemoryCommand(0x3801, false)), true),
            ),
            memory.routeViews(),
        )
    }
}
