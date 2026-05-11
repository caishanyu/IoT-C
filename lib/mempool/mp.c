/*
 * Copyright (C) cai<sybstudy@yeah.net>. All rights reserved.
 *
 * @file    mp.c
 * @brief   内存池实现
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

#include "mempool/mp.h"

/* ========================================================================== */
/*                           Function Prototypes                              */
/* ========================================================================== */

/**
 * @brief       cmp two mtype attr
 *
 * @param[in]   attr1
 * @param[in]   attr2
 * 
 * @retval      cmp result
 * 
 * @note        compare by attr->name
 */
static attr_pure int mtype_attr_cmp(mtype_attr_t *attr1, mtype_attr_t *attr2);

/**
 * @brief       hash mtype attr
 *
 * @param[in]   attr
 * 
 * @retval      hash val
 * 
 * @note        hash by attr->name
 */
static attr_pure unsigned int mtype_attr_hash(mtype_attr_t *attr);

/* ========================================================================== */
/*                             Macro Definitions                              */
/* ========================================================================== */

#define MTYPE_ATTR_HASH_MIN_SHIFT   (3)
#define MTYPE_ATTR_HASH_MAX_SHIFT   (10)

// 定义mtype_attr哈希表操作
declare_hash(mtype_attr, mtype_attr, mtype_attr_t, item,    \
    MTYPE_ATTR_HASH_MIN_SHIFT, MTYPE_ATTR_HASH_MAX_SHIFT,   \
    mtype_attr_cmp, mtype_attr_hash)
// 定义fixed_mem双链表操作，用于链接空闲内存节点
declare_dlist(fixed_mem, fixed_mem, fixed_mem_node_t, u_item.dlist_item)

/* ========================================================================== */
/*                          Global/Static Variables                           */
/* ========================================================================== */

// 存储所有内存类型的哈希表
static mtype_attr_hash_head_t s_attr_hash_head = {};

/* ========================================================================== */
/*                         Private Function Implementations                   */
/* ========================================================================== */

static int mtype_attr_cmp(mtype_attr_t *attr1, mtype_attr_t *attr2)
{
    assert(attr1 && attr2 && attr1->name && attr2->name);
    return strcmp(attr1->name, attr2->name);
}

static unsigned int mtype_attr_hash(mtype_attr_t *attr)
{
    assert(attr && attr->name);
    return type_hash_jhash(attr->name, strlen(attr->name), 0);
}

/**
 * @brief       mempool new fixed mem node
 *
 * @param[in]   attr    - mem type attr
 * @param[in]   tid     - logic tid
 * 
 * @retval      ptr to new fixed_mem_node_t
 * 
 * @note        新建固定内存节点
 */
static attr_force_inline fixed_mem_node_t* mp_fixed_mem_node_new(mtype_attr_t *attr, unsigned short tid)
{
    fixed_mem_node_t *mem_node = NULL;

    mem_node = mp_calloc(attr, attr->nsize + sizeof(fixed_mem_node_t), sizeof(char));   // 申请内存
    assert(mem_node);

    mem_node->size = attr->nsize;
    mem_node->tid = tid;
    mem_node->owner = attr;

    return mem_node;
}

/* ========================================================================== */
/*                         Public Function Implementations                    */
/* ========================================================================== */

void mtype_attr_init(mtype_attr_t *attr)
{
    mtype_attr_hash_add(&s_attr_hash_head, attr);
}

void mtype_attr_st_fixed_mem_list_create(mtype_attr_t *attr)
{
    unsigned int short tid = 0; // 线程id
    fixed_mem_dlist_head_t *mem_dlist = NULL;

    // 在线程的mem_dlist记录中找，如果没有就创建
    mem_dlist = mp_thd_lo_mem_dlist_find_or_create(attr);
    // 获取线程tid
    tid = mp_thd_lo_tid();

    // 分配足量的内存节点给空闲链表
    while(fixed_mem_dlist_count(mem_dlist) < attr->max_free_num)
    {
        fixed_mem_node_t *node = NULL;
        node = mp_fixed_mem_node_new(attr, tid);    // 新建固定内存节点
        mp_stat_fixed_mem_free_grow(node);          // 统计数据更新
        fixed_mem_dlist_add_head(mem_dlist, node);  // 添加到空闲内存双链表中
    }
}

void mp_fixed_mem_dlist_init(fixed_mem_dlist_head_t *head)
{
    fixed_mem_dlist_init(head);
}

void* _mp_fixed_node_get(const char *name)
{
    // TODO
    return NULL;
}