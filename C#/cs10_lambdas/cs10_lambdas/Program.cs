// See https://aka.ms/new-console-template for more information


using System.Runtime.CompilerServices;

int Filtre1(int v)
{
    if (v < 0)
        return 0;

    return v;
}

int Operation (int a, int b, Func<int, int> f, [CallerArgumentExpression("f")] string? nom_param = null)
{
    a = f(a);
    b = f(b);

    // CallerArgumentExpression
    Console.WriteLine($"nom du param : {nom_param}");

    return a + b;
}


Func<int, int> Filtre2 = (int v) => v > 10 ? 10 : v;
/*idem -> var filtre2 = (int v) => v > 10 ? 10 : v;*/

Console.WriteLine(Operation(-5, 20, Filtre2));
