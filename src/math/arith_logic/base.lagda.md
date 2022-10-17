---
title: 基本定义
description: 这里的逻辑系统中的一些基本定义
...

这一章包含了senioria在形式化中使用的基本定义, 主要是各种逻辑符号和表达式.

```agda
module base where

open import Data.Nat using (ℕ)
```

首先是基本的逻辑式子的定义, 基本就是定义了一下单个变量, 以及各个逻辑联结词.
之所以使用自然数而非字符串和符号... 是因为符号不会, 而字符串写起来有点过于费力了... (超小声(

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
```

