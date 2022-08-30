---
title: 测试文档
author: [Virginia, Senioria]
keywords: [markdown, 测试]
description: 用来测试生成器是否正常的文档
...

这是一个测试文档, 用来检测我的这个生成器~~发育得正不正常~~是否工作正常.

然后, 加个段落用来测试段落支持.

另外, 虽然我大概不会这么干, 但是试一下列表:

- First
- Second
- Third

稀疏列表:

- Sparse Lists

  Paragraphs in sparse lists.

- Another Item

  - Embedded lists
  - Another item

- Last item...

# 在第一个标题之后的内容应该不会被加入brief

加一段代码?(
以及`print(hello)`这样的行内代码.

code `inside` same lang ><

```cpp
// A hello world: Comment
#include <cstdio>

int main()
{
    printf("Hello world! %d\n", 0x5f8700);
    return 0;
}
```

数学: $i + j = k$

$$
\begin{alignedat}{2}
\because{}&& i^2 &= 2\sum_{j=1}^i j - i \\
\therefore{}&& \sum_{i=1}^n i^2 &= 2\sum_{i=1}^n\sum{j=1}^i j - \sum_{i=1}^n i \\
    && &= 2\sum_{i=1}^n i(n + 1 - i) - \sum_{i=1}^n i \\
    && &= 2(n+1)\sum_{i=1}^n i - 2\sum_{i=1}^n i^2 - \sum_{i=1}^n i \\
\therefore{}&& 3\sum_{i=1}^n i^2 &= (2n+1)\sum_{i=1}^n i
\end{alignedat}
$$

# 对元素的测试

## 标题测试
### 三级...
#### 四级...

[链接](https://example.com)

**加粗**
*斜体*
***粗斜体***
~~划去~~
上标?^嗯^

然后... 试一下分割线的样子?(

----

> 引用, 和[引用里的链接](https://example.com)
>
> ---
>
> 这是引用里的一个段落唔... ><
>
> > 引用里嵌入的引用
> >
> > ---
> >
> > (以及嵌入的另一个段落 x

# 对页面的测试

1\
2\
3\
4\
5\
6\
7\
8\
9\
10\
11\
12\
13\
14\
15\
16\
17\
18\
19\
20\
21\
22\
23\
24\
25\
26\
27\
28\
29\
30\
31\
32\
33\
34\
35\
36\
37\
38\
39\
40\
41\
42\
43\
44\
45\
46\
47\
48\
49\
50\
51\
52\
53\
54\
55\
56\
57\
58\
59\
60\
61\
62\
63\
64\

