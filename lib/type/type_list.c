/*
 * Copyright (C) cai<sybstudy@yeah.net>. All rights reserved.
 *
 * @file    type_list.c
 * @brief   通用链表定义实现
 *
 * @author  cai<sybstudy@yeah.net>
 * @date    2026-04-28
 * @version 1.0
 *
 * @note    部分不进行内联的函数实现
 *
 * @history
 *   1.0 | 2026-04-28 | cai | Initial creation.
 */

/* ========================================================================== */
/*                               Include Files                                */
/* ========================================================================== */

#include "type/type_list.h"

/* ========================================================================== */
/*                          Global/Static Variables                           */
/* ========================================================================== */

list_item_t __g_list_end = {NULL};

/* ========================================================================== */
/*                         Public Function Implementations                    */
/* ========================================================================== */

list_item_t *type_list_del(list_head_t *head, list_item_t *item)
{
    list_item_t **it;

    it = &head->first;
    while(*it != __list_end && *it != item)
        it = &((*it)->next);

    if(*it == __list_end)
        return NULL;

    *it = item->next;
    if(*it == __list_end)
        head->lastNext = it;

    item->next = NULL;

    head->count --;

    return item;
}

list_item_t* type_list_pop(list_head_t *head)
{
    list_item_t *it = NULL;

    if(head->first == __list_end)
        return NULL;

    it = head->first;
    head->first = it->next;
    if(head->first == __list_end)
        head->lastNext = &head->first;

    it->next = NULL;
    head->count --;

    return it;
}