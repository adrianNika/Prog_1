using System;

namespace MyApp // Note: actual namespace depends on the project name.
{
    internal class Program
    {

        /*--------------------------------FONCTION----------------------------------*/

        static string DemanderNom(int numeroPersonne)
        {
            string nom = "";

            while (nom == "")
            {
                Console.Write($"Quelle est le nom de la personne N°{numeroPersonne} ? ");
                nom = Console.ReadLine();
                nom = nom.Trim();

                if (nom == "")
                {
                    Console.WriteLine("Erreur, le nom doit pas êtres vide");
                }
            }

            return nom;
        }

        static int DemanderAge(string nomPersonne)
        {
            string age_str = "";
            int age = 0;

            while (age <= 0)
            {
                Console.Write(nomPersonne + " quelle est ton age ? ");
                age_str = Console.ReadLine();

                try
                {
                    /*passer un entier string en valeurs numérique*/
                    age = int.Parse(age_str);
                    if (age < 0)
                    {
                        Console.WriteLine("Erreur, vous ne pouvez par rentrer un age négatif");
                    }
                    else if (age == 0)
                    {
                        Console.WriteLine("Erreur, l'age ne dois pas etres a 0");
                    }
                }
                catch
                {
                    Console.WriteLine("ERREUR, voud devez entrez un age valide.");
                }
            }

            return age;
        }

        static float DemanderTaille()
        {
            string taille = "";
            float tailleF = 0.0f;

            Console.Write("Quelle taille faisez-vous ? ");
            taille = Console.ReadLine();
            taille = taille.Trim();

            if (taille == "")
            {
                taille = "0";
            }
            tailleF = float.Parse(taille);

            return tailleF;
        }

        static void AfficheInfoPersonne(string nom, int age, float taille = 0)
        {
            Console.WriteLine($"Personne enregistré   : {nom}");
            Console.WriteLine($"Age de la personne    : {age}ans");

            if (taille != 0)
            {
                Console.WriteLine($"Taille de la personne : {taille}m");
            }
            else
            {
                Console.WriteLine("Taille de la personne : Non définie");
            }
            
            Console.Write("maturité              : ");

            if (age == 18)
            {
                Console.WriteLine("tout juste majeur");
            }
            else if (age == 17)
            {
                Console.WriteLine("bientot majeur");
            }
            else if (age >= 12 && age < 18)
            {
                Console.WriteLine("ado");
            }
            else if (age >= 60)
            {
                Console.WriteLine("senior");
            }
            else if (age >= 18)
            {
                Console.WriteLine("majeur");
            }
            else if (age == 1 || age == 2)
            {
                Console.WriteLine("bébé");
            }
            else if (age < 10)
            {
                Console.WriteLine("enfant");
            }
            else
            {
                Console.WriteLine("mineur");
            }

            Console.WriteLine();
        }

/*-------------------------------MAIN--------------------------------*/
        
        static void Main(string[] args)
        {
            int age = 0;
            int flag = 0;
            float taille = 0.0f;
            string nom = "";

            string nbrPersonne = "";
            
            while (flag == 0)
            {
                Console.Write("Combien de personne voulez vous enregistrez ? ");
                nbrPersonne = Console.ReadLine();

                try
                {
                    int nbrInt = int.Parse(nbrPersonne);

                    if (nbrInt < 0)
                    {
                        Console.WriteLine("Vous pouvez pas rentrer un nombre negatif !");
                    }
                    else
                    {
                        flag = 1;
                    }
                }
                catch
                {
                    Console.WriteLine("Erreur, vous devez entrer un nombre valide !");
                }

                Console.WriteLine();
            }

            Console.WriteLine();

            int nbr_Personne = int.Parse(nbrPersonne);

            for (int i = 1; i <= nbr_Personne; i++)
            {
                nom = DemanderNom(i);
                age = DemanderAge(nom);
                taille = DemanderTaille();

                AfficheInfoPersonne(nom, age, taille);
                Console.WriteLine("---");
            }
        }
    }
}