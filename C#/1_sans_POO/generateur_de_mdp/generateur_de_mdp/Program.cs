using System;
using FormationCS;
//using System.Threading;

namespace generateur_de_mdp // Note: actual namespace depends on the project name.
{
    internal class Program
    {
        enum e_choixSytleMDP
        { 
            MINUSCULE = 1,
            MINUSCULE_MAJUSCULE = 2,
            MINUSCULE_MAJUSCULE_CHIFFRE = 3,
            MINUSCULE_MAJUSCULE_CHIFFRE_CARACTERE = 4
        }

        static void Main(string[] args)
        {
            Random rand = new Random();

            const int NOMBRE_DE_MDP = 10;
            string renew = "Yes";

            while (renew == "Yes" || renew == "yes" || renew == "y" || renew == "Y" || renew == "YES")
            {
                int longeurMotDePasse = 0;
                int longueurAlphabet = 0;
                int lettreAleatoire = 0;

                string minuscules = "abcdefghijklmnopqrstuvwxyz";
                string majuscules = minuscules.ToUpper();
                string chiffres = "0123456789";
                string caractereSpeciaux = "#!$%&+-_@^*=~':;,./|?£`";
                string alphabet = "";
                string motDePasse = "";

                Console.WriteLine("----------------Générateur de Mot de Passe----------------");
                Console.WriteLine();
                Console.WriteLine();


                // 1 - Demander la longuer du mdp (demanderNombre)
                longeurMotDePasse = outils.DemanderNombrePositifNonNull("Longeur du mot de passe : ");
                Console.WriteLine();

                e_choixSytleMDP choixTypeAlphabetMdp = (e_choixSytleMDP)outils.DemanderNombreEntre(
                "Quelle type de mot de passe voulais vous générer : \n" +
                "1 - Uniquement des caractères en minuscule \n" +
                "2 - Des caractères minuscules et majuscules \n" +
                "3 - Des caractère et des chiffres \n" +
                "4 - Caractères, chiffres et caractère spéciaux \n" +
                "Votre choix : ", 1, 4);

                Console.WriteLine();

                switch (choixTypeAlphabetMdp)
                {
                    case e_choixSytleMDP.MINUSCULE:
                        alphabet = minuscules;
                        break;
                    case e_choixSytleMDP.MINUSCULE_MAJUSCULE:
                        alphabet = minuscules + majuscules;
                        break;
                    case e_choixSytleMDP.MINUSCULE_MAJUSCULE_CHIFFRE:
                        alphabet = minuscules + majuscules + chiffres;
                        break;
                    case e_choixSytleMDP.MINUSCULE_MAJUSCULE_CHIFFRE_CARACTERE:
                        alphabet = minuscules + majuscules + chiffres + caractereSpeciaux;
                        break;

                    default:
                        break;
                }

                longueurAlphabet = alphabet.Length;

                // 4 - Générer le mot de passe
                for (int i = 1; i <= NOMBRE_DE_MDP; i++)
                {
                    motDePasse = "";

                    for (int j = 0; j < longeurMotDePasse; j++)
                    {
                        lettreAleatoire = rand.Next(0, longueurAlphabet);
                        motDePasse += alphabet[lettreAleatoire];
                    }

                    Console.WriteLine($"Mot de passe n°{i} : {motDePasse}");
                }

                while (true)
                {
                    Console.WriteLine();
                    Console.WriteLine("Voulez vous en générer d'autre : Yes or No");
                    renew = Console.ReadLine();

                    if (renew == "Yes" || renew == "yes" || renew == "y" || renew == "Y" || renew == "YES")
                    {
                        break;
                    }
                    else if (renew == "No" || renew == "no" || renew == "N" || renew == "n" || renew == "NO")
                    {
                        break;
                    }
                    else
                    {
                        Console.WriteLine();
                        Console.WriteLine("ERREUR");
                        Console.WriteLine("Entrez sois Yes, yes, Y, y ou YES, pour reccomencer");
                        Console.WriteLine("Entrez sois No, no, N, n ou NO, pour arreter le programme");
                    }
                }
            }
        }
    }
}