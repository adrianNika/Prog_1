using Newtonsoft.Json;
using System;
using System.Diagnostics;

namespace programme_json // Note: actual namespace depends on the project name.
{
    class Personne
    {
        public string nom { get;  private set; }
        public int age { get; private set; }
        public bool majeur { get; private set; }

        public Personne(string nom, int age, bool majeur) 
        {
            this.nom = nom;
            this.age = age; 
            this.majeur = majeur;
        }

        public void Affiche()
        {
            Console.WriteLine($"Nom: {nom} - Age: {age}ans");
        }
    }

    internal class Program
    {
        static void Main(string[] args)
        {
            /*var personne1 = new Personne("Toto", 20, true);
            personne1.Affiche();

            string json = JsonConvert.SerializeObject( personne1 );
            Console.WriteLine(json);

            var noms = new List<string>() { "Jean", "Paul", "Claire"};
            string jsonListe = JsonConvert.SerializeObject( noms );
            Console.WriteLine(jsonListe);

            string jsonTiti = "{ \"nom\":\"Titi\",\"age\":10,\"majeur\":false}";
            Personne titi = JsonConvert.DeserializeObject<Personne>(jsonTiti);
            titi.Affiche();*/

            var path = "out";

            if (!Directory.Exists(path))
            {
                Directory.CreateDirectory(path);
            }

            Directory.CreateDirectory(path);

            string filename = "personnes.txt";

            string pathAndFile = Path.Combine(path, filename);

            /*var personnes = new List<Personne>()
            {
                new Personne("Titi", 20, true),
                new Personne("Toto", 12, false),
                new Personne("Clara", 35, true),
                new Personne("Nathan", 16, false)
            };

            string json = JsonConvert.SerializeObject(personnes);
            Console.WriteLine(json);

            File.WriteAllText(pathAndFile, json);  */


            string jsonPersonne = File.ReadAllText(pathAndFile);
            Console.WriteLine(jsonPersonne);
            var personnes = JsonConvert.DeserializeObject<List<Personne>>(jsonPersonne);

            foreach (var personne in personnes)
            {
                personne.Affiche();
            }
        }
    }
}