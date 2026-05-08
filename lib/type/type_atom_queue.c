/*
 * Copyright (C) cai<sybstudy@yeah.net>. All rights reserved.
 *
 * @file    type_atomic_queue.c
 * @brief   通用原子队列实现
 *
 * @author  cai<sybstudy@yeah.net>
 * @date    2026-05-08
 * @version 1.0
 *
 * @note    
 *
 * @history
 *   1.0 | 2026-05-08 | cai | Initial creation.
 */

/* ========================================================================== */
/*                               Include Files                                */
/* ========================================================================== */

#include "type/type_atom_queue.h"

/* ========================================================================== */
/*                         Public Function Implementations                    */
/* ========================================================================== */

void type_spsc_atom_queue_push(spsc_atom_queue_head_t *head, spsc_atom_queue_item_t *item)
{
    uintptr_t prev_next = 0;

    if(0 == ATOM_FETCH_ADD(&head->count, 1, MORDER_RELAXED))
        type_spsc_atom_queue_init(head);

    ATOM_STORE(&item->next, ATOM_PTR_NULL, MORDER_RELAXED);
    prev_next = ATOM_XCHG(&head->last_next, ATOM_PTR2UNIT(&item->next), MORDER_ACQ_REL);
    ATOM_STORE((ATOMIC_UINTPTR_T*)ATOM_UINT2PTR(prev_next), ATOM_PTR2UNIT(item), MORDER_RELEASE);
}

spsc_atom_queue_item_t* type_spsc_atom_queue_pop(spsc_atom_queue_head_t *head)
{
    uintptr_t first = 0;
    uintptr_t expect = 0;
    uintptr_t next = 0;
    spsc_atom_queue_item_t *it = NULL;
    unsigned int n = 0;

    first = ATOM_LOAD(&head->first_item.next, MORDER_ACQUIRE);
    if(!first)
        return NULL;

    expect = first;
    it = ATOM_UINT2PTR(first);
    n = ATOM_SUB_FETCH(&head->count, 1, MORDER_RELAXED);
    if(n)
    {
        do
        {
            next = ATOM_LOAD(&it->next, MORDER_ACQUIRE);
        }while(!next);
    }

#if 0
    while(!ATOM_CMP_XCHG_WEAK(&head->first_item.next, &expect, next, MORDER_ACQ_REL, MORDER_RELAXED))
    {
        if(expect != first)
            break;
    }
#endif

    ATOM_STORE(&head->first_item.next, next, MORDER_RELEASE);

    return it;
}