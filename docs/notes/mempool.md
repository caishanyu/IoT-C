# memory pool

此项目实现一个内存池。值得记录的内容还是不少，这里详细记录一下，也方便自己日后复习巩固

## 实现细节

`mtype_attr_t`结构体描述了memory type attr，用户在其中描述内存类型的名称、标志（固定大小or动态）、每个内存块的大小和最大内存块的数量

系统使用一个全局哈希表`s_attr_hash_head`，把用户声明的所有类型链接起来

1. `_declare_mtype`，这个宏内部使用，定义了一个`mtype_attr_t`类型变量，然后定义一个构造函数，将这个变量加到全局哈希表中存储起来
2. 基于上述宏，封装`declare_mtype_st_fixed`给外部使用，用来定义一类固定大小（static fixed）的内存类型
3. 在有了内存类型后，定义了一个宏`mtype_st_init`，这个宏很重要，它创建并初始化了一组内存节点，给这个mtype类型使用

`fixed_mem_node_t`结构体描述了固定大小内存的节点，其中包含我们管理用的头部+用户真正使用的内存

```C
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
```

`mp_thd_lo_ml_t`结构体（mempool thread local memory list），定义了线程本地内存链表的情况，作为一个哈希表item，统一到哈希表中管理。里边存储的是线程持有的内存链表

`s_thd_lo_items`全局变量，记录了本线程持有的内存情况，其中包含空闲链表哈希表等。这个变量被声明为`thread_local`，意味着每个线程独立持有的一份，因而线程使用时无需加锁

`mp_thd_lo_mq_t`结构，包含了线程与其它线程进行交互的各个链表。其中使用逻辑tid和is_running标志来区分对内存节点的操作

