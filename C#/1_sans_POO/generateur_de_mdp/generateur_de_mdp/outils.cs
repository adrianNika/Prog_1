using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


// FormationCD.outils.nomFonction

namespace FormationCS
{
    static class outils
    {
        public static int DemanderNombrePositifNonNull(string question)
        {
            return DemanderNombreEntre(question, 1, int.MaxValue, "ERREUR : le nombre doit etres positif et non nul");
        }

        public static int DemanderNombreEntre(string question, int min, int max, string messageErreurPerso = null)
        {
            int nombreEntre = 0;

            nombreEntre = DemanderNombre(question);

            if (nombreEntre >= min && nombreEntre <= max)
            {
                return nombreEntre;
            }

            if (messageErreurPerso == null) 
            {
                Console.WriteLine($"ERREUR : Valeurs invalides, veuillez sélectionner un nombre entre {min} et {max}");
            }
            else
            {
                Console.WriteLine(messageErreurPerso);
            }
            
            Console.WriteLine();

            return DemanderNombreEntre(question, min, max, messageErreurPerso);
        }

        public static int DemanderNombre(string question)
        {
            string valueStr = "";
            int valueInt = 0;

            while (true)
            {
                Console.Write(question);
                valueStr = Console.ReadLine();

                try
                {
                    valueInt = int.Parse(valueStr);
                    return valueInt;
                }
                catch
                {
                    Console.WriteLine("ERREUR : valeurs invalide, veuillez rentrer un nombre");
                    Console.WriteLine();
                }
            }
        }
    }
}
