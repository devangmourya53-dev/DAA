#include "optimizer.h"
#include <stdlib.h>
#include <string.h>
void network_init(Network *g,int n){int i,j;g->n=n;for(i=0;i<n;i++)for(j=0;j<n;j++)g->road[i][j]=(i==j)?0:INF;}
int road_add(Network *g,int a,int b,int cost){if(a<0||b<0||a>=g->n||b>=g->n||a==b||cost<0)return 0;g->road[a][b]=g->road[b][a]=cost;return 1;}
int order_add(Plan *p,Order o){int i;if(p->count>=MAX_O||o.id<=0||o.destination<0||o.destination>=p->network.n||o.weight<=0||o.value<0||o.priority<0)return 0;for(i=0;i<p->count;i++)if(p->orders[i].id==o.id)return 0;p->orders[p->count++]=o;return 1;}
void dijkstra(const Network *g,int s,int *d){int used[MAX_N]={0},i,j,u,best;for(i=0;i<g->n;i++)d[i]=INF;if(s<0||s>=g->n)return;d[s]=0;for(i=0;i<g->n;i++){u=-1;best=INF;for(j=0;j<g->n;j++)if(!used[j]&&d[j]<best){best=d[j];u=j;}if(u<0)break;used[u]=1;for(j=0;j<g->n;j++)if(g->road[u][j]<INF&&d[u]+g->road[u][j]<d[j])d[j]=d[u]+g->road[u][j];}}
void bellman_ford(const Network *g,int s,int *d){int i,j,k,changed;for(i=0;i<g->n;i++)d[i]=INF;if(s<0||s>=g->n)return;d[s]=0;for(k=1;k<g->n;k++){changed=0;for(i=0;i<g->n;i++)for(j=0;j<g->n;j++)if(d[i]<INF&&g->road[i][j]<INF&&d[i]+g->road[i][j]<d[j]){d[j]=d[i]+g->road[i][j];changed=1;}if(!changed)break;}}
static int before(Order a,Order b){return a.priority>b.priority||(a.priority==b.priority&&a.id<b.id);}
static void merge_rec(Order *a,Order *tmp,int lo,int hi){int mid,i,j,k;if(hi-lo<=1)return;mid=(lo+hi)/2;merge_rec(a,tmp,lo,mid);merge_rec(a,tmp,mid,hi);i=lo;j=mid;for(k=lo;k<hi;k++)tmp[k]=(j==hi||(i<mid&&before(a[i],a[j])))?a[i++]:a[j++];memcpy(a+lo,tmp+lo,(size_t)(hi-lo)*sizeof(Order));}
void merge_sort(Order *a,int n){Order *tmp;if(n<2)return;tmp=malloc((size_t)n*sizeof(Order));if(!tmp)return;merge_rec(a,tmp,0,n);free(tmp);}
static void qsort_rec(Order *a,int lo,int hi){int i,j;Order pivot,t;while(lo<hi){i=lo;j=hi;pivot=a[lo+(hi-lo)/2];while(i<=j){while(before(a[i],pivot))i++;while(before(pivot,a[j]))j--;if(i<=j){t=a[i];a[i]=a[j];a[j]=t;i++;j--;}}if(j-lo<hi-i){if(lo<j)qsort_rec(a,lo,j);lo=i;}else{if(i<hi)qsort_rec(a,i,hi);hi=j;}}}
void quick_sort(Order *a,int n){if(n>1)qsort_rec(a,0,n-1);}
int greedy_value(const Order *a,int n,int cap){int used[MAX_O]={0},best,i,total=0;long long lhs,rhs;if(n<0||n>MAX_O||cap<0)return -1;for(;;){best=-1;for(i=0;i<n;i++)if(!used[i]&&a[i].weight<=cap){if(best<0){best=i;continue;}lhs=(long long)a[i].value*a[best].weight;rhs=(long long)a[best].value*a[i].weight;if(lhs>rhs||(lhs==rhs&&a[i].id<a[best].id))best=i;}if(best<0)break;used[best]=1;cap-=a[best].weight;total+=a[best].value;}return total;}
int optimal_value(const Order *a,int n,int cap){int *dp,i,w,ans;if(n<0||n>MAX_O||cap<0||cap>MAX_CAP)return -1;dp=calloc((size_t)cap+1,sizeof(int));if(!dp)return -1;for(i=0;i<n;i++)for(w=cap;w>=a[i].weight;w--){int x=dp[w-a[i].weight]+a[i].value;if(x>dp[w])dp[w]=x;}ans=dp[cap];free(dp);return ans;}
