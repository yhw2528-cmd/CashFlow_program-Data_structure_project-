#ifndef MONTH_QUEUE_H
#define MONTH_QUEUE_H

#define MAX_MONTHS 6

typedef struct {
    int year;
    int month;
    int income;
    int expense;
} MonthStat;

typedef struct {
    MonthStat data[MAX_MONTHS];
    int front;
    int rear;
    int size;
} MonthQueue;

void init_queue(MonthQueue* q);
int  is_queue_full(MonthQueue* q);
void enqueue_month(MonthQueue* q, MonthStat ms);
void build_month_queue(MonthQueue* q);
void print_trend_graph(MonthQueue* q);

#endif
