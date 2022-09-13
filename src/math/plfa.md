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

``` text
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

# `products` at Part2.Bisimulation

题面: 证明两个解构操作`proj₁`和`proj₂`的集合和一个解构操作`case×`之间存在双模拟.

我们可以选择用`case×`来模拟`proj₁`和`proj₂`, debruijn表示下的模拟如下:

``` agda
-- 我们知道, `case×`的签名是`Γ ⊢ A `× B → Γ , A , B ⊢ C → Γ ⊢ C`
proj₁ M = case× (M †) (` (S Z))
proj₂ M = case× (M †) (` Z)
```

它们之间的每个化约的模拟关系大概都比较显然, 不论是正向还是反向.
同时, 注意到, 这些模拟的化约并非正文里那样在通过变换已有的化约得到新的化约,
更多的是在已有化约的结果之后加上一步新的化约;
所以, 这里的化约不是一对一的, 在两边的模拟中, 每一步的化约都会对应另一边的多步化约.

具体的证明思路的话, 简单地抄一下上面的思路, 再枚举一下构造, 利用归纳法就可以了.

## 不可行的另一种模拟思路

plfa里首先给出的是另一种模拟方法:

``` agda
case× L [⟨ x , y ⟩⇒ M ] = let z = L † ; x = proj₁ z ; y = proj₂ z in M †
                        = (λ z ⇒ (λ x y ⇒ M †) · proj₁ z · proj₂ z) · L †
```

然而这种模拟方式无法构成双模拟, 甚至连正向模拟也有问题:
plfa里的语言无法引入`z`这个新变量, 所以正向模拟的时候无法写出对应的表达式;
另外, 反过来的时候, lambda表达式里的每次应用并不能对应到`case×`这个表达式里的一个化约序列 ---
下面的lambda表达式的确就比上面的`case×`粒度更小.

# Part2.Untyped

这里说的是最后的几道习题... (超小声(

## 关于`progress`, 和`μ`的无限化约

我们知道, 不动点组合子`μ`化约后的式子里会出现自己(指`μ f →ᵦ f (μ f)`),
所以使用了它的表达式都存在一条无限长的化约路径.

不幸的是, 对于`progress`函数, 存在一种看起来很自然的实现方式:

``` agda
progress : ∀ { Γ A } ( M : Γ ⊢ A ) → Progress M
progress (` x) = done (′ (` x))
progress (ƛ M) with progress M
...               | step M→ᵦM′ = step (ζ M→ᵦM′)
...               | done NM = done (ƛ NM)
progress (L · M) with progress L
...                 | step L→ᵦL′ = step (ξ₁ L→ᵦL′)
...                 | done (ƛ NL) = step β
...                 | done (′ x) with progress M
...                                 | step M→ᵦM′ = step (ξ₂ M→ᵦM′)
...                                 | done NM = done (′ x · NM)
```

这一实现也并没有问题, 但不幸的是, 因为求值顺序的缘故, 它会在下面的习题里一头撞进无穷的化约路径里.

我们考虑`eval`的实现: 反复试图使用`progress`化约表达式, 直到步数耗尽或者表达式不可约.
不幸的是, 这里的这一版本每次都试图优先化约最左边的可以化约的表达式;
这样, 在试图化约一个带不动点组合子的函数应用时, 组合子的部分总是会被优先化约, 从而陷入无穷的化约路径.

相反, plfa里使用的版本长这样 (这似乎也是haskell的求值逻辑):

``` agda
progress : ∀ { Γ A } ( M : Γ ⊢ A ) → Progress M
progress (` x) = done (′ (` x))
progress (ƛ M) with progress M
...               | step M→ᵦM′ = step (ζ M→ᵦM′)
...               | done NM = done (ƛ NM)
progress ((` x) · M) with progress M
...                     | step M→ᵦ = step (ξ₂ M→ᵦ)
...                     | done NM = done (′ (` x) · NM)
progress ((ƛ L) · M) = step β
progress (L@(_ · _) · M) with progress L
...                         | step L→ᵦL′ = step (ξ₁ L→ᵦL′)
...                         | done (′ NL) with progress M
...                                      | step M→ᵦM′ = step (ξ₂ M→ᵦM′)
...                                      | done NM = done (′ (NL · NM))
```

这样, 在试图化约一个表达式时, 这一版本会先试图解决最顶层的可以化约的表达式, 这样, 藏在表达式深处的组合子就有机会被消除掉.

TODO: 似乎ttfp里也有相关的习题, 要求证明,
如果表达式在这一化约策略下仍陷入了无穷的化约路径, 则这一表达式不存在有限的化约路径.

一个口胡的证明是, 观察最顶层的表达式, 如果它可以化约的话,
那么既然表达式的化约是局部的, 所有化约策略下都应该包含这一化约步骤;
如果它不可以化约, 注意到, 化约右边的子表达式并不会让上一层的表达式变得可以化约,
但化约左边的子表达式可以, 并且, 只要左边的子表达式还可以化约, 就还存在进一步的化约步骤,
所以, 先化约左边的子表达式不会带来更多的化约步骤.
这样, 这一化约策略一定会构造出最短的化约路径.

## 只要结果不要路径: 另一种`eval`

(虽然这个是上一节里提到的那个失败的结果... (超小声(

我们看`Steps`的定义:

``` agda
data Steps : ∀ { Γ A } → Γ ⊢ A → Set where
  steps : ∀ { Γ A } { L M : Γ ⊢ A } → L ↠ M → Finished M → Steps L
```

这里包含了两个部分: `L`化约到`M`的所有步骤, 以及求值在`M`上结束了的证明.

然而很多时候, 我们其实不需要这里面的所有步骤, 只是想查看和比较结果, 并且我们知道表达式一定会停机.
这时候, 我们可以直接不管那些, 直接定义一个只包含化约结果的结果类型. 当然, 已经写好了的东西还是复用一下的好:

``` agda
data EvalRes : Set where
  result : ∀ { Γ A } { M : Γ ⊢ A } → Finished M → EvalRes
eval-res : ∀ { Γ A } → Gas → ( L : Γ ⊢ A ) → EvalRes
eval-res (gas zero) L = result {M = L} out-of-gas
eval-res (gas (suc m)) L with progress L
...                         | done NL = result (done NL)
...                         | step {M} L→ᵦL′ with eval-res (gas m) M
...                                             | result fin = result fin
```

这样, 在下面的习题里, 我们就可以开心地用`eval-res`直接求值表达式, 不用想着怎么从`Steps`里拿到求值的结果了.

# Part2.Substitution

~~一句话: 放弃万恶的类型标注, 反正也就一种类型 x (超小声(被打死((((((~~

认真地说... 很多地方自然可以有类型标注... 不过为此我们需要改动一些Untyped一章里的内容... (超小声(
但... 反正也是UTLC, 类型写了也几乎没有什么作用... 所以... 照着PLFA的来, 其它的摸鱼 x (超小声(被打死((((((

以及, 得说的一句是, 满屏的rewrite看着确实很舒服, 写起来也很舒服, 但读起来确实一股非人类的味道... (超小声(

