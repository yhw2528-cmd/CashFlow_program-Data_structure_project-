#include <stdio.h>
#include <string.h>
#include "stats.h"
#include "transaction.h"

void print_category_stats(void) {
    const char* cats[]   = {"식비", "교통", "쇼핑", "기타"};
    int totals[4]        = {0};
    int income_total     = 0;
    int expense_total    = 0;

    for (Transaction* t = head; t != NULL; t = t->next) {
        if (t->amount > 0) {
            income_total += t->amount;
        } else if (t->amount < 0) {
            expense_total += t->amount;
            for (int i = 0; i < 4; i++) {
                if (strcmp(t->category, cats[i]) == 0) {
                    totals[i] += t->amount;
                    break;
                }
            }
        }
    }

    printf("========== 카테고리별 통계 ==========\n");
    printf("[지출]\n");
    for (int i = 0; i < 4; i++) {
        double ratio = (expense_total != 0)
                       ? (double)totals[i] / expense_total * 100.0
                       : 0.0;
        printf("  %-6s: %8d원  (%.1f%%)\n", cats[i], totals[i], ratio);
    }
    printf("총 지출 : %d원\n", expense_total);
    printf("[수입]\n");
    printf("총 수입 : +%d원\n", income_total);
    printf("=====================================\n");
}

void print_monthly_summary(int year, int month) {
    int income = 0, expense = 0;

    for (Transaction* t = head; t != NULL; t = t->next) {
        int y, m;
        sscanf(t->date, "%d-%d", &y, &m);
        if (y == year && m == month) {
            if (t->amount > 0) income  += t->amount;
            else               expense += t->amount;
        }
    }

    printf("===== %d년 %d월 요약 =====\n", year, month);
    printf("  총 수입  : +%d원\n", income);
    printf("  총 지출  :  %d원\n", expense);
    printf("  잔  액   : %+d원\n", income + expense);
    printf("============================\n");
}

void print_balance(void) {
    int balance = 0;
    for (Transaction* t = head; t != NULL; t = t->next)
        balance += t->amount;
    printf("현재 잔액: %+d원\n", balance);
}
