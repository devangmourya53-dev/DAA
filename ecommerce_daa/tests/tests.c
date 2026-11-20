#include "optimizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int passed, total;

static void check_int(const char *category, const char *name,
                      const char *input, int expected, int actual) {
    int ok = (expected == actual);
    ++total;
    if (ok) ++passed;
    printf("[%02d] %-5s | %-12s | %-37s | %s\n",
           total, ok ? "PASS" : "FAIL", category, name, input);
    printf("     Expected: %d | Actual: %d\n", expected, actual);
}

static void check_equal(const char *category, const char *name,
                        const char *input, int equal) {
    check_int(category, name, input, 1, equal);
}

int run_tests(void) {
    Plan *p = malloc(sizeof(*p));
    int a[MAX_N], b[MAX_N], i;
    Order tiny[] = {{1,1,10,1,60}, {2,1,20,2,100}, {3,1,30,3,120}};
    Order sorted[] = {{3,1,1,1,1}, {1,1,1,3,1}, {2,1,1,2,1}};
    Order sorted2[3];
    passed = total = 0;
    if (!p) { puts("ERROR: cannot allocate test plan"); return 0; }

    puts("\n============================================================");
    puts("  E-COMMERCE OPTIMIZER - DETAILED AUTOMATED TEST REPORT");
    puts("============================================================");
    puts("Each test shows input, expected result, actual result and status.");

    puts("\n--- ROUTING: UNIT TESTS + SMALL-INSTANCE ORACLES ---");
    scenario(p, 0);
    dijkstra(&p->network, 0, a);
    bellman_ford(&p->network, 0, b);
    check_int("Routing", "Warehouse distance", "source=0, destination=0", 0, a[0]);
    check_int("Routing", "Known shortest path to 1", "normal network, 0 -> 1", 2, a[1]);
    check_int("Routing", "Known shortest path to 5", "normal network, 0 -> 5", 7, a[5]);
    for (i = 0; i < p->network.n; ++i) {
        char name[48], input[60];
        snprintf(name, sizeof(name), "Dijkstra vs Bellman-Ford %d", i);
        snprintf(input, sizeof(input), "source=0, destination=%d", i);
        check_int("Comparison", name, input, a[i], b[i]);
    }

    puts("\n--- CAPACITY: EXACT ORACLE + EDGE/INVALID INPUTS ---");
    check_int("Oracle", "0/1 Knapsack optimum", "weights=10,20,30; values=60,100,120; cap=50", 220,
              optimal_value(tiny, 3, 50));
    check_int("Comparison", "Greedy counterexample", "same 3 orders, cap=50", 160,
              greedy_value(tiny, 3, 50));
    check_int("Edge", "Zero vehicle capacity", "cap=0", 0,
              optimal_value(tiny, 3, 0));
    check_int("Invalid", "Negative capacity", "cap=-1", -1,
              optimal_value(tiny, 3, -1));
    check_int("Invalid", "Excessive capacity", "cap=MAX_CAP+1", -1,
              optimal_value(tiny, 3, MAX_CAP + 1));

    puts("\n--- INPUT VALIDATION ---");
    check_int("Invalid", "Negative road endpoint", "road=(-1,2), cost=4", 0,
              road_add(&p->network, -1, 2, 4));
    check_int("Invalid", "Self-loop road", "road=(1,1), cost=4", 0,
              road_add(&p->network, 1, 1, 4));
    check_int("Invalid", "Negative road cost", "road=(1,2), cost=-4", 0,
              road_add(&p->network, 1, 2, -4));
    check_int("Invalid", "Duplicate order ID", "add existing order ID=101", 0,
              order_add(p, (Order){101,1,1,1,10}));
    check_int("Invalid", "Invalid destination", "order ID=999, destination=99", 0,
              order_add(p, (Order){999,99,1,1,10}));

    puts("\n--- SORTING: REGRESSION + CROSS-ALGORITHM CHECK ---");
    memcpy(sorted2, sorted, sizeof(sorted));
    merge_sort(sorted, 3);
    quick_sort(sorted2, 3);
    check_equal("Regression", "Merge Sort expected order", "IDs=3,1,2; priorities=1,3,2",
                sorted[0].id==1 && sorted[1].id==2 && sorted[2].id==3);
    check_equal("Comparison", "Merge Sort vs Quick Sort", "same 3-order dataset",
                memcmp(sorted, sorted2, sizeof(sorted)) == 0);

    puts("\n--- INTEGRATION: SCENARIO REGRESSION TESTS ---");
    scenario(p, 1);
    check_int("Integration", "Capacity shock", "scenario=1", 5, p->capacity);
    scenario(p, 2);
    check_equal("Integration", "Large stress dataset", "scenario=2; orders=700; locations=120",
                p->count == 700 && p->network.n == 120);
    scenario(p, 3);
    check_equal("Integration", "Adversarial dataset", "scenario=3; orders=80; locations=20",
                p->count == 80 && p->network.n == 20);

    printf("\n============================================================\n");
    printf("FINAL RESULT: %d/%d tests passed | %d failed\n",
           passed, total, total - passed);
    printf("OVERALL STATUS: %s\n", passed == total ? "PASS" : "FAIL");
    puts("============================================================");
    free(p);
    return passed == total;
}
