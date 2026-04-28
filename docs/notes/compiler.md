# compiler

compiler相关的代码位于[compiler](../../lib/include/plat/compiler.h)，包含一些编译属性相关的

## __attribute__

`__attribute__((attr))`是GCC和Clang编译器提供的关键字，用于传达额外的指令，作用是告诉编译器，按照额外的特殊方式来处理变量、函数或类型

可以组合起来使用，语法`__attribute__((attr0, attr1, attr2...))`

常见使用以下属性：

- `always_inline`：强制内联
- `unused`：未使用，即使代码里没有调用，编译器也不会报错
- `pure`：纯函数，表示函数不会去修改任何全局变量、静态变量、不会IO操作，返回结果只与输入有关。一般get、dump可以使用

## offsetof

```C
#define offsetof(type, member) ((size_t) &((type *)0)->member)
```

其中，将`0`强制转换为`type*`指针，并且使用`&`来读取他的`member`成员的地址。对于编译器而言不会有非法解引用的报错，而是得到`member`的地址，那也就是相对于`member`相较于`type*`的偏移量

## container_of

```C
#define container_of(ptr, type, member) \
    ({ \
        const typeof(((type *)0)->member) *__mptr = (ptr); \
        (type *)((char *)__mptr - offsetof(type, member)); \
    })
```

这里首先是定义了一个指针`__mptr`，类型是通过`typeof()`获取到的`member`成员的类型

然后将`__mptr`指向传入的`ptr`，再减掉偏移量，推导出对应的`type`变量的首地址