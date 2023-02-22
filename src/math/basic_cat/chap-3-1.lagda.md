---
title: 3.1 Constructions with sets
...

# Premable

```agda
module chap-3-1 where

open import Categories.Category using (Category)
open import Level renaming (zero to lzero; suc to lsuc)
open import Relation.Binary using (Rel; IsEquivalence)
open import Function using () renaming (id to idf; _∘_ to _f∘_)
open import Relation.Binary.PropositionalEquality using (_≡_; refl; cong) renaming (sym to eq-sym; trans to eq-trans)

postulate
  extensionality : ∀ {ℓ} {A B : Set ℓ} {f g : A → B} → (∀ x → f x ≡ g x) → f ≡ g
```

# 3.1.1: Left and right adjoints of $\Delta (A : Set) : Set \times Set := (A, A)$

The categories and the functor is straightforward:

```agda
module prob-3-1-1 where
  open import Data.Product using (_×_; proj₁; proj₂) renaming (_,_ to ⟨_,_⟩)
  open import Data.Sum using (_⊎_; inj₁; inj₂)
  open import Categories.Functor using (Functor)
  open import Categories.Adjoint using (Adjoint)

  cset : Category (lsuc lzero) lzero lzero
  cset = record
           { Obj = Set
           ; _⇒_ = λ A B → A → B
           ; _≈_ = _≡_
           ; id = idf
           ; _∘_ = λ g f → g f∘ f
           ; assoc = refl
           ; sym-assoc = refl
           ; identityˡ = refl
           ; identityʳ = refl
           ; identity² = refl
           ; equiv = record { refl = refl ; sym = eq-sym ; trans = eq-trans }
           ; ∘-resp-≈ = λ { refl refl → refl }
           }

  cset2 : Category (lsuc lzero) lzero lzero
  cset2 = record
            { Obj = Set × Set
            ; _⇒_ = λ { ⟨ A₁ , A₂ ⟩ ⟨ B₁ , B₂ ⟩ → (A₁ → B₁) × (A₂ → B₂) }
            ; _≈_ = _≡_
            ; id = ⟨ idf , idf ⟩
            ; _∘_ = λ { ⟨ f₁ , f₂ ⟩ ⟨ g₁ , g₂ ⟩ → ⟨ f₁ f∘ g₁ , f₂ f∘ g₂ ⟩ }
            ; assoc = refl
            ; sym-assoc = refl
            ; identityˡ = refl
            ; identityʳ = refl
            ; identity² = refl
            ; equiv = record { refl = refl ; sym = eq-sym ; trans = eq-trans }
            ; ∘-resp-≈ = λ { refl refl → refl }
            }

  Δ : Functor cset cset2
  Δ = record
        { F₀ = λ x → ⟨ x , x ⟩
        ; F₁ = λ f → ⟨ f , f ⟩
        ; identity = refl
        ; homomorphism = refl
        ; F-resp-≈ = λ { refl → refl }
        }
```

The adjoints is also straightforward, since their properties (propositional equality) are far too good:

```agda
  ladj : Functor cset2 cset
  ladj = record
           { F₀ = λ { ⟨ A , B ⟩ → A × B }
           ; F₁ = λ { ⟨ f₁ , f₂ ⟩ ⟨ a , b ⟩ → ⟨ f₁ a , f₂ b ⟩ }
           ; identity = refl
           ; homomorphism = refl
           ; F-resp-≈ = λ { refl → refl }
           }

  module cset = Category cset
  module cset2 = Category cset2
  module Δ = Functor Δ

  left-adj : Adjoint Δ ladj
  left-adj = record
               { unit = record { η = λ _ x → ⟨ x , x ⟩ ; commute = λ f → refl ; sym-commute = λ f → refl }
               ; counit = record { η = λ X → ⟨ proj₁ , proj₂ ⟩ ; commute = λ f → refl ; sym-commute = λ f → refl }
               ; zig = refl
               ; zag = refl
               }

  radj : Functor cset2 cset
  radj = record
           { F₀ = λ { ⟨ A , B ⟩ → A ⊎ B }
           ; F₁ = λ { ⟨ f₁ , _ ⟩ (inj₁ x) → inj₁ (f₁ x) ; ⟨ _ , f₂ ⟩ (inj₂ x) → inj₂ (f₂ x) }
           ; identity = extensionality λ { (inj₁ x) → refl ; (inj₂ y) → refl }
           ; homomorphism = extensionality λ { (inj₁ x) → refl ; (inj₂ y) → refl }
           ; F-resp-≈ = λ { refl → refl }
           }

  right-adj : Adjoint radj Δ
  right-adj = record
                { unit = record { η = λ _ → ⟨ inj₁ , inj₂ ⟩ ; commute = λ f → refl ; sym-commute = λ f → refl }
                ; counit = record
                             { η = λ { _ (inj₁ x) → x ; _ (inj₂ x) → x }
                             ; commute = λ f → extensionality λ { (inj₁ x) → refl ; (inj₂ y) → refl }
                             ; sym-commute = λ f → extensionality λ { (inj₁ x) → refl ; (inj₂ y) → refl }
                             }
                ; zig = extensionality λ { (inj₁ x) → refl ; (inj₂ y) → refl }
                ; zag = refl
                }
```

# 3.1.2: $(\mathbb{N}, 0, \text{suc})$ be initial of a category

Let's write out the type of object first:

```agda
module prob-3-1-2 where
  open import Data.Nat using (ℕ; suc)
  open import Categories.Object.Initial using (IsInitial)

  record Object {ℓ} : Set (lsuc ℓ) where
    field
      Ty : Set ℓ
      init : Ty
      succ : Ty → Ty
```

To proof our problem, we need to construct the arrow, and prove that the object is an initial.
Simply a function can fit the properties of an arrow in the category well:

```agda
  record Arrow {ℓ} (A B : Object {ℓ}) : Set (lsuc ℓ) where
    module A = Object A
    module B = Object B

    field
      fn : A.Ty → B.Ty
```

But with only the function, it's obvious that the arrow from $(\mathbb{N}, 0, \text{suc})$ is not unique,
since, for example, for set $\{0, 1, 2\}$, there are infinite functions.
The uniqueness can be ensured by the following properties:

```agda
      intro : fn A.init ≡ B.init
      keep : ∀ {a} → fn (A.succ a) ≡ B.succ (fn a)
```

With extensionality, we can see that this makes a category.
The equality proof in the arrows may differ, so they're not counted in the equivalence:

```agda
  cat : ∀ {ℓ} → Category (lsuc ℓ) (lsuc ℓ) ℓ
  cat {ℓ} = record
              { Obj = Object {ℓ}
              ; _⇒_ = Arrow
              ; _≈_ = _≈_
              ; id = record { fn = idf ; intro = refl ; keep = refl }
              ; _∘_ = compose
              ; assoc = refl
              ; sym-assoc = refl
              ; identityˡ = refl
              ; identityʳ = refl
              ; identity² = refl
              ; equiv = record { refl = refl ; sym = eq-sym ; trans = eq-trans }
              ; ∘-resp-≈ = λ { refl refl → refl }
              }
    where
    _≈_ : ∀ {A B : Object {ℓ}} → Rel (Arrow A B) ℓ
    _≈_ f g = Arrow.fn f ≡ Arrow.fn g
    compose : {A B C : Object} → Arrow B C → Arrow A B → Arrow A C
    compose g f = record
                    { fn = g.fn f∘ f.fn
                    ; intro = eq-trans (cong g.fn f.intro) g.intro
                    ; keep = eq-trans (cong g.fn f.keep) g.keep
                    }
      where
      module g = Arrow g
      module f = Arrow f
  module cat {ℓ} = Category (cat {ℓ})
```

The construction of the arrows is canonical, and with extensionality, we can show that the object is initial:

```agda
  obj : Object
  obj = record { Ty = ℕ ; init = 0 ; succ = suc }
  module obj = Object obj

  init : IsInitial cat obj
  init = record { ! = arr ; !-unique = λ f → extensionality (uniq f) }
    where
    fn : ∀ (a : Object) → ℕ → Object.Ty a
    fn record { init = init } ℕ.zero = init
    fn a@(record { succ = succ }) (suc i) = succ (fn a i)
    arr : {a : Object} → Arrow obj a
    arr {a} = record { fn = fn a ; intro = refl ; keep = refl }
    uniq : ∀ {a : Object} (f : Arrow obj a) → ∀ x → fn a x ≡ Arrow.fn f x
    uniq {record { init = .(f 0) }} record { fn = f ; intro = refl } ℕ.zero = refl
    uniq {record { succ = succ }} f@(record { keep = keep }) (suc x) = eq-trans (cong succ (uniq f x)) (eq-sym (keep {x}))
```

