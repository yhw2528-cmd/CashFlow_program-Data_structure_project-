#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_io.h"
#include "transaction.h"

/* 메모에서 | 문자를 / 로 치환 (파일 저장 시 파싱 오류 방지) */
static void sanitize_memo(const char* src, char* dst, int size) {
    strncpy(dst, src, size - 1);
    dst[size - 1] = '\0';
    for (int i = 0; dst[i]; i++)
        if (dst[i] == '|') dst[i] = '/';
}

void save_to_file(const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("파일 저장에 실패했습니다.\n");
        return;
    }

    for (Transaction* t = head; t != NULL; t = t->next) {
        /* [수정 3] 저장 전 메모의 | 를 / 로 치환 */
        char safe_memo[50];
        sanitize_memo(t->memo, safe_memo, sizeof(safe_memo));

        fprintf(fp, "%d|%s|%d|%s|%s\n",
                t->id, t->date, t->amount, t->category, safe_memo);
    }

    fclose(fp);
}

void load_from_file(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) return;   /* 최초 실행: 파일 없음 = 정상 */

    char line[256];
    int  max_id = 0;

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';

        /* 형식: id|date|amount|category|memo */
        char* tok;
        int   id, amount;
        char  date[12], category[20], memo[50];

        tok = strtok(line, "|"); if (!tok) continue;
        id = atoi(tok);

        tok = strtok(NULL, "|"); if (!tok) continue;
        strncpy(date, tok, 11); date[11] = '\0';

        tok = strtok(NULL, "|"); if (!tok) continue;
        amount = atoi(tok);

        tok = strtok(NULL, "|"); if (!tok) continue;
        strncpy(category, tok, 19); category[19] = '\0';

        tok = strtok(NULL, "|"); if (!tok) continue;
        strncpy(memo, tok, 49); memo[49] = '\0';

        Transaction* node = create_node(date, amount, category, memo);
        if (node == NULL) {
            printf("메모리 할당 실패\n");
            fclose(fp);
            return;
        }
        node->id = id;   /* 저장된 id로 덮어씌움 */
        insert_node(node);

        if (id > max_id) max_id = id;
    }

    if (max_id > 0) next_id = max_id + 1;

    fclose(fp);
}
