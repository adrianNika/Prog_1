using System;

namespace Test_compteur
{
    internal class Program
    {
        static void Main(string[] args)
        {
            int x = 5;
            int y = 5;

            if (x > y) 
            {
                while (x >= 10)
                {
                    x = x + 1;
                    y = y + 5; 
                }
            }
            else
            {
                while (y >= 10)
                {
                    x = x + 5;
                    y = y + 1;
                }
            }

            Console.WriteLine($"x = {x}");
            Console.WriteLine($"y = {y}");
        }
    }
}
