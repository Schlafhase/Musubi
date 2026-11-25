using Musubi.Compiler.Scanning;

namespace Musubi.Tests
{
    public static class SharedValues
    {
        public const string ModuleSource = """
            let
                a := 123,
                b := "hello",
                c := 'c',
                d := someId,
                ns := 10s,
                nc := 15c,
                nb := 17b,
            in
            """;

        public static TokenType[] ModuleExpectedTokens =
        [
            TokenType.Let,
            TokenType.Identifier,
            TokenType.Definition,
            TokenType.Number,
            TokenType.ListSeparator,
            //
            TokenType.Identifier,
            TokenType.Definition,
            TokenType.String,
            TokenType.ListSeparator,
            //
            TokenType.Identifier,
            TokenType.Definition,
            TokenType.Char,
            TokenType.ListSeparator,
            //
            TokenType.Identifier,
            TokenType.Definition,
            TokenType.Identifier,
            TokenType.ListSeparator,
            //
            TokenType.Identifier,
            TokenType.Definition,
            TokenType.ScottNumber,
            TokenType.ListSeparator,
            //
            TokenType.Identifier,
            TokenType.Definition,
            TokenType.ChurchNumber,
            TokenType.ListSeparator,
            //
            TokenType.Identifier,
            TokenType.Definition,
            TokenType.BinaryNumber,
            TokenType.ListSeparator,
            //
            TokenType.In,
            TokenType.EOF,
        ];

        public const string DirectivesSource = """
            #once
            #include basic.mbi
            #define -> "helo"
            c
            ->
            #once
            """;

        public const string NestedIncludeSource = """
            #include nested.mbi
            world
            """;
        public const string BinaryArithmeticsSource = """
; Arithmetics for binary numbers encoded using
; scott lists of booleans
#once
#include stdlib/recursion.mbim
#include stdlib/lists.mbim
#include stdlib/generalMacros.mbim
#include stdlib/boolean.mbim
#include stdlib/pipe.mbim
#include  stdlib/tupel.mbim

;!typedef uintb = List<bool>
; binary integers are defined as a tupel (sign, abs)
;!typedef intb = Tupel<bool, uintb>

let
  ; Successor function for binary numerals
  ; Parameter n:
  ;  The numeral to take the successor of
  ; Returns:
  ;  The successor of `n` as a binary encoded numeral
  S<uintb> := 
    rec (\S<uintb>.\n ->
      n [ true ] (\x.\xs ->
       if x then
         cons false (S<uintb> xs)
       else
         cons true xs
       end)
    ),

  ; NOTE: i don't remember why i made this and now it
  ; seems useless to me
  ; lengthb :=
  ;   rec (\lengthb.\n ->
  ;     n ([ false ]) (\x.\xs -> Sb (lengthb xs))
  ;   ),

  pred<uintb> :=
    rec (\pred<uintb>.\n ->
      n [ false ] (\x.\xs ->
        if x then
          false : xs
        else
          true : (pred<uintb> xs)
        end
      )
    ),

  isZero<uintb> := \n -> not (any n),

  eq<uintb> := 
    rec (\eq<uintb>.\a -> \b ->
      if isZero<uintb> a then
        isZero<uintb> b
      else
        if isZero<uintb> b then
          isZero<uintb> a
        else
          ((head a) xnor (head b)) and (eq<uintb> (tail a) (tail b))
        end
      end
    ),


  compareGeneric<uintb> :=
    \checkGreater ->
      let
        gtb' :=
          rec(\gtb'.\a -> \b -> \prev ->
            if isZero<uintb> a then
              if isZero<uintb> b then
                prev
              else
                not checkGreater
              end
            else
              a --- ; don't care about nil case (since already checked by isZero<uintb>)
                (\headA.\tailA ->
                  b
                    checkGreater
                    (\headB.\tailB ->
                      let
                        curr :=
                          if headA xnor headB then
                            prev
                          else
                            if headA and (not headB) then
                              checkGreater
                            else
                              not checkGreater
                            end
                          end
                      in
                        gtb' tailA tailB curr
                    )
                )
            end
          )
      in
        \a -> \b -> gtb' a b false,

  gt<uintb> := compareGeneric<uintb> true,
  lt<uintb> := compareGeneric<uintb> false,
  geq<uintb> := \a -> \b -> not (lt<uintb> a b),
  leq<uintb> := \a -> \b -> not (gt<uintb> a b),

  minimiseIntb := \n -> reverse n | skipWhile (\x -> not x) | reverse,

  add<uintb> :=
    let
      addb' :=
        rec(\addb'.\a -> \b -> \c -> ; a: intb, b: intb, c: bool
          a
            ; a = 0:
            (b 
              ; b = 0:
              [ c ]
              ; b = x:xs:
              (\_.\_.c (S<uintb> b) b)
            )
            ; a = headA : tailA:
            (\headA.\tailA ->
              b (c (S<uintb> a) a)
                (\headB.\tailB ->
                  let
                    result := (headA xor headB xor c) or (headA and headB and c),
                    carry := (headA and headB) or (c and (headA xor headB))
                  in
                    result : (addb' tailA tailB carry)
                )
            )
        )
    in
      \a -> \b -> addb' a b false,

  sub<uintb> :=
    let
      ; `subb' :: uintb -> uintb -> bool -> (uintb, bool)`
      subb' :=
        rec(\subb'.\a -> \b -> \br ->
          a
            (T nil br)
            (\headA.\tailA ->
              if isZero<uintb> a then
                T nil br
              else
                b
                  (T (br (pred<uintb> a) a) false)
                  (\headB.\tailB ->
                    let
                      result := (headA xor headB xor br),
                      brOut := ((not headA) and (headB or br)) or (headB and br)
                    in
                      T (result : fst (subb' tailA tailB brOut)) false
                  )
              end
            )
        )
    in
      \a -> \b -> 
        let
          result := subb' a b false
        in
          if snd result then
            nil
          else
            fst result
          end,

  ; `shiftl<uintb> :: uintb -> uintb -> uintb`
  shiftl<uintb> := 
    rec (\shiftl<uintb>.\n -> \amount -> 
      if (isZero<uintb> amount) or (isZero<uintb> n) then
        n
      else
        false : (shiftl<uintb> n (pred<uintb> amount))
      end
    ),
  ; `shiftr<uintb> :: uintb -> uintb -> uintb`
  shiftr<uintb> := 
    rec (\shiftr<uintb>.\n -> \amount -> 
      if (isZero<uintb> amount) then
        n
      else
        n nil (\x.\xs -> shiftr<uintb> xs (pred<uintb> amount))
      end
    ),

  ; TODO: doesn't work
  mult<uintb> := 
    \a -> \b ->
      aggregate 
        a 
        (\bit -> \acc -> 
          let n := fst acc, v := snd acc in
          if bit then
            T (S<uintb> n) (add<uintb> v b)
          else
            acc
          end
        )
        (true : nil)
      | snd
in
""";
        public const string FacSourceNoIncludes = """
let
  fac :=
    rec (\fac.\n. ; rec is the Z fix-point combinator
      n 1s (\p.n * (fac p))
    )
in
  (fac 5s) | toString<ints>
""";
    }
}
