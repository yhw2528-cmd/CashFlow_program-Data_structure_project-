#include <stdio.h>
#include <string.h>
#include "month_queue.h"
#include "transaction.h"

void init_queue(MonthQueue* q) {
    q->front = q->rear = q->size = 0;
}

int is_queue_full(MonthQueue* q) {
    return q->size == MAX_MONTHS;
}

void enqueue_month(MonthQueue* q, MonthStat ms) {
    if (is_queue_full(q))
        q->front = (q->front + 1) % MAX_MONTHS;   /* 가장 오래된 달 제거 */
    else
        q->size++;
    q->data[q->rear] = ms;
    q->rear = (q->rear + 1) % MAX_MONTHS;
}

void build_month_queue(MonthQueue* q) {
    int ys[256], ms_arr[256], inc[256], exp_arr[256];
    int count = 0;

    for (Transaction* t = head; t != NULL; t = t->next) {
        int y, m;
        sscanf(t->date, "%d-%d", &y, &m);

        int found = 0;
        for (int i = 0; i < count; i++) {
            if (ys[i] == y && ms_arr[i] == m) {
                if (t->amount > 0) inc[i]     += t->amount;
                else               exp_arr[i] += t->amount;
                found = 1;
                break;
            }
        }
        if (!found && count < 256) {
            ys[count]      = y;
            ms_arr[count]  = m;
            inc[count]     = (t->amount > 0) ? t->amount : 0;
            exp_arr[count] = (t->amount < 0) ? t->amount : 0;
            count++;
        }
    }

    /* 연도·월 오름차순 버블 정렬 */
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (ys[j] * 12 + ms_arr[j] > ys[j+1] * 12 + ms_arr[j+1]) {
                int tmp;
                tmp = ys[j];      ys[j]      = ys[j+1];      ys[j+1]      = tmp;
                tmp = ms_arr[j];  ms_arr[j]  = ms_arr[j+1];  ms_arr[j+1]  = tmp;
                tmp = inc[j];     inc[j]      = inc[j+1];     inc[j+1]     = tmp;
                tmp = exp_arr[j]; exp_arr[j] = exp_arr[j+1]; exp_arr[j+1] = tmp;
            }
        }
    }

    /* 최근 MAX_MONTHS 개월만 enqueue */
    int start = (count > MAX_MONTHS) ? count - MAX_MONTHS : 0;
    for (int i = start; i < count; i++) {
        MonthStat stat = {ys[i], ms_arr[i], inc[i], exp_arr[i]};
        enqueue_month(q, stat);
    }
}

void print_trend_graph(MonthQueue* q) {
    if (q->size == 0) {
        printf("거래 내역이 없습니다.\n");
        return;
    }

    /* 최대 지출 절댓값 계산 */
    int max_exp = 0;
    for (int i = 0; i < q->size; i++) {
        int idx     = (q->front + i) % MAX_MONTHS;
        int abs_exp = -(q->data[idx].expense);
        if (abs_exp > max_exp) max_exp = abs_exp;
    }

    printf("======== 최근 6개월 지출 추이 ========\n");
    for (int i = 0; i < q->size; i++) {
        int idx      = (q->front + i) % MAX_MONTHS;
        MonthStat* s = &q->data[idx];
        int abs_exp  = -(s->expense);
        int filled   = (max_exp > 0) ? (int)((double)abs_exp / max_exp * 20) : 0;
        int empty    = 20 - filled;

        printf("%04d-%02d |", s->year, s->month);
        for (int j = 0; j < filled; j++) printf("\xe2\x96\x88");   /* █ UTF-8 */
        for (int j = 0; j < empty;  j++) printf("\xe2\x96\x91");   /* ░ UTF-8 */
        printf("| %11d원", s->expense);
        if (abs_exp == max_exp && max_exp > 0) printf("  <- 최대");
        printf("\n");
    }
    printf("=======================================\n");
}
