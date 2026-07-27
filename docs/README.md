# Improved Anpan Manufacturing Application

This version uses the hardware simulator package supplied in
`OneDrive_1_2026-7-25.zip` and adds a safer, concurrent application scheduler.
The simulator behavior is unchanged. Two compatibility aliases are added to
`breadmachine.h` because the supplied `oven1.c` refers to names missing from
the supplied header.

## Design

- `main.c` starts the application.
- `controller.c` schedules the conveyor and both ovens.
- Simulator behavior comes from the teacher's reference package. Minimal syntax
  and documented API corrections are listed in `BUG_REPORT.md`.
- Exactly one application state records the conveyor load: empty, dough, or
  finished bread.
- Only the controller thread operates the conveyor.
- One worker thread per active oven waits for baking and performs low-heat
  reheating when required.
- Oven 1 is loaded before oven 2 at startup.
- The earliest-ready oven completes its unload-store-refill cycle first.
- `InitDough()` is treated as an AGV refill. Additional refills are requested
  until the exact production target is reached.

## Build and run

The simulator uses POSIX functions, so build it on Linux, macOS, WSL, or the
course's Unix development environment:

```sh
make clean
make
./anpan 2
```

The first test above produces two anpan. After it finishes successfully, run
the required 12-anpan production test:

```sh
./anpan 12
```

`./anpan` also produces exactly 12 anpan. A positive argument selects a
different exact production target, while zero selects continuous operation:

```sh
./anpan 20
./anpan 0
```

Always run `make clean` and `make` after replacing or editing source files.
Otherwise `./anpan` can still be an older executable.

Clean generated files with:

```sh
make clean
```
