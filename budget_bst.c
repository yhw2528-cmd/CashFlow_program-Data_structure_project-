#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "budget_bst.h"
#include "transaction.h"

BudgetNode* bst_insert(BudgetNode* root, const char* category, int limit) {
    if (root == NULL) {
        BudgetNode* node = malloc(sizeof(BudgetNode));
        if (node == NULL) { printf("메모리 할당 실패\n"); exit(1); }
        strncpy(node->category, category, 19);
        node->category[19] = '\0';
        node->limit = limit;
        node->left = node->right = NULL;
        return node;
    }
    int cmp = strcmp(category, root->category);
    if      (cmp < 0) root->left  = bst_insert(root->left,  category, limit);
    else if (cmp > 0) root->right = bst_insert(root->right, category, limit);
    else              root->limit = limit;   /* 중복 카테고리: 한도 업데이트 */
    return root;
}

BudgetNode* bst_search(BudgetNode* root, const char* category) {
    if (root == NULL) return NULL;
    int cmp = strcmp(category, root->category);
    if (cmp < 0) return bst_search(root->left,  category);
    if (cmp > 0) return bst_search(root->right, category);
    return root;
}

void bst_free(BudgetNode* root) {
    if (root == NULL) return;
    bst_free(root->left);
    bst_free(root->right);
    free(root);
}

void set_budget(BudgetNode** root) {
    const char* cats[] = {"식비", "교통", "쇼핑", "기타"};
    int choice, limit;

    printf("카테고리 선택:\n  1) 식비  2) 교통  3) 쇼핑  4) 기타\n선택: ");
    if (scanf("%d", &choice) != 1 || choice < 1 || choice > 4) {
        getchar();
        printf("잘못된 선택입니다.\n");
        return;
    }
    getchar();

    printf("월 예산 한도 입력 (원): ");
    if (scanf("%d", &limit) != 1 || limit <= 0) {
        getchar();
        printf("0원 이하는 설정할 수 없습니다.\n");
        return;
    }
    getchar();

    *root = bst_insert(*root, cats[choice - 1], limit);
    printf("%s 예산이 %d원으로 설정되었습니다.\n", cats[choice - 1], limit);
}

void check_budget(BudgetNode* root, const char* category, int year, int month) {
    BudgetNode* node = bst_search(root, category);
    if (node == NULL) return;

    int spent = 0;
    for (Transaction* t = head; t != NULL; t = t->next) {
        int y, m;
        sscanf(t->date, "%d-%d", &y, &m);
        if (y == year && m == month &&
            strcmp(t->category, category) == 0 && t->amount < 0)
            spent += t->amount;
    }

    int abs_spent = -spent;
    int over      = abs_spent - node->limit;
    double ratio  = (double)abs_spent / node->limit * 100.0;

    if (over > 0)
        printf("[경고!] %s 예산을 %d원 초과했습니다!\n", category, over);
    else
        printf("[예산 경고] %s: %d원 / %d원 사용 (%.1f%%)\n",
               category, abs_spent, node->limit, ratio);
}

/* BST 중위 순회 — 각 노드의 해당 월 지출 계산 후 출력 */
static int month_expense(const char* category, int year, int month) {
    int total = 0;
    for (Transaction* t = head; t != NULL; t = t->next) {
        int y, m;
        sscanf(t->date, "%d-%d", &y, &m);
        if (y == year && m == month &&
            strcmp(t->category, category) == 0 && t->amount < 0)
            total += t->amount;
    }
    return total;   /* 음수 */
}

static void inorder_print(BudgetNode* node, int year, int month) {
    if (node == NULL) return;
    inorder_print(node->left, year, month);

    int spent    = month_expense(node->category, year, month);
    int abs_spent = -spent;
    int remain   = node->limit - abs_spent;
    double ratio = (double)abs_spent / node->limit * 100.0;

    printf("  %-6s  %9d원  %9d원  %9d원  %6.1f%%%s\n",
           node->category,
           node->limit,
           abs_spent,
           remain,
           ratio,
           (remain < 0) ? "  [초과!]" : "");

    inorder_print(node->right, year, month);
}

void print_budget_status(BudgetNode* root, int year, int month) {
    if (root == NULL) {
        printf("설정된 예산이 없습니다.\n");
        return;
    }
    printf("========== 예산 현황 (%d년 %d월) ==========\n", year, month);
    printf("  %-6s  %9s  %9s  %9s  %7s\n",
           "카테고리", "한도", "사용", "잔여", "비율");
    printf("  ──────────────────────────────────────────\n");
    inorder_print(root, year, month);
    printf("============================================\n");
}

/* budget.txt 저장 — 중위 순회로 기록 */
static void inorder_save(BudgetNode* node, FILE* fp) {
    if (node == NULL) return;
    inorder_save(node->left, fp);
    fprintf(fp, "%s|%d\n", node->category, node->limit);
    inorder_save(node->right, fp);
}

void save_budget(BudgetNode* root, const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("예산 파일 저장에 실패했습니다.\n");
        return;
    }
    inorder_save(root, fp);
    fclose(fp);
}

void load_budget(BudgetNode** root, const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) return;   /* 최초 실행: 파일 없음 = 정상 */

    char line[64];
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        char* tok = strtok(line, "|");
        if (!tok) continue;
        char category[20];
        strncpy(category, tok, 19);
        category[19] = '\0';

        tok = strtok(NULL, "|");
        if (!tok) continue;
        int limit = atoi(tok);

        *root = bst_insert(*root, category, limit);
    }
    fclose(fp);
}
