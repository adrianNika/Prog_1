using System;
using System.Dynamic;
using System.IO;
using System.Net;
using System.Text;
using FormationCS;
using Newtonsoft.Json;

namespace projet_pizza_v1 // Note: actual namespace depends on the project name.
{

    class PizzaPersonnalisee : Pizza
    {
        static int nbPizzaPersonnalisee = 0;

        public PizzaPersonnalisee() : base("Personnalisée", 5, false, null)
        {
            nbPizzaPersonnalisee++;
            nom = nom + " " + nbPizzaPersonnalisee;

            ingredients = new List<string>();

            Console.WriteLine();
            while (true)
            {
                Console.Write($"Rentrez un ingrédient pour la pizza {nom} (ENTER pour terminer) : ");

                string ingredient = Console.ReadLine();

                if (string.IsNullOrWhiteSpace(ingredient))
                {
                    Console.Clear();
                    break;
                }
                else if (!ingredients.Contains(ingredient))
                {
                    ingredients.Add(ingredient);
                    Console.WriteLine(string.Join(", ", ingredients));
                }
                else
                {
                    Console.WriteLine("ERREUR : Cette ingredient existe déjà dans votre liste");
                }

                Console.WriteLine();
            }

            prix = prix + (ingredients.Count * 1.5f);
        }
    }


    class Pizza
    {
        public string nom { get; protected set; }
        public float prix { get; protected set; }
        public bool vegetarienne { get; init; }

        public List<string> ingredients { get; protected set; }

        public Pizza(string nom, float prix, bool vegetarienne, List<string> ingredients)
        {
            this.nom = nom;
            this.prix = prix;
            this.vegetarienne = vegetarienne;
            this.ingredients = ingredients;
        }



        public virtual void Afficher()
        {
            string badgeVegetarienne = vegetarienne ? " (V)" : "";

            string nomAfficher = FormatPremiereLettreMajuscule(nom);

            /*méthode par boucle basique*/
            /*var affichageIngredient = new List<string>();
            foreach (var ingredient in ingredients)
            {
                affichageIngredient.Add(FormatPremiereLettreMajuscule(ingredient));
            }*/

            /*méthode par linq avec la nouvelle notion "Select", permet de mettre les majuscule au début des nom d'aliment*/
            var affichageIngredient = ingredients.Select(i => FormatPremiereLettreMajuscule(i)).ToList();

            Console.WriteLine($"{nomAfficher} {badgeVegetarienne} - {prix}CHF");
            Console.WriteLine(string.Join(", ", affichageIngredient));
            Console.WriteLine();
        }

        private static string FormatPremiereLettreMajuscule(string s)
        {
            if (string.IsNullOrEmpty(s))
                return s;


            string sMinuscule = s.ToLower();
            string sMajuscule = s.ToUpper();

            string resultat = sMajuscule[0] + sMinuscule[1..];

            return resultat;
        }

        public bool ContientIngredient(string ingredient)
        {
            return ingredients.Where(i => i.ToLower().Contains("tomate")).ToList().Count > 0;
        }
    }

    internal class Program
    {
        static List<Pizza> GetPizzasFromCode()
        {
            /*-------génération des donnée---------*/
            var pizzas = new List<Pizza>()
            {
                new Pizza("4 Fromage", 11.5f, true, new List<string> { "Cantal", "mozzarella", "fromage de chèvre", "gruyère"}),
                new Pizza("indienne", 10.5f, false, new List < string > { "curry", "mozzarella", "poulet", "poivron", "oignon", "coriandre"}),
                new Pizza("mexicaine", 13f, false, new List < string > { "boeuf", "mozzarella", "maïs", "tomates", "oignon", "coriandre"}),
                new Pizza("margherita", 8f, true, new List<string> { "sauce tomate", "mozzarella", "basilic"}),
                new Pizza("calzone", 12f, false, new List < string > { "tomate", "jambon", "persil", "oignons"}),
                new Pizza("complete", 9.5f, false, new List < string > { "jambon", "oeuf", "fromage"})
            };

            return pizzas;
        }




        static List<Pizza> GetPizzasFromFile(string filename)
        {
            List<Pizza> pizzas = null;
            string json = null;

            try
            {
                json = File.ReadAllText(filename);

            }
            catch
            {
                Console.WriteLine($"ERREUR de lecture du fichier : {filename}");
                return null;
            }

            try
            {
                pizzas = JsonConvert.DeserializeObject<List<Pizza>>(json);
            }
            catch
            {
                Console.WriteLine("ERREUR : Les données json ne sont pas valide");
                return null;
            }

            return pizzas;
        }


        static List<Pizza> GetPizzasFromUrl(string url)
        {
            var webClient = new WebClient();

            List<Pizza> pizzas = null;
            string json = null;

            try
            {
                json = webClient.DownloadString(url);

            }
            catch
            {
                Console.WriteLine("ERREUR ");
            }


            try
            {
                pizzas = JsonConvert.DeserializeObject<List<Pizza>>(json);
            }
            catch
            {
                Console.WriteLine("ERREUR : Les données json ne sont pas valide");
                return null;
            }

            return pizzas;
        }




        static void GenerateJsonFile(List<Pizza> pizzas, string filename)
        {
            var json = JsonConvert.SerializeObject(pizzas);
            File.WriteAllText(filename, json);
        }




        static void Main(string[] args)
        {
            //string url = "https://codeavecjonathan.com/res/pizzas2.json";

            string filename = "pizzas.json";


            //var pizzas = GetPizzasFromFile(filename);
            var pizzas = GetPizzasFromFile(filename);

            /*---------------Exercice perso : demander le nombre de pizza perso que l'utilisateur veux-------------------*/
            var nombreDePersonnalise = outils.DemanderNombrePositif("Combien de pizza pérsonalisée voulez vous faire : ");
            for (int i = 0; i < nombreDePersonnalise; i++)
            {
                pizzas.Add(new PizzaPersonnalisee());
            }

            Console.WriteLine();
            Console.Clear();

            if (pizzas != null)
            {
                foreach (var pizza in pizzas)
                {
                    pizza.Afficher();
                }
            }
        }
    }
}