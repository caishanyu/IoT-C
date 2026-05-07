/*
 * Copyright (C) cai<sybstudy@yeah.net>. All rights reserved.
 *
 * @file    trie.h
 * @brief   字典树头文件
 *
 * @author  cai<sybstudy@yeah.net>
 * @date    2026-05-06
 * @version 1.0
 *
 * @note    
 *
 * @history
 *   1.0 | 2026-05-06 | cai | Initial creation.
 */

#ifndef __TRIE_H__
#define __TRIE_H__

/* ========================================================================== */
/*                               Include Files                                */
/* ========================================================================== */

#include "plat/compiler.h"

/* ========================================================================== */
/*                             Type Declaration                               */
/* ========================================================================== */

// 终止节点callback
typedef void* (*trie_item_cb_func)(void *args);

// 外部可见前缀树管理结构
typedef struct trie_head_s trie_head_t;

/* ========================================================================== */
/*                           Function Prototypes                              */
/* ========================================================================== */

/**
 * @brief       create trie
 *
 * @retval      ptr to trie head, NULL - create fail
 * 
 * @note       
 */
extern trie_head_t* trie_create();

/**
 * @brief       destroy trie
 *
 * @param[in]   trie_head       head of trie
 * 
 * @note       
 */
extern void trie_destroy(trie_head_t* trie_head);

/**
 * @brief       dump a trie
 *
 * @param[in]   trie_head       head of trie
 * 
 * @note       
 */
extern void trie_dump(trie_head_t* trie_head);

/**
 * @brief       add line into trie
 *
 * @param[in]   trie_head       head of trie
 * @param[in]   line            a line
 * @param[in]   cb_func         callback function
 * @param[in]   helper          help string
 * 
 * @note       
 */
extern void trie_add(trie_head_t* trie_head, const char *line, trie_item_cb_func cb_func, const char *helper);

/**
 * @brief       find line from trie
 *
 * @param[in]   trie_head       head of trie
 * @param[in]   line            a line
 * 
 * @retval      find result, 1 - found, 0 - NOT found
 * 
 * @note       
 */
extern attr_pure int trie_find(trie_head_t* trie_head, const char *line);

#endif
/* __TRIE_H__ end */