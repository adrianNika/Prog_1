using System;
using AsciiArt;
using System.IO;

namespace jeu_du_pendu // Note: actual namespace depends on the project name.
{
    internal class Program
    {
        static string fichierDeMot = "mots.txt";

/*----------------------------FONCTION---------------------------*/

        /*affiche trait-d'union et lettre trouver*/
        static void afficherMot(string mot, List<char> lettre)
        {
            Console.WriteLine();

            for (int i = 0; i < mot.Length; i++) 
            {
                if (lettre.Contains(mot[i]))
                {
                    Console.Write($"{mot[i]} ");
                }
                else
                {
                    Console.Write("_ ");
                }
            }

            Console.WriteLine();
        }




        /*verifie si toute les lettere on était vérifier*/
        static bool TouteLettreDevines(string mot, List<char> lettres)
        {
            foreach(var lettre in lettres)
            {
                mot = mot.Replace(lettre.ToString(), "");
            }


            if (mot.Length == 0)
            {
                return true;
            }
            return false;
        }



        /*Demander une lettre*/
        static char DemanderUneLettre(string message = "")
        {
            while (true)
            {
                Console.WriteLine();
                Console.Write(message);

                string lettreUser = "";
                lettreUser = Console.ReadLine();

                if (lettreUser.Length == 1)
                {
                    lettreUser = lettreUser.ToUpper();
                    return lettreUser[0];
                }
                else
                {
                    Console.WriteLine("ERREUR : vous devez rentrer une Lettre");
                }
                Console.WriteLine();
            }
        }


        /*fonction principale du deviner mot*/
        static void DevinerMot(string mot)
        {
            var lettreDevine = new List<char> ();
            var lettresFausse = new List<char> ();  

            const int NB_VIES = 6;
            int viesRestante = NB_VIES;
            bool victoire = false;

            while (viesRestante > 0)
            {
                Console.WriteLine(Ascii.PENDU[NB_VIES - viesRestante]);


                afficherMot(mot, lettreDevine);
                var lettre = DemanderUneLettre("Donner une lettre : ");

                Console.Clear();

                if (mot.Contains(lettre))
                {
                    Console.WriteLine($"Cette lettre est dans le mot : {lettre}");
                    lettreDevine.Add(lettre);
                    victoire = TouteLettreDevines(mot, lettreDevine);
                    if(victoire == true)
                    {
                        break;
                    }
                }
                else
                {
                    if (!lettresFausse.Contains(lettre))
                    {
                        viesRestante--;
                        lettresFausse.Add(lettre);
                    }

                    Console.WriteLine($"Vies restantes : {viesRestante}");
                }

                if (lettresFausse.Count > 0)
                {
                    Console.WriteLine($"Le mot ne contient pas les lettre : {String.Join(", ", lettresFausse)}");
                }
                Console.WriteLine();
            }


            Console.WriteLine(Ascii.PENDU[NB_VIES - viesRestante]);

            if (viesRestante == 0)
            {
                Console.WriteLine($"Vous avez Perdu ! Le mot était : {mot}");
            }
            else
            {
                Console.WriteLine("Vous avez gagner !!!");
            }
        }


        static string[] ChargerLesMots(string nomFichier)
        {
            while (true)
            {
                try
                {
                    Console.Clear();
                    return File.ReadAllLines(nomFichier); 
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"ERREUR : de lecture du fichier : {nomFichier} ({ex.Message})");
                    Console.WriteLine();
                    Console.Write("Ecrivez le nom correct du fichier en ajoutant bien '.txt' à la fin : ");
                    nomFichier = Console.ReadLine();
                    fichierDeMot = nomFichier;
                }
            }

            return null;
        }


        static bool DemanderDeRejouer ()
        {
            var renew = DemanderUneLettre("Voulez vous rejouez O/N : ");

            if (renew == 'O' || renew == 'o')
            {
                //Rejouer
                return true;
            }
            else if (renew == 'N' || renew == 'n')
            {
                return false;
            }
            else
            {
                Console.WriteLine("Veuillez entrer oui : O ou non : N");
                return DemanderDeRejouer();
            }
        }

        static void Main(string[] args)
        {
            
            var mots = ChargerLesMots(fichierDeMot);

            if (mots == null || mots.Length == 0)
            {
                Console.WriteLine("La liste de mots est vide");
            }
            else
            {
                bool jouer = true;
                while(jouer)
                {
                    Random r = new Random();
                    var motAleatoire = r.Next(0, mots.Length);
                    string mot = mots[motAleatoire].Trim().ToUpper();
                    DevinerMot(mot);

                    jouer = DemanderDeRejouer();
                    Console.Clear();
                }

                Console.WriteLine("Merci et à bientôt !");
            }   
        }
    }
}