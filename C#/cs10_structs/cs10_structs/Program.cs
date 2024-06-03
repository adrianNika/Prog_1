using System;

namespace cs10_structs // Note: actual namespace depends on the project name.
{
    struct PersonneStruct
    {
        public string nom { get; set; }
        public int age { get; set; }

        public PersonneStruct() 
        {
            nom = "Inconnue";
            age = -1;
        }

    }

    /**/
    /*record struct PersonneRecord
    {
        public string? nom { get; set; }
        public int age { get; set; }
    }*/

    record PersonneRecord(string nom, int age);

    internal class Program
    {
        static void Main(string[] args)
        {
            var pr1 = new PersonneRecord ( "Toto",  20);
            var pr2 = pr1 with { nom = "Tata", age = 50 };

            var ps1 = new PersonneStruct { nom = "Paul", age = 20 };
            var ps2 = new PersonneStruct { nom = "Paul", age = 20 };

            //pr1.nom = "Toto";
            Console.WriteLine($"ps1 : {ps1.nom} - {ps1.age}ans");
            Console.WriteLine(ps1.Equals(ps2));
            Console.WriteLine();

            Console.WriteLine($"pr1 : {pr1}");
            Console.WriteLine($"pr2 : {pr2}");
            Console.WriteLine(pr1 == pr2);
            Console.WriteLine();

            string nom = "";
            /*int age = 0;*/

            ( nom, var age) = pr1;

            Console.WriteLine(nom);
            Console.WriteLine(age);
        }
    }
}