/*
 * Copyright (C) cai<sybstudy@yeah.net>. All rights reserved.
 *
 * @file    mp.h
 * @brief   内存池头文件
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

#ifndef __MP_H__
#define __MP_H__

/* ========================================================================== */
/*                               Include Files                                */
/* ========================================================================== */

#include <stdlib.h>
#include "type/type_dlist.h"
#include "type/type_hash.h"
#include "type/type_skiplist.h"
#include "mempool/mp_thd_lo.h"
/* ========================================================================== */
/*                             Macro Definitions                              */
/* ========================================================================== */

// 预定义mtype_attr哈希表类型
pre_declare_hash(mtype_attr)
// 预定义fixed_mem双链表类型，用于链接空闲内存节点
pre_declare_dlist(fixed_mem)
// 预定义fixed_mem_free双链表类型，用于链接待释放的内存节点
pre_declare_dlist(fixed_mem_free)

// 内部使用，定义了一个mtype_attr_t类型变量，存储内存属性，然后对其初始化，加入记录双链表
#define _declare_mtype(_name, _nsize, _num, _flag)  \
static mtype_attr_t _mtype_ ## _name = {    \
    .name = "m_type_" # _name,  \
    .flag = _flag,  \
    .nszie = _nsize,    \
    .max_free_num = num,    \
};  \
static void _mtype_ ## _name ## _init(void) attr_ctor(CTOR_MID_MID);   \
static void _mtype_ ## _name ## _init(void) \
{ \
    mtype_attr_init(&_mtype_ ## _name); \
} \
/* _declare_mtype end */

// 给外部使用，定义一种固定大小的内存类型
#define declare_mtype_st_fixed(name, nsize, num)    \
    _declare_mtype(name, nsize, num, MTYPE_ATTR_NODE_SIZE_FIXED)    \
/* declare_mtype_st_fixed end */

// 给外部使用，为name这种静态固定大小的内存类型，预分配、初始化一组空闲内存节点
#define mtype_st_init(name) \
    mtype_attr_st_fixed_mem_list_create(&_mtype_ ## _name); \
/* mtype_st_init init */

// 指定内存类型，从内存池获取固定大小的内存
#define mp_fixed_node_get(name) \
    _mp_fixed_node_get("m_type_" # name)    \
/* mp_fixed_node_get end */

/* ========================================================================== */
/*                             Type Definitions                               */
/* ========================================================================== */

// 内存类型的定义
typedef struct{
    const char *name;   // 内存类型名称
    unsigned int flag;  // 标志位
    unsigned nsize;     // 内存大小
    unsigned int max_free_num;      // 最大空闲个数
#ifdef MEM_USAGE_STAT
    unsigned int mem_total;
    unsigned int mem_total_num;
    unsigned int mem_used_user;
    unsigned int mem_used_sys;
    unsigned int mem_used_num;
    unsigned int mem_free;
    unsigned int mem_free_num;
    unsigned int mem_frozen;
    unsigned int mem_frozen_num;
#endif
    mtype_attr_hash_item_t item;    // 哈希表item
}mtype_attr_t;

// 固定大小内存节点定义
typedef struct{
    union{
        fixed_mem_dlist_item_t dlist_item;      // 空闲双链表item
        fixed_mem_spsc_atom_queue_item_t aq_item;   // 跨线程释放的原子队列item
    } u_item;   // 内存节点同一时刻只会有上述两种状态之一，因此可以复用item
    fixed_mem_free_dlist_item_t free_dlist_item;    // 待释放双链表item
    mtype_attr_t *owner;        // 关键：所属的内存类型
    unsigned int size;          // 内存大小
    unsigned short free_sec;    // 释放时长
    unsigned short tid;         // 分配的线程ID，用于识别是否跨线程释放
    char attr_aligned(8) data[];    // 柔性数组，指向用户使用的内存起始
}attr_aligned(8) fixed_mem_node_t;

// 节点内存属性枚举
typedef enum{
    MTYPE_ATTR_NODE_SIZE_FIXED = 1 << 0,    // 节点大小固定
    MTYPE_ATTR_DYNAMIC = 1 << 1,            // 动态?
}mtype_attr_flag_e;

// 线程持有内存属性枚举
typedef enum{
    MP_THD_MEM_FIXED = 0,
    MP_THD_MEM_NONFIXED = 1,
}mp_thd_mem_type_e;

/* ========================================================================== */
/*                           Function Prototypes                              */
/* ========================================================================== */

/**
 * @brief       init mtype attr
 *
 * @param[in]   attr
 * 
 * @note        将attr加入到全局哈希表中
 */
extern void mtype_attr_init(mtype_attr_t *attr);

/**
 * @brief       create and init memory list for static fixed memory type!
 *
 * @param[in]   attr
 * 
 * @note        重要函数，创建并初始化内存链表，给attr这种内存类型
 */
extern void mtype_attr_st_fixed_mem_list_create(mtype_attr_t *attr);

/**
 * @brief       init fixed_mem_dlist_head_t variable
 *
 * @param[in]   head
 * 
 * @note        封装dlist_init，以便mp_thd_lo.h也能用
 */
extern void mp_fixed_mem_dlist_init(fixed_mem_dlist_head_t *head);

#ifdef MEM_USAGE_STAT

static attr_force_inline void mp_stat_mem_total_grow(mtype_attr_t *attr, unsigned int size)
{
    // TODO
}

static attr_force_inline void mp_stat_fixed_mem_free_grow(fixed_mem_node_t *node)
{
    // TODO
}

#else

#define mp_stat_mem_total_grow(m, s)    
#define mp_stat_fixed_mem_free_grow(n)  

#endif

/**
 * @brief       check if mem node type is fixed
 *
 * @param[in]   attr
 * 
 * @retval      1 - fixed, 0 non-fixed
 */
static attr_force_inline int mtype_attr_node_size_fixed(mtype_attr_t *attr)
{
    return attr->flag & MTYPE_ATTR_NODE_SIZE_FIXED;
}

/**
 * @brief       get mp_thd_mem_type_e from mtype attr flag
 *
 * @param[in]   attr
 * 
 * @retval      mp_thd_mem_type_e
 * 
 * @note        根据内存类型属性，获取对应的线程独立内存类型
 */
static attr_force_inline mp_thd_mem_type_e mtype_attr_thd_mem_type(mtype_attr_t *attr)
{
    return mtype_attr_node_size_fixed(attr) ? MP_THD_MEM_FIXED : MP_THD_MEM_NONFIXED;
}

/**
 * @brief       calloc mem from sys
 *
 * @param[in]   attr    - mtype attr
 * @param[in]   num     - block num
 * @param[in]   size    - block size
 * 
 * @retval      ptr to blank memory
 * 
 * @note        真正调用了calloc
 */
static attr_force_inline void* mp_calloc(mtype_attr_t *attr, size_t num, size_t size)
{
    mp_stat_mem_total_grow(attr, size*num); // 记录数据
    return calloc(num, size);       // 真正分配内存
}

/**
 * @brief       mp normal calloc, non attr
 *
 * @param[in]   num     - block num
 * @param[in]   size    - block size
 * 
 * @retval      ptr to blank memory
 */
static attr_force_inline void* mp_normal_calloc(size_t num, size_t size)
{
    return mp_calloc(NULL, num, size);
}

extern void* _mp_fixed_node_get(const char *name);

#endif
/* __MP_H__ end */