---
title: 2.1 Adjoints' definitions and examples
...

# Premable

```agda
module chap-2-1 where

open import Categories.Category using (Category)
open import Categories.Functor using (Functor)
import Categories.Morphism.Reasoning as MR
open import Level
open import Data.Product using (Σ-syntax; proj₁; proj₂; _×_) renaming (_,_ to ⟨_,_⟩)

private variable
  l-o l-l l-e : Level
```

Adjoints in the library is defined in a different way as the book,
with the naturality between objects.
Due to Senioriae lack to essential knowledge, to avoid cyclic proof,
and to agree with the definition in the book,
we define adjoints in another way, focusing on the naturality between arrows.

```agda
record Adjoint {A B : Category l-o l-l l-e} (F : Functor A B) (G : Functor B A)
               : Set (l-o ⊔ l-l ⊔ l-e) where
  private
    module A = Category A
    module B = Category B
    module F = Functor F
    module G = Functor G

  field
    A⇒B : ∀ {a b} → a A.⇒ G.₀ b → F.₀ a B.⇒ b
    B⇒A : ∀ {a b} → F.₀ a B.⇒ b → a A.⇒ G.₀ b

  field
    inv-A : ∀ {a b} {f : a A.⇒ G.₀ b} → B⇒A (A⇒B f) A.≈ f
    inv-B : ∀ {a b} {f : F.₀ a B.⇒ b} → A⇒B (B⇒A f) B.≈ f
    A⇒B-distrib : ∀ {a b c} {f : a A.⇒ b} {g : b A.⇒ G.₀ c} → A⇒B (g A.∘ f) B.≈ A⇒B g B.∘ F.₁ f
    B⇒A-distrib : ∀ {a b c} {f : F.₀ a B.⇒ b} {g : b B.⇒ c} → B⇒A (g B.∘ f) A.≈ G.₁ g A.∘ B⇒A f
    A⇒B-resp-≈ : ∀ {a b} {f g : a A.⇒ G.₀ b} → f A.≈ g → A⇒B f B.≈ A⇒B g
    B⇒A-resp-≈ : ∀ {a b} {f g : F.₀ a B.⇒ b} → f B.≈ g → B⇒A f A.≈ B⇒A g
```

# Problems

## 2.1.15: preservation of left and right adjoints

Before we discuss the properties of initials and terminals,
again, we should define them.
The definitions are quite trivial and clear.

```agda
record Initial {A : Category l-l l-o l-e} (i : Category.Obj A) : Set (l-l ⊔ l-o ⊔ l-e) where
  open Category A
  field
    map : ∀ (a : Obj) → i ⇒ a
    uniq : ∀ {a} {f g : i ⇒ a} → f ≈ g

record Terminal {A : Category l-l l-o l-e} (t : Category.Obj A) : Set (l-l ⊔ l-o ⊔ l-e) where
  open Category A
  field
    map : ∀ (a : Obj) → a ⇒ t
    uniq : ∀ {a} {f g : a ⇒ t} → f ≈ g
```

The proof to the initials:

```agda
module prob-2-1-15 {A B : Category l-l l-o l-e} {F : Functor A B} {G : Functor B A} where
  module A = Category A
  module B = Category B
  module F = Functor F
  module G = Functor G

  resp-init : Adjoint F G → ∀ {i} → Initial {A = A} i → Initial {A = B} (F.₀ i)
  resp-init adj init = record
            { map = λ a → adj.A⇒B (init.map (G.₀ a))
            ; uniq = λ {a} {f} {g} → begin
                   f ≈˘⟨ adj.inv-B ⟩
                   adj.A⇒B (adj.B⇒A f) ≈⟨ adj.A⇒B-resp-≈ init.uniq ⟩
                   adj.A⇒B (adj.B⇒A g) ≈⟨ adj.inv-B ⟩
                   g ∎
            }
    where
    module adj = Adjoint adj
    module init = Initial init
    open MR B
    open B.HomReasoning
```

The proof to the terminals is similar:

```agda
  resp-term : Adjoint F G → ∀ {t} → Terminal {A = B} t → Terminal {A = A} (G.₀ t)
  resp-term adj term = record
            { map = λ a → adj.B⇒A (term.map (F.₀ a))
            ; uniq = λ {a} {f} {g} → begin
                   f ≈˘⟨ adj.inv-A ⟩
                   adj.B⇒A (adj.A⇒B f) ≈⟨ adj.B⇒A-resp-≈ term.uniq ⟩
                   adj.B⇒A (adj.A⇒B g) ≈⟨ adj.inv-A ⟩
                   g ∎
            }
    where
    module adj = Adjoint adj
    module term = Terminal term
    open MR A
    open A.HomReasoning
```

