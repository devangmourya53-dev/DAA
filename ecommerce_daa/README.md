# E-Commerce Order Fulfilment and Delivery Optimizer

Portable C11 console application, reusable algorithm interfaces, deterministic scenarios, tests, benchmark reports, and project documentation.

## Build
`make` (GCC/Make) or `gcc -std=c11 -O2 -Wall -Wextra -Iinclude src/*.c tests/tests.c -o ecommerce`

## Run
`./ecommerce` (Windows: `ecommerce.exe`)
`make test` / `./ecommerce --test`
`make benchmark` / `./ecommerce --benchmark`

## Project structure
- `include/optimizer.h`: reusable API
- `src/algorithms.c`: algorithms
- `src/scenarios.c`: deterministic datasets
- `src/report.c`: comparative report and benchmarks
- `src/main.c`: console
- `tests/tests.c`: automated tests and exact small-instance oracle
- `docs/`: user, design, complexity and demonstration guides
- `data/`: sample scenario descriptions

## Git
`git init && git add . && git commit -m "Initial DAA project" && git tag v1.0.0`

**Scope:** Independent single-source shortest paths and single-vehicle 0/1 capacity selection, not full multi-stop vehicle-routing optimization. Time values depend on hardware; memory values are algorithmic estimates, not measured peak RSS. Syllabus-wide completeness requires the syllabus algorithm list.

## Detailed test report (menu option 9)
Choose `9 Run tests` or run `./ecommerce --test` to display 24 named tests with inputs, expected results, actual results, PASS/FAIL status, and a final summary. Only `tests/tests.c` was changed for this improvement; the optimization algorithms and menu logic remain unchanged. Boolean tests display 1 for true and 0 for false.

## Detailed demonstration output (updated)
Menu options 3-8 now show input orders, per-location shortest-path comparisons,
priority-sorted order IDs, selected order IDs and capacity use, followed by the
original algorithm time/auxiliary-memory/quality comparison. The optimization
algorithms and automated test suite remain unchanged. The knapsack selection
reconstruction is for display only and is not included in the measured
algorithm time or auxiliary-memory estimate. For large datasets the display
limits order IDs to keep output readable.
