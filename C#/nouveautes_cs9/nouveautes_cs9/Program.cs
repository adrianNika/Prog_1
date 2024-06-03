using System;

namespace nouveautes_cs9 // Note: actual namespace depends on the project name.
{
    struct PersonneStruct
    {
        public string nom { get; set; }
        public int age { get; set; }

        public void Afficher()
        {
            Console.WriteLine($"nom : {nom} - age : {age}ans");
        }
    }
    class PersonneClasse
    {
        public string nom { get; set; }
        public int age { get; set; }

        public void Afficher()
        {
            Console.WriteLine($"nom : {nom} - age : {age}ans");
        }

        public override bool Equals(object obj)
        {
            return base.Equals(obj);
        }
    }

    /*record PersonneRecord
    {
        public string nom { get; set; }
        public int age { get; set; }

        public PersonneRecord(string nom, int age)
        {
            this.nom = nom;
            this.age = age;
        }

        public void Deconstruct(out string nom, out int age)
        {
            nom = this.nom;
            age = this.age;
        }

        public void Afficher()
        {
            Console.WriteLine($"nom : {nom} - age : {age}ans");
        }
    }*/


    /*IMMUTABLE*/
    record PersonneRecord(string nom, int age);
     


    internal class Program
    {
        static void Main(string[] args)
        {

            /*int a = 5;
            int b = 10;

            b = a;
            a = 5;

            Console.WriteLine($"a = {a}");
            Console.WriteLine($"b = {b}");*/

            var personne1 = new PersonneRecord("Toto", 20);
            //var personne2 = personne1 with { nom = "Tata"};

            var (nom, age) = personne1;

            Console.WriteLine($"nom : {personne1.nom}");
            Console.WriteLine($"age : {personne1.age}");

            Console.WriteLine($"nom : {nom}");
            Console.WriteLine($"age : {age}");

            //personne1.Afficher();
            //personne2.Afficher();

            //Console.WriteLine(personne1.Equals(personne2));
        }
    }
}