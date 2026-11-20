#include "optimizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static double elapsed(clock_t start) { return (double)(clock()-start)/CLOCKS_PER_SEC; }

static void print_orders(const Plan *p, int limit) {
    int i, shown = p->count < limit ? p->count : limit;
    puts("\n--- INPUT ORDERS ---");
    puts("ID     Destination  Weight  Priority  Value");
    for (i=0; i<shown; ++i) {
        const Order *o=&p->orders[i];
        printf("%-6d %-12d %-7d %-9d %d\n",o->id,o->destination,o->weight,o->priority,o->value);
    }
    if (shown<p->count) printf("... %d additional orders omitted from display\n",p->count-shown);
}

/* Display a reproducible selected order set for the two value algorithms.
   This does not modify the library's optimization functions. */
static void print_selection(const Plan *p, int greedy) {
    int *used=calloc((size_t)p->count+1,sizeof(int));
    int *dp=NULL, *take=NULL;
    int cap=p->capacity, total=0, weight=0, selected=0, i, j;
    if (!used) { puts("Selection display unavailable (memory)"); return; }
    if (greedy) {
        while (1) {
            int best=-1;
            for (i=0;i<p->count;i++) if (!used[i] && p->orders[i].weight<=cap) {
                if (best<0 || (long long)p->orders[i].value*p->orders[best].weight >
                   (long long)p->orders[best].value*p->orders[i].weight ||
                   ((long long)p->orders[i].value*p->orders[best].weight ==
                   (long long)p->orders[best].value*p->orders[i].weight && p->orders[i].id<p->orders[best].id)) best=i;
            }
            if (best<0) break;
            used[best]=1; cap-=p->orders[best].weight;
        }
    } else {
        /* For display, reconstruct an exact solution with a 2D decision table.
           This uses additional memory only for reporting, not the benchmarked algorithm. */
        size_t width=(size_t)cap+1, rows=(size_t)p->count+1;
        if (rows > (size_t)-1 / width || rows*width > 9000000) {
            puts("Selected IDs omitted for large dataset (report memory limit)");free(used);return;
        }
        dp=calloc(rows*width,sizeof(int));
        take=calloc(rows*width,sizeof(int));
        if (!dp||!take) {puts("Selection display unavailable (memory)");free(dp);free(take);free(used);return;}
        for (i=1;i<=p->count;i++) for (j=0;j<=cap;j++) {
            int w=p->orders[i-1].weight, v=p->orders[i-1].value;
            dp[(size_t)i*width+j]=dp[(size_t)(i-1)*width+j];
            if (w<=j && dp[(size_t)(i-1)*width+j-w]+v > dp[(size_t)i*width+j]) {
                dp[(size_t)i*width+j]=dp[(size_t)(i-1)*width+j-w]+v;
                take[(size_t)i*width+j]=1;
            }
        }
        j=cap;
        for (i=p->count;i>=1;i--) if (take[(size_t)i*width+j]) {
            used[i-1]=1; j-=p->orders[i-1].weight;
        }
    }
    printf("%s selected order IDs: ",greedy?"Greedy":"Knapsack");
    for (i=0;i<p->count;i++) if (used[i]) {
        if (selected<25) printf("%d ",p->orders[i].id);
        selected++;weight+=p->orders[i].weight;total+=p->orders[i].value;
    }
    if (!selected) printf("(none)");
    if (selected>25) printf("... (%d more)",selected-25);
    printf("\n  Orders selected: %d | Weight: %d/%d | Value: %d\n",selected,weight,p->capacity,total);
    free(dp);free(take);free(used);
}

void report(const Plan *p) {
    int a[MAX_N],b[MAX_N],i,agree=1,sort_agree=1,g,opt;
    Order *x,*y;
    clock_t t;
    double td,tb,tm,tq,tg,to;
    size_t graph_mem=(size_t)p->network.n*p->network.n*sizeof(int);
    printf("\n========== ORDER-TO-DELIVERY REPORT ==========\n");
    printf("Locations=%d | Orders=%d | Vehicle capacity=%d\n",p->network.n,p->count,p->capacity);
    print_orders(p,p->count<=20?p->count:10);
    t=clock();dijkstra(&p->network,0,a);td=elapsed(t);
    t=clock();bellman_ford(&p->network,0,b);tb=elapsed(t);
    puts("\n--- SHORTEST PATHS FROM WAREHOUSE (LOCATION 0) ---");
    puts("Destination  Dijkstra  Bellman-Ford  Match");
    for(i=0;i<p->network.n;i++) {
        if(a[i]!=b[i])agree=0;
        if(p->network.n<=20 || i<10) {
            printf("%-12d ",i);
            if(a[i]>=INF) printf("%-9s ","INF");else printf("%-9d ",a[i]);
            if(b[i]>=INF) printf("%-13s ","INF");else printf("%-13d ",b[i]);
            puts(a[i]==b[i]?"YES":"NO");
        }
    }
    if(p->network.n>20) printf("... %d additional locations omitted; all checked\n",p->network.n-10);
    x=malloc(((size_t)p->count+1)*sizeof(Order));
    y=malloc(((size_t)p->count+1)*sizeof(Order));
    if(!x||!y){free(x);free(y);puts("Allocation failure");return;}
    memcpy(x,p->orders,(size_t)p->count*sizeof(Order));
    memcpy(y,p->orders,(size_t)p->count*sizeof(Order));
    t=clock();merge_sort(x,p->count);tm=elapsed(t);
    t=clock();quick_sort(y,p->count);tq=elapsed(t);
    for(i=0;i<p->count;i++)if(x[i].id!=y[i].id)sort_agree=0;
    puts("\n--- SORTED ORDERS (HIGH PRIORITY FIRST) ---");
    printf("Merge Sort IDs: ");
    for(i=0;i<p->count&&i<20;i++)printf("%d ",x[i].id);
    if(p->count>20)printf("... ");
    printf("\nQuick Sort IDs: ");
    for(i=0;i<p->count&&i<20;i++)printf("%d ",y[i].id);
    if(p->count>20)printf("... ");
    printf("\nSorting results match: %s\n",sort_agree?"YES":"NO");
    t=clock();g=greedy_value(p->orders,p->count,p->capacity);tg=elapsed(t);
    t=clock();opt=optimal_value(p->orders,p->count,p->capacity);to=elapsed(t);
    puts("\n--- VEHICLE CAPACITY OPTIMIZATION ---");
    print_selection(p,1);
    print_selection(p,0);
    printf("Greedy value=%d | Knapsack optimal value=%d | Greedy quality=%.2f%%\n",g,opt,opt>0?100.0*g/opt:100.0);
    puts("\n--- COMPARATIVE ALGORITHM REPORT ---");
    puts("Strategy          Time(s)       Extra memory estimate(bytes)    Quality");
    printf("Dijkstra          %-13.8f %-31zu exact\n",td,(size_t)p->network.n*(sizeof(int)*2));
    printf("Bellman-Ford      %-13.8f %-31zu exact\n",tb,(size_t)p->network.n*sizeof(int));
    printf("Merge Sort        %-13.8f %-31zu exact\n",tm,(size_t)p->count*sizeof(Order));
    printf("Quick Sort        %-13.8f %-31s exact\n",tq,"O(log n) typical stack");
    printf("Greedy Selection  %-13.8f %-31zu %.2f%% (%d)\n",tg,(size_t)p->count*sizeof(int),opt>0?100.0*g/opt:100.0,g);
    printf("0/1 Knapsack      %-13.8f %-31zu 100.00%% (%d)\n",to,((size_t)p->capacity+1)*sizeof(int),opt);
    printf("Common graph storage: %zu bytes | Route comparison: %s | Sorting comparison: %s\n",graph_mem,agree?"MATCH":"MISMATCH",sort_agree?"MATCH":"MISMATCH");
    puts("NOTE: Times are single-run measurements; 0.00000000 means below clock resolution.");
    puts("NOTE: Routes are warehouse-to-destination shortest paths, not a multi-stop tour.");
    puts("NOTE: Memory figures describe core algorithms, not reporting overhead or peak RAM.");
    free(x);free(y);
}
void benchmark(void){Plan *p=malloc(sizeof(Plan));int k;if(!p)return;for(k=0;k<4;k++){printf("\n===== %s =====\n",(const char*[]){"NORMAL / SMALL","CAPACITY SHOCK / SMALL","LARGE STRESS","ADVERSARIAL / MEDIUM"}[k]);scenario(p,k);report(p);}free(p);}
