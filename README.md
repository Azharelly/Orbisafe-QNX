# Orbisafe-QNX

Distance-based safety controller for robots on QNX 8.0. It decides
MOVE / SLOW / STOP from distance readings, rejects invalid input, and enters
SAFE MODE on timeout until a valid recovery.

> Status: work in progress (QNX Global Hackathon 2026).

## Features (planned)
- MOVE / SLOW / STOP decision with hysteresis around the thresholds
- Input validation (INVALID readings never drive a decision)
- Timeout detection and SAFE MODE with explicit recovery
- Scenario-based automated tests (CSV scenarios)
- QNX-specific parts: native message passing, slogger2 logging

## QNX connection
_TODO: list the files and functions that use QNX APIs, and the demo that shows them._

## Requirements
- QNX SDP 8.0 (non-commercial license)
- Host: Windows 11 x64, QNX Toolkit for VS Code
- Target: _TODO (QEMU or VM)_
- Toolchain: QNX GCC 12.2 via `qcc`

## Build and run
_TODO: exact steps, from a clean clone._

## Repository layout
- `src/` source code
- `tests/` automated tests
- `scenarios/` CSV test scenarios
- `tools/` helper scripts
- `docs/` design notes (transition table, message format)

## License
MIT