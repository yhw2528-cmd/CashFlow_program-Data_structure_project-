#ifndef STACK_H
#define STACK_H

#include "transaction.h"

#define MAX_UNDO 50

/* transaction.h에서 이미 typedef 선언됨 — struct body만 완성 */
struct UndoStack {
    Transaction* data[MAX_UNDO];
    int top;
};

void         init_stack(UndoStack* stk);
int          is_empty(UndoStack* stk);
int          is_full(UndoStack* stk);
void         push_undo(UndoStack* stk, Transaction* node);
Transaction* pop_undo(UndoStack* stk);

#endif
