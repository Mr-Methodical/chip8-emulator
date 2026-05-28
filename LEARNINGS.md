# CHIP-8 Emulator Debugging Learnings

## Issue Summary
Screen appearing but then becoming "very bad" approximately 1 second after starting Tetris or Pong. The main branch lacked diagnostic logging, making it difficult to identify where execution was failing.

## Root Cause Analysis

### What We Added (array-fix branch with diagnostics)
1. **Cycle-level timing and logging** - Tracked elapsed milliseconds and cycle counts
2. **PC and Opcode logging** - Logged every instruction fetch and execution
3. **Stack operation logging** - Tracked CALL (0x2nnn) and RET (0x00EE) with SP values
4. **Memory access logging** - Logged sprite drawing (0xDxyn) with positions, heights, and memory addresses
5. **Timer state logging** - Logged delay timer (DT) and sound timer (ST) values
6. **Bounds checking** - Added guards for memory access, stack operations, and PC bounds

### Key Findings
- **Initial execution was correct**: The diagnostics showed proper instruction fetching, stack operations, and timer management
- **Delay timer was working**: We could see DT counting down from 0x5F (95) to 0x00, confirming the timer logic was sound
- **Sprite drawing was occurring**: The DRAW operations (0xDxyn) were being executed with correct positions and memory addresses
- **PC management was accurate**: Jumps, calls, and returns were all modifying PC correctly

### Likely Primary Issues in Main Branch

#### 1. **Lack of Visibility**
- **Problem**: No diagnostic logging made it impossible to see:
  - Whether opcodes were being fetched correctly
  - Whether timers were updating properly
  - Where execution flow was going
  - If memory operations were accessing valid addresses
- **Impact**: When the screen "went bad," there was no way to determine if it was:
  - A rendering/platform issue
  - An infinite loop or crash
  - Memory corruption
  - A logic error in opcode execution

#### 2. **Potential Memory/Array Issues**
- **From "array-fix" branch name**: Suggests previous array indexing bugs
- **Risk areas**:
  - Register array bounds (16 registers, indices 0-15, especially VF at 0xF)
  - Video array bounds (64x32 = 2048 pixels)
  - Stack array bounds (16 levels)
  - Memory array bounds (4096 bytes)
  - Keypad array bounds (16 keys)
- **Without bounds checking**: Invalid memory writes could corrupt adjacent data structures

#### 3. **Timer Update Location**
- **In array-fix**: Timers are decremented in `Cycle()` function
- **Critical**: The timing logic in main.cpp has:
  ```
  if (dt > cycleDelay) {
    lastCycleTime = currentTime;
    chip8.Cycle();  // This decrements timers
  }
  ```
- **Potential issue**: If timers are ALSO decremented elsewhere, double-decrement could cause incorrect behavior

#### 4. **Platform/Rendering Sync**
- **Issue**: The main loop handles three separate concerns:
  - Input processing (continuous)
  - CPU cycles (when dt > cycleDelay)
  - Screen rendering (every 16.67ms for 60Hz)
- **Potential problem**: If the display buffer isn't properly synchronized with CPU cycles, you could see:
  - Partial/corrupted screen updates
  - Timing issues where rendering happens before/after sprite drawing completes

#### 5. **Display Buffer Corruption**
- **Sprite XOR operation**: `*screen_pixel ^= 0xFFFFFFFF;`
- **Risk**: If `video` array wasn't initialized properly or bounds weren't checked, writes to wrong memory could corrupt game state
- **Symptom**: "Very bad" display that appears then degrades matches screen buffer getting overwritten

## Recommendations for Investigation

1. **In main branch, look for**:
   - Array indexing without bounds checks
   - Missing or duplicate timer decrements
   - Uninitialized memory or video buffer
   - Race conditions between CPU cycles and rendering
   - Whether registers, stack, or video arrays have access violations

2. **Should verify**:
   - Stack pointer initialization and bounds
   - Register initialization (especially VF)
   - Video buffer initialization (all pixels set to 0)
   - Memory bounds in all read operations

3. **Do NOT fix**: Just identify where the same patterns appear in main branch

## Diagnostic Tools Added
- Cycle counter and elapsed time tracking
- Per-instruction logging with register state
- Memory operation logging with bounds info
- Stack operation logging
- Bounds checking guards (informational only)
