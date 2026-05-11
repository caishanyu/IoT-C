/*
 * Copyright (C) cai<sybstudy@yeah.net>. All rights reserved.
 *
 * @file    mp_thd_lo.h
 * @brief   线程独立内存头文件
 *
 * @author  cai<sybstudy@yeah.net>
 * @date    2026-05-09
 * @version 1.0
 *
 * @note    thd_lo      - thread local
 *
 * @history
 *   1.0 | 2026-05-09 | cai | Initial creation.
 */

#ifndef __MP_THD_LO_H__
#define __MP_THD_LO_H__

/* ========================================================================== */
/*                               Include Files                                */
/* ========================================================================== */

#include <pthread.h>
#include "type/type_atom_queue.h"
#include "type/type_list.h"

/* ========================================================================== */
/*                             Type Definitions                               */
/* ========================================================================== */

// 预定义记录线程独立使用的消息队列的链表结构
pre_declare_list(mq_thd_lo_mq)

// 定义线程持有的消息队列mq，用来和别的线程交互
typedef struct{
    char *name; // name
    unsigned short tid;     // 逻辑tid，用于更快速比较
    ATOMIC_UINT8_T is_running;  // 标记线程存活，和tid一块使用
    mq_thd_lo_mq_list_item_t item;  // item
}mp_thd_lo_mq_t;

/* ========================================================================== */
/*                             Macro Definitions                              */
/* ========================================================================== */

// 预定义fixed_mem的原子队列类型，用于给跨线程释放的静态固定大小内存节点
pre_declare_spsc_atom_queue(fixed_mem)

/* ========================================================================== */
/*                           Function Prototypes                              */
/* ========================================================================== */

/**
 * @brief       find mem dlist by attr
 *
 * @param[in]   attr    - mem type attr
 * 
 * @retval      ptr to mem dlist, if exist
 * 
 * @note        在全局记录中搜索attr对应的记录链表
 */
extern void *mp_thd_lo_mem_dlist_find(void *attr);

/**
 * @brief       find mem dlist by attr, if non exist, create it
 *
 * @param[in]   attr
 * 
 * @retval      ptr to mem dlist, if exist
 * 
 * @note        在全局记录中搜索attr对应的记录链表，没有的话，就创建新的加入
 */
extern void* mp_thd_lo_mem_dlist_find_or_create(void *attr);

/**
 * @brief       get mem tid
 * 
 * @retval      logic tid of current thread
 * 
 * @note        获取当前线程的逻辑tid
 */
extern unsigned short mp_thd_lo_tid(void);

#endif