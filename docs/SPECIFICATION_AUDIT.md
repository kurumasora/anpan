# Specification Audit and Correction Report

## 1. Audit scope

The corrected project was reviewed against all material supplied on 25 July
2026:

1. `要求仕様 (1).pdf`
2. `実習成果発表_実施要項.pdf`
3. `あんぱん製造機_要求仕様 (1).pdf`
4. `ハードウエアシミュレータIF関数仕様.pdf`
5. `ベルトコンベアのプラントモデルと離散化.pdf`
6. `OneDrive_1_2026-7-25.zip`

All PDF pages were text-extracted and visually rendered. Every source and header
in the ZIP was compared with the project copy.

## 2. Findings by document

### 2.1 `要求仕様 (1).pdf`

Applicable requirements:

- Maximum 12 dough pieces per stocker load.
- Required coordinates: dough `3.0`, oven 1 `2.0`, oven 2 `1.0`, stocker `0.5`.
- Transfers require arrival within the specified coordinate tolerance.
- A coordinate below `0.0` or at least `3.5` destroys the conveyor.
- The conveyor may carry only one dough or one anpan.
- Both ovens produce equal quality and may return underbaked bread.
- Underbaked bread must be reheated at low heat.
- The finished-anpan stocker has unlimited capacity.
- The application should prioritize speed, quality, safety, and scalability.

Result: implemented. Conveyor content is an explicit application state, unsafe
targets are rejected, both ovens are scheduled concurrently, and underbaking is
handled until the reheat result is successful.

### 2.2 `実習成果発表_実施要項.pdf`

This document mainly defines deliverables and evaluation, rather than runtime
behavior. It requires quantitative presentation of:

- development scale and KLOC;
- documentation quantity and pages/KLOC;
- source steps/lines;
- review count and reviews/KLOC;
- test count and tests/KLOC;
- bug count and bugs/KLOC; and
- improvement, quality, performance, sustainability, and reflection.

The corrected project currently contains 1,032 physical lines across its `.c`
and `.h` files, approximately `1.032 KLOC` using that simple counting method.
The group should agree on one counting rule and use it consistently for the
final presentation. Review minutes, executed test results, and presentation
slides remain group deliverables and cannot be inferred from source code alone.

The newer implementation guide gives deadlines different from the older
requirement slides. The group should use the newer guide:

- development documents and program: 28 July 2026, 23:59;
- PowerPoint: 29 July 2026, 17:00;
- presentation: 30 July 2026; and
- individual Word report: 6 August 2026, 23:59.

### 2.3 `あんぱん製造機_要求仕様 (1).pdf`

This is a visually different copy of the same six-page requirement deck. It
also states explicitly that hardware design and manufacturing changes are not
allowed. The corrected design keeps the supplied simulator `.c` files exact and
places production behavior in `controller.c`.

### 2.4 `ハードウエアシミュレータIF関数仕様.pdf`

API rules verified:

| API | Required handling |
|---|---|
| `InitConveyor()` | Call before movement; only success result is defined |
| `GetPosConveyor()` | Treat `-666.0` as an error/unsafe position |
| `StartConveyor(target)` | Check for `-666.0`; otherwise returned value is current position |
| `InitDough()` | Treat each call as an AGV refill with a variable quantity |
| `GetDoughCount()` | Use returned quantity as the current batch size |
| `GetOneDough()` | Continue only after return `0` |
| `SetDough1/2()` | Continue only after return `0` |
| `StartBake1/2(0)` | Start normal baking |
| `WaitBake1/2()` | `0` ready, `10` underbaked, `666` error |
| `StartBake1/2(1)` | Reheat; after `0`, bread may be removed immediately |
| `SetBread1/2()` | Transfer only after ready result |
| `ThrowBread()` | Store only when return is `0` |

The API specification explicitly describes `InitDough()` as AGV delivery. The
corrected program requests as many refills as necessary to reach an exact
production target, or operates continuously when target zero is selected.

### 2.5 `ベルトコンベアのプラントモデルと離散化.pdf`

The supplied `conveyor.c` was checked against the discrete model values:

| Parameter | Specification | Supplied source |
|---|---:|---:|
| `a0` | 0.07859368981489481 | exact |
| `b1` | -1.717726878071494 | exact |
| `b2` | 0.7963205678863887 | exact |
| Sampling time `T` | 100 ms | exact |

The application does not alter the plant model. `move_conveyor()` divides long
commands into bounded waypoints and validates each requested waypoint before
calling the uncontrolled target API.

### 2.6 `OneDrive_1_2026-7-25.zip`

The ZIP is treated as the authoritative simulator package. The following files
in the corrected project remain exact matches:

- `conveyor.c`, `conveyor.h`
- `breadstocker.h`, `dough.h`
- `oven1.c`, `oven1.h`
- `oven2.c`, `oven2.h`

The audit found these supplied defects:

- `oven1.c` uses `KLIN1_MAX` and `KLIN1_MIN`;
- `breadmachine.h` defines `OVEN1_MAX` and `OVEN1_MIN` instead.
- `breadstocker.c` places a semicolon between `ThrowBread` and its body.
- `breadstocker.c` has a missing closing parenthesis in its position check.
- `dough.c` has a missing closing parenthesis in its position check.
- `dough.c` calls `time()` without including `<time.h>`.
- `dough.c` returns `-666` on error, while the I/F specifies `666`.

The project applies minimal corrections: two constant aliases, the required
syntax fixes, the missing standard header, and the API-specified positive error
value. No plant coefficient, delay, coordinate, or state transition is
redesigned.

## 3. Corrections applied

1. Restored the simulator package from the authoritative ZIP.
2. Corrected the supplied compile/API defects listed in section 2.6.
3. Added target-based AGV refill processing:
   - no argument: exactly 12 anpan;
   - positive argument: exactly the requested number of anpan;
   - zero: continuous production.
4. Retained a single conveyor owner and explicit empty/dough/bread state.
5. Retained concurrent blocking waits for the two independent ovens.
6. Timestamp oven completion and immediately service the earliest-ready oven.
7. Complete unload-store-refill for that oven before servicing the next ready
   oven, minimizing oven idle time.
8. Added thread cleanup on production failure.
9. Updated the project documentation to match the complete API specification.

## 4. Static compliance result

| Area | Result |
|---|---|
| Coordinates and safety boundaries | Pass |
| One item on conveyor | Pass by explicit state checks |
| Use of two ovens | Pass |
| Concurrent simulated baking | Pass by one worker per active oven |
| Underbaked reheating | Pass |
| Immediate removal after successful reheat | Pass |
| Variable AGV refill quantity | Pass |
| Exact target and continuous operation | Pass |
| Plant-model coefficients | Pass; authoritative source preserved |
| Supplied simulator behavior | Pass after minimal compile/API corrections |
| Build recipe includes all modules and threads | Pass by Makefile inspection |

## 5. Required dynamic tests

These tests should be executed in the course's POSIX/GCC environment:

| ID | Test | Expected result |
|---|---|---|
| T01 | Build with `make clean && make` | No errors; review all warnings |
| T02 | Run `./anpan` | Exactly 12 anpan are stored |
| T03 | Run `./anpan 2` | Exactly two anpan are stored |
| T04 | Run `./anpan 0` and interrupt after observation | Continuous refill operation; no conveyor conflict |
| T05 | Observe initial two dough pieces | Oven waits overlap rather than add sequentially |
| T06 | Observe every transfer | Conveyor carries at most one item |
| T07 | Trigger/observe return `10` from `WaitBake` | Low-heat reheat is called; bread is removed after result `0` |
| T08 | Repeat runs | Dough quantities vary and every reported dough is accounted for |
| T09 | Use invalid command arguments | Usage message and exit status `2` |
| T10 | Compare produced count with stored messages | Counts match with no silent loss |

## 6. Verification limitation

Static requirements, source identity, state logic, coefficients, interfaces,
and build inputs were verified on this computer. Dynamic compilation and
execution could not be completed here because no GCC, Clang, MSVC, Make, or WSL
toolchain is installed. Therefore T01-T10 must be recorded as planned tests
until they are executed in the course environment or after WSL/GCC is installed.
