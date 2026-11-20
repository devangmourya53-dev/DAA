#ifndef OPTIMIZER_H
#define OPTIMIZER_H
#include <stddef.h>
#define MAX_N 160
#define MAX_O 1200
#define INF 100000000
#define MAX_CAP 6000
typedef struct {int id, destination, weight, priority, value;} Order;
typedef struct {int n; int road[MAX_N][MAX_N];} Network;
typedef struct {Order orders[MAX_O]; int count, capacity; Network network;} Plan;
void network_init(Network *g,int n);
int road_add(Network *g,int a,int b,int cost);
int order_add(Plan *p,Order o);
void dijkstra(const Network *g,int source,int *dist);
void bellman_ford(const Network *g,int source,int *dist);
void merge_sort(Order *a,int n);
void quick_sort(Order *a,int n);
int greedy_value(const Order *a,int n,int cap);
int optimal_value(const Order *a,int n,int cap);
void scenario(Plan *p,int kind);
void report(const Plan *p);
void benchmark(void);
int run_tests(void);
#endif
