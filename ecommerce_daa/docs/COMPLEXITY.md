# Complexity report
Let V=locations, E=roads, N=orders, C=capacity.

| Algorithm | Time | Auxiliary memory | Correctness / quality |
|---|---|---|---|
| Dijkstra (matrix) | O(V^2) | O(V) | Exact, nonnegative weights |
| Bellman-Ford (matrix scan) | O(V^3) worst case | O(V) | Exact, nonnegative graph used here |
| Merge Sort | O(N log N) | O(N) | Exact sorting |
| Quick Sort (middle pivot) | O(N log N) average, O(N^2) worst | O(log N) average stack, O(N) worst | Exact sorting |
| Greedy selection (repeated scan) | O(N^2) worst | O(N) | Heuristic; may be suboptimal |
| 0/1 Knapsack | O(NC) | O(C) | Optimal for independent items |

Shared network storage is O(V^2). Reported memory values are simplified estimates of working arrays, excluding shared graph/orders and allocator/runtime overhead. Timings are single runs, not statistically robust microbenchmarks.
