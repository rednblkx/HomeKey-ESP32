# PN7160 connection-fault regression

Run `python3 tests/pn7160_faults/run.py` with Python 3 and a C++17 compiler
supporting AddressSanitizer and UndefinedBehaviorSanitizer.

The runner compiles the actual `pollForTag`, `isTagStillPresent`, `releaseTag`
and `healthCheck` method bodies from `main/Pn7160Reader.cpp` into a host harness.
The harness supplies a scripted NCI engine, simplified packet/event types,
FreeRTOS tick values and logging stubs. It requires no SDK, external packages,
submodules or physical reader.

Checks cover a valid activation, harmless idle timeouts, uninitialized NCI,
polling errors, asynchronous tag removal and a failed presence-probe write
while the engine still caches `tag_in_field() == true`. After a fault, presence
and health must return false immediately and release/poll operations must not
send more work to the broken connection. Resetting the simulated initialized
connection state restores the normal probe/release path.

These tests validate the connection-state decisions in production method bodies.
They do not compile the complete SDK reader class or test background-worker
concurrency, firmware initialization, RF or physical SPI behavior.
