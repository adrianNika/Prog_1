using System;

namespace delegates // Note: actual namespace depends on the project name.
{
    internal class Program
    {
        public delegate string ValidationChaine(string s);


        static void Main(string[] args)
        {
            string nom = DemanderChaineUser("Quel est votre nom ? ", CheckValidationNom);
            string tel = DemanderChaineUser("Quel est votre numéro de téléphone ? ", CheckValidationTel);

            Console.WriteLine();
            Console.WriteLine($"Bonjour {nom} vous êtes joignable au numero de téléphone : {tel}");
        }

        static string DemanderChaineUser(string message, ValidationChaine fonctionValidation = null)
        {
            Console.Write(message);
            string reponse = Console.ReadLine();

            if (fonctionValidation != null)
            {
                string erreur = fonctionValidation(reponse);
                if (erreur != null)
                {
                    Console.WriteLine($"ERREUR : {erreur}");
                    return DemanderChaineUser(message, fonctionValidation);
                }
            }
            
            return reponse;
        }

        static string CheckValidationNom(string s)
        {
            if (string.IsNullOrWhiteSpace(s))
            {
                return "Le nom ne doit pas être vide";
            }

            if (s.Any(char.IsDigit))
            {
                return "Le nom ne doit pas contenir de chiffre";
            }

            return null;
        }

        static string CheckValidationTel(string s)
        {
            if (string.IsNullOrWhiteSpace(s))
            {
                return "Le numéro de téléphone ne doit pas être vide";
            }

            if(s.Length != 10)
            {
                return "Le numero de téléphone doit contenir 10 chiffres";
            }

            if (!s.All(char.IsDigit))
            {
                return "Le numéro de téléphone doit contenir uniquement des chiffre";
            }

            return null;
        }
    }
}