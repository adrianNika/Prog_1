using Microsoft.VisualBasic;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.WebSockets;

namespace programme_poo // Note: actual namespace depends on the project name.
{

    class Enfant : Etudiant
    {
        string classeEcole;
        Dictionary<string, float> notes;


        public Enfant(string nom, int age, string classeEcole, Dictionary<string, float> notes) : base(nom, age, null)
        {
            this.classeEcole = classeEcole;
            this.notes = notes;
        }

        public override void Afficher()
        {
            AfficherNomEtAge();
            Console.WriteLine($"  Enfant en classe de {classeEcole}");
            Console.WriteLine();

            if (notes != null && notes.Count > 0)
            {
                Console.WriteLine($"  Notes moyennes :");
                foreach (var item in notes)
                {
                    Console.WriteLine($"    {item.Key} : {item.Value} / 10");
                }
            }

            Console.WriteLine();
            AfficherProfesseurPrincipal();
        }
    }

    class Etudiant : Personne
    {
        string infoEtudes;
        public Personne professeurPrincipal { get; init; }

        public Etudiant(string nom, int age, string infoEtudes) : base(nom, age, "Etudiant")
        {
            this.infoEtudes = infoEtudes;
        }


        public override void Afficher()
        {
            AfficherNomEtAge();
            Console.WriteLine($"  Etudiant en {infoEtudes}");
            Console.WriteLine();
            AfficherProfesseurPrincipal();
        }  
        
        public void AfficherProfesseurPrincipal()
        {
            if (professeurPrincipal != null)
            {
                Console.WriteLine("Le professeur principal est :");
                professeurPrincipal.Afficher();
            }
        }
    }



    class Personne : IAffichable
    {
        private static int nombreDePersonne = 0;

        public string nom { get; init; }
        public int age { get; init; }
        protected string emploi; 

        protected int numeroParPersonne;

        public Personne(string nom, int age, string emploi = null)
        {
            this.nom = nom;
            this.age = age;
            this.emploi = emploi;

            nombreDePersonne++;
            this.numeroParPersonne = nombreDePersonne;
        }


        public virtual void Afficher()
        {
            AfficherNomEtAge();
            if (emploi != null)
            {
                Console.WriteLine($"  emploi : {emploi}");
            }
            else
            {
                Console.WriteLine("  Aucun emplois spécifié");
            }

            Console.WriteLine();
        }

        protected void AfficherNomEtAge()
        {
            Console.WriteLine($"Personne N° : {numeroParPersonne}");
            Console.WriteLine($"  NOM : {nom}");
            Console.WriteLine($"  AGE : {age} ans");
        }

        protected static void AfficherNombrePersonne()
        {
            Console.WriteLine();
            Console.WriteLine($"Nombre total de personne : {nombreDePersonne}");
        }
    }


    class TableMulitiplication : IAffichable
    {
        int numero;
        int nombreDeCalcule;

        public TableMulitiplication(int numero, int nombreDeCalcule)
        {
            this.numero = numero;
            this.nombreDeCalcule = nombreDeCalcule;
        }

        public void Afficher()
        {
            Console.WriteLine();
            Console.WriteLine($"Table de multiplication de {numero}");

            for (int i = 0; i <= nombreDeCalcule; i++)
            {
                Console.WriteLine($"{i} X {numero} = {i * numero}");
            }
        }
    }

    interface IAffichable
    {
        void Afficher();
    }

    internal class Program
    {
        static void Main(string[] args)
        {

            var elements = new List<IAffichable>
            {
                new Personne("Paul", 30 , "Dévloppeur"),
                new Personne("Sam", 35, "Prof"),
                new Etudiant("Jaque", 20, "ES"),
                new Enfant("Juliette", 8, "CP", null),
                new TableMulitiplication(3,12)
            };

            //personnes = personnes.Where(p => (p.nom[0] == 'S') && (p.age > 30)).ToList();

            foreach (var element in elements)
            {
                element.Afficher();
            }

            var multiplication = new TableMulitiplication(5, 5);
            multiplication.Afficher();
        }
    }
}
