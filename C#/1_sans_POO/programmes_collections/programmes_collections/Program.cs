using System;
using System.Collections;
using System.Linq;

namespace programmes_collections // Note: actual namespace depends on the project name.
{
    internal class Program
    {

/*--------------------------------TABLEAU---------------------------------*/

        static int SommeTableuxInt(int[] t)
        {
            int somme = 0;

            for (int i = 0; i < t.Length; i++) 
            {
                somme += t[i];
            }

            return somme;
        }

        static void AfficherTableaux(int[] tableau)
        {
            for (int i = 0; i < tableau.Length; i++)
            {
                Console.WriteLine($"[{i}] {tableau[i]}");
            }
        }

        static void AfficherValeursMax(int[] tableau)
        {
            int maxValue = tableau[0];

            for (int i = 0; i < tableau.Length; i++)
            {
                if (maxValue < tableau[i])
                {
                    maxValue = tableau[i];
                }
            }

            Console.WriteLine($"Valeurs maximale est : {maxValue}");
        }

        static void AfficherValeursMin(int[] tableau)
        {
            int minValue = tableau[0];

            for (int i = 0; i < tableau.Length; i++)
            {
                if (minValue > tableau[i])
                {
                    minValue = tableau[i];
                }
            }

            Console.WriteLine($"Valeurs minimale est : {minValue}");
        }

        static void Tableux()
        {
            Random rnd = new Random();

            //int[] t = { 200, 40, 15, 8, 12 };

            const int taillTableaux = 20;

            int[] t = new int[taillTableaux];

            for (int i = 0; i < t.Length; i++)
            {
                t[i] = rnd.Next(101);
            }

            AfficherTableaux(t);
            AfficherValeursMax(t);
            AfficherValeursMin(t);
        }


/*--------------------------------LISTE---------------------------------*/

        static void AfficherListeINT(List<int> liste)
        {
            for (int i = 0;i < liste.Count;i++)
            {
                Console.WriteLine(liste[i]);
            }
        }

        static void AfficherListSTR(List<string> liste, bool ordreDescendant = false)
        {
            string nomPlusGrandSTR = "";

            if (!ordreDescendant)
            {
                for (int i = 0; i < liste.Count; i++)
                {
                    Console.WriteLine(liste[i]);
                }
            }
            else
            {
                for (int i = liste.Count-1; i >= 0; i--)
                {
                    Console.WriteLine(liste[i]);
                }
            }

            for (int i = 0; i < liste.Count; i++)
            {
                if (liste[i].Length > nomPlusGrandSTR.Length)
                {
                    nomPlusGrandSTR = liste[i];
                }
            }

            Console.WriteLine();
            Console.WriteLine($"Le nom le plus grand est : {nomPlusGrandSTR}");

        }

        static void AfficherElementComun(List<string> liste1, List<string> liste2)
        {
            for(int i = 0; i < liste1.Count; i++)
            {
                if (liste2.Contains(liste1[i]))
                {
                    Console.WriteLine(liste1[i]);
                }
            }
        }

        static void Listes()
        {
            string keepNom = "";

            List<int> liste = new List<int>();

            liste.Add(5);
            liste.Add(8);
            liste.Add(2);
            liste.Add(2);
            liste.Add(2);

            liste[2]++;
            liste.Remove(8);
            liste.RemoveAt(0);

            AfficherListeINT(liste);

            //AfficherListSTR(liste);


            List<string> listeNom = new List<string>();

            while (true)
            {
                Console.Write("Rentrez un nom (ENTER pour finir) : ");
                string nom = Console.ReadLine();

                if (nom == "")
                {
                    break;
                }

                if(listeNom.Contains(nom))
                {
                    Console.WriteLine("ERREUR : se nom est déja dans la liste !");
                    Console.WriteLine();
                }
                else
                {
                    listeNom.Add(nom);
                }
            }

            /*afficher une partie de la liste*/
            //List<string> lesPremiersNoms = listeNom.GetRange(0, 3);

            /* for (int i = listeNom.Count-1; i >= 0 ; i--)
             {
                 keepNom = listeNom[i];
                 if (keepNom[keepNom.Length-1] == 'e')
                 {
                     listeNom.RemoveAt(i);
                 }
             }


             AfficherListSTR(listeNom);


            var liste1 = new List<string>() { "paul", "jean", "pierre", "emilie", "martin" };
            var liste2 = new List<string>() { "sophie", "jean", "martin", "toto", "pierre", "emilie" };
            //en commun jean martin


            AfficherElementComun(liste1, liste2);*/

        }


/*--------------------------------ARRAYLIST---------------------------------*/

        static void ArrayList()
        {
            ArrayList a = new ArrayList();
            a.Add("toto");
            a.Add(21);
            a.Add(true);

            for (int i = 0; i < a.Count; i++)
            {
                Console.WriteLine(a[i]);
            }
        }


/*--------------------------------LISTE DE LISTE------------------------------*/


        static void ListesDeListes()
        {
            // France : Paris, Toulouse, Nice, Bordeaux, Lille
            // Etats-unis : New-York, Chicago, Los angeles, San Francisco
            // Espagne : Madrid, Barcelone, Sévile
            // Italie : Venise, Florence, Milan, Pise

            /*var villes = new List<string>();


            villes.Add("France : Paris, Toulouse, Nice, Bordeaux, Lille");
            villes.Add("Etats-unis : New-York, Chicago, Los angeles, San Francisco");
            villes.Add("Espagne : Madrid, Barcelone, Sévile");
            villes.Add("Italie : Venise, Florence, Milan, Pise");*/

            var pays = new List<List<string>>();

            pays.Add(new List<string> { "France", "Paris", "Toulouse", "Nice", "Bordeaux", "Lille" });
            pays.Add(new List<string> { "Etats-unis", "New-York", "Chicago", "Los angeles", "San Francisco" });
            pays.Add(new List<string> { "Espagne", "Madrid", "Barcelone", "Sévile" });
            pays.Add(new List<string> { "Italie", "Venise", "Florence", "Milan", "Pise" });

            for (int i = 0; i < pays.Count; i++)
            {
                var p = pays[i];
                Console.WriteLine($"{p[0]} - {(p.Count - 1)} villes");
                for (int j = 1; j < p.Count; j++)
                {
                    Console.WriteLine($"    {p[j]}");
                }
            }

           // AfficherListSTR(villes);
        }


/*--------------------------------DICTIONNAIRE------------------------------*/

        static void Dictionnaire()
        {
            //Dictionary
            // clef -> valeurs 
            //  |           |
            // nom  -> numero de théléphone

            var d = new Dictionary<string, string>();

            d.Add("Jean", "0622334455");
            d.Add("Marie", "0725337485");
            d["Julien"] = "0827384475";
            d["Estienne"] = "0325332459";
            d["Martin"] = "07939293840";

            foreach (var dic in d)
            {
                Console.WriteLine($"{dic.Key}");
            }

            string personneAChercher = Console.ReadLine();

            if (d.ContainsKey(personneAChercher))
            {
                Console.WriteLine(d[personneAChercher]);
            }
            else
            {
                Console.WriteLine("Cette personne n'existe pas dans le dictionnaire");
            }

            //liste nom numero de tel

            var l = new List<string[]>();
            l.Add(new string[] { "Jean", "06234277429" });
            l.Add(new string[] { "Marie", "0725337485" });
            l.Add(new string[] { "Julien", "0827384475" });
            l.Add(new string[] { "Martin", "07939293840" });

            for (int i = 0; i < l.Count; i++)
            {
                if (l[i][0] == personneAChercher)
                {
                    Console.WriteLine(l[i][1]);
                    break;
                }
            }
        }

/*--------------------------------BoucleForEach------------------------------*/

        static void BoucleForEach()
        {
            //var noms = new string[] { "Toto", "jean", "Pierre" };
            //var noms = new List<string>() { "Toto", "jean", "Pierre" };

            /*foreach (var nom in noms) 
            {
                Console.WriteLine(nom);
            }*/

            var d = new Dictionary<string, string>();

            d.Add("Jean", "0622334455");
            d.Add("Marie", "0725337485");
            d["Julien"] = "0827384475";
            d["Estienne"] = "0325332459";
            d["Martin"] = "07939293840";

            foreach (var e in d) 
            {
                Console.WriteLine($"{e.Key} -> {e.Value}");
            }

        }


/*--------------------------------Tris et linq------------------------------*/

        static void TrisEtLinq()
        {
            //Une liste
            var noms = new List<string>() { "Toto", "jean", "Pierre", "Emilie", "Sophia", "Vincent", "Benoit" };

            //Un tableau
            //var noms = new string[] { "Toto", "jean", "Pierre", "Emilie", "Sophia", "Vincent", "Benoit" };

            //noms.Sort(); // for liste

            //Array.Sort(noms); // pour tableau

            noms = noms.OrderByDescending(nom => nom).ToList();
            //noms = noms.Where(nom => nom.Length > 4).ToList();

            foreach (var nom in noms)
            {
                Console.WriteLine(nom);
            }
        }



        static void MaFunction(int p)
        {

        }

        static void PassageValeursOuRef()
        {

        }

        static void Main(string[] args)
        {
            //Tableux();
            //Listes();
            //ArrayList();
            //ListesDeListes();
            Dictionnaire();
            //BoucleForEach();
            //TrisEtLinq();
            //PassageValeursOuRef();
        }
    }
}