/*
 * Copyright (C) cai<sybstudy@yeah.net>. All rights reserved.
 *
 * @file    type_skiplist.c
 * @brief   通用跳表定义实现
 *
 * @author  cai<sybstudy@yeah.net>
 * @date    2026-05-06
 * @version 1.0
 *
 * @note    skiplist结构belike:
 *          level-2                 40          70
 *          level-1     10          40          70
 *          level-0     10  20  30  40  50  60  70  80
 *
 * @history
 *   1.0 | 2026-05-06 | cai | Initial creation.
 */

/* ========================================================================== */
/*                               Include Files                                */
/* ========================================================================== */

#include <stdlib.h>
#include "type/type_skiplist.h"

/* ========================================================================== */
/*                             Macro Definitions                              */
/* ========================================================================== */

#define RAND2LEVEL(r)   (__builtin_ctz(r)+1)        // __builtin_ctz获取二进制中末尾连续0的长度，天然满足跳表所需要的概率分布

// 在level-1层中，从prev开始搜索item，由于链表递增，所以如果本层找到第一个比他大，那么肯定在下一层
/**
 * 例如level-1，要添加25。通过这个宏搭配外层while自上而下，将prev更新为level-1的10，next更新为level-1的40，level更新为1
 *          level-2                 40
 *          level-1     10          40          70
 *          level-0     10  20  30  40  50  60  70  80
 */
#define SL_FIND_EQ(item, prev, next, level, cmp_func)   \
do  \
{   \
    int cmp_val = 0;    \
    next = type_skiplist_level_next_get(prev, level-1); \
    if(!next)   \
    {   \
        level --;   \
        break;      \
    }   \
    cmp_val = cmp_func(next, item); \
    if(cmp_val < 0) \
    {   \
        prev = next;    \
        break;  \
    }   \
    if(0 == cmp_val)    \
        return next;    \
    level --;   \
}while(0)   \
/* SL_FIND_EQ end */

/* ========================================================================== */
/*                         Private Function Implementations                   */
/* ========================================================================== */

/**
 * @brief       generate a random value
 * 
 * @retval      random value
 */
static attr_force_inline long type_skiplist_weak_random(void)
{
    return random();
}

/**
 * @brief       calculate a random level for skiplist item.
 * 
 * @retval      random level in range of [1, SKIPLIST_DEPTH_MAX]
 * 
 * @note        计算出的Level从1开始，但是next数组索引从0开始
 */
static attr_force_inline int type_skiplist_random_level_cal(void)
{
    short rand = 0;
    int level = 0;

    rand = type_skiplist_weak_random();
    level = rand == 0 ? 1 : RAND2LEVEL(rand);

    return level > SKIPLIST_DEPTH_MAX ? SKIPLIST_DEPTH_MAX : level;
}

/**
 * @brief       get skiplist item 's next of level.
 * 
 * @param[in]   item    skiplist item
 * @param[in]   level   level of skiplist item
 * 
 * @retval      item->next[level]
 */
static attr_pure_inline skiplist_item_t *type_skiplist_level_next_get(skiplist_item_t *item, int level)
{
    return item->next[level];
}

/**
 * @brief       set skiplist item 's next of level.
 * 
 * @param[in]   item    skiplist item
 * @param[in]   level   level of skiplist item
 * @param[in]   value   next value
 *  
 * @note        设置item的next[level]值
 */
static attr_force_inline void type_skiplist_level_next_set(skiplist_item_t *item, int level, skiplist_item_t *value)
{
    item->next[level] = value;
}

/* ========================================================================== */
/*                         Public Function Implementations                    */
/* ========================================================================== */

skiplist_item_t* type_skiplist_add(skiplist_head_t *head, skiplist_item_t *item, skiplist_item_cmp_func cmp_func)
{
    int new_level = 0;
    int level = SKIPLIST_DEPTH_MAX;
    skiplist_item_t *prev = NULL;
    skiplist_item_t *next = NULL;

    assert(cmp_func);

    new_level = type_skiplist_random_level_cal();       // 随机获取一个level

    prev = &head->skiplist_head;
    while(level >= new_level)   // 从最高层往下搜索，直到找到第一个插入位置
        SL_FIND_EQ(item, prev, next, level, cmp_func);
    
    memset(item, 0, sizeof(*item));
    // 插入本层（目的最高层）
    type_skiplist_level_next_set(item, level, next);
    type_skiplist_level_next_set(prev, level, item);

    // 完成低层插入，每一层都可以从上一层获取的prev节点开始查找，无需从头开始
    while(level)
    {
        level --;
        next = type_skiplist_level_next_get(prev, level);
        while(next && cmp_func(next, item) < 0)
        {
            prev = next;
            next = type_skiplist_level_next_get(prev, level);
        }

        type_skiplist_level_next_set(item, level, next);
        type_skiplist_level_next_set(prev, level, item);
    }

    head->count ++;

    return NULL;
}

skiplist_item_t* type_skiplist_find_greater_euqal(skiplist_head_t *head, skiplist_item_t *item, skiplist_item_cmp_func cmp_func)
{
    int level = SKIPLIST_DEPTH_MAX;
    skiplist_item_t *prev = NULL;
    skiplist_item_t *next = NULL;

    prev = &head->skiplist_head;
    while(level)
        SL_FIND_EQ(item, prev, next, level, cmp_func);

    return next;
}

skiplist_item_t* type_skiplist_find_less(skiplist_head_t *head, skiplist_item_t *item, skiplist_item_cmp_func cmp_func)
{
    int level = SKIPLIST_DEPTH_MAX;
    skiplist_item_t *prev = NULL;
    skiplist_item_t *next = NULL;
    skiplist_item_t *retval = NULL;

    prev = &head->skiplist_head;

    while(level)
    {
        next = type_skiplist_level_next_get(prev, level-1);
        // next==NULL说明本层找完，继续下一层
        if(!next)
        {
            level --;
            continue;
        }
        // next比item小，那么继续找可能的更大值
        if(cmp_func(next, item) < 0)
        {
            retval = prev = next;
            continue;
        }
        // next比item大，那么去下层找
        -- level;
    }

    return retval;
}

skiplist_item_t* type_skiplist_del(skiplist_head_t *head, skiplist_item_t *item, skiplist_item_cmp_func cmp_func)
{
    int level = SKIPLIST_DEPTH_MAX;
    skiplist_item_t *prev = NULL;
    skiplist_item_t *next = NULL;
    int found = 0;

    prev = &head->skiplist_head;

    while(level)
    {
        next = type_skiplist_level_next_get(prev, level-1);
        // 没有next，那么去下一层找
        if(!next)
        {
            level --;
            continue;
        }
        // 找到item，修改这一层的指针，继续往下查找
        if(item == next)
        {
            skiplist_item_t *nnext = type_skiplist_level_next_get(item, level-1);
            type_skiplist_level_next_set(prev, level-1, nnext);
            found = 1;
            level --;
            continue;
        }

        // next小于item，继续本层查找
        if(cmp_func(next, item) < 0)
        {
            prev = next;
            continue;
        }

        // next大于item，去下层找
        -- level;
    }

    if(!found)
        return NULL;

    memset(item, 0, sizeof(*item));
    head->count --;

    return item;
}