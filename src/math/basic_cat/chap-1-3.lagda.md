---
title: 1.3 自然变换
...

代码确实很一次性... qaq... 不过毕竟确实是一次性代码, 所以... x x (超小声(缩成球(

# Premable

```agda
module chap-1-3 where

open import Categories.Category using (Category)
open import Categories.Functor using (Functor)
open import Categories.NaturalTransformation using (NaturalTransformation; ntHelper)
open import Categories.NaturalTransformation.NaturalIsomorphism using (NaturalIsomorphism; niHelper)
open import Categories.Morphism using (Iso; IsIso)
import Categories.Morphism.Reasoning as MR
open import Level
open import Data.Product using (Σ-syntax; proj₁; proj₂; _×_) renaming (_,_ to ⟨_,_⟩)

private variable
  l-o l-l l-e : Level
```

# Problems

## 1.3.26: Lemma 1.3.11

```agda
module lemma-1-3-11 {A B : Category l-o l-l l-e} {F G : Functor A B} where
  private
    module A = Category A
    module B = Category B
    module F = Functor F
    module G = Functor G
    module NI = NaturalIsomorphism
    module NT = NaturalTransformation

  cis : ∀ (α : NaturalIsomorphism F G) → ∀ {a : A.Obj} → (IsIso B (NT.η (NI.F⇒G α) a))
  cis α {a} = record
          { inv = NT.η α.F⇐G a
          ; iso = α.iso a
          }
      where
      module α = NaturalIsomorphism α
  trans : ∀ (α : NaturalTransformation F G) → (∀ {a : A.Obj} → IsIso B (NT.η α a)) → NaturalIsomorphism F G
  trans α iso = record
                { F⇒G = α
                ; F⇐G = ntHelper
                  record
                  { η = λ a → iso.inv
                  ; commute = commute
                  }
                ; iso = λ a → iso.iso
                }
    where
    module iso {a} = IsIso (iso {a})
    module α = NaturalTransformation α
    open Category B
    open Category.HomReasoning B
    open MR B

    commute : ∀ {a b : A.Obj} (f : a A.⇒ b) → iso.inv ∘ G.F₁ f ≈ F.F₁ f ∘ iso.inv
    commute {a} {b} f =
            begin iso.inv ∘ G.F₁ f ≈˘⟨ refl⟩∘⟨ cancelʳ (Iso.isoʳ iso.iso) ⟩
            iso.inv ∘ (G.F₁ f ∘ α.η a) ∘ iso.inv ≈˘⟨ refl⟩∘⟨ α.commute f ⟩∘⟨refl ⟩
            iso.inv ∘ ((α.η b ∘ F.F₁ f) ∘ iso.inv) ≈⟨ sym-assoc ⟩
            (iso.inv ∘ (α.η b ∘ F.F₁ f)) ∘ iso.inv ≈⟨ cancelˡ (Iso.isoˡ iso.iso) ⟩∘⟨refl ⟩
            F.F₁ f ∘ iso.inv ∎
```

## 1.3.32: equivalence functor iff faithful, full, and essentially surjective

~~不能摆烂了好可恶 x (超小声(被打死(((((~~

先定义一下那些性质:

```agda
module prop-1-3-18 where
  open Categories.Functor renaming (id to f-id)
  import Function

  record Equivalence {l-a l-b l-c} {A B : Category l-a l-b l-c} (F : Functor A B) : Set (l-a ⊔ l-b ⊔ l-c) where
    field
      G : Functor B A
      G∘F : NaturalIsomorphism (G ∘F F) f-id
      F∘G : NaturalIsomorphism (F ∘F G) f-id
    module G∘F = NaturalIsomorphism G∘F
    module F∘G = NaturalIsomorphism F∘G
    module F = Functor F
    module G = Functor G
```

正方向的证明:

```agda
  module cis-faithful {A B : Category l-o l-l l-e} {F : Functor A B} where
    module A = Category A
    module B = Category B
    module F = Functor F
    open A
    open A.HomReasoning
    open MR A

    cis-faithful : Equivalence F → ∀ {a b : A.Obj} {f g : a A.⇒ b} → F.₁ f B.≈ F.₁ g → f A.≈ g
    cis-faithful record { G = G ; G∘F = G∘F ; F∘G = F∘G } {a} {b} {f} {g} f≈g =
               begin f ≈˘⟨ cancelʳ (Iso.isoʳ (G∘F.iso a)) ⟩
               (f ∘ G∘F.⇒.η a) ∘ G∘F.⇐.η a ≈˘⟨ G∘F.⇒.commute f ⟩∘⟨refl ⟩
               (G∘F.⇒.η b ∘ G.F₁ (F.₁ f)) ∘ G∘F.⇐.η a ≈⟨ assoc ⟩
               G∘F.⇒.η b ∘ ((G.F₁ (F.₁ f)) ∘ G∘F.⇐.η a) ≈⟨ refl⟩∘⟨ G∘F-f≈g ⟩∘⟨refl ⟩
               G∘F.⇒.η b ∘ ((G.F₁ (F.₁ g)) ∘ G∘F.⇐.η a) ≈˘⟨ refl⟩∘⟨ G∘F.⇐.commute g ⟩
               G∘F.⇒.η b ∘ (G∘F.⇐.η b ∘ g) ≈⟨ cancelˡ (Iso.isoʳ (G∘F.iso b)) ⟩
               g ∎
      where
      module G = Functor G
      module G∘F = NaturalIsomorphism G∘F
      G∘F-f≈g : G.₁ (F.₁ f) ≈ G.₁ (F.₁ g)
      G∘F-f≈g = G.F-resp-≈ f≈g
  open cis-faithful using (cis-faithful)

  module cis-full {A B : Category l-o l-l l-e} {F : Functor A B} where
    module A = Category A
    module B = Category B
    module F = Functor F
    open A using (_⇒_; _≈_; _∘_; assoc)
    open A.HomReasoning
    open MR A

    cis-full : Equivalence F → ∀ {a b : A.Obj} {g : F.F₀ a B.⇒ F.F₀ b} → Σ[ f ∈ a A.⇒ b ] F.F₁ f B.≈ g
    cis-full record { G = G ; G∘F = G∘F ; F∘G = F∘G } {a} {b} {g} =
             ⟨ f , cis-faithful (record { G = F ; G∘F = F∘G ; F∘G = G∘F }) G∘F/f≈G/g ⟩
      where
      module G = Functor G
      module G∘F = NaturalIsomorphism G∘F
      module F∘G = NaturalIsomorphism F∘G

      f : a A.⇒ b
      f = G∘F.⇒.η b A.∘ G.₁ g A.∘ G∘F.⇐.η a

      G∘F/f≈G/g : G.₁ (F.₁ f) A.≈ G.₁ g
      G∘F/f≈G/g =
                begin G.₁ (F.₁ f) ≈˘⟨ cancelˡ (Iso.isoˡ (G∘F.iso b)) ⟩
                G∘F.⇐.η b ∘ G∘F.⇒.η b ∘ G.₁ (F.₁ f) ≈⟨ refl⟩∘⟨ G∘F.⇒.commute f ⟩
                G∘F.⇐.η b ∘ f ∘ G∘F.⇒.η a ≈⟨ A.Equiv.refl ⟩
                G∘F.⇐.η b ∘ ((G∘F.⇒.η b ∘ (G.₁ g ∘ G∘F.⇐.η a)) ∘ G∘F.⇒.η a) ≈˘⟨ assoc ⟩
                (G∘F.⇐.η b ∘ (G∘F.⇒.η b ∘ (G.₁ g ∘ G∘F.⇐.η a))) ∘ G∘F.⇒.η a ≈⟨ cancelˡ (Iso.isoˡ (G∘F.iso b)) ⟩∘⟨refl ⟩
                (G.₁ g ∘ G∘F.⇐.η a) ∘ G∘F.⇒.η a ≈⟨ cancelʳ (Iso.isoˡ (G∘F.iso a)) ⟩
                G.₁ g ∎
  open cis-full using (cis-full)

  module cis-essen-surj {A B : Category l-o l-l l-e} {F : Functor A B} where
    module A = Category A
    module B = Category B
    module F = Functor F
    open A using (_⇒_; _≈_; _∘_; assoc)
    open Categories.Morphism B using (_≅_)

    cis-essen-surj : Equivalence F → ∀ {a′ : B.Obj} → Σ[ a ∈ A.Obj ] (F.F₀ a ≅ a′)
    cis-essen-surj record { G = G ; G∘F = G∘F ; F∘G = F∘G } {a′} =
                   ⟨ G.₀ a′ , record { from = F∘G.⇒.η a′ ; to = F∘G.⇐.η a′ ; iso = F∘G.iso a′ } ⟩
      where
      module G = Functor G
      module G∘F = NaturalIsomorphism G∘F
      module F∘G = NaturalIsomorphism F∘G
  open cis-essen-surj using (cis-essen-surj)
```

反方向的证明:

```agda
  record trans {l-a l-b l-c} {A B : Category l-a l-b l-c} {F : Functor A B} : Set (l-a ⊔ l-b ⊔ l-c) where
    module A = Category A
    module B = Category B
    module F = Functor F
    open Categories.Morphism B using (_≅_)

    field
      faithful : ∀ {a b : A.Obj} {f g : a A.⇒ b} → F.₁ f B.≈ F.₁ g → f A.≈ g
      full : ∀ {a b : A.Obj} {g : F.F₀ a B.⇒ F.F₀ b} → Σ[ f ∈ a A.⇒ b ] F.F₁ f B.≈ g
      essen-surj : ∀ {a′ : B.Obj} → Σ[ a ∈ A.Obj ] (F.F₀ a ≅ a′)

    G₀ : ∀ (a′ : B.Obj) → A.Obj
    G₀ a′ = proj₁ (essen-surj {a′})
    G₀-iso : ∀ {a′ : B.Obj} → F.₀ (G₀ a′) ≅ a′
    G₀-iso {a′} = proj₂ (essen-surj {a′})
    module G₀-iso {a′} = _≅_ (G₀-iso {a′})
    G₁ : ∀ {a′ b′} → a′ B.⇒ b′ → G₀ a′ A.⇒ G₀ b′
    G₁ {a′} {b′} f′ = proj₁ (full {g = G₀-iso.to B.∘ f′ B.∘ G₀-iso.from})
    G₁-iso : ∀ {a′ b′} {f′ : a′ B.⇒ b′} → F.₁ (G₁ f′) B.≈ G₀-iso.to B.∘ f′ B.∘ G₀-iso.from
    G₁-iso {a′} {b′} {f′} = proj₂ (full)

    G : Functor B A
    G = record
          { F₀ = G₀
          ; F₁ = G₁
          ; identity = faithful identity
          ; homomorphism = faithful homomorphism
          ; F-resp-≈ = λ f≈g → faithful (resp-≈ f≈g)
          }
      where
      open Category B
      open B.HomReasoning
      open MR B
      identity : ∀ {a′} → F.₁ (G₁ {a′} B.id) B.≈ F.₁ A.id
      identity {a′} =
               begin F.₁ (G₁ B.id) ≈⟨ G₁-iso ⟩
               G₀-iso.to ∘ B.id ∘ G₀-iso.from ≈˘⟨ refl⟩∘⟨ lemma-binid ⟩∘⟨refl ⟩
               G₀-iso.to ∘ (G₀-iso.from ∘ B.id ∘ G₀-iso.to) ∘ G₀-iso.from ≈˘⟨ refl⟩∘⟨ (refl⟩∘⟨ F.identity ⟩∘⟨refl) ⟩∘⟨refl ⟩
               G₀-iso.to ∘ (G₀-iso.from ∘ F.₁ A.id ∘ G₀-iso.to) ∘ G₀-iso.from ≈⟨ sym-assoc ⟩
               (G₀-iso.to ∘ (G₀-iso.from ∘ F.₁ A.id ∘ G₀-iso.to)) ∘ G₀-iso.from ≈⟨ cancelˡ (Iso.isoˡ G₀-iso.iso) ⟩∘⟨refl ⟩
               (F.₁ A.id ∘ G₀-iso.to) ∘ G₀-iso.from ≈⟨ cancelʳ (Iso.isoˡ G₀-iso.iso) ⟩
               F.₁ A.id ∎
        where
        lemma-binid : G₀-iso.from ∘ B.id ∘ G₀-iso.to ≈ B.id
        lemma-binid =
                    begin G₀-iso.from ∘ B.id ∘ G₀-iso.to ≈⟨ refl⟩∘⟨ B.identityˡ ⟩
                    G₀-iso.from ∘ G₀-iso.to ≈⟨ Iso.isoʳ G₀-iso.iso ⟩
                    B.id ∎

      homomorphism : ∀ {a′ b′ c′} {f : a′ B.⇒ b′} {g : b′ B.⇒ c′} → F.₁ (G₁ (g B.∘ f)) B.≈ F.₁ (G₁ g A.∘ G₁ f)
      homomorphism {f = f} {g} =
                   begin F.₁ (G₁ (g ∘ f)) ≈⟨ G₁-iso ⟩
                   G₀-iso.to ∘ (g ∘ f) ∘ G₀-iso.from ≈˘⟨ refl⟩∘⟨ (refl⟩∘⟨ B.identityˡ) ⟩∘⟨refl ⟩
                   G₀-iso.to ∘ (g ∘ id ∘ f) ∘ G₀-iso.from ≈˘⟨ refl⟩∘⟨ (refl⟩∘⟨ Iso.isoʳ G₀-iso.iso ⟩∘⟨refl) ⟩∘⟨refl ⟩
                   G₀-iso.to ∘ (g ∘ (G₀-iso.from ∘ G₀-iso.to) ∘ f) ∘ G₀-iso.from ≈⟨ refl⟩∘⟨ (refl⟩∘⟨ assoc) ⟩∘⟨refl ⟩
                   G₀-iso.to ∘ (g ∘ (G₀-iso.from ∘ (G₀-iso.to ∘ f))) ∘ G₀-iso.from ≈⟨ refl⟩∘⟨ sym-assoc ⟩∘⟨refl ⟩
                   G₀-iso.to ∘ ((g ∘ G₀-iso.from) ∘ (G₀-iso.to ∘ f)) ∘ G₀-iso.from ≈⟨ refl⟩∘⟨ assoc ⟩
                   G₀-iso.to ∘ (g ∘ G₀-iso.from) ∘ (G₀-iso.to ∘ f) ∘ G₀-iso.from ≈⟨ refl⟩∘⟨ refl⟩∘⟨ assoc ⟩
                   G₀-iso.to ∘ (g ∘ G₀-iso.from) ∘ (G₀-iso.to ∘ f ∘ G₀-iso.from) ≈˘⟨ refl⟩∘⟨ refl⟩∘⟨ G₁-iso ⟩
                   G₀-iso.to ∘ (g ∘ G₀-iso.from) ∘ F.₁ (G₁ f) ≈⟨ sym-assoc ⟩
                   (G₀-iso.to ∘ (g ∘ G₀-iso.from)) ∘ F.₁ (G₁ f) ≈˘⟨ G₁-iso ⟩∘⟨refl ⟩
                   F.₁ (G₁ g) ∘ F.₁ (G₁ f) ≈˘⟨ F.homomorphism ⟩
                   F.₁ (G₁ g A.∘ G₁ f) ∎

      resp-≈ : ∀ {a′ b′} {f g : a′ B.⇒ b′} → f ≈ g → F.₁ (G₁ f) ≈ F.₁ (G₁ g)
      resp-≈ {f = f} {g} f≈g =
             begin F.₁ (G₁ f) ≈⟨ G₁-iso ⟩
             G₀-iso.to ∘ f ∘ G₀-iso.from ≈⟨ refl⟩∘⟨ f≈g ⟩∘⟨refl ⟩
             G₀-iso.to ∘ g ∘ G₀-iso.from ≈˘⟨ G₁-iso ⟩
             F.₁ (G₁ g) ∎

    F∘G-id : NaturalIsomorphism (F ∘F G) f-id
    F∘G-id = niHelper (record
             { η = λ a → G₀-iso.from
             ; η⁻¹ = λ a → G₀-iso.to
             ; commute = commute
             ; iso = λ a → G₀-iso.iso
             })
      where
      open Category B
      open B.HomReasoning
      open MR B
      commute : ∀ {a b} (f : a B.⇒ b) → G₀-iso.from ∘ F.₁ (G₁ f) ≈ f ∘ G₀-iso.from
      commute f =
              begin G₀-iso.from ∘ F.₁ (G₁ f) ≈⟨ refl⟩∘⟨ G₁-iso ⟩
              G₀-iso.from ∘ (G₀-iso.to ∘ f ∘ G₀-iso.from) ≈⟨ cancelˡ (Iso.isoʳ G₀-iso.iso) ⟩
              f ∘ G₀-iso.from ∎
    module F∘G-id = NaturalIsomorphism F∘G-id

    G∘F-id : NaturalIsomorphism (G ∘F F) f-id
    G∘F-id = niHelper (record
             { η = λ a → proj₁ (full {g = G₀-iso.from})
             ; η⁻¹ = λ a → proj₁ (full {g = G₀-iso.to})
             ; commute = λ f → faithful commute
             ; iso = λ a → record { isoˡ = faithful (iso G₀-iso.isoˡ) ; isoʳ = faithful (iso G₀-iso.isoʳ) }
             })
      where
      open Category B
      open B.HomReasoning
      open MR B
      module G = Functor G
      commute : ∀ {a b} {f : a A.⇒ b} → F.₁ (proj₁ full A.∘ G.₁ (F.₁ f)) ≈ F.₁ (f A.∘ proj₁ full)
      commute {a} {b} {f = f} =
              begin F.₁ (proj₁ full A.∘ G.₁ (F.₁ f)) ≈⟨ F.homomorphism ⟩
              F.₁ (proj₁ full) ∘ F.₁ (G.₁ (F.₁ f)) ≈⟨ proj₂ full ⟩∘⟨refl ⟩
              G₀-iso.from ∘ F.₁ (G.₁ (F.₁ f)) ≈⟨ F∘G-id.⇒.commute (F.₁ f) ⟩
              F.₁ f ∘ G₀-iso.from ≈˘⟨ refl⟩∘⟨ proj₂ full ⟩
              F.₁ f ∘ F.₁ (proj₁ full) ≈˘⟨ F.homomorphism ⟩
              F.₁ (f A.∘ proj₁ full) ∎
      iso : ∀ {a b} {f : F.₀ a B.⇒ F.₀ b} {g : F.₀ b B.⇒ F.₀ a} → g ∘ f ≈ B.id
            → F.₁ (proj₁ (full {g = g}) A.∘ proj₁ (full {g = f})) ≈ F.₁ (A.id {a})
      iso {f = f} {g = g} g∘f≈id =
          begin F.₁ (proj₁ full A.∘ proj₁ full) ≈⟨ F.homomorphism ⟩
          F.₁ (proj₁ full) ∘ F.₁ (proj₁ full) ≈⟨ proj₂ full ⟩∘⟨refl ⟩
          g ∘ F.₁ (proj₁ full) ≈⟨ refl⟩∘⟨ proj₂ full ⟩
          g ∘ f ≈⟨ g∘f≈id ⟩
          B.id ≈˘⟨ F.identity ⟩
          F.₁ A.id ∎
```

## 1.3.34: category equivalence is equivalence relation

(这个在库里其实有, 而且代码确实挺简单的... (超小声(
~~因为口胡出来比较容易所以咕咕了 (!) x (超小声(被打死((((((~~

## (The Rest)

因为懒得找记号所以咕咕了... \>\< (超小声(缩成球(

