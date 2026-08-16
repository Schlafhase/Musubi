using Musubi.Compiler;

string compiled;
try
{
    compiled = MusubiCompiler.Compile(
        """
        let
            true := \a.\b.a;
            false := \a.\b.b;
            if := \x.x;
            and := \a.\b.a b false;
            or := \a.\b.a true b;

            S := \n.\f.\x.f (n f x);
            pred := λn.λf.λx.n (λg.λh.h (g f)) (λu.x) (λu.u);
            + := \a.\b.a S b;
            - := \a.\b.b pred a;

            IsZero := \n.n (\x.false) true;
            <= := \a.\b.IsZero (- a b);
            = := \a.\b.and (<= a b) (<= b a);
            == := =;
        in
            if (== 1 1) then 1 else 0;
        """,
        false
    );
}
catch (InvalidOperationException e)
{
    Console.WriteLine(e.Message);
    return 1;
}
File.WriteAllText("compiled.c", compiled);

return 0;
