# skiplist

跳表的代码位于[type_skiplist.h](../../lib/include/type/type_skiplist.h)和[type_skiplist.c](../../lib/type/type_skiplist.c)

## 跳表

跳表是一类实现简单，性能接近红黑树的数据结构，在Redis中就是使用的跳表

跳表维护多级有序链表，例如：

```C
/**
 *      level 2     1               5
 *      level 1     1       3       5
 *      level 0     1   2   3   4   5   6
*/
```

增删查都是从高层开始查找的，比如查找6，那么在level2中查找1->5，跳转到levl1的5，level0的5，再找到6。相比单级链表，比较的次数减少了

在插入前需要确定层高，一般是越高层概率越低。理论指出，最好是第一层1/2，第二层1/4，第三层1/8，以此类推。实现时使用了一个技巧如下

```C
#define RAND2LEVEL(r)   (__builtin_ctz(r)+1)
```

天然满足上述的概率分布，由于是CPU指令，因此计算更快