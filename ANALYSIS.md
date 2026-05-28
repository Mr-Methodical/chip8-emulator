# Main Branch Issue Analysis

## CRITICAL ISSUE FOUND: Timer Frequency Mismatch

### The Problem
In **main.cpp** (line 27-33):
```cpp
if (dt > cycleDelay) {
  lastCycleTime = currentTime;
  chip8.Cycle();                    // <-- Timer decrement happens here
  platform.Update(chip8.video, videoPitch);
}
```

In **chip8.cpp Cycle()** (line 416-423):
```cpp
void Chip8::Cycle() {
  // ... fetch and execute ...
  if (delayTimer > 0) --delayTimer;    // <-- DECREMENT AT CYCLE SPEED
  if (soundTimer > 0) --soundTimer;
}
```

### Why This Is Wrong
- `cycleDelay` parameter = 2 (milliseconds)
- **Cycle() gets called every 2ms** → Timers decrement at **500Hz**
- **Games expect timers to decrement at 60Hz** (every ~16.67ms)
- **Timers count down ~8.3x TOO FAST**

### Game Impact
1. **Initial state (first ~1 second)**: Game seems fine while timers are counting down
2. **After delay timer expires**: Game enters next sequence
3. **Problem**: Timer expired at 500Hz instead of 60Hz, so game logic timing is completely wrong
4. **Result**: "Screen becomes very bad" as game state diverges from expectations

### Symptoms This Causes
- Games that use delay timers for animation/pacing will have timing completely off
- Collision detection or move logic might trigger at wrong times
- Visual glitches as sprites/game state update at incorrect intervals

---

## Secondary Issues Found

### Issue 2: Rendering Frequency
**In main.cpp** (line 31):
```cpp
if (dt > cycleDelay) {
  chip8.Cycle();
  platform.Update(chip8.video, videoPitch);  // <-- Called every CPU cycle, not 60Hz
}
```

**Problem**: Screen updates at CPU cycle speed (500Hz with cycleDelay=2), not 60Hz
- Wastes CPU cycles on excessive rendering
- Potential tearing/sync issues
- Should decouple rendering from CPU cycles

### Issue 3: Missing Bounds Checks

#### In OP_Fx55() (line 405):
```cpp
void Chip8::OP_Fx55() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  for (uint8_t i = 0; i <= Vx; ++i) {
    memory[index + i] = registers[i];  // <-- No check if index+i >= 4096
  }
}
```
**Risk**: If `index=4090` and `Vx=15`, writes to memory[4090..4105] → OUT OF BOUNDS

#### In OP_Fx65() (line 412):
```cpp
void Chip8::OP_Fx65() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  for (uint8_t i = 0; i <= Vx; ++i) {
    registers[i] = memory[index + i];  // <-- No check if index+i >= 4096
  }
}
```
**Risk**: Reading past end of memory array

#### In OP_Dxyn() (line 290-314):
```cpp
void Chip8::OP_Dxyn() {
  // ...
  for (uint8_t row = 0; row < height; ++row) {
    uint8_t sprite_byte = memory[index + row];  // <-- No check if index+row >= 4096
    // ...
  }
}
```
**Risk**: If `index=4094` and `height=10`, reads from memory[4094..4103] → OUT OF BOUNDS

### Issue 4: Stack Overflow/Underflow Protection
**In OP_2nnn() (line 142)**:
```cpp
void Chip8::OP_2nnn() {
  assert(sp < 16 && "stack overflow");  // <-- Only an assert (debug-only)
  stack[sp++] = pc;                     // <-- Proceeds anyway in release builds
}
```

**In OP_00EE() (line 132)**:
```cpp
void Chip8::OP_00EE() {
  assert(sp > 0 && "stack underflow");  // <-- Only an assert
  --sp;
  pc = stack[sp];
}
```
**Problem**: Asserts are disabled in Release builds. Real stack violations won't be caught.

### Issue 5: Keypad Bounds
**In OP_ExA1() (line 323)**:
```cpp
void Chip8::OP_ExA1() {
  uint8_t key = registers[Vx];
  if (!keypad[key]) pc += 2;  // <-- If key > 15, accesses invalid keypad index
}
```
**Risk**: If register value is > 15, accesses keypad[16+] → OUT OF BOUNDS

### Issue 6: PC Jump Bounds
**In OP_Bnnn() (line 268)**:
```cpp
void Chip8::OP_Bnnn() {
  uint16_t address = (opcode & 0x0FFFu);
  pc = registers[0] + address;  // <-- Could result in PC > 4095
}
```
**Risk**: If `registers[0] + address > 4095`, PC is out of valid memory range

---

## Summary

| Issue | Severity | Location | Impact |
|-------|----------|----------|--------|
| Timer decrements at 500Hz instead of 60Hz | **CRITICAL** | chip8.cpp Cycle(), main.cpp loop | Complete game timing failure after ~1sec |
| Rendering at CPU speed not 60Hz | High | main.cpp Update call | Visual glitches, waste |
| No bounds check in OP_Fx55/65 | High | chip8.cpp lines 405, 412 | Memory corruption |
| No bounds check in OP_Dxyn | High | chip8.cpp line 290 | Memory read overflow |
| No bounds check OP_Bnnn | Medium | chip8.cpp line 268 | PC out of bounds |
| Keypad array access unchecked | Medium | chip8.cpp line 323 | Array access violation |
| Stack protect is assert-only | Medium | chip8.cpp lines 132, 142 | Not caught in Release |

## Most Likely Root Cause
**The timer frequency issue (500Hz vs 60Hz)** is almost certainly why games work fine initially then become "very bad" after ~1 second. The games are checking delay timers to pace their logic, and the timers are expiring way too fast.
