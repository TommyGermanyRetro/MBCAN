package de.mbc89.android

import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertTrue
import org.junit.Test

class ControlUnitSlotsTest {
    @Test
    fun thirdDigitStartsNewAddressAndClaimIsTakenOnCompletion() {
        var slot = ControlUnitSlot()
        slot = slot.withDigit(2, 5)
        assertEquals(ControlUnitSlot("2", 0), slot)
        slot = slot.withDigit(4, 5)
        assertEquals(ControlUnitSlot("24", 5), slot)
        slot = slot.withDigit(7, 6)
        assertEquals(ControlUnitSlot("7", 0), slot)
    }

    @Test
    fun firstClaimKeepsTheAddress() {
        val slots = listOf(ControlUnitSlot("24", 2), ControlUnitSlot("24", 1), ControlUnitSlot("30", 3))
        assertTrue(addressOwnedElsewhere(slots, 0))
        assertFalse(addressOwnedElsewhere(slots, 1))
        assertFalse(addressOwnedElsewhere(slots, 2))
    }

    @Test
    fun incompleteOrOutOfRangeAddressesNeverConflict() {
        val slots = listOf(ControlUnitSlot("2", 0), ControlUnitSlot("2", 0), ControlUnitSlot("00", 4), ControlUnitSlot("00", 5))
        assertFalse(addressOwnedElsewhere(slots, 0))
        assertFalse(addressOwnedElsewhere(slots, 3))
    }

    @Test
    fun functionKeySelectsOwnDecoderAddressAndLocoKeepsIt() {
        var slot = ControlUnitSlot("10", 1)
        assertTrue(slot.locoLed && slot.functionLed)

        slot = slot.select(AddressEntry.FUNCTION).withDigit(2, 2).withDigit(5, 2)
        assertEquals("10", slot.address)
        assertEquals("25", slot.functionTarget)
        assertEquals("25", slot.displayed)
        assertFalse(slot.locoLed)
        assertTrue(slot.functionLed)

        slot = slot.select(AddressEntry.LOCO).withDigit(0, 3).withDigit(8, 3)
        assertEquals("08", slot.address)
        assertEquals("25", slot.functionTarget)
        assertEquals("08", slot.displayed)
        assertTrue(slot.locoLed)
        assertFalse(slot.functionLed)
    }

    @Test
    fun enteringTheSameAddressLinksLocoAndFunctionDecoderAgain() {
        var slot = ControlUnitSlot("08", 1, functionAddress = "10", entry = AddressEntry.LOCO)
        slot = slot.select(AddressEntry.FUNCTION).withDigit(0, 2).withDigit(8, 2)
        assertEquals(AddressEntry.LINKED, slot.entry)
        assertEquals(null, slot.functionAddress)
        assertTrue(slot.locoLed && slot.functionLed)

        slot = slot.withDigit(3, 3).withDigit(3, 3)
        assertEquals("33", slot.address)
        assertEquals("33", slot.functionTarget)
    }
}
