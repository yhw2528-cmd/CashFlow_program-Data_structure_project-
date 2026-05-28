#include <stdlib.h>
#include <string.h>
#include "stack.h"

void init_stack(UndoStack* stk) {
    stk->top = -1;
}

int is_empty(UndoStack* stk) {
    return stk->top == -1;
}

int is_full(UndoStack* stk) {
    return stk->top == MAX_UNDO - 1;
}

void push_undo(UndoStack* stk, Transaction* node) {
    if (is_full(stk)) {
        /* 가장 오래된 항목(data[0]) 해제 후 한 칸씩 앞으로 이동 */
        free(stk->data[0]);
        memmove(stk->data, stk->data + 1, (MAX_UNDO - 1) * sizeof(Transaction*));
        /* top은 MAX_UNDO-1 그대로 유지하고 마지막 자리에 새 노드 저장 */
        stk->data[MAX_UNDO - 1] = node;
    } else {
        stk->data[++stk->top] = node;
    }
}

Transaction* pop_undo(UndoStack* stk) {
    if (is_empty(stk)) return NULL;
    return stk->data[stk->top--];
}
