# Troubleshooting — Quick Reference

Symptom → most likely cause → fix. Most-likely cause listed first.

---

## I2C / Sensor

**No output from sensor / "Read error: check pull-ups" printed continuously**
1. Pull-ups not set → verify `PULLTYPE PULLUP` is in XDC for both SDA and SCL, synthesis was re-run after saving constraints, and implementation used the updated constraints
2. Wiring incorrect → confirm SDA→W11 (JB1) and SCL→W12 (JB2) with jumper cables
3. Wrong I2C address → confirm JP1 and JP2 on Pmod TMP2 are both open (default) → address should be 0x4B
4. Power issue → measure 3.3V between JB VCC and GND with multimeter before connecting Pmod
5. Using W8/W10 instead of W11/W12 → known open issue; switch to jumper cables on W11/W12

**XIicPs send/recv returns failure (`-999.0f`)**
1. Bus stuck low — pull-ups missing or ineffective
2. I2C address mismatch — sensor not ACKing
3. Wiring open or intermittent — reseat jumper cables

**Bus busy timeout (`-997.0f`)**
1. Previous transaction didn't complete — sensor may be in bad state; power cycle the board

**Temperature reads correctly but value seems wrong**
1. Negative temperature path — not validated; see [bugs_and_fixes.md](bugs_and_fixes.md) Bug #4
2. JP1/JP2 configuration changed — recheck I2C address matches `ADT7420_I2C_ADDR` in header

---

## Vivado

**`[Common 17-180] Spawn failed: The operation completed successfully`**
- Known Vivado 2025 Windows bug; synthesis didn't actually launch
- Fix: Use Tcl console:
  ```tcl
  reset_run synth_1
  launch_runs synth_1 -jobs 2
  ```

**IO Ports shows 6 scalar ports (`I2C0_SCL_I_0`, etc.) instead of 2 bidirectional ports**
- The 6 individual tri-state scalar ports were made external instead of the IIC_0 interface bus
- Fix: Return to block design, delete the 6 external ports, right-click the `IIC_0` interface port → **Make External**, rename from `IIC_0_0` to `IIC_0`, re-validate, re-run synthesis

**Constraints not applying after save**
- Synthesis is out-of-date; constraints saved after the last synthesis run are not reflected
- Fix: Re-run synthesis after saving the XDC, then re-run implementation

**Wrapper reverts to auto-generated version**
- Only relevant if using the old manual wrapper approach
- Fix: Vivado may regenerate the wrapper on design changes; keep a backup copy outside the project directory

---

## Vitis

**Application builds but doesn't behave as expected after hardware change**
1. Workspace was reused with a new `.xsa` → known issue; create a new workspace directory
2. Platform was not rebuilt after `.xsa` change → build platform before building application

**`XPAR_XIICPS_0_BASEADDR` undefined**
- Platform was not built, or build failed silently
- Fix: Build Platform first, check build output for errors, then build application

**No UART output in Tera Term**
1. Wrong COM port selected
2. Baud rate not 115200
3. Application not running — confirm bitstream was programmed and application was launched via Run/Debug

**"I2C init failed" printed, application exits**
- `XIicPs_LookupConfig` returned NULL — base address doesn't match hardware
- Verify `XPAR_XIICPS_0_BASEADDR` exists in `xparameters.h` (open it from the platform build output)
- Confirm I2C0 was enabled via EMIO in the block design and hardware was re-exported

---

## General / Nuclear Options

**Nothing works, unclear where the problem is**
1. Power cycle the board
2. Reprogram the bitstream
3. Create a new Vitis workspace from the current `.xsa`
4. Re-export hardware from Vivado (regenerate bitstream) and start a fresh workspace

**Complete reset**
- New Vivado project → new block design → new `.xsa` → new Vitis workspace
- Faster than it sounds; the block design takes ~15 minutes from scratch
