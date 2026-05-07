/*
 * Copyright (C) cai<sybstudy@yeah.net>. All rights reserved.
 *
 * @file    trie.c
 * @brief   字典树实现
 *
 * @author  cai<sybstudy@yeah.net>
 * @date    2026-05-06
 * @version 1.0
 *
 * @note    
 *
 * @history
 *   1.0 | 2026-05-06 | cai | Initial creation.
 */

/* ========================================================================== */
/*                               Include Files                                */
/* ========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "type/trie.h"

/* ========================================================================== */
/*                             Macro Definitions                              */
/* ========================================================================== */

#define TRIE_WORD_SIZE_MAX      (16)    // 一个单词的最大长度
#define TRIE_WORD_COUNT_MAX     (64)    // 一个句子中最大单词个数
#define TRIE_LINE_SIZE_MAX      (TRIE_WORD_SIZE_MAX * TRIE_WORD_COUNT_MAX)

/* ========================================================================== */
/*                             Type Definitions                               */
/* ========================================================================== */

// 字典树节点定义
typedef struct trie_item_s{
    char word[TRIE_WORD_SIZE_MAX];  // 单词文本
    struct trie_item_s **next;
    unsigned int next_count;
    unsigned int is_end;            // 是否终止节点
    trie_item_cb_func cb;           // 终止节点回调
    char* helper;                   // 终止节点提示信息，内存动态申请
}trie_item_t;

// 字典树管理结构定义
struct trie_head_s{
    trie_item_t root;
};

/* ========================================================================== */
/*                         Private Function Implementations                   */
/* ========================================================================== */

/**
 * @brief       split words of line
 *
 * @param[in]   line            a line
 * @param[out]  count_out       counts of words
 * 
 * @retval      ptr to words array
 * 
 * @note        将输入的line分解为多个words
 *              例如 " hello  this is me   " -> "hello" "this" "is" "me"
 *              调用者需要负责释放内存
 */
static char** split_words(const char *line, unsigned int *count_out)
{
    unsigned int count = 0;
    int idx = 0;
    const char *c = NULL;
    int in_word = 0;

    if(!line || !count_out)
    {
        if(count_out)
            *count_out = 0;
        return NULL;
    }

    // 统计单词数量先
    c = line;
    while('\0' != *c)
    {
        if(' ' == *c)
            in_word = 0;
        else if(!in_word)
            {
                ++ count;
                in_word = 1;
            }
        c ++;
    }

    if(!count)
    {
        *count_out = 0;
        return NULL;
    }

    // 分配指针数组内存
    char** words = (char**)malloc(sizeof(char*) * count);   // TODO: mp
    if(!words)
    {
        *count_out = 0;
        return NULL;
    }

    // 摘取单词
    size_t left =0, right = 0;
    size_t line_size = strlen(line);
    while(left < line_size)
    {
        // 找到下一个左边界
        while(left < line_size && line[left] == ' ')
            ++ left;
        // 超出长度，说明找完了
        if(left == line_size)
            break;
        // 找到右边界
        right = left+1;
        while(right < line_size && line[right] != ' ')
            ++ right;
        // 此时word范围为[left, right)，多分配一个给'\0'
        size_t word_len = right-left;
        words[idx] = (char*)malloc(sizeof(char)*(word_len+1));  // TODO: mp
        if(!words[idx])
        {
            for(idx = idx-1; idx >= 0; --idx)
            {
                free(words[idx]);
            }
            *count_out = 0;
            return NULL;
        }
        memcpy(words[idx], &line[left], word_len);
        words[idx][word_len] = '\0';
        ++ idx;     // 更新下标

        left = right+1; // 更新left
    }
    
    *count_out = count;
    return words;
}

/**
 * @brief       new an trie item
 * 
 * @retval      ptr to new item
 */
static attr_force_inline trie_item_t* trie_item_new()
{
    trie_item_t *item = (trie_item_t*)malloc(sizeof(trie_item_t));      // TODO: mp
    if(!item)
        return NULL;
    memset(item, 0, sizeof(trie_item_t));
    return item;
}

static attr_force_inline void trie_item_free(trie_item_t *item)
{
    free(item);     // TODO: mp
}

/**
 * @brief       compare with two words
 * 
 * @param[in]   w1  - word1
 * @param[in]   w2  - word2
 * 
 * @retval      negative - lt, 0 - eq, possitive - gt
 */
static attr_pure_inline int trie_word_cmp(char *w1, char *w2)
{
    if(!w1 && !w2)
        return 0;
    if(!w1 || !w2)
        return !w1 ? -1 : 1;

    size_t len = strlen(w1) > strlen(w2) ? strlen(w1) : strlen(w2);
    len = len < TRIE_WORD_SIZE_MAX ? len : TRIE_WORD_SIZE_MAX;
    return strncmp(w1, w2, len);
}

/**
 * @brief       recursive dump trie
 * 
 * @param[in]   trie_item       cur item of trie
 * @param[in]   line            cur line
 * @param[in]   line_size_max   max size of line buffer
 * 
 * @note        递归辅助dump
 */
static void _trie_dump_recursive(trie_item_t *trie_item, char *line, size_t line_size_max)
{
    if(!trie_item)
        return;

    size_t prev_line_len = strlen(line);    // 记录原来的长度
    
    strncat(line, trie_item->word, line_size_max-prev_line_len);

    if(trie_item->is_end)
    {
        printf("line: %s, helper: %s\n", line, trie_item->helper);
    }
    
    // is_end节点不一定是叶子，需要继续往下查找
    strncat(line, " ", line_size_max-prev_line_len);
    unsigned int i = 0;
    for(; i < trie_item->next_count; ++ i)
    {
        _trie_dump_recursive(trie_item->next[i], line, line_size_max);
    }

    // 恢复原来的line
    memset(line+prev_line_len, 0, sizeof(char)*(line_size_max-prev_line_len));
}

/**
 * @brief       find end item in trie by line
 * 
 * @param[in]   trie_item       cur item of trie
 * @param[in]   line            cur line
 * 
 * @retval      ptr to item, if exist, else return NULL
 */
static trie_item_t* trie_find_item(trie_head_t *trie_head, const char *line)
{
    if(!trie_head || !line)
        return NULL;

    unsigned int count = 0;
    unsigned int i = 0;
    char **words_array = split_words(line, &count);
    if(!words_array)
        return NULL;

    trie_item_t *prev = &trie_head->root;
    for(; i < count; ++ i)
    {
        int found = 0;
        unsigned int j = 0;
        for(; j < prev->next_count; ++ j)
        {
            if(!trie_word_cmp(words_array[i], prev->next[j]->word))
            {
                found = 1;
                break;
            }
        }
        if(!found)
        {
            free(words_array);
            return NULL;
        }
        prev = prev->next[j];
    }

    free(words_array);

    return prev->is_end ? prev : NULL;
}

/* ========================================================================== */
/*                         Public Function Implementations                    */
/* ========================================================================== */

trie_head_t* trie_create()
{
    trie_head_t* head = malloc(sizeof(trie_head_t));    // TODO: mp
    if(!head)
        return NULL;
    memset(head, 0, sizeof(*head));
    return head;
}

void trie_destroy(trie_head_t* trie_head)
{
    trie_item_t *prev = NULL;
    trie_item_t *next = NULL;

    if(!trie_head)
        return;

    // dfs destroy items
    prev = &trie_head->root;

    // TODO
}

void trie_add(trie_head_t* trie_head, const char *line, trie_item_cb_func cb_func, const char *helper)
{
    char **words_array = NULL;
    unsigned int words_count = 0;
    trie_item_t *prev = NULL;
    unsigned int idx = 0;
    trie_item_t* new_item_ptrs[TRIE_WORD_COUNT_MAX] = {};   // 记录分配的item，失败时需要回滚free
    unsigned int new_item_count = 0;

    if(!trie_head || !line)
        return;

    words_array = split_words(line, &words_count);
    if(!words_array)
        return;

    // 开始加入
    prev = &trie_head->root;
    for(idx = 0; idx < words_count; ++ idx)
    {
        // 查看是否已存在下层节点
        unsigned int j = 0;
        int found = 0;
        for(j = 0; j < prev->next_count; ++ j)
        {
            if(!trie_word_cmp(prev->next[j]->word, words_array[idx]))
            {
                found = 1;
                break;
            }
        }
        if(found)   // 找到的话，从该节点继续找
        {
            prev = prev->next[j];
            continue;
        }
        else        // 否则新建节点
        {
            trie_item_t *new_item = trie_item_new();
            if(!new_item)       // 失败，释放之前成功的节点内存
            {
                goto error_label;
            }

            strncpy(new_item->word, words_array[idx], TRIE_WORD_SIZE_MAX-1);

            // 扩展prev的next
            trie_item_t** new_next = (trie_item_t**)realloc(prev->next, (prev->next_count + 1) * sizeof(trie_item_t*)); // TODO
            if(!new_next)
            {
                goto error_label;
            }
            prev->next = new_next;  // 成功则更新指针
            prev->next[prev->next_count] = new_item;
            prev->next_count ++;

            prev = new_item;    // 继续往下查找

            new_item_ptrs[new_item_count] = new_item;
            new_item_count ++;
        }
    }

    // 此时prev指向最后一个节点
    prev->is_end = 1;
    if(cb_func)
        prev->cb = cb_func;
    if(helper)
        prev->helper = strdup(helper);

    free(words_array);

    return;

error_label:
    unsigned int ii = 0;
    for(; ii < new_item_count; ++ ii)
        trie_item_free(new_item_ptrs[ii]);
    free(words_array);      // TODO: mp
}

void trie_dump(trie_head_t* trie_head)
{
    printf("Trie Dump:\n");

    if(!trie_head)
        return;

    trie_item_t *prev = &trie_head->root;
    char* line = (char*)malloc(sizeof(char)*TRIE_LINE_SIZE_MAX);    // TODO: mp
    if(!line)
        return;
    memset(line, 0, sizeof(char)*TRIE_LINE_SIZE_MAX);

    _trie_dump_recursive(prev, line, TRIE_LINE_SIZE_MAX);

    free(line);     // TODO: mp
}

inline int trie_find(trie_head_t* trie_head, const char *line)
{
    return trie_find_item(trie_head, line) ? 1 : 0;
}