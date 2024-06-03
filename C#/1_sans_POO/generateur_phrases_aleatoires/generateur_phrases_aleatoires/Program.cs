using System;
using System.Security.Cryptography;

namespace generateur_phrases_aleatoires // Note: actual namespace depends on the project name.
{
    internal class Program
    {
        static string OptenireElementAleatoire(string[] t)
        {
            Random rnd = new Random();

            int tailleTableau = 0;
            string elmentAleatoire = "";

            tailleTableau = rnd.Next(t.Length);
            elmentAleatoire = t[tailleTableau] + " ";

            return elmentAleatoire;
        }

        static void Main(string[] args)
        {
            List<string> phraseUnique = new List<string>();

            var sujets = new string[] 
            { 
                "Un lapin", 
                "Un étranger", 
                "Un chat", 
                "Un huamin", 
                "Une baleine",
                "Une tortue",
                "Une limace",
                "Un chien",
                "Un gorille",
                "Un ange",
                "Satan",
                "Dieux",
                "Un Demon"
            };

            var verbes = new string[]
            {
                "mange",
                "écrase",
                "détruit",
                "observe",
                "attrape",
                "regarde",
                "avale",
                "néttoie",
                "se bat avec",
                "s'accroche à"
            };

            var complements = new string[]
            {
                "un arbre",
                "un livre",
                "la lune",
                "le soleil",
                "un serpent",
                "une carte",
                "une girafe",
                "le ciel",
                "une piscine",
                "un gateau",
                "une souris",
                "un crapaud"
            };

            const int NB_PHRASE = 100;
            int doublons = 0;

            while (phraseUnique.Count != NB_PHRASE)
            {
                var sujet = OptenireElementAleatoire(sujets);
                var verbe = OptenireElementAleatoire(verbes);
                var comlements = OptenireElementAleatoire(complements);

                var phrase = sujet + verbe + comlements.Trim() + ".";

                phrase = phrase.Replace("à le", "au");

                if (!phraseUnique.Contains(phrase))
                {
                    phraseUnique.Add(phrase);
                }
                else
                {
                    doublons++;
                }
            }

            foreach (var phrase in phraseUnique)
            {
                Console.WriteLine(phrase);
            }

            Console.WriteLine();
            Console.WriteLine($"Nombre de phrase unique : {phraseUnique.Count}");
            Console.WriteLine($"Nombre de doublons : {doublons}");
        }
    }
}