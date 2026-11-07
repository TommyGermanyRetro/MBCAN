package de.mbc89.android

// Which address the digit keys enter (6021 manual 5.5): both together, only the loco ("L") or only
// the function decoder ("F").
enum class AddressEntry { LINKED, LOCO, FUNCTION }

// One entry per physical device: index 0 = the CU6021, 1..n = the Control 80f panels. The function
// decoder address equals the loco address unless "F" selected an own one (functionAddress != null).
data class ControlUnitSlot(
    val address: String = "",
    val claimSeq: Long = 0,
    val functionAddress: String? = null,
    val entry: AddressEntry = AddressEntry.LINKED,
) {
    val complete: Boolean get() = address.length == 2
    val functionTarget: String get() = functionAddress ?: address
    val displayed: String get() = if (entry == AddressEntry.FUNCTION) functionTarget else address
    val locoLed: Boolean get() = entry != AddressEntry.FUNCTION
    val functionLed: Boolean get() = entry != AddressEntry.LOCO
}

// "L"/"F": the following digits enter only the loco or only the function decoder address; the
// other address is kept.
fun ControlUnitSlot.select(target: AddressEntry): ControlUnitSlot =
    copy(functionAddress = functionTarget, entry = target)

// A locomotive can only be called up on one controller at a time: whichever slot completed the
// address first keeps it, every later duplicate is owned elsewhere (first come, first served).
fun addressOwnedElsewhere(slots: List<ControlUnitSlot>, index: Int): Boolean {
    val mine = slots.getOrNull(index)?.takeIf { it.complete } ?: return false
    val address = mine.address.toInt()
    if (address !in 1..80) return false
    return slots.withIndex().any { (other, slot) ->
        other != index && slot.complete && slot.address.toInt() == address &&
            slot.claimSeq < mine.claimSeq
    }
}

// Third digit starts a new address; the claim order is only taken when the loco address becomes
// complete. Entering the address the other one already has links both again (both LEDs lit).
fun ControlUnitSlot.withDigit(digit: Int, nextClaimSeq: Long): ControlUnitSlot {
    if (entry == AddressEntry.FUNCTION) {
        val text = nextDigits(functionTarget, digit)
        return if (text.length == 2 && text == address) {
            copy(functionAddress = null, entry = AddressEntry.LINKED)
        } else {
            copy(functionAddress = text)
        }
    }
    val text = nextDigits(address, digit)
    val claim = if (text.length == 2) nextClaimSeq else 0
    return if (entry == AddressEntry.LINKED || (text.length == 2 && text == functionAddress)) {
        ControlUnitSlot(text, claim)
    } else {
        copy(address = text, claimSeq = claim)
    }
}

private fun nextDigits(current: String, digit: Int): String =
    if (current.length == 1) current + digit else digit.toString()
