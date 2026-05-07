/*
 * Copyright (C) cai<sybstudy@yeah.net>. All rights reserved.
 *
 * @file    type_list.h
 * @brief   通用链表定义实现
 *
 * @author  cai<sybstudy@yeah.net>
 * @date    2026-04-28
 * @version 1.0
 *
 * @note    这是一个侵入式链表模板，业务模块若要使用，需要：
 *          1. 定义业务数据结构体，其中包含list_item_t成员
 *          2. 使用pre_declare_list宏，设置sprefix - struct prefix
 *          3. 定义业务数据结构类型，其中包含类型为sprefix_item_t的成员，名字任意xxx
 *          4. 使用declare_list宏，设置fprefix - function prefix, type是业务数据类型，field是其中的list_item_t类型成员名xxx
 *          此后即可使用fprefix_xxx函数
 *          链表没有包含锁机制，多线程时需要外部加锁
 *
 * @history
 *   1.0 | 2026-04-28 | cai | Initial creation.
 */

#ifndef __TYPE_LIST_H__
#define __TYPE_LIST_H__

/* ========================================================================== */
/*                               Include Files                                */
/* ========================================================================== */

#include <assert.h>
#include <string.h>
#include "plat/compiler.h"

/* ========================================================================== */
/*                             Macro Definitions                              */
/* ========================================================================== */

typedef struct list_item_s{
    struct list_item_s *next;
}list_item_t;

typedef struct{
    list_item_t *first;
    list_item_t **lastNext;
    unsigned int count;
}list_head_t;

#define pre_declare_list(sprefix)   \
typedef struct { list_item_t list_item; } sprefix ## _list_item_t;   \
typedef struct { list_head_t list_head; } sprefix ## _list_head_t;   \
/* pre_declare_list end */

#define declare_list(sprefix, fprefix, type, field)     \
static attr_force_inline void fprefix ## _list_init(sprefix ## _list_head_t *head)  \
{ \
    assert(head);   \
    memset(head, 0, sizeof(*head)); \
    type_list_init(&head->list_head);   \
} \
static attr_force_inline void fprefix ## _list_fini(sprefix ## _list_head_t *head)  \
{ \
    assert(head && !head->list_head.count); \
    memset(head, 0, sizeof(*head)); \
} \
static attr_force_inline void fprefix ## _list_add_head(sprefix ## _list_head_t *head, type *item)  \
{ \
    assert(head && item);   \
    type_list_add_head(&head->list_head, &item->field.list_item);   \
} \
static attr_force_inline void fprefix ## _list_add_tail(sprefix ## _list_head_t *head, type *item)  \
{ \
    assert(head && item);   \
    type_list_add_tail(&head->list_head, &item->field.list_item);   \
} \
static attr_force_inline void fprefix ## _list_add_after(sprefix ## _list_head_t *head, type *after, type *item)    \
{ \
    assert(head && after && item);   \
    type_list_add_after(&head->list_head, &after->field.list_item, &item->field.list_item);   \
} \
static attr_force_inline type* fprefix ## _list_del(sprefix ## _list_head_t *head, type *item)  \
{ \
    assert(head && item);   \
    list_item_t *it = NULL; \
    it = type_list_del(&head->list_head, &item->field.list_item);   \
    return it ? item : NULL;    \
} \
static attr_force_inline type* fprefix ## _list_pop(sprefix ## _list_head_t *head)  \
{ \
    assert(head);   \
    list_item_t *it = NULL; \
    it = type_list_pop(&head->list_head);   \
    return it ? container_of(it, type, field.list_item) : NULL; \
} \
static attr_pure_inline type* fprefix ## _list_first(sprefix ## _list_head_t *head) \
{ \
    assert(head);   \
    list_item_t *it = NULL; \
    it = type_list_first(&head->list_head); \
    return it ? container_of(it, type, field.list_item) : NULL; \
} \
static attr_pure_inline type* fprefix ## _list_next(type* item) \
{ \
    assert(item);   \
    list_item_t *it = NULL; \
    it = type_list_next(&item->field.list_item);    \
    return it ? container_of(it, type, field.list_item) : NULL; \
} \
static attr_pure_inline unsigned int fprefix ## _list_count(sprefix ## _list_head_t *head)  \
{ \
    return type_list_count(&head->list_head);   \
} \
/* declare_list end */

#define __list_end  (&__g_list_end)

/* ========================================================================== */
/*                          Global/Static Variables                           */
/* ========================================================================== */

extern list_item_t __g_list_end;

/* ========================================================================== */
/*                            Function Prototypes                             */
/* ========================================================================== */

static attr_force_inline void type_list_init(list_head_t *head);
static attr_force_inline void type_list_add(list_head_t *head, list_item_t **pos, list_item_t *item);
static attr_force_inline void type_list_add_tail(list_head_t *head, list_item_t *item);
static attr_force_inline void type_list_add_after(list_head_t *head, list_item_t *after, list_item_t *item);
extern list_item_t* type_list_del(list_head_t *head, list_item_t *item);
extern list_item_t* type_list_pop(list_head_t *head);
static attr_pure_inline list_item_t* type_list_first(list_head_t *head);
static attr_pure_inline list_item_t* type_list_next(list_item_t *item);
static attr_pure_inline unsigned int type_list_count(list_head_t *head);

/* ========================================================================== */
/*                         Private Function Implementations                   */
/* ========================================================================== */

static inline void type_list_init(list_head_t *head)
{
    head->first = __list_end;           // 后续简化处理
    head->lastNext = &head->first;
}

/**
 * @brief       list add
 * @note        插入成为pos位置
 */
static inline void type_list_add(list_head_t *head, list_item_t **pos, list_item_t *item)
{
    item->next = *pos;
    *pos = item;
    if(pos == head->lastNext)
        head->lastNext = &item->next;

    head->count ++;
}

static inline void type_list_add_head(list_head_t *head, list_item_t *item)
{
    type_list_add(head, &head->first, item);
}

static inline void type_list_add_tail(list_head_t *head, list_item_t *item)
{
    type_list_add(head, head->lastNext, item);
}

static inline void type_list_add_after(list_head_t *head, list_item_t *after, list_item_t *item)
{
    type_list_add(head, &after->next, item);
}

static inline list_item_t* type_list_first(list_head_t *head)
{
    return head->first == __list_end ? NULL : head->first;
}

static inline list_item_t* type_list_next(list_item_t *item)
{
    return item->next == __list_end ? NULL : item->next;
}

static inline unsigned int type_list_count(list_head_t *head)
{
    return head ? head->count : 0;
}

#endif