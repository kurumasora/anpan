# Bug Report

## Reference-package defects

### BUG-REF-001 - Invalid `ThrowBread` function definition

- Source: supplied `breadstocker.c`
- Problem: a semicolon terminates the function declaration immediately before
  the function body.
- Effect: compilation fails at the opening brace.
- Correction: removed the semicolon.
- Behavior change: none; the intended body becomes compilable.

### BUG-REF-002 - Missing parenthesis in bread-stocker position check

- Source: supplied `breadstocker.c`
- Problem: the `if` condition is not closed.
- Effect: compilation fails.
- Correction: added the missing closing parenthesis.
- Behavior change: none.

### BUG-REF-003 - Missing parenthesis in dough-stocker position check

- Source: supplied `dough.c`
- Problem: the `if` condition is not closed.
- Effect: compilation fails.
- Correction: added the missing closing parenthesis.
- Behavior change: none.

### BUG-REF-004 - Missing declaration for `time()`

- Source: supplied `dough.c`
- Problem: `time(NULL)` is used without including `<time.h>`.
- Effect: compiler diagnostic and nonportable implicit declaration behavior.
- Correction: included `<time.h>`.
- Behavior change: none.

### BUG-REF-005 - Dough error value contradicts I/F specification

- Source: supplied `dough.c`
- Problem: `GetOneDough()` initializes its error result to `-666`; the hardware
  I/F specification requires `666`.
- Effect: an application following the documented API could misclassify it.
- Correction: changed the error result to `666`.
- Behavior change: error reporting now conforms to the documented interface.

### BUG-REF-006 - Oven 1 coordinate constant names do not exist

- Sources: supplied `oven1.c` and `breadmachine.h`
- Problem: `oven1.c` uses `KLIN1_MAX/MIN`, but the shared header defines only
  `OVEN1_MAX/MIN`.
- Effect: compilation fails with undefined identifiers.
- Correction: added compatibility aliases in `breadmachine.h`.
- Behavior change: none; aliases use the supplied oven 1 coordinates.

### BUG-REF-007 - Position-sensitive transfers read noisy coordinates twice

- Sources: supplied `dough.c`, `breadstocker.c`, `oven1.c`, and `oven2.c`.
- Problem: a single range test calls `GetPosConveyor()` separately for its
  upper and lower comparisons. The simulator adds independent noise to every
  call.
- Effect: one physical conveyor position can be evaluated using two different
  measurements, causing intermittent dough and bread transfer rejection.
- Correction: capture one coordinate and use it for both range comparisons.
- Behavior change: the same specified coordinate limits are retained; each
  transfer decision now uses one coherent sensor sample.

## Earlier application defects corrected

### BUG-APP-001 - Oven waits executed sequentially

- Effect: two full simulator bake waits were added rather than overlapped.
- Correction: one worker thread waits for each active oven.

### BUG-APP-002 - Conveyor contents were implicit

- Effect: the one-item safety rule could not be directly checked.
- Correction: added explicit empty, dough, and bread states.

### BUG-APP-003 - AGV refill behavior was incomplete

- Effect: a previous revision processed one stocker initialization only.
- Correction: added configurable finite or continuous AGV refill processing.

### BUG-APP-004 - Ready-oven order did not prioritize throughput

- Effect: unloading every ready oven before refilling increased oven idle time.
- Correction: timestamp completion, service the earliest-ready oven, and
  immediately complete its unload-store-refill cycle.

### BUG-APP-005 - Batch limits could not guarantee exactly 12 anpan

- Effect: variable AGV quantities could produce fewer or more than the desired
  total across batch-count runs.
- Correction: use an exact production target and limit committed dough so stored
  plus in-process production never exceeds the target.

### BUG-APP-006 - Noisy position feedback caused repeated `666` failures

- Effect: the supplied simulator adds a new positive random error to every
  `GetPosConveyor()` call. Feeding those readings back into the move loop caused
  unnecessary moves, and only ten transfer attempts could still fail by chance.
- Correction: the controller now tracks positions that it successfully
  commanded, because `StartConveyor()` already blocks until motion completes.
  Position-sensitive transfers retry the unchanged operation up to 100 times
  without moving the conveyor again.
- Efficiency: failed sensor samples are handled immediately; the controller no
  longer spends extra time repeatedly commanding the same conveyor position.
- Compatibility: the supplied simulator behavior and transfer-coordinate
  limits are unchanged.
