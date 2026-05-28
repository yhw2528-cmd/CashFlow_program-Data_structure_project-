#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "transaction.h"
#include "stack.h"
#include "stats.h"
#include "file_io.h"

UndoStack undo_stk;

static void add_transaction(void) {
    char date[12];
    int  amount, type, cat_choice;
    char memo[50];
    const char* cats[] = {"식비", "교통", "쇼핑", "기타", "수입"};

    printf("날짜 입력 (YYYY-MM-DD): ");
    scanf("%11s", date);
    getchar();

    if (strlen(date) != 10 || date[4] != '-' || date[7] != '-') {
        printf("날짜 형식이 올바르지 않습니다 (YYYY-MM-DD).\n");
        return;
    }

    printf("  1) 수입  2) 지출\n선택: ");
    if (scanf("%d", &type) != 1 || (type != 1 && type != 2)) {
        getchar();
        printf("잘못된 선택입니다.\n");
        return;
    }
    getchar();

    printf("금액 입력 (양수로 입력): ");
    if (scanf("%d", &amount) != 1 || amount <= 0) {
        getchar();
        printf("0원은 입력할 수 없습니다.\n");
        return;
    }
    getchar();

    if (type == 2) amount = -amount;

    printf("카테고리 선택:\n  1) 식비  2) 교통  3) 쇼핑  4) 기타  5) 수입\n선택: ");
    if (scanf("%d", &cat_choice) != 1 || cat_choice < 1 || cat_choice > 5) {
        getchar();
        printf("잘못된 선택입니다.\n");
        return;
    }
    getchar();

    printf("메모 입력: ");
    fgets(memo, sizeof(memo), stdin);
    memo[strcspn(memo, "\n")] = '\0';

    Transaction* node = create_node(date, amount, cats[cat_choice - 1], memo);
    if (node == NULL) {
        printf("메모리 할당 실패\n");
        exit(1);
    }
    insert_node(node);
    printf("거래가 추가되었습니다. (ID: %d)\n", node->id);
}

int main(void) {
    init_list();
    init_stack(&undo_stk);
    load_from_file("data.txt");

    int choice;
    while (1) {
        printf("\n============ 가계부 관리 시스템 ============\n");
        printf("  1. 거래 추가\n");
        printf("  2. 거래 삭제\n");
        printf("  3. 거래 수정\n");
        printf("  4. 전체 내역 출력\n");
        printf("  5. 카테고리별 통계\n");
        printf("  6. 월별 요약\n");
        printf("  7. 거래 삭제 취소 (undo)\n");
        printf("  0. 저장 후 종료\n");
        printf("==========================================\n");
        printf("선택: ");

        if (scanf("%d", &choice) != 1) {
            getchar();
            printf("잘못된 선택입니다.\n");
            continue;
        }
        getchar();

        switch (choice) {
            case 1:
                add_transaction();
                break;

            case 2: {
                print_all();
                if (head == NULL) break;
                int id;
                printf("삭제할 거래 번호 입력: ");
                scanf("%d", &id); getchar();
                if (delete_node(id, &undo_stk))
                    printf("거래 %d이(가) 삭제되었습니다. (undo로 복구 가능)\n", id);
                break;
            }

            case 3: {
                print_all();
                if (head == NULL) break;
                int id;
                printf("수정할 거래 번호 입력: ");
                scanf("%d", &id); getchar();
                modify_node(id);
                break;
            }

            case 4:
                print_all();
                print_balance();
                break;

            case 5:
                print_category_stats();
                break;

            case 6: {
                int year, month;
                printf("조회할 연도 입력: "); scanf("%d", &year);
                printf("조회할 월 입력: ");   scanf("%d", &month);
                getchar();
                print_monthly_summary(year, month);
                break;
            }

            case 7: {
                if (is_empty(&undo_stk)) {
                    printf("취소할 거래가 없습니다.\n");
                    break;
                }
                Transaction* node = pop_undo(&undo_stk);
                node->prev = NULL;
                node->next = NULL;
                insert_node(node);
                printf("거래 %d이(가) 복구되었습니다.\n", node->id);
                break;
            }

            case 0: {
                save_to_file("data.txt");
                printf("저장 완료. 프로그램을 종료합니다.\n");

                /* undo 스택에 남은 노드 메모리 해제 */
                while (!is_empty(&undo_stk))
                    free(pop_undo(&undo_stk));

                /* 연결 리스트 전체 메모리 해제 */
                Transaction* cur = head;
                while (cur != NULL) {
                    Transaction* nxt = cur->next;
                    free(cur);
                    cur = nxt;
                }
                return 0;
            }

            default:
                printf("잘못된 선택입니다.\n");
        }
    }
}
