# State transition table

Experimental prototype. Not certified, no real-time guarantee.

## States

| State | Meaning | Motor command |
|---|---|---|
| MOVE | Obstacle far away | MOVE |
| SLOW | Obstacle near | SLOW |
| STOP | Obstacle very close | STOP |
| SAFE_MODE | Something is wrong; movement blocked until recovery | STOP (always) |

## Events

| Event | When it happens | Parameter |
|---|---|---|
| VALID_FAR | Valid reading, distance above `slow_cm` | `slow_cm` (default 70) |
| VALID_MID | Valid reading, distance above `stop_cm` and up to `slow_cm` | `stop_cm` (default 30) |
| VALID_NEAR | Valid reading, distance up to `stop_cm` | `stop_cm` |
| INVALID | Reading is negative, out of range, empty or not a number | valid range 2 to 400 cm |
| TIMEOUT | No valid reading for longer than the timeout | 200 ms |
| RESET | Explicit reset order received | none |
| MISS_LIMIT | K consecutive deadline misses | K = 3 |

## Parameters

| Name | Default | Used by |
|---|---|---|
| `stop_cm` | 30 | VALID_NEAR / VALID_MID boundary |
| `slow_cm` | 70 | VALID_MID / VALID_FAR boundary |
| `margin_cm` | 5 | Hysteresis (rule 3) |
| `N` | 5 | Valid readings needed to leave SAFE_MODE (rule 4) |
| `K` | 3 | Deadline misses in a row that trigger MISS_LIMIT |
| timeout | 200 ms | TIMEOUT |
| valid range | 2 to 400 cm | INVALID |

## Transition table

Every state and event pair is defined (4 states x 7 events = 28 cells).

| State \ Event | VALID_FAR | VALID_MID | VALID_NEAR | INVALID | TIMEOUT | RESET | MISS_LIMIT |
|---|---|---|---|---|---|---|---|
| MOVE | MOVE | SLOW | STOP | MOVE (log, reading ignored) | SAFE_MODE | MOVE (log, ignored) | SAFE_MODE |
| SLOW | MOVE | SLOW | STOP | SLOW (log, reading ignored) | SAFE_MODE | SLOW (log, ignored) | SAFE_MODE |
| STOP | MOVE | SLOW | STOP | STOP (log, reading ignored) | SAFE_MODE | STOP (log, ignored) | SAFE_MODE |
| SAFE_MODE | SAFE_MODE (count + 1) | SAFE_MODE (count + 1) | SAFE_MODE (count + 1) | SAFE_MODE (count = 0) | SAFE_MODE (count = 0) | SAFE_MODE (reset flag set) | SAFE_MODE |

The VALID_* cells of the MOVE, SLOW and STOP rows show the result with the plain thresholds. Rule 3 (hysteresis) is applied on top. In the SAFE_MODE row, rule 4 says when the state is left.

## Rules

1. **Start state.** The controller always begins in SAFE_MODE, also after a restart by HAM. The counter is 0 and the reset flag is not set.
2. **Invalid readings are never used to decide.** An INVALID event keeps the current state and is logged. Because an invalid reading does not refresh the "last valid reading" time, a long run of invalid readings ends in TIMEOUT and then SAFE_MODE.
3. **Hysteresis.** To avoid flapping near a threshold, relaxing needs a margin. When the plain result is less restrictive than the current state (STOP to SLOW, STOP to MOVE, SLOW to MOVE), the decision is taken again with the thresholds raised by `margin_cm`: `stop_cm + margin_cm` (35) and `slow_cm + margin_cm` (75). Examples: in SLOW, a reading of 72 cm stays SLOW and only above 75 cm goes to MOVE. In STOP, 33 cm stays STOP, and 72 cm goes to SLOW (not MOVE). Entering a more restrictive state always uses the plain thresholds.
4. **SAFE_MODE recovery.** It keeps a counter of consecutive valid readings and a flag for RESET. It leaves SAFE_MODE only when the counter reaches `N` and RESET was received, in any order. INVALID or TIMEOUT sets the counter back to 0 (the reset flag is kept). On entering and on leaving SAFE_MODE, the counter and the flag are cleared. When it leaves, the next state comes from the current distance with the plain thresholds (MOVE, SLOW or STOP).
5. **Undefined pair.** Any state and event pair not in the table is logged and leads to SAFE_MODE.
6. **Logging.** Every state change is logged with a timestamp, previous state, new state and cause.