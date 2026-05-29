#ifndef BUDGET_BST_H
#define BUDGET_BST_H

typedef struct BudgetNode {
    char category[20];
    int  limit;
    struct BudgetNode* left;
    struct BudgetNode* right;
} BudgetNode;

BudgetNode* bst_insert(BudgetNode* root, const char* category, int limit);
BudgetNode* bst_search(BudgetNode* root, const char* category);
void        bst_free(BudgetNode* root);
void        set_budget(BudgetNode** root);
void        check_budget(BudgetNode* root, const char* category, int year, int month);
void        print_budget_status(BudgetNode* root, int year, int month);
void        save_budget(BudgetNode* root, const char* filename);
void        load_budget(BudgetNode** root, const char* filename);

#endif
