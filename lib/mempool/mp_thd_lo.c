/*
 * Copyright (C) cai<sybstudy@yeah.net>. All rights reserved.
 *
 * @file    mp_thd_lo.c
 * @brief   线程独立内存实现
 *
 * @author  cai<sybstudy@yeah.net>
 * @date    2026-05-09
 * @version 1.0
 *
 * @note    
 *
 * @history
 *   1.0 | 2026-05-09 | cai | Initial creation.
 */

/* ========================================================================== */
/*                               Include Files                                */
/* ========================================================================== */

#include "mempool/mp.h"
#include "mempool/mp_thd_lo.h"

/* ========================================================================== */
/*                             Type Definitions                               */
/* ========================================================================== */

// 预定义mp_thd_lo_ml哈希表类型，用于存储线程自己的mem list
pre_declare_hash(mp_thd_lo_ml)

// 定义线程内部持有的空闲内存链表
typedef struct{
    mp_thd_lo_ml_hash_item_t item;  // 哈希表item
    mtype_attr_t *attr;             // 内存类型
    union{
        fixed_mem_dlist_head_t fixed_head;  // 固定大小内存链表头
    } thd_mem_list;
}mp_thd_lo_ml_t;

// 定义线程内部持有的各种记录结构
typedef struct{
    mp_thd_lo_ml_hash_head_t mem_dlists;     // 线程持有的空闲链表，哈希表头
    // TODO 补充
    mp_thd_lo_mq_t *mq;
}mp_thd_lo_items_t;

/* ========================================================================== */
/*                          Global/Static Variables                           */
/* ========================================================================== */

// 全局记录线程内部持有的各种items
static thread_local mp_thd_lo_items_t s_thd_lo_items = {};

/* ========================================================================== */
/*                           Static Function Prototypes                       */
/* ========================================================================== */

/**
 * @brief       cmp two mp_thd_lo_ml
 *
 * @param[in]   ml1
 * @param[in]   ml2
 * 
 * @retval      compare result
 * 
 * @note        cmp by attr name
 */
static attr_pure int mp_thd_lo_ml_cmp(mp_thd_lo_ml_t *ml1, mp_thd_lo_ml_t *ml2)
{
    assert(ml1 && ml2 && ml1->attr && ml2->attr && ml1->attr->name && ml2->attr->name);
    return strcmp(ml1->attr->name, ml2->attr->name);
}

/**
 * @brief       hash mp_thd_lo_ml
 *
 * @param[in]   ml
 * 
 * @retval      hash value
 * 
 * @note        hash by attr name
 */
static attr_pure unsigned int mp_thd_lo_ml_hash(mp_thd_lo_ml_t *ml)
{
    assert(ml && ml->attr && ml->attr->name);
    return type_hash_jhash(ml->attr->name, strlen(ml->attr->name), 0);
}

/**
 * @brief       add mem list into global dlist head
 *
 * @param[in]   ml
 * 
 * @note        将memlist添加到全局记录的hash head中
 */
static attr_force_inline void mp_thd_lo_mem_dlist_add(mp_thd_lo_ml_t *dl);

/* ========================================================================== */
/*                             Macro Definitions                              */
/* ========================================================================== */

#define MP_THD_LO_ML_HASH_MIN_SHIFT (3)
#define MP_THD_LO_ML_HASH_MAX_SHIFT (10)

// 定义mp_thd_lo_ml哈希表操作
declare_hash(mp_thd_lo_ml, mp_thd_lo_ml, mp_thd_lo_ml_t, item,
    MP_THD_LO_ML_HASH_MIN_SHIFT, MP_THD_LO_ML_HASH_MAX_SHIFT,
    mp_thd_lo_ml_cmp, mp_thd_lo_ml_hash)

/* ========================================================================== */
/*                         Function Implementations                           */
/* ========================================================================== */

static inline void mp_thd_lo_mem_dlist_add(mp_thd_lo_ml_t *dl)
{
    mp_thd_lo_ml_hash_add(&s_thd_lo_items.mem_dlists, dl);
}

/**
 * @brief       create new mp_thd_lo_ml_t type variable
 *
 * @param[in]   attr
 * 
 * @retval      ptr to variable
 * 
 * @note        根据attr创建一个新的mp_thd_lo_ml_t变量，初始化内部持有的链表头
 */
static mp_thd_lo_ml_t* mp_thd_lo_mem_list_new(mtype_attr_t *attr)
{
    mp_thd_mem_type_e type = 0;
    mp_thd_lo_ml_t *ml = NULL;

    ml = mp_normal_calloc(1, sizeof(mp_thd_lo_ml_t));   // 分配ml结构的内存
    assert(ml);

    ml->attr = attr;
    type = mtype_attr_thd_mem_type(attr);   // 获取对应的类型
    switch(type)
    {
        case MP_THD_MEM_FIXED:      // 固定大小
            mp_fixed_mem_dlist_init((fixed_mem_dlist_head_t*)&ml->thd_mem_list);
            break;
        case MP_THD_MEM_NONFIXED:   // 不固定大小
            // TODO
            break;
        default:
            break;
    }

    return ml;
}

void *mp_thd_lo_mem_dlist_find(void *attr)
{
    mp_thd_lo_ml_t key = {
        .attr = attr,
    };
    mp_thd_lo_ml_t *ml = NULL;

    ml = mp_thd_lo_ml_hash_find(&s_thd_lo_items.mem_dlists, &key);  // 从全局记录中查找
    return ml ? &ml->thd_mem_list : NULL;
}

void* mp_thd_lo_mem_dlist_find_or_create(void *attr)
{
    void *mem_list = NULL;

    mem_list = mp_thd_lo_mem_dlist_find(attr);  // 查找有无记录
    if(!mem_list)   // 没有的话就创建
    {
        mp_thd_lo_ml_t *ml = NULL;
        ml = mp_thd_lo_mem_list_new(attr);      // 创建新的，内部实际分配内存
        mp_thd_lo_mem_dlist_add(ml);            // 加入到哈希表中
        mem_list = &ml->thd_mem_list;
    }

    return mem_list;
}

unsigned short mp_thd_lo_tid(void)
{
    return ATOM_LOAD(&s_thd_lo_items.mq->tid, MORDER_RELAXED);
}