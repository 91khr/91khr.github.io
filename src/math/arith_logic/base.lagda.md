---
title: 基本定义
description: 这里的逻辑系统中的一些基本定义
...

这一章包含了senioria在形式化中使用的基本定义, 主要是各种逻辑符号和表达式.

```agda
module base where

open import Data.Nat using (ℕ)
open import Data.Nat using (_≟_)
open import Relation.Nullary using (Dec; yes; no)
open import Relation.Binary.PropositionalEquality using (_≡_; refl)
```

首先是基本的逻辑式子的定义, 基本就是定义了一下单个变量, 以及各个逻辑联结词.
之所以使用自然数而非字符串和符号... 是因为符号不会, 而字符串写起来有点过于费力了... (超小声(

那个乱入的bottom符号是为了一些空式子而加入的 ()(超小声(

```agda
infixr 20 _⇔_
infixr 21 _⇒_
infixl 22 _∨_
infixl 23 _∧_
infix 24 ¬_
infix 25 `_

data Term : Set where
  `_ : ℕ → Term
  _∧_ : Term → Term → Term
  _∨_ : Term → Term → Term
  ¬_ : Term → Term
  _⇒_ : Term → Term → Term
  _⇔_ : Term → Term → Term
  t⊥ : Term
```

Don't want to bother to use the reflection... ><
It may be more complicated than manually matching n^2 cases...

```agda
_t≟_ : (a b : Term) → Dec (a ≡ b)
(` x) t≟ (` y) with x ≟ y
...               | yes refl = yes refl
...               | no ne = no λ { refl → ne refl }
(` x) t≟ (b ∧ b₁) = no (λ ())
(` x) t≟ (b ∨ b₁) = no (λ ())
(` x) t≟ (¬ b) = no (λ ())
(` x) t≟ (b ⇒ b₁) = no (λ ())
(` x) t≟ (b ⇔ b₁) = no (λ ())
(` x) t≟ t⊥ = no (λ ())
(a ∧ a₁) t≟ (` x) = no (λ ())
(a ∧ b) t≟ (c ∧ d) with a t≟ c | b t≟ d
...                    | yes refl | yes refl = yes refl
...                    | no a≢c | _ = no λ { refl → a≢c refl }
...                    | _ | no b≢d = no λ { refl → b≢d refl }
(a ∧ a₁) t≟ (b ∨ b₁) = no (λ ())
(a ∧ a₁) t≟ (¬ b) = no (λ ())
(a ∧ a₁) t≟ (b ⇒ b₁) = no (λ ())
(a ∧ a₁) t≟ (b ⇔ b₁) = no (λ ())
(a ∧ a₁) t≟ t⊥ = no (λ ())
(a ∨ a₁) t≟ (` x) = no (λ ())
(a ∨ a₁) t≟ (b ∧ b₁) = no (λ ())
(a ∨ b) t≟ (c ∨ d) with a t≟ c | b t≟ d
...                    | yes refl | yes refl = yes refl
...                    | no a≢c | _ = no λ { refl → a≢c refl }
...                    | _ | no b≢d = no λ { refl → b≢d refl }
(a ∨ a₁) t≟ (¬ b) = no (λ ())
(a ∨ a₁) t≟ (b ⇒ b₁) = no (λ ())
(a ∨ a₁) t≟ (b ⇔ b₁) = no (λ ())
(a ∨ a₁) t≟ t⊥ = no (λ ())
(¬ a) t≟ (` x) = no (λ ())
(¬ a) t≟ (b ∧ b₁) = no (λ ())
(¬ a) t≟ (b ∨ b₁) = no (λ ())
(¬ a) t≟ (¬ b) with a t≟ b
...               | yes refl = yes refl
...               | no a≢b = no λ { refl → a≢b refl }
(¬ a) t≟ (b ⇒ b₁) = no (λ ())
(¬ a) t≟ (b ⇔ b₁) = no (λ ())
(¬ a) t≟ t⊥ = no (λ ())
(a ⇒ a₁) t≟ (` x) = no (λ ())
(a ⇒ a₁) t≟ (b ∧ b₁) = no (λ ())
(a ⇒ a₁) t≟ (b ∨ b₁) = no (λ ())
(a ⇒ a₁) t≟ (¬ b) = no (λ ())
(a ⇒ b) t≟ (c ⇒ d) with a t≟ c | b t≟ d
...                   | yes refl | yes refl = yes refl
...                   | no a≢c | _ = no λ { refl → a≢c refl }
...                   | _ | no b≢d = no λ { refl → b≢d refl }
(a ⇒ a₁) t≟ (b ⇔ b₁) = no (λ ())
(a ⇒ a₁) t≟ t⊥ = no (λ ())
(a ⇔ a₁) t≟ (` x) = no (λ ())
(a ⇔ a₁) t≟ (b ∧ b₁) = no (λ ())
(a ⇔ a₁) t≟ (b ∨ b₁) = no (λ ())
(a ⇔ a₁) t≟ (¬ b) = no (λ ())
(a ⇔ a₁) t≟ (b ⇒ b₁) = no (λ ())
(a ⇔ b) t≟ (c ⇔ d) with a t≟ c | b t≟ d
...                   | yes refl | yes refl = yes refl
...                   | no a≢c | _ = no λ { refl → a≢c refl }
...                   | _ | no b≢d = no λ { refl → b≢d refl }
(a ⇔ a₁) t≟ t⊥ = no (λ ())
t⊥ t≟ (` x) = no (λ ())
t⊥ t≟ (b ∧ b₁) = no (λ ())
t⊥ t≟ (b ∨ b₁) = no (λ ())
t⊥ t≟ (¬ b) = no (λ ())
t⊥ t≟ (b ⇒ b₁) = no (λ ())
t⊥ t≟ (b ⇔ b₁) = no (λ ())
t⊥ t≟ t⊥ = yes refl
```

大概也没有更多的了, 这里列出的性质某种程度上来看其实都挺显然的.

