---
title: 自然推理系统FPC (under construction)
description: 利用假设的变换和回收完成推理的系统
...

# Premable

```agda
{-# OPTIONS --allow-unsolved-metas #-}

module fpc where

open import base
```

# 假设的定义

假设是FPC的核心, 我们在此只在形式上定义假设, 定义假设的扩充和推理规则放到下一节.

```agda
infixl 15 _::_
data Hyposis : Term → Set where
  [_] : (x : Term) → Hyposis x
  _::_ : ∀ {x} → Hyposis x → Term → Hyposis x
  _::[_] : ∀ {x y} → Hyposis x → Hyposis y → Hyposis x
```

PC依赖之前已经推出来的式子来拓展得到的式子的集合, 从而得到结论.
而FPC依赖之前的假设里得到的式子来拓展当前假设可以推出的式子, 从而得到结论.

为了完成这一拓展, 我们首先需要一个用于判断一个式子是否在之前的假设里出现过的谓词:

```agda
infix 14 _∋_
data _∋_ : ∀ {x} → Hyposis x → Term → Set where
  hyp : ∀ {x} {hyp : Hyposis x} → hyp ∋ x
  here : ∀ {x elem} {pre : Hyposis x} → pre :: elem ∋ elem
  there : ∀ {x elem tail} {pre : Hyposis x} → pre ∋ elem → pre :: tail ∋ elem
  inner : ∀ {x y elem} {pre : Hyposis x} {sub : Hyposis y} → sub ∋ elem → pre ::[ sub ] ∋ elem
  outer : ∀ {x y elem} {pre : Hyposis x} {sub : Hyposis y} → pre ∋ elem → pre ::[ sub ] ∋ elem
```

为了让生活更容易, 我们使用自然数来定义一个查找系统:

```agda
module lookup where
  open import Data.Nat using (ℕ; suc; _+_; _≤_; z≤n; s≤s)
  open import Relation.Nullary.Decidable using (True; toWitness)

  length : ∀ {x} → Hyposis x → ℕ
  length [ _ ] = 1
  length (base :: x) = suc (length base)
  length (base ::[ sub ]) = (length sub) + (length base)
  lookup : ∀ {x} {base : Hyposis x} {n : ℕ} (p : suc n ≤ length base) → Term
  lookup {base = [ x ]} {0} (s≤s z≤n) = x
  lookup {base = [ x ]} {suc n} (s≤s ())
  lookup {base = base :: x} {0} (s≤s z≤n) = x
  lookup {base = base :: x} {suc n} (s≤s p) = lookup {base = base} p
  lookup {base = base ::[ sub ]} {0} p = {!!}
  lookup {base = base ::[ sub ]} {suc n} p = {!!}
```

# 定义FPC中的推理

```agda
module fpc-definition where
  private variable
    x y res : Term
    base : Hyposis x
    cur : Hyposis y
  infixl 8 _:>_⊢_
  data _:>_⊢_ : Hyposis x → Hyposis y → Term → Set

  infixr 1 _>>_
  _>>_ : ∀ {A B : Set} → (A → B) → A → B
  f >> g = f g
  data ⊢∎ : Set where
    ∎ : ⊢∎
  infix 8 ⊢_
  data ⊢_ : Term → Set where
    hyp_⟨_⟩ : ∀ (z) → [ z ] :> [ z ] ⊢ res → ⊢ res

  data _:>_⊢_ where
    hyp_⟨_⟩ : ∀ (z) {imm} → base ::[ cur ] :> [ z ] ⊢ imm → base :> cur :: imm ⊢ res → base :> cur ⊢ res
    rep⟨_⟩ : ∀ {z} → cur ∋ z → base :> cur :: z ⊢ res → base :> cur ⊢ res
    -- TODO: under construction
    ⇒⁺ : ∀ {z} → base :> cur :: z ⊢ y ⇒ z
```

```agda
module altdef-examples where
  open fpc-definition

  _ : ⊢ ` 0 ⇒ ` 0
  _ = hyp ` 0 ⟨ rep⟨ {!!} ⟩ ⇒⁺ ⟩
```

