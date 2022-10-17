---
title: 自然推理系统FPC
description: 利用假设的变换和回收完成推理的系统
...

# Premable

```agda
module fpc where

open import Relation.Binary.PropositionalEquality using (_≡_; refl)
open import base
open import Relation.Nullary using (Dec; yes; no)
open import Relation.Nullary.Decidable using (True)
```

# 假设的定义

假设是FPC的核心, 我们在此只在形式上定义假设, 定义假设的扩充和推理规则放到下一节.

```agda
infixl 9 _::_
data Hyposis : Term → Set where
  [] : Hyposis t⊥
  [_] : (x : Term) → Hyposis x
  _::_ : ∀ {x} → Hyposis x → Term → Hyposis x
  _::[_] : ∀ {x y} → Hyposis x → Hyposis y → Hyposis x
```

然后是判断一个式子是否在假设中出现过:

```agda
data InHyp : ∀ {x} → Term → Hyposis x → Set where
  hyp : ∀ {x} {hyp : Hyposis x} → InHyp x hyp
  here : ∀ {x elem} {pre : Hyposis x} → InHyp elem (pre :: elem)
  there : ∀ {x elem tail} {pre : Hyposis x} → InHyp elem pre → InHyp elem (pre :: tail)
  inner : ∀ {x y elem} {pre : Hyposis x} {sub : Hyposis y} → InHyp elem sub → InHyp elem (pre ::[ sub ])
  outer : ∀ {x y elem} {pre : Hyposis x} {sub : Hyposis y} → InHyp elem pre → InHyp elem (pre ::[ sub ])

InHyp? : ∀ {x} (res : Term) (cur : Hyposis x) → Dec (InHyp res cur)
InHyp? res [] with res t≟ t⊥
...              | yes refl = yes hyp
...              | no r⊥ = no λ { hyp → r⊥ refl }
InHyp? res [ x ] with res t≟ x
...                 | yes refl = yes hyp
...                 | no r⊥ = no λ { hyp → r⊥ refl }
InHyp? res (cur :: x) with res t≟ x
...                      | yes refl = yes here
...                      | no res≢x with InHyp? res cur
...                                     | yes incur = yes (there incur)
...                                     | no ¬incur = no λ { hyp → ¬incur hyp ; here → res≢x refl ; (there y) → ¬incur y }
InHyp? res (cur ::[ sub ]) with InHyp? res sub
...                           | yes insub = yes (inner insub)
...                           | no ¬insub with InHyp? res cur
...                                          | yes incur = yes (outer incur)
...                                          | no ¬incur = no λ { hyp → ¬incur hyp ; (inner y) → ¬insub y ; (outer y) → ¬incur y }
```

# 推理规则的定义

我们先定义一些符号, 以及一些我们的定义过程中会频繁用到的隐含变量.
其中, `a ∋ b`表示在假设`a`下, 可以推出假设`b`; `a :> b ⊢ res`表示在假设`a`下, 假设`b`可以推出结论`res`.

```agda
module implication-impl where
  private variable
    x y a b : Term
    base : Hyposis x
    cur : Hyposis y
  infixl 8 _∋_ _:>_⊢_
  data _∋_ : Hyposis x → Hyposis y → Set
  data _:>_⊢_ : Hyposis x → Hyposis y → Term → Set
```

在我们开始定义之前, 我们还要定义一些用于让生活更容易的helpers: 我们把所有推理步骤都变成函数的复合,
然后定义一个推理的起点, 把推理的结论本身变为从这一起点开始的函数;
这样, 我们就可以定义算符, 并且利用do notations完成推理了.
当然这样有滥用语法糖之嫌, 不过, 反正这本身也只是玩具项目 ()(

```agda
  private data ∋⊥ : Set where
  infixl 8 _:>′_⊢_
  _:>′_⊢_ : Hyposis x → Hyposis y → Term → Set
  base :>′ cur ⊢ res = ∋⊥ → base :> cur ⊢ res
  infixl 1 _>>_
  _>>_ : ∀ {A B C : Set} → (A → B) → (B → C) → A → C
  g >> f = λ x → f (g x)
  infix 8 _⊢_
  data _⊢_ : ∀ {x} → Hyposis x → Term → Set where
    imply_ : ∀ {res} → base :>′ cur ⊢ res → base ⊢ res
```

终于, 我们可以开始定义推理了.

当然, 我们这里也可以利用类似debruijn index的方法来定义`rep`和`reit`中的索引,
不过这个就暂时咕咕了吧... x x (超小声(

看样子我们其实可以结合`hyp`和`cont`, 让`hyp`作为一个算符直接同时引入假设和推理步骤
(也即让`hyp x ⟨ y ⟩`变成和这里`cont⟨ do hyp⟨ x ⟩ y ⟩`一样的东西).
但实际上senioria并没有足够的能力完成这样的定义: 如果确实如此定义的话,
`hyp`作为一个构造器无法通过positivity check.

```agda
  data _∋_ where
    hyp⟨_⟩ : ∀ (y) → ∋⊥ → base ∋ [ y ]
    cont⟨_⟩ : ∀ {z res} {pre : Hyposis z} → base ::[ pre ] :>′ cur ⊢ res → base ∋ pre → base ∋ pre :: res
    rep⟨_⟩ : ∀ (res) {incur : True (InHyp? res cur)} → base ∋ cur → base ∋ cur :: res
    reit⟨_⟩ : ∀ (res) {inbase : True (InHyp? res base)} → base ∋ cur → base ∋ cur :: res
    ⇒⁻ :  base ∋ cur :: a ⇒ b :: a → base ∋ cur :: a ⇒ b :: a :: b
    ∨⁺ˡ :  base ∋ cur :: a → base ∋ cur :: a :: a ∨ b
    ∨⁺ʳ :  base ∋ cur :: a → base ∋ cur :: a :: b ∨ a
    ∨⁻ : ∀ {c} → base ∋ cur :: a ⇒ c :: b ⇒ c :: a ∨ b → base ∋ cur :: a ⇒ c :: b ⇒ c :: a ∨ b :: c
    ∧⁺ :  base ∋ cur :: a :: b → base ∋ cur :: a :: b :: a ∧ b
    ∧⁻ˡ :  base ∋ cur :: a ∧ b → base ∋ cur :: a ∧ b :: a
    ∧⁻ʳ :  base ∋ cur :: a ∧ b → base ∋ cur :: a ∧ b :: b
    ⇔⁺ :  base ∋ cur :: a ⇒ b :: b ⇒ a → base ∋ cur :: a ⇒ b :: b ⇒ a :: a ⇔ b
    ⇔⁻ˡ :  base ∋ cur :: a ⇔ b → base ∋ cur :: a ⇔ b :: a ⇒ b
    ⇔⁻ʳ :  base ∋ cur :: a ⇔ b → base ∋ cur :: a ⇔ b :: b ⇒ a
  
  data _:>_⊢_ where
    ⇒⁺ : ∀ {res} → base ∋ cur :: res → base :> cur :: res ⊢ y ⇒ res
    ¬elim : ∀ {cur : Hyposis (¬ y)} → base ∋ cur :: a :: ¬ a → base :> cur :: a :: ¬ a ⊢ y
open implication-impl
```

# 例子

一些推理的例子, 很可惜按我们这里定义的语言, 我们完全无法利用agda的自动证明搜索...
而且这些证明看起来也确实很不好看...

```agda
_ : [] ⊢ ` 0 ⇒ ` 0
_ = imply do
  hyp⟨ ` 0 ⟩
  rep⟨ ` 0 ⟩
  ⇒⁺
_ : [] ⊢ ` 0 ∨ ` 0 ⇒ ` 0
_ = imply do
  hyp⟨ ` 0 ∨ ` 0 ⟩
  cont⟨(do
    hyp⟨ ` 0 ⟩
    rep⟨ ` 0 ⟩
    ⇒⁺
    )⟩
  rep⟨ ` 0 ⇒ ` 0 ⟩
  rep⟨ ` 0 ∨ ` 0 ⟩
  ∨⁻
  ⇒⁺
_ : [] ⊢ ¬ ¬ ` 0 ⇒ ` 0
_ = imply do
  hyp⟨ ¬ ¬ ` 0 ⟩
  cont⟨(do
    hyp⟨ ¬ ` 0 ⟩
    rep⟨ ¬ ` 0 ⟩
    reit⟨ ¬ ¬ ` 0 ⟩
    ¬elim
    )⟩
  ⇒⁺
_ : [] ⊢ ` 0 ⇒ ¬ ¬ ` 0
_ = imply do
  hyp⟨ ` 0 ⟩
  cont⟨(do
    hyp⟨ ¬ ¬ ¬ ` 0 ⟩
    cont⟨(do
      hyp⟨ ¬ ¬ ` 0 ⟩
      rep⟨ ¬ ¬ ` 0 ⟩
      reit⟨ ¬ ¬ ¬ ` 0 ⟩
      ¬elim
      )⟩
    reit⟨ ` 0 ⟩
    rep⟨ ¬ ` 0 ⟩
    ¬elim
    )⟩
  ⇒⁺
```

