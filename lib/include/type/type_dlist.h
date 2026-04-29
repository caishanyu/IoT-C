/*
 * Copyright (C) cai<sybstudy@yeah.net>. All rights reserved.
 *
 * @file    type_dlist.h
 * @brief   通用双链表定义实现
 *
 * @author  cai<sybstudy@yeah.net>
 * @date    2026-04-29
 * @version 1.0
 *
 * @note    
 *
 * @history
 *   1.0 | 2026-04-28 | cai | Initial creation.
 */

#ifndef __TYPE_DLIST_H__
#define __TYPE_DLIST_H__

/* ========================================================================== */
/*                               Include Files                                */
/* ========================================================================== */

#include <assert.h>
#include <string.h>
#include "plat/compiler.h"

/* ========================================================================== */
/*                             Type Definitions                               */
/* ========================================================================== */

// 双链表link
typedef struct dlist_item_s{
    struct dlist_item_s *next;
    struct dlist_item_s *prev;
}dlist_item_t;

// 双链表头
typedef struct{
    dlist_item_t item;
    unsigned int count;
}dlist_head_t;

/* ========================================================================== */
/*                             Macro Definitions                              */
/* ========================================================================== */

#define pre_declare_dlist(sprefix)  \
typedef struct { dlist_item_t dlist_item; } sprefix ## _dlist_item_t;   \
typedef struct { dlist_head_t dlist_head; } sprefix ## _dlist_head_t;   \
/* pre_declare_dlist end */

#define declare_dlist(sprefix, fprefix, type, field)    \
static attr_force_inline void fprefix ## _init(sprefix ## _dlist_head_t *head)  \
{ \
    assert(head);   \
    memset(head, 0, sizeof(*head)); \
    type_dlist_init(&head->dlist_head); \
} \
static attr_force_inline void fprefix ## _fini(sprefix ## _dlist_head_t *head)  \
{ \
    assert(head && !head->dlist_head.count);    \
    memset(head, 0, sizeof(*head)); \
} \
static attr_force_inline void fprefix ## _add_head(sprefix ## _dlist_head_t *head, type *item)  \
{ \
    assert(head && item);   \
    type_dlist_add_head(&head->dlist_head, &item->field.dlist_item);    \
} \
static attr_force_inline void fprefix ## _add_tail(sprefix ## _dlist_head_t *head, type *item)  \
{ \
    assert(head && item);   \
    type_dlist_add_tail(&head->dlist_head, &item->field.dlist_item);    \
} \
static attr_force_inline void fprefix ## _add_after(sprefix ## _dlist_head_t *head, type *after, type *item)    \
{ \
    assert(head && item);   \
    type_dlist_add_after(&head->dlist_head, &after->field.dlist_item, &item->field.dlist_item);    \
} \
static attr_force_inline type* fprefix ## _del(sprefix ## _dlist_head_t *head, type *item)  \
{ \
    assert(head && item);   \
    dlist_item_t *it = NULL;    \
    it = type_dlist_del(&head->dlist_head, & item->field.dlist_item);  \
    return it ? item : NULL;    \
} \
static attr_force_inline type* fprefix ## _pop(sprefix ## _dlist_head_t *head)  \
{ \
    assert(head);   \
    dlist_item_t *it = NULL;    \
    it = type_dlist_pop(&head->dlist_head); \
    return it ? container_of(it, type, field.dlist_item) : NULL;    \
} \
static attr_pure_inline type* fprefix ## _first(sprefix ## _dlist_head_t *head) \
{ \
    assert(head);   \
    dlist_item_t *it = NULL;    \
    it = type_dlist_first(&head->dlist_head);   \
    return it ? container_of(it, type, field.dlist_item) : NULL;    \
} \
static attr_pure_inline type* fprefix ## _next(sprefix ## _dlist_head_t *head, type *item)  \
{ \
    assert(head);   \
    dlist_item_t *it = NULL;    \
    it = type_dlist_next(&head->dlist_head, &item->field.dlist_item);   \
    return it ? container_of(it, type, field.dlist_item) : NULL;    \
} \
static attr_pure_inline unsigned int fprefix ## _count(sprefix ## _dlist_head_t *head) \
{ \
    return type_dlist_count(&head->dlist_head);   \
} \
/* declare_dlist end */

/* ========================================================================== */
/*                            Function Prototypes                             */
/* ========================================================================== */

static attr_force_inline void type_dlist_init(dlist_head_t *head);
static attr_force_inline void type_dlist_add(dlist_head_t *head, dlist_item_t *prev, dlist_item_t *item);
static attr_force_inline void type_dlist_add_head(dlist_head_t *head, dlist_item_t *item);
static attr_force_inline void type_dlist_add_tail(dlist_head_t *head, dlist_item_t *item);
static attr_force_inline void type_dlist_add_after(dlist_head_t *head, dlist_item_t *after, dlist_item_t *item);
static attr_force_inline dlist_item_t* type_dlist_del(dlist_head_t *head, dlist_item_t *item);
static attr_force_inline dlist_item_t* type_dlist_pop(dlist_head_t *head);
static attr_pure_inline dlist_item_t* type_dlist_first(dlist_head_t *head);
static attr_pure_inline dlist_item_t* type_dlist_next(dlist_head_t *head, dlist_item_t *item);
static attr_pure_inline unsigned int type_dlist_count(dlist_head_t *head);

/* ========================================================================== */
/*                         Private Function Implementations                   */
/* ========================================================================== */

static inline void type_dlist_init(dlist_head_t *head)
{
    head->item.next = &head->item;
    head->item.prev = &head->item;
}

static inline void type_dlist_add(dlist_head_t *head, dlist_item_t *prev, dlist_item_t *item)
{
    item->next = prev->next;
    item->prev = prev;
    prev->next->prev = item;
    prev->next = item;
    head->count ++;
}

static inline void type_dlist_add_head(dlist_head_t *head, dlist_item_t *item)
{
    type_dlist_add(head, &head->item, item);
}

static inline void type_dlist_add_tail(dlist_head_t *head, dlist_item_t *item)
{
    type_dlist_add(head, head->item.prev, item);
}

static inline void type_dlist_add_after(dlist_head_t *head, dlist_item_t *after, dlist_item_t *item)
{
    type_dlist_add(head, after, item);
}

static inline dlist_item_t* type_dlist_del(dlist_head_t *head, dlist_item_t *item)
{
    if(!item->next || !item->prev)
        return NULL;

    item->prev->next = item->next;
    item->next->prev = item->prev;
    item->next = item->prev = NULL;
    head->count --;

    return item;
}

static inline dlist_item_t* type_dlist_pop(dlist_head_t *head)
{
    dlist_item_t *item = head->item.next;

    if(item == &head->item)
        return NULL;

    return type_dlist_del(head, item);
}

static inline dlist_item_t* type_dlist_first(dlist_head_t *head)
{
    return head->item.next == &head->item ? NULL : head->item.next;
}

static inline dlist_item_t* type_dlist_next(dlist_head_t *head, dlist_item_t *item)
{
    return item->next == &head->item ? NULL : item->next;
}

static inline unsigned int type_dlist_count(dlist_head_t *head)
{
    return head ? head->count : 0;
}

#endif
/* __TYPE_DLIST_H__ end */