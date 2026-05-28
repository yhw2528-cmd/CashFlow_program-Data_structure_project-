#ifndef TRANSACTION_H
#define TRANSACTION_H

/* UndoStack 전방 선언 (stack.h와의 순환 의존 방지) */
typedef struct UndoStack UndoStack;

typedef struct Transaction {
    int id;
    char date[12];      /* "YYYY-MM-DD" */
    int amount;         /* 양수=수입, 음수=지출 */
    char category[20];
    char memo[50];
    struct Transaction* prev;
    struct Transaction* next;
} Transaction;

extern Transaction* head;
extern Transaction* tail;
extern int next_id;

void         init_list(void);
Transaction* create_node(const char* date, int amount, const char* category, const char* memo);
void         insert_node(Transaction* node);
int          delete_node(int id, UndoStack* stk);
void         modify_node(int id);
void         print_all(void);
Transaction* find_node(int id);

#endif
