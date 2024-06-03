using System;
using System.Text;

namespace programme_fichiers // Note: actual namespace depends on the project name.
{
    internal class Program
    {
        static void Main(string[] args)
        {
            //var path = Environment.GetFolderPath(Environment.SpecialFolder.Desktop);
            var path = "out";

            if (!Directory.Exists(path))
            {
                Directory.CreateDirectory(path);
            }

            Directory.CreateDirectory(path);

            string filename = "monFichier.txt";
            string filename2 = "monFichier2.txt";

            string pathAndFile = Path.Combine(path, filename);
            string pathAndFile2 = Path.Combine(path, filename2);

            if (File.Exists(pathAndFile))
            {
                Console.WriteLine("Le fichier existes déjà, on vas ecraser son contenu");
            }
            else
            {
                Console.WriteLine("Le fichier n'existe pas, on vas le crée");
            }

            Console.WriteLine($"Fichier : {pathAndFile}");

            //File.AppendAllText(filename, "Salut");

            /*var noms = new List<string>()
            {
                "jean",
                "paul",
                "martin"
            };*/

            StringBuilder texte = new StringBuilder();
            int nbLinges = 5000;

            //---------------------------------------------------------
            /*
            Console.Write("Préparation des données... ");
            for (int i = 1; i <= nbLinges; i++)
            {
                texte.Append("Ligne " + i + "\n");
            }
            Console.WriteLine("OK");


            DateTime t1 = DateTime.Now;
            Console.Write("Ecriture des données... ");
            File.WriteAllText(pathAndFile, texte.ToString());
            Console.WriteLine("OK");

            DateTime t2 = DateTime.Now;
            var diff = (int)((t2 - t1).TotalMilliseconds);
            Console.WriteLine($"Durée (ms) : {diff}");*/
            //----------------------------------------------------------


            /*écriture avec un stream*/
            DateTime t1 = DateTime.Now;
            Console.Write("Ecriture des données... ");

            using (var writeStream = File.CreateText(pathAndFile))
            {
                for (int i = 1; i <= nbLinges; i++)
                {
                    writeStream.Write("Ligne " + i + "\n");
                }
            }
            Console.WriteLine("OK");

            DateTime t2 = DateTime.Now;
            var diff = (int)((t2 - t1).TotalMilliseconds);
            Console.WriteLine($"Durée (ms) : {diff}");


            /*Lecture avec un stream*/
            using (var readStream = File.OpenText(pathAndFile))
            {
                while(true)
                {
                    var line = readStream.ReadLine();
                    if (line == null) break;
                    Console.WriteLine(line);
                }
            }


            //File.WriteAllLines(pathAndFile, noms);

            /*try
            {
                var lignes = File.ReadAllLines(pathAndFile);

                foreach ( string line in lignes )
                {
                    Console.WriteLine(line);
                }
            }
            catch (FileNotFoundException ex)
            {
                Console.WriteLine($"Erreur : le fichier n'existe pas ({ex.Message})");
            }
            catch 
            {
                Console.WriteLine("ERREUR : une erreur inconu a surgi");
            }

           // File.Copy(pathAndFile, pathAndFile2);
           File.Delete(pathAndFile2);

           //File.Move(pathAndFile2, pathAndFile);*/
        }
    }
}