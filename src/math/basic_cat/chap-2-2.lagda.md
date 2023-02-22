---
title: 2.2 Adjunctions via units and counits
...

~~This chapter is kinda ease since adjunctions in the library
are defined with units and counits.~~

# Premable

```agda
module chap-2-2 where

open import Categories.Category using (Category)
open import Categories.Functor using (Functor; _∘F_) renaming (id to F-id)
open import Categories.NaturalTransformation using (NaturalTransformation)
open import Categories.NaturalTransformation.NaturalIsomorphism using (NaturalIsomorphism; niHelper; IsNI)
import Categories.Morphism as Morphism
import Categories.Morphism.Reasoning as MR
open import Level
open import Data.Product using (Σ-syntax; proj₁; proj₂; _×_) renaming (_,_ to ⟨_,_⟩)
open import Categories.Adjoint using (Adjoint)
open import Categories.Category.SubCategory using (FullSubCategory; SubCat)

private variable
  l-o l-l l-e : Level
```

# Problems

## 2.2.11: Equivalence on full subcategories definied by $η_A$

Senioria don't know how to qualify the object set of $A$ to its subset
fulfilling the condition, so the formal proof is omitted ;;><;;

```agda
module prob-2-2-11 {A B : Category l-o l-l l-e} {F : Functor A B} {G : Functor B A}
                   {adj : Adjoint F G} where
  module A = Category A
  module B = Category B
  module F = Functor F
  module G = Functor G
  module adj = Adjoint adj

  -- TODO: ??
```

## 2.2.12: Right adjoint is full and faithful iff counit is isomorphism

```agda
module prob-2-2-12 {A B : Category l-o l-l l-e} {F : Functor A B} {G : Functor B A}
                   {adj : Adjoint F G} where
  record LHS {A B : Category l-o l-l l-e} (F : Functor A B) : Set (l-o ⊔ l-l ⊔ l-e) where
    private
      module A = Category A
      module B = Category B
      module F = Functor F
    field
      faithful : ∀ {a b : A.Obj} {f g : a A.⇒ b} → F.₁ f B.≈ F.₁ g → f A.≈ g
      full : ∀ {a b : A.Obj} {g : F.₀ a B.⇒ F.₀ b} → Σ[ f ∈ a A.⇒ b ] F.₁ f B.≈ g

  module A = Category A
  module B = Category B
  module F = Functor F
  module G = Functor G
  module adj = Adjoint adj

  cis : LHS G → NaturalIsomorphism (F ∘F G) F-id
  cis (record { faithful = faithful ; full = full }) = niHelper (record
    { η = adj.counit.η
    ; η⁻¹ = η⁻¹
    ; commute = adj.counit.commute
    ; iso = λ a → record
      { isoˡ = isoˡ
      ; isoʳ = faithful isoʳ
      }
    })
    where
    η⁻¹ : ∀ (a : B.Obj) → a B.⇒ F.₀ (G.₀ a)
    η⁻¹ a = proj₁ (full {g = adj.unit.η (G.₀ a)})
    isoˡ : ∀ {a} → η⁻¹ a B.∘ adj.counit.η a B.≈ B.id
    isoˡ {a} = begin
         η⁻¹ a ∘ adj.counit.η a ≈˘⟨ adj.counit.commute (η⁻¹ a) ⟩
         adj.counit.η (F.₀ (G.₀ a)) ∘ F.₁ (G.₁ (η⁻¹ a)) ≈⟨ refl⟩∘⟨ F.F-resp-≈ (proj₂ full) ⟩
         adj.counit.η (F.₀ (G.₀ a)) ∘ F.₁ (adj.unit.η (G.₀ a)) ≈⟨ adj.zig ⟩
         B.id ∎
      where
      open B
      open MR B
      open B.HomReasoning
    isoʳ : ∀ {a} → G.₁ (adj.counit.η a B.∘ η⁻¹ a) A.≈ G.₁ B.id
    isoʳ {a} = begin
         G.₁ (adj.counit.η a B.∘ η⁻¹ a) ≈⟨ G.homomorphism ⟩
         G.₁ (adj.counit.η a) ∘ G.₁ (η⁻¹ a) ≈⟨ refl⟩∘⟨ proj₂ full ⟩
         G.₁ (adj.counit.η a) ∘ adj.unit.η (G.₀ a) ≈⟨ adj.zag ⟩
         A.id ≈˘⟨ G.identity ⟩
         G.₁ B.id ∎
      where
      open A
      open MR A
      open A.HomReasoning

  trans : IsNI adj.counit → LHS G
  trans ni = record
    { faithful = faithful
    ; full = full
    }
    where
    module ni = IsNI ni
    module unit = adj.unit
    module counit = adj.counit
    module counit⁻¹ = NaturalTransformation ni.F⇐G
    module iso {a} = Morphism.Iso (ni.iso a)
    faithful : ∀ {a b} {f g : a B.⇒ b} → G.₁ f A.≈ G.₁ g → f B.≈ g
    faithful {a} {b} {f} {g} Gf≈g = begin
             f ≈˘⟨ cancelʳ iso.isoʳ ⟩
             (f ∘ counit.η a) ∘ counit⁻¹.η a ≈˘⟨ counit.commute f ⟩∘⟨refl ⟩
             (counit.η b ∘ F.₁ (G.₁ f)) ∘ counit⁻¹.η a ≈⟨ (refl⟩∘⟨ F.F-resp-≈ Gf≈g) ⟩∘⟨refl ⟩
             (counit.η b ∘ F.₁ (G.₁ g)) ∘ counit⁻¹.η a ≈⟨ counit.commute g ⟩∘⟨refl ⟩
             (g ∘ counit.η a) ∘ counit⁻¹.η a ≈⟨ cancelʳ iso.isoʳ ⟩
             g ∎
      where
      open B
      open MR B
      open B.HomReasoning
    full : ∀ {a b} {g : G.₀ a A.⇒ G.₀ b} → Σ[ f ∈ a B.⇒ b ] G.₁ f A.≈ g
    full {a} {b} {g} = ⟨ f , f≈g ⟩
      where
      f : a B.⇒ b
      f = counit.η b B.∘ F.₁ g B.∘ counit⁻¹.η a
      f≈g : G.₁ f A.≈ g
      f≈g = begin
          G.₁ f ≈⟨ G.homomorphism ⟩
          G.₁ (counit.η b) ∘ G.₁ (F.₁ g B.∘ counit⁻¹.η a) ≈⟨ refl⟩∘⟨ G.homomorphism ⟩
          G.₁ (counit.η b) ∘ G.₁ (F.₁ g) ∘ G.₁ (counit⁻¹.η a) ≈˘⟨ A.assoc ⟩
          (G.₁ (counit.η b) ∘ G.₁ (F.₁ g)) ∘ G.₁ (counit⁻¹.η a) ≈˘⟨ refl⟩∘⟨ cancelʳ adj.zag ⟩
          (G.₁ (counit.η b) ∘ G.₁ (F.₁ g)) ∘ (G.₁ (counit⁻¹.η a) ∘ G.₁ (counit.η a)) ∘ unit.η (G.₀ a)
            ≈⟨ refl⟩∘⟨ elimˡ (A.Equiv.sym G.homomorphism ○ G.F-resp-≈ iso.isoˡ ○ G.identity) ⟩
          (G.₁ (counit.η b) ∘ G.₁ (F.₁ g)) ∘ unit.η (G.₀ a) ≈⟨ A.assoc ⟩
          G.₁ (counit.η b) ∘ G.₁ (F.₁ g) ∘ unit.η (G.₀ a) ≈˘⟨ refl⟩∘⟨ unit.commute g ⟩
          G.₁ (counit.η b) ∘ unit.η (G.₀ b) ∘ g ≈⟨ cancelˡ adj.zag ⟩
          g ∎
        where
        open A
        open MR A
        open A.HomReasoning
```

## (The rest)

咕咕咕咕 (超小声(被打死(((((((

