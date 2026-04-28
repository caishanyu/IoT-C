/*
 * Copyright (C) cai<sybstudy@yeah.net>. All rights reserved.
 *
 * @file    compiler.h
 * @brief   编译属性头文件
 *
 * @author  cai<sybstudy@yeah.net>
 * @date    2026-04-28
 * @version 1.0
 *
 * @note    
 *
 * @history
 *   1.0 | 2026-04-28 | cai | Initial creation.
 */

#ifndef __COMPILER_H__
#define __COMPILER_H__

/* ========================================================================== */
/*                             Macro Definitions                              */
/* ========================================================================== */

#define attr_unused         __attribute__((unused))                         // 即使不使用，也不会报错
#define attr_pure           __attribute__((pure))                           // 纯函数：不会修改全局变量、传入指针的内存、无IO操作，返回值仅依赖于函数参数和某些全局/静态变量
#define attr_force_inline   inline __attribute__((unused, always_inline))
#define attr_pure_inline    inline __attribute__((unused, always_inline, pure))

// 获取type类型member的偏移
#ifndef offsetof
#define offsetof(type, member) ((size_t) &((type *)0)->member)
#endif

// 根据指向member成员的ptr指针，反推指向type的指针
#ifndef container_of
#define container_of(ptr, type, member) \
    ({ \
        const typeof(((type *)0)->member) *__mptr = (ptr); \
        (type *)((char *)__mptr - offsetof(type, member)); \
    })
#endif

// 计算数组长度
#ifndef array_size
#define array_size(x)   (sizeof((x))/sizeof((*x)))
#endif

#endif