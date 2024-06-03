using System;

namespace nombre_magique // Note: actual namespace depends on the project name.
{
    internal class Program
    {

/*-------------------------FONCTION-------------------------*/

        static int DemanderNombre(int nombreMin, int nombreMax)
        {
            int nombreChoix = nombreMin - 1;

            while (nombreChoix < nombreMin || nombreChoix > nombreMax)
            {
                Console.Write($"Choisissez un nombre entre {nombreMin} et {nombreMax} : ");
                string nombre = Console.ReadLine();

                try
                {
                    nombreChoix = int.Parse(nombre);

                    if (nombreChoix < nombreMin || nombreChoix > nombreMax)
                    {
                        Console.WriteLine($"ERREUR : Le nombre doit être en {nombreMin} et {nombreMax} !");
                    }
                }
                catch
                {
                    Console.WriteLine("ERREUR : ce nombre n'est pas valide");
                }
            }

            return nombreChoix;
        }



/*--------------------------MAIN-------------------------*/
        static void Main(string[] args)
        {
            Random rand = new Random();

            const int NOMBRE_MIN = 1;
            const int NOMBRE_MAX = 10;
            const int NOMBRE_VIE = 5;
            int NOMBRE_MAGIQUE = rand.Next(NOMBRE_MIN, NOMBRE_MAX + 1);

            int nombreMagique = 0;
            int nbVie = NOMBRE_VIE;

            for (; nbVie > 0; nbVie--)
            {
                Console.WriteLine();
                Console.WriteLine($"Vie restante : {nbVie}");
                nombreMagique = DemanderNombre(NOMBRE_MIN, NOMBRE_MAX);

                if (NOMBRE_MAGIQUE > nombreMagique)
                {
                    Console.WriteLine("le nombre magique est plus grand !");
                }
                else if (NOMBRE_MAGIQUE < nombreMagique)
                {
                    Console.WriteLine("Le nombre magique est plus petit !");
                }
                else
                {
                    Console.WriteLine();
                    Console.WriteLine("BRAVO : vous avez trouvez le nombre magique");
                    break;
                }
            }

            if (nbVie == 0)
            {
                Console.WriteLine();
                Console.Write("PERDU : vous n'avez plus de vie, ");
                Console.WriteLine($"Le nombre magique était : {NOMBRE_MAGIQUE}");
            }
        }
    }
}