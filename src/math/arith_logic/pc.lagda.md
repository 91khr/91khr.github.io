---
title: 公理系统PC
description: 在命题的各个形式之间变换的系统
...

# Premable

```agda
module pc where

open import base
```

# 表达式的变换

一些联结词的形式可以变换成另一些形式, 我们在下文中会看到, 这些变换至关重要, 所以我们在此定义它们.

```agda
infix 15 _↝_
data _↝_ : Term → Term → Set where
  ↝var : ∀ {a} → ` a ↝ ` a
  ↝∧ : ∀ {a b a′ b′} → a ↝ a′ → b ↝ b′ → (a ∧ b) ↝ (a′ ∧ b′)
  ↝∨ : ∀ {a b a′ b′} → a ↝ a′ → b ↝ b′ → (a ∨ b) ↝ (a′ ∨ b′)
  ↝¬ : ∀ {a a′} → a ↝ a′ → ¬ a ↝ ¬ a′
  ↝⇒ : ∀ {a b a′ b′} → a ↝ a′ → b ↝ b′ → (a ⇒ b) ↝ (a′ ⇒ b′)
  ↝⇔ : ∀ {a b a′ b′} → a ↝ a′ → b ↝ b′ → (a ⇔ b) ↝ (a′ ⇔ b′)
  Df1 : ∀ (a b : Term) → a ∧ b ↝ ¬ (¬ a ∨ ¬ b)
  Dr1 : ∀ (a b : Term) → ¬ (¬ a ∨ ¬ b) ↝ a ∧ b
  Df2 : ∀ (a b : Term) → a ⇒ b ↝ ¬ a ∨ b
  Dr2 : ∀ (a b : Term) → ¬ a ∨ b ↝ a ⇒ b
  Df3 : ∀ (a b : Term) → a ⇔ b ↝ (a ⇒ b) ∧ (b ⇒ a)
  Dr3 : ∀ (a b : Term) → (a ⇒ b) ∧ (b ⇒ a) ↝ a ⇔ b
```

当然, 最后的几条变换应该作为定理而非构造器给出, 不过为了能够使用Agda默认的证明搜索, 这样会方便不少.

然后, 我们很容易看到, 变换`↝`是自反的:

```agda
↝refl : ∀ {x} → x ↝ x
↝refl {` x} = ↝var
↝refl {x ∧ x₁} = ↝∧ ↝refl ↝refl
↝refl {x ∨ x₁} = ↝∨ ↝refl ↝refl
↝refl {¬ x} = ↝¬ ↝refl
↝refl {x ⇒ x₁} = ↝⇒ ↝refl ↝refl
↝refl {x ⇔ x₁} = ↝⇔ ↝refl ↝refl
```

# 上下文

之后的演算需要利用之前的演算里得出的式子, 这就需要一些上下文和对应的查找系统.
这里使用的语言和plfa中定义lambda演算的上下文类似, 因为抄一份总是方便的 ()(

```agda
infixl 15 _,_
data Context : Set where
  ∅ : Context
  _,_ : Context → Term → Context

infix 14 _∋_
data _∋_ : Context → Term → Set where
  Z : ∀ {Γ x} → Γ , x ∋ x
  S_ : ∀ {Γ x y} → Γ ∋ y → Γ , x ∋ y

module lookup where
  open import Data.Nat using (ℕ; suc; _≤_; _≤?_; z≤n; s≤s)
  open import Relation.Nullary.Decidable using (True; toWitness)
  length : Context → ℕ
  length ∅ = 0
  length (Γ , x) = suc (length Γ)
  lookup : {Γ : Context} {n : ℕ} (p : suc n ≤ length Γ) → Term
  lookup {Γ , x} {0} p = x
  lookup {Γ , x} {suc n} (s≤s p) = lookup p
  count : ∀ {Γ} {n : ℕ} (p : suc n ≤ length Γ) → Γ ∋ lookup p
  count {Γ , x} {0} p = Z
  count {Γ , x} {suc n} (s≤s p) = S count p
  infix 18 #_
  #_ : ∀ {Γ} (n : ℕ) {n∈Γ : True (suc n ≤? length Γ)} → Γ ∋ lookup (toWitness n∈Γ)
  #_ n {n∈Γ} = count (toWitness n∈Γ)
  infix 18 ##_
  ##_ : ∀ {Γ} (n : ℕ) {n∈Γ : True (suc n ≤? length Γ)} → Term
  ##_ n {n∈Γ} = lookup (toWitness n∈Γ)
open lookup using (#_; ##_)
```

# 演算

然后就是一些演算规则, 它们都是显然的: 扩充已经得到的公式, 应用蕴含式, 引入一些重言式(公理), 或者应用一些变换.

```agda
infix 14 _⊢_
infixr 16 ⟨_⟩→_
data _⊢_ : Context → Term → Set where
  ⟨_⟩→_ : ∀ {Γ imm res} → Γ ⊢ imm → Γ , imm ⊢ res → Γ ⊢ res
  ∎ : ∀ {Γ res} → Γ , res ⊢ res
  MP : ∀ {Γ a b} → Γ ∋ a ⇒ b → Γ ∋ a → Γ ⊢ b
  A1 : ∀ {Γ} (a) → Γ ⊢ a ∨ a ⇒ a
  A2 : ∀ {Γ} (a b : Term) → Γ ⊢ a ⇒ a ∨ b
  A3 : ∀ {Γ} (a b : Term) → Γ ⊢ a ∨ b ⇒ b ∨ a
  A4 : ∀ {Γ} (a b c : Term) → Γ ⊢ (b ⇒ c) ⇒ (a ∨ b ⇒ a ∨ c)
  ↝app : ∀ {Γ a a′} → Γ ∋ a → a ↝ a′ → Γ ⊢ a′
```

# 一些定理

这些是书上的一些定理, 它们的证明都很显然.

```agda
Ax1 : ∀ {Γ} (a b c : Term) → Γ ⊢ (b ⇒ c) ⇒ ((a ⇒ b) ⇒ (a ⇒ c))
Ax1 a b c = ⟨ A4 (¬ a) b c ⟩→ ⟨ ↝app (# 0) (↝⇒ ↝refl (↝⇒ (Dr2 a b) (Dr2 a c))) ⟩→ ∎
Ax2 : ∀ {Γ} (a : Term) → Γ ⊢ a ⇒ a
Ax2 a = ⟨ A2 a a ⟩→ ⟨ A1 a ⟩→ ⟨ Ax1 a (a ∨ a) a ⟩→ ⟨ MP (# 0) (# 1) ⟩→ ⟨ MP (# 0) (# 3) ⟩→ ∎
Ax3 : ∀ {Γ} (a : Term) → Γ ⊢ ¬ a ∨ a
Ax3 a = ⟨ Ax2 a ⟩→ ⟨ ↝app (# 0) (Df2 a a) ⟩→ ∎
Ax4 : ∀ {Γ} (a : Term) → Γ ⊢ a ∨ ¬ a
Ax4 a = ⟨ Ax3 a ⟩→ ⟨ A3 (¬ a) a ⟩→ ⟨ MP (# 0) (# 1) ⟩→ ∎
Ax5 : ∀ {Γ} (a : Term) → Γ ⊢ a ⇒ ¬ ¬ a
Ax5 a = ⟨ Ax4 (¬ a) ⟩→ ⟨ ↝app (# 0) (Dr2 a (¬ ¬ a)) ⟩→ ∎
Ax6 : ∀ {Γ} (a : Term) → Γ ⊢ ¬ ¬ a ⇒ a
Ax6 a = ⟨ Ax5 (¬ a) ⟩→ ⟨ A4 a (¬ a) (¬ ¬ ¬ a) ⟩→ ⟨ MP (# 0) (# 1) ⟩→ ⟨ Ax4 a ⟩→ ⟨ MP (# 1) (# 0) ⟩→
        ⟨ A3 a (¬ ¬ ¬ a) ⟩→ ⟨ MP (# 0) (# 1) ⟩→ ⟨ ↝app (# 0) (Dr2 (¬ ¬ a) a) ⟩→ ∎
Ax7 : ∀ {Γ} (a b : Term) → Γ ⊢ (a ⇒ b) ⇒ (¬ b ⇒ ¬ a)
Ax7 a b = ⟨ Ax5 b ⟩→ ⟨ A4 (¬ a) b (¬ ¬ b) ⟩→ ⟨ MP (# 0) (# 1) ⟩→ ⟨ Ax1 (¬ a ∨ b) (¬ a ∨ ¬ ¬ b) (¬ ¬ b ∨ ¬ a) ⟩→
          ⟨ A3 (¬ a) (¬ ¬ b) ⟩→ ⟨ MP (# 1) (# 0) ⟩→ ⟨ MP (# 0) (# 3) ⟩→
          ⟨ ↝app (# 0) (↝⇒ (Dr2 a b) (Dr2 (¬ b) (¬ a))) ⟩→ ∎
Ax8 : ∀ {Γ} (a b : Term) → Γ ⊢ ¬ (a ∧ b) ⇒ ¬ a ∨ ¬ b
Ax8 a b = ⟨ Ax6 (¬ a ∨ ¬ b) ⟩→ ↝app (# 0) (↝⇒ (↝¬ (Dr1 a b)) ↝refl)
Ax9 : ∀ {Γ} (a b : Term) → Γ ⊢ ¬ a ∨ ¬ b ⇒ ¬ (a ∧ b)
Ax9 a b = ⟨ Ax5 (¬ a ∨ ¬ b) ⟩→ ↝app (# 0) (↝⇒ ↝refl (↝¬ (Dr1 a b)))
Ax10 : ∀ {Γ} (a b : Term) → Γ ⊢ a ⇒ b ∨ a
Ax10 a b = ⟨ A3 a b ⟩→ ⟨ Ax1 a (a ∨ b) (b ∨ a) ⟩→ ⟨ MP (# 0) (# 1) ⟩→ ⟨ A2 a b ⟩→ MP (# 1) (# 0)
```

