# Message format (simulator and controller)

Experimental prototype. Two processes talk with native QNX message passing: the simulator is the client (`name_open`, `MsgSend`) and the controller is the server (`name_attach`, `MsgReceive`, `MsgReply`).

## Rules for every message

- The first field is always `type` (16 bits). It must be greater than `_IO_MAX` (511), because lower values are reserved for QNX.
- The server receives into a buffer large enough for a message or a system pulse.
- One request gets exactly one reply. The reply repeats the request sequence.
- Both processes use the same header file for the structs, so the field order and padding are identical.

## Request (simulator to controller)

| Field | Type | Meaning |
|---|---|---|
| `type` | uint16 | Message type. 1000 = reading, 1001 = reset |
| `sequence` | uint32 | Starts at 1 and grows by 1 for every message |
| `timestamp_ms` | uint32 | Scenario time in milliseconds (not wall clock), so runs are reproducible |
| `raw` | char[16] | The sensor value as text, ending in 0. Empty or not a number is allowed on purpose |

The reading is sent as text so that the controller does the validation. A reset message ignores `raw`. The simulator cuts longer values to 15 characters plus the ending 0, and the controller never reads past the 16 bytes, even if the last byte is not 0.

## Reply (controller to simulator)

| Field | Type | Meaning |
|---|---|---|
| `sequence` | uint32 | Copy of the request sequence |
| `status` | uint16 | 0 = processed, 1 = unknown type, 2 = repeated or older sequence |
| `valid` | uint16 | 1 = reading was valid, 0 = rejected as INVALID (also 0 for a reset message) |
| `state` | uint16 | 0 MOVE, 1 SLOW, 2 STOP, 3 SAFE_MODE |
| `command` | uint16 | 0 MOVE, 1 SLOW, 2 STOP (STOP while in SAFE_MODE) |

## Order, loss and timing

- The controller expects `sequence` to grow by 1. A gap means a lost message: it is logged, not corrected, and the message is processed.
- A repeated or older `sequence` is logged and not processed. It still gets a reply, with `status` = 2 and the current state.
- Latency per message is measured on both sides (send to reply in the simulator, receive to decision in the controller) and saved to CSV.