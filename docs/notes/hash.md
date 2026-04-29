# hash

链表的代码位于[type_hash.h](../../lib/include/type/type_hash.h)和[type_hash.c](../../lib/type/type_hash.c)

## 技巧

`type_list_add`中有一个标准的无头链表插入的技巧，值得学习一下

1. `head->entries`初始化，数组里边存储的是无头链表的首个节点的指针
2. `hash_item_t **it`是指针的指针
3. `it = &(*it)->next;`这步操作，`it`实际上是改成了指向当前`it`的`next`的指针，`*it`就是`it->next`
4. `*it = item;`，这样修改可以改动到前一个节点的`next`
5. 初始插入时，相当于修改了`head->entries[high_bits]`内存储的值

```C
void type_hash_init(hash_head_t *head, unsigned char min_shift, unsigned char max_shift)
{
    if(max_shift)
        assert(min_shift <= max_shift);

    assert(min_shift >= HASH_MIN_SHIFT && max_shift <= HASH_MAX_SHIFT);

    head->count = 0;
    head->min_shift = head->tab_shift = min_shift;
    head->max_shift = max_shift;

    head->entries = calloc(hash_size(head->tab_shift), sizeof(hash_item_t*));   // TODO: mp
}

hash_item_t* type_hash_add(hash_head_t *head, hash_item_t *item, type_hash_cmp_f cmp_func, type_hash_hash_f hash_func)
{
    unsigned int high_bits = 0;
    hash_item_t **it = NULL;

    assert(head->entries);
    assert(cmp_func && hash_func);

    item->hash_val = hash_func(item);
    high_bits = hash_key(head->tab_shift, item->hash_val);  // 计算放入的桶的索引
    it = &head->entries[high_bits];

    // 找到首个hash_val不小于item的hash_val的节点
    while(hash_val_cmp(*it, item, <))
        it = &(*it)->next;

    // 处理哈希碰撞，跳过所有hash_val一致的item，如果业务数据也一致，那么说明已存在，直接返回
    while(hash_val_cmp(*it, item, ==))
    {
        if(!cmp_func(*it, item))
            return *it;
        it = &(*it)->next;
    }

    // 修改link，item插入到it前边
    item->next = *it;
    *it = item;

    head->count ++;

    return NULL;
}
```

`type_hash_grow`扩容函数的实现也有些小技巧

- 无需为每个节点重新计算哈希，而是利用2次幂哈希的特性
    - 比如从8扩容到16，原本在桶0的，新的哈希桶可能在桶0或者桶0+8，依次类推
    - 利用按照hash_val有序递增，以及使用高shift位作为桶索引的特性，巧妙实现切分移动桶的扩容方式