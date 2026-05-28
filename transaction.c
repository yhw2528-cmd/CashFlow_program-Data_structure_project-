#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "transaction.h"
#include "stack.h"

Transaction* head    = NULL;
Transaction* tail    = NULL;
int          next_id = 1;

/* 금액을 쉼표 포함 문자열로 변환 (+500,000원 / -12,000원) */
static void format_amount(char* buf, size_t size, int amount) {
    char sign    = (amount >= 0) ? '+' : '-';
    int  abs_val = (amount < 0) ? -amount : amount;

    if (abs_val < 1000)
        snprintf(buf, size, "%c%d\xEC\x9B\x90", sign, abs_val);
    else if (abs_val < 1000000)
        snprintf(buf, size, "%c%d,%03d\xEC\x9B\x90", sign, abs_val / 1000, abs_val % 1000);
    else
        snprintf(buf, size, "%c%d,%03d,%03d\xEC\x9B\x90", sign,
                 abs_val / 1000000, (abs_val % 1000000) / 1000, abs_val % 1000);
}

void init_list(void) {
    head    = NULL;
    tail    = NULL;
    next_id = 1;
}

Transaction* create_node(const char* date, int amount, const char* category, const char* memo) {
    Transaction* node = (Transaction*)malloc(sizeof(Transaction));
    if (node == NULL) return NULL;

    node->id = next_id++;
    strncpy(node->date,     date,     11); node->date[11]     = '\0';
    node->amount = amount;
    strncpy(node->category, category, 19); node->category[19] = '\0';
    strncpy(node->memo,     memo,     49); node->memo[49]     = '\0';
    node->prev = NULL;
    node->next = NULL;
    return node;
}

void insert_node(Transaction* node) {
    if (head == NULL) {
        head = tail = node;
    } else {
        tail->next = node;
        node->prev = tail;
        tail = node;
    }
}

/* 성공 시 1, 해당 id 없으면 0 반환 */
int delete_node(int id, UndoStack* stk) {
    Transaction* node = find_node(id);
    if (node == NULL) {
        printf("해당 번호의 거래가 없습니다.\n");
        return 0;
    }

    if (node->prev != NULL)
        node->prev->next = node->next;
    else
        head = node->next;

    if (node->next != NULL)
        node->next->prev = node->prev;
    else
        tail = node->prev;

    push_undo(stk, node);
    return 1;
}

void modify_node(int id) {
    Transaction* node = find_node(id);
    if (node == NULL) {
        printf("해당 번호의 거래가 없습니다.\n");
        return;
    }

    char confirm;
    printf("정말 수정하시겠습니까? (수정은 undo로 복구 불가) (y/n): ");
    scanf(" %c", &confirm);
    getchar();
    if (confirm != 'y' && confirm != 'Y') {
        printf("수정이 취소되었습니다.\n");
        return;
    }

    printf("수정할 항목을 선택하세요:\n");
    printf("  1) 금액\n  2) 카테고리\n  3) 메모\n  4) 날짜\n선택: ");
    int choice;
    scanf("%d", &choice);
    getchar();

    const char* cats[] = {"식비", "교통", "쇼핑", "기타", "수입"};

    switch (choice) {
        /* [수정 1] 금액 수정 — add_transaction과 동일하게 수입/지출 먼저 선택 후 양수 입력 */
        case 1: {
            int type, amt;
            printf("  1) 수입  2) 지출\n선택: ");
            if (scanf("%d", &type) != 1 || (type != 1 && type != 2)) {
                getchar();
                printf("잘못된 선택입니다.\n");
                return;
            }
            getchar();
            printf("금액 입력 (양수로 입력): ");
            if (scanf("%d", &amt) != 1 || amt <= 0) {
                getchar();
                printf("0원은 입력할 수 없습니다.\n");
                return;
            }
            getchar();
            node->amount = (type == 2) ? -amt : amt;
            break;
        }
        case 2: {
            printf("카테고리 선택:\n  1) 식비  2) 교통  3) 쇼핑  4) 기타  5) 수입\n선택: ");
            int c;
            scanf("%d", &c);
            getchar();
            if (c < 1 || c > 5) { printf("잘못된 선택입니다.\n"); return; }
            strncpy(node->category, cats[c - 1], 19);
            break;
        }
        case 3:
            printf("새 메모 입력: ");
            fgets(node->memo, 50, stdin);
            node->memo[strcspn(node->memo, "\n")] = '\0';
            break;
        case 4: {
            char d[12];
            printf("새 날짜 입력 (YYYY-MM-DD): ");
            scanf("%11s", d);
            getchar();
            if (strlen(d) != 10 || d[4] != '-' || d[7] != '-') {
                printf("날짜 형식이 올바르지 않습니다 (YYYY-MM-DD).\n");
                return;
            }
            strncpy(node->date, d, 11);
            break;
        }
        default:
            printf("잘못된 선택입니다.\n");
            return;
    }
    printf("수정이 완료되었습니다.\n");
}

void print_all(void) {
    if (head == NULL) {
        printf("거래 내역이 없습니다.\n");
        return;
    }

    printf("+------+------------+----------------+----------+----------------------+\n");
    printf("| NO   |    날짜    |      금액      | 카테고리 |         메모         |\n");
    printf("+------+------------+----------------+----------+----------------------+\n");

    for (Transaction* t = head; t != NULL; t = t->next) {
        char amt[20];
        format_amount(amt, sizeof(amt), t->amount);
        printf("| %4d | %-10s | %14s | %-8s | %-20s |\n",
               t->id, t->date, amt, t->category, t->memo);
    }

    printf("+------+------------+----------------+----------+----------------------+\n");
}

Transaction* find_node(int id) {
    for (Transaction* t = head; t != NULL; t = t->next)
        if (t->id == id) return t;
    return NULL;
}
