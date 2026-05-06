/*
 * Copyright (C) cai<sybstudy@yeah.net>. All rights reserved.
 *
 * @file    type_skiplist.h
 * @brief   通用跳表定义实现
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

#ifndef __TYPE_SKIPLIST_H__
#define __TYPE_SKIPLIST_H__

/* ========================================================================== */
/*                               Include Files                                */
/* ========================================================================== */

#include <assert.h>
#include <string.h>
#include "plat/compiler.h"

/* ========================================================================== */
/*                             Type Definitions                               */
/* ========================================================================== */

#define SKIPLIST_DEPTH_MAX      (8)     // 跳表的最大层数
// 跳表节点定义
typedef struct skiplist_item_s{
    struct skiplist_item_s* next[SKIPLIST_DEPTH_MAX];
}skiplist_item_t;

// 跳表管理结构定义
typedef struct{
    skiplist_item_t skiplist_head;
    unsigned int count;
}skiplist_head_t;

typedef int (*skiplist_item_cmp_func)(const skiplist_item_t *it1, const skiplist_item_t *it2);  // 比较函数

/* ========================================================================== */
/*                             Macro Definitions                              */
/* ========================================================================== */

#define pre_declare_skiplist(sprefix)   \
typedef struct { skiplist_item_t skiplist_item; } sprefix ## _skiplist_item_t;  \
typedef struct { skiplist_head_t skiplist_head; } sprefix ## _skiplist_head_t;  \
/* pre_declare_skiplist end */

#define declare_skiplist(sprefix, fprefix, type, field, cmp_func)   \
static attr_force_inline void fprefix ## _skiplist_init(sprefix ## _skiplist_head_t *head)  \
{ \
    assert(head);   \
    memset(head, 0, sizeof(*head)); \
} \
static attr_force_inline void fprefix ## _skiplist_fini(sprefix ## _skiplist_head_t *head)  \
{ \
    assert(head);   \
    memset(head, 0, sizeof(*head)); \
} \
static attr_pure_inline int fprefix ## _skiplist_cmp(const skiplist_item_t *it1, const skiplist_item_t *it2)    \
{ \
    return cmp_func(container_of(it1, type, field.skiplist_item), container_of(it2, type, field.skiplist_item));    \
} \
static attr_pure_inline int fprefix ## _skiplist_unique_cmp(const skiplist_item_t *it1, const skiplist_item_t *it2) \
{ \
    int cmp_val = cmp_func(container_of(it1, type, field.skiplist_item), container_of(it2, type, field.skiplist_item)); \
    if(cmp_val) \
        return cmp_val; \
    if((unsigned long)it1 < (unsigned long)it2) \
        return -1;  \
    if((unsigned long)it1 > (unsigned long)it2) \
        return 1;   \
    return 0;   \
} \
static attr_force_inline void fprefix ## _skiplist_add(sprefix ## _skiplist_head_t *head, type *item)   \
{ \
    assert(head && item);   \
    type_skiplist_add(&head->skiplist_head, &item->field.skiplist_item, fprefix ## _skiplist_unique_cmp);   \
} \
static attr_pure_inline type* fprefix ## _skiplist_find_greater_euqal(sprefix ## _skiplist_head_t *head, type *item)    \
{ \
    assert(head && item);   \
    skiplist_item_t *it = NULL; \
    it = type_skiplist_find_greater_euqal(&head->skiplist_head, &item->field.skiplist_item, fprefix ## _skiplist_cmp);  \
    return it ? container_of(it, type, field.skiplist_item) : NULL; \
} \
static attr_pure_inline type* fprefix ## _skiplist_find_less(sprefix ## _skiplist_head_t *head, type *item) \
{ \
    assert(head && item);   \
    skiplist_item_t *it = NULL; \
    it = type_skiplist_find_less(&head->skiplist_head, &item->field.skiplist_item, fprefix ## _skiplist_cmp);   \
    return it ? container_of(it, type, field.skiplist_item) : NULL; \
} \
static attr_force_inline type* fprefix ## _skiplist_del(sprefix ## _skiplist_head_t *head, type *item)  \
{ \
    assert(head && item);   \
    skiplist_item_t *it = NULL; \
    it = type_skiplist_del(&head->skiplist_head, &item->field.skiplist_item, fprefix ## _skiplist_unique_cmp);  \
    return it ? container_of(it, type, field.skiplist_item) : NULL; \
} \
static attr_pure_inline type* fprefix ## _skiplist_first(sprefix ## _skiplist_head_t *head) \
{ \
    assert(head);   \
    skiplist_item_t *it = NULL; \
    it = type_skiplist_first(&head->skiplist_head); \
    return it ? container_of(it, type, field.skiplist_item) : NULL; \
} \
static attr_pure_inline type* fprefix ## _skiplist_next(type *item) \
{ \
    assert(item);   \
    skiplist_item_t *it = NULL; \
    it = type_skiplist_next(&item->field.skiplist_item);    \
    return it ? container_of(it, type, field.skiplist_item) : NULL; \
} \
static attr_pure_inline unsigned int fprefix ## _skiplist_count(sprefix ## _skiplist_head_t *head) \
{ \
    return type_skiplist_count(&head->skiplist_head);   \
} \
/* declare_skiplist end */

/* ========================================================================== */
/*                            Function Prototypes                             */
/* ========================================================================== */

/**
 * @brief       add item into skiplist.
 *
 * @param[in]   head        skiplist head
 * @param[in]   item        skiplist item
 * @param[in]   cmp_func    cmp func between skiplist item
 * 
 * @retval      ptr to skiplist item, if it is already exist, else return NULL.
 */
extern skiplist_item_t* type_skiplist_add(skiplist_head_t *head, skiplist_item_t *item, skiplist_item_cmp_func cmp_func);

/**
 * @brief       get first item which greater than / euqal to input item
 *
 * @param[in]   head        skiplist head
 * @param[in]   item        skiplist item
 * @param[in]   cmp_func    cmp func between skiplist item
 * 
 * @retval      ptr to item, if it exist, else return NULL.
 */
extern skiplist_item_t* type_skiplist_find_greater_euqal(skiplist_head_t *head, skiplist_item_t *item, skiplist_item_cmp_func cmp_func);

/**
 * @brief       get last item which less than input item
 *
 * @param[in]   head        skiplist head
 * @param[in]   item        skiplist item
 * @param[in]   cmp_func    cmp func between skiplist item
 * 
 * @retval      ptr to item, if it exist, else return NULL.
 */
extern skiplist_item_t* type_skiplist_find_less(skiplist_head_t *head, skiplist_item_t *item, skiplist_item_cmp_func cmp_func);

/**
 * @brief       del item from skiplist.
 *
 * @param[in]   head        skiplist head
 * @param[in]   item        skiplist item
 * @param[in]   cmp_func    cmp func between skiplist item
 * 
 * @retval      ptr to skiplist item, if exist, else return NULL.
 */
extern skiplist_item_t* type_skiplist_del(skiplist_head_t *head, skiplist_item_t *item, skiplist_item_cmp_func cmp_func);

/* ========================================================================== */
/*                         Private Function Implementations                   */
/* ========================================================================== */

/**
 * @brief       get first item of skiplist.
 *
 * @param[in]   head        skiplist head
 * 
 * @retval      ptr to first skiplist item, if exist, else return NULL.
 */
static inline skiplist_item_t* type_skiplist_first(skiplist_head_t *head)
{
    return head->skiplist_head.next[0];
}

/**
 * @brief       get next item of item in skiplist.
 *
 * @param[in]   item        skiplist item
 * 
 * @retval      ptr to next item, if exist, else return NULL.
 */
static inline skiplist_item_t* type_skiplist_next(skiplist_item_t *item)
{
    return item->next[0];
}

/**
 * @brief       get counts of item in skiplist.
 *
 * @param[in]   head        skiplist head
 * 
 * @retval      counts of items in skiplist.
 */
static inline unsigned int type_skiplist_count(skiplist_head_t *head)
{
    return head ? head->count : 0;
}

#endif
/* __TYPE_SKIPLIST_H__ end */