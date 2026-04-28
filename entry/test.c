/*
 * Copyright (C) cai<sybstudy@yeah.net>. All rights reserved.
 *
 * @file    test.c
 * @brief   自测文件
 *
 * @author  cai<sybstudy@yeah.net>
 * @date    2026-04-28
 * @version 1.0
 *
 * @note    提供测试接口给main函数
 *
 * @history
 *   1.0 | 2026-04-28 | cai | Initial creation.
 */

/* ========================================================================== */
/*                               Include Files                                */
/* ========================================================================== */

#include <stdio.h>
#include "type/type_list.h"

/* ========================================================================== */
/*                             Macro Definitions                              */
/* ========================================================================== */

pre_declare_list(test_list_s)
typedef struct test_data_s{
    int data_val;
    test_list_s_item_t list_item;
}test_data_t;
declare_list(test_list_s, test_list_f, test_data_t, list_item)

#define TEST_LIST_DATA_COUNT    (5)

/* ========================================================================== */
/*                          Function Implementations                          */
/* ========================================================================== */

void test_type_list()
{
    test_list_s_head_t list_head = {};
    test_data_t data[TEST_LIST_DATA_COUNT] = {};
    int i = 0;

    for( ; i < TEST_LIST_DATA_COUNT; i++)
    {
        data[i].data_val = i;
    }

    test_list_f_init(&list_head);

    test_list_f_add_head(&list_head, &data[0]);
    // 0->
    assert(data[0].data_val == test_list_f_first(&list_head)->data_val);
    test_list_f_add_head(&list_head, &data[1]);
    test_list_f_add_head(&list_head, &data[2]);
    // 2->1->0->
    assert(data[2].data_val == test_list_f_pop(&list_head)->data_val);
    // 1->0->
    assert(data[1].data_val == test_list_f_pop(&list_head)->data_val);
    // 0->
    test_list_f_add_after(&list_head, &data[0], &data[3]);
    test_list_f_add_after(&list_head, &data[0], &data[2]);
    // 0->2->3->
    assert(data[0].data_val == test_list_f_pop(&list_head)->data_val);
    assert(data[2].data_val == test_list_f_pop(&list_head)->data_val);
    assert(data[3].data_val == test_list_f_pop(&list_head)->data_val);
    // NULL
    assert(!test_list_f_pop(&list_head));
    assert(!test_list_f_first(&list_head));

    printf("type list test passed.\n");
}


void test_all()
{
    test_type_list();

    printf("ALL test passed.\n");
}