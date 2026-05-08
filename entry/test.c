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
#include <pthread.h>
#include <stdlib.h>
#include "type/type_list.h"
#include "type/type_dlist.h"
#include "type/type_hash.h"
#include "type/type_skiplist.h"
#include "plat/atom.h"
#include "type/trie.h"
#include "type/type_atom_queue.h"

/* ========================================================================== */
/*                             Macro Definitions                              */
/* ========================================================================== */

pre_declare_list(test)
pre_declare_dlist(test)
pre_declare_hash(test)
pre_declare_skiplist(test)
pre_declare_spsc_atom_queue(test)

typedef struct test_data_s{
    int data_val;
    test_list_item_t list_item;
    test_dlist_item_t dlist_item;
    test_hash_item_t hash_item;
    test_skiplist_item_t skiplist_item;
    test_spsc_atom_queue_item_t spsc_aq_item;
}test_data_t;

static attr_pure_inline int cmp_test(test_data_t *t1, test_data_t *t2)  { return t1->data_val - t2->data_val; }
static attr_pure_inline int hash_test(test_data_t *t)                   { return t->data_val; }

declare_list(test, test, test_data_t, list_item)
declare_dlist(test, test, test_data_t, dlist_item)
declare_hash(test, test, test_data_t, hash_item, 3, 10, cmp_test, hash_test)
declare_skiplist(test, test, test_data_t, skiplist_item, cmp_test)
declare_spsc_atom_queue(test, test, test_data_t, spsc_aq_item)

#define TEST_LIST_DATA_COUNT    (5)

/* ========================================================================== */
/*                          Function Implementations                          */
/* ========================================================================== */

void test_type_list()
{
    test_list_head_t list_head = {};
    test_data_t data[TEST_LIST_DATA_COUNT] = {};
    int i = 0;

    for( ; i < TEST_LIST_DATA_COUNT; i++)
    {
        data[i].data_val = i;
    }

    test_list_init(&list_head);

    test_list_add_head(&list_head, &data[0]);
    // 0->
    assert(data[0].data_val == test_list_first(&list_head)->data_val);
    test_list_add_head(&list_head, &data[1]);
    test_list_add_head(&list_head, &data[2]);
    // 2->1->0->
    assert(data[2].data_val == test_list_pop(&list_head)->data_val);
    // 1->0->
    assert(data[1].data_val == test_list_pop(&list_head)->data_val);
    // 0->
    test_list_add_after(&list_head, &data[0], &data[3]);
    test_list_add_after(&list_head, &data[0], &data[2]);
    // 0->2->3->
    assert(data[0].data_val == test_list_pop(&list_head)->data_val);
    assert(data[2].data_val == test_list_pop(&list_head)->data_val);
    assert(data[3].data_val == test_list_pop(&list_head)->data_val);
    // NULL
    assert(!test_list_pop(&list_head));
    assert(!test_list_first(&list_head));

    printf("type list test passed.\n");
}

void test_trie()
{
    trie_head_t *trie_head = trie_create();
    assert(trie_head);

    trie_add(trie_head, "hello world test1", NULL, "hello1");
    trie_add(trie_head, "hello world test2", NULL, "hello2");
    trie_add(trie_head, "hello world1 test1", NULL, "hello11");
    trie_add(trie_head, "hello world2 test1", NULL, "hello21");
    trie_add(trie_head, "hello", NULL, "hello only");
    trie_add(trie_head, "hello world", NULL, "hello world only");

    trie_dump(trie_head);

    assert(trie_find(trie_head, "hello world test1"));
    assert(trie_find(trie_head, "hello"));
    assert(trie_find(trie_head, "hello  world "));
    assert(!trie_find(trie_head, "1 hello  world "));
    assert(!trie_find(trie_head, "hello world test1 ccb"));
}

static unsigned int sum = 0;
static test_spsc_atom_queue_head_t spsc_aq = {};

void* test_spsc_producer(void *args)
{
    int i = 1;

    while(i <= 100)
    {
        test_data_t *data = (test_data_t*)malloc(sizeof(test_data_t));
        memset(data, 0, sizeof(test_data_t));
        data->data_val = i;
        test_spsc_atom_queue_push(&spsc_aq, data);
        ++ i;
    }
}

void* test_spsc_consumer(void *args)
{
    int i = 1;

    while(i <= 100)
    {
        test_data_t *data = test_spsc_atom_queue_pop(&spsc_aq);
        if(!data)
            continue;
        else
        {
            sum += data->data_val;
            free(data);
            i ++;
        }
    }
}

void test_spsc_aq()
{
    test_spsc_atom_queue_init(&spsc_aq);

    pthread_t producer, consumer;
    pthread_create(&producer, NULL, test_spsc_producer, NULL);
    pthread_create(&consumer, NULL, test_spsc_consumer, NULL);
    pthread_join(producer, NULL);
    pthread_join(consumer, NULL);
    printf("sum %u\n", sum);
    assert(sum == (1+100)*100/2);
}

void test_all()
{
    test_type_list();

    test_trie();

    test_spsc_aq();

    printf("ALL test passed.\n");
}