---
vim: nofoldenable
title: plfa笔记
description: 大概是读plfa时候qwq到的一点东西
...

# 杂项

就是一些可能用到但也不知道放哪的东西, 索性都丢这算了. (超小声(

## 启用Agda的输入法

senioria是用的fcitx5做全局的输入法, 然后在各个编辑器里都稍微配置了下.
但要输入Agda里的众多Unicode字符的话, 那自然还是直接用agda-mode自带的输入法香.

当然, 这两个并不冲突, 在Emacs中配置fcitx5属于其它话题, 在非Agda的文件里启用Agda的输入法的话:

``` { .text .number-lines }
M-: (require 'agda-input)
(insert mode) C-\ Agda
```

(senioria某种程度上把Emacs当Vim在用: 要用的时候拉起一个进程, 写完了就关掉... (超小声(

# `double-subst` at Part2.More

题面:

``` agda
double-subst : ∀ {Γ A B C} {V : Γ ⊢ A} {W : Γ ⊢ B} (N : Γ , A , B ⊢ C)
               → N [ V ][ W ] ≡ (N [ rename S_ W ]) [ V ]
```

(TODO: 据cld说, 这是个错题 x (超小声(

但其实我们很明显地可以看到, 这个命题本身是对的:
等号左边是一次替换掉context最上层的两个变量, 右边是依次替换掉两个变量.
那么因为这两个表达式做的都只是替换这两个变量, 为了证明它们相等, 我们只需要证明它们替换掉的变量一样即可.
同时, 根据替换的性质, 我们可以知道, 唯一可能产生区别的地方是, 左边替换的`W`中的内容不会被`V`替换, 而右边可能.
又, 注意到左边的`rename S_ W`提升了`W`中每一个变量在context中的深度, 而替换`V`的时候只替换栈顶的变量,
所以, `V`不会替换`W`替换产生的变量, 等式成立.

那么, 等有能力了再来形式化地证 x (超小声(被打死((((((

