# Bugs and Fixes

Bugs listed in the order they were encountered.

---

## Bug #1 — Vivado wrapper contained dead wires (typo in signal names)

**Symptom:** In the manually-modified `tempSystem_wrapper.v` (old approach), two wires were declared but never connected or driven. Synthesis completed without error because unconnected wires are legal in Verilog, but the intent was for these to be useful.

**Root cause:** Typo — wires were declared as `I2CO_SCL` and `I2CO_SDA` (letter O) instead of `I2C0_SCL` and `I2C0_SDA` (zero). The correctly-named IOBUF instantiations connected to `I2C0_SCL` and `I2C0_SDA` (auto-created wires from the port declarations), so the IOBUFs worked. The misspelled wires were simply dead.

```verilog
// Dead wires — never driven, never read (typo: I2CO not I2C0)
wire I2CO_SCL;
wire I2CO_SDA;
```

**Fix:** Remove the dead wire declarations, or correct the typo. Synthesis is unaffected either way since the IOBUFs connected to the correctly-named wires. Moot in the improved flow — manual wrapper modification is no longer done at all.

**Lesson:** When manually editing auto-generated Verilog, verify every signal name character-by-character. `0` vs `O` is a silent failure in Verilog.

---

## Bug #2 — Updating `.xsa` in existing Vitis workspace fails

**Symptom:** After re-exporting hardware from Vivado (new `.xsa`), importing the updated `.xsa` into an existing Vitis platform component and rebuilding does not produce a working application. Build may succeed but behavior is incorrect, or the platform does not reflect the hardware changes.

**Root cause:** Unknown. Vitis 2025.x platform update flow does not appear to cleanly propagate all hardware changes through the BSP regeneration.

**Fix (confirmed working):** Create a new Vitis workspace directory for every new `.xsa`. Do not reuse workspaces across hardware exports.

**Lesson:** Treat each hardware export as a clean slate. Keep `ws/` as a throwaway directory — all persistent work lives in the source files (`main.c`, `ADT7420.c`, `ADT7420.h`), not in the workspace.

---

## Bug #3 — Direct Pmod plug-in (W8/W10) fails silently

**Symptom:** Assigning SDA to W8 and SCL to W10 (which correspond to the JB pin positions that would allow the Pmod TMP2 to plug in directly without jumper cables) resulted in no I2C communication. No error from the driver — the bus simply never responded.

**Root cause:** Unknown. Not yet investigated.

**Attempted:** XDC constraints with W8/W10, same LVCMOS33 and PULLUP settings as the working W11/W12 configuration.

**Workaround (confirmed working):** Use jumper cables. Assign SCL to W12 (JB1) and SDA to W11 (JB2).

**Open investigation items:**
- Confirm W8 and W10 are in the same I/O bank as W11/W12 (bank 13, 3.3V)
- Check ZedBoard schematic for any differences at those specific pins
- Try with explicit `set_property DRIVE 8` or `DRIVE 12` in XDC
- Verify constraints were actually saved and applied for the W8/W10 attempt (synthesis re-run after saving)

**Status:** Unresolved. Jumper cable workaround is stable.

---

## Bug #4 — Negative temperature output is untested

**Symptom:** Not a runtime bug — a known gap in validation. The driver has only been tested with positive ambient temperatures.

**Root cause:** The ADT7420 uses two's complement for sub-zero readings. The `s16` cast and arithmetic right-shift (`>>= 3`) should preserve the sign correctly, but the integer/fractional split in `ADT7420_Print_Temp` does not account for negative fractional parts. For example, `-5.50°C` would likely print incorrectly because `(t - c_int)` where both are negative produces unexpected behavior.

**Fix (untested hypothesis):**
```c
// In ADT7420_Print_Temp, replace fractional calculation with:
if (t >= 0) {
    c_frac = (int)((t - c_int) * 100);
} else {
    c_frac = (int)((c_int - t) * 100);  // abs value of fractional part
}
```

Apply the same pattern for the Fahrenheit fractional part.

**Status:** Untested. Validate with a cold environment (ice bath, freezer) before relying on sub-zero readings.
