using System;
using System.Dynamic;
using System.Text;
using FormationCS;

namespace projet_pizza_v1 // Note: actual namespace depends on the project name.
{

    class PizzaPersonnalisee : Pizza
    {
        static int nbPizzaPersonnalisee = 0;

        public PizzaPersonnalisee() : base("Personnalisée", 5, false,  null) 
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
        protected string nom;
        public float prix { get; protected set;}
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
            if(string.IsNullOrEmpty(s))
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
        static void Main(string[] args)
        {
            Console.OutputEncoding = Encoding.UTF8;

            var pizzas = new List<Pizza>()
            {
                new Pizza("4 Fromage", 11.5f, true, new List<string> { "Cantal", "mozzarella", "fromage de chèvre", "gruyère"}),
                new Pizza("indienne", 10.5f, false, new List < string > { "curry", "mozzarella", "poulet", "poivron", "oignon", "coriandre"}),
                new Pizza("mexicaine", 13f, false, new List < string > { "boeuf", "mozzarella", "maïs", "tomates", "oignon", "coriandre"}),
                new Pizza("margherita", 8f, true, new List<string> { "sauce tomate", "mozzarella", "basilic"}),
                new Pizza("calzone", 12f, false, new List < string > { "tomate", "jambon", "persil", "oignons"}),
                new Pizza("complete", 9.5f, false, new List < string > { "jambon", "oeuf", "fromage"})
            };

            /*plus grand -> plus petit*/
            //pizzas = pizzas.OrderBy(p => p.prix).ToList();

            /*plus petit -> plus grand*/
            //pizzas = pizzas.OrderByDescending(p => p.prix).ToList();


            /*Pizza pizzaPrixMin = null;
            Pizza pizzaPrixMax = null;

            pizzaPrixMin = pizzas[0];
            pizzaPrixMax = pizzas[0];

            foreach (var pizza in pizzas)
            {
                if (pizza.prix < pizzaPrixMin.prix)
                {
                    pizzaPrixMin = pizza;
                }

                if (pizza.prix > pizzaPrixMax.prix)
                {
                    pizzaPrixMax = pizza;
                }
            }

            Console.WriteLine($"La pizza la moin cher est : ");
            pizzaPrixMin.Afficher();

            Console.WriteLine($"La pizza la plus cher est : ");
            pizzaPrixMax.Afficher();*/

            /*afficher que les pizza vegetarienne (mettre ! devant x.vegetarienne pour affcicher les non vegetarienne)*/
            //pizzas = pizzas.Where(x => x.vegetarienne).ToList();
            //pizzas = pizzas.Where(x => !x.vegetarienne).ToList();

            /*var choixPizza = outils.DemanderNombreEntre("Choisire le type de pizza voulez vous voir : 1 -> normal / 2 -> vegetarienne / 3 -> Les deux :  ", 1, 3);
            Console.WriteLine();*/

            /*if (choixPizza == 1)
            {
                pizzas = pizzas.Where(x => !x.vegetarienne).ToList();
            }
            else if (choixPizza == 2)
            {
                pizzas = pizzas.Where(x => x.vegetarienne).ToList();
            }
            else
            {
                
            }*/


            /*permet de séléctionner que les pizza qui contien tomate dans LA CHAINE DE CARACTERE de la liste*/
            //pizzas = pizzas.Where(p => p.ContientIngredient("tomate")).ToList();

            /*Exercice perso : demander le nombre de pizza perso que l'utilisateur veux*/
            var nombreDePersonnalise = outils.DemanderNombrePositif("Combien de pizza pérsonalisée voulez vous faire : ");
            for (int i = 0; i < nombreDePersonnalise; i++)
            {
                pizzas.Add(new PizzaPersonnalisee());
            }
            Console.WriteLine();
            Console.Clear();

            foreach (var pizza in pizzas)
            {
                pizza.Afficher();
            }
        }
    }
}