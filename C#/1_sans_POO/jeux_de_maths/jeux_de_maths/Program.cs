using System;
using System.Threading.Channels;

namespace jeux_de_maths // Note: actual namespace depends on the project name.
{
    internal class Program
    {
        static int reponse = 0;

        enum e_Operateur
        {
            ADDITION = 1,
            SOUSTRACTION = 2,
            MULTIPLICATION = 3,
            DIVISION = 4
        }

        static bool PoserQuestion(int min, int max)
        {
            Random rand = new Random();

            string reponseUser = "";
            int valueInt = 0;


            while (true)
            {
                int a = rand.Next(min, max + 1);
                int b = rand.Next(min, max + 1);
                e_Operateur o = (e_Operateur)rand.Next(1, 5);

                switch (o)
                {
                    case e_Operateur.ADDITION:
                        reponse = a + b;
                        Console.Write($"{a} + {b} = ");
                        break;

                    case e_Operateur.SOUSTRACTION:
                        reponse = a - b;
                        Console.Write($"{a} - {b} = ");
                        break;

                    case e_Operateur.MULTIPLICATION:
                        reponse = a * b;
                        Console.Write($"{a} X {b} = ");
                        break;

                    case e_Operateur.DIVISION:
                        reponse = a / b;
                        Console.Write($"{a} / {b} = ");
                        break;

                    default:
                        Console.WriteLine("ERREUR : Operateur inconu");
                        return false;
                        break;
                };

                
                reponseUser = Console.ReadLine();

                try
                {
                    valueInt = int.Parse(reponseUser);

                    if (valueInt == reponse)
                    {
                        return true;
                    }

                    return false;
                }
                catch
                {
                    Console.WriteLine("ERREUR : vous devez rentrer un nombre valide");
                }
            }
        }


        /*--------------------------------MAIN---------------------------------*/
        static void Main(string[] args)
        {
            const int NOMBRE_MIN = 1;
            const int NOMBRE_MAX = 10;
            const int nbQuestion = 10;

            int points = 0;

            bool bonneReponse = false;

            for (int i = 1; i <= nbQuestion; i++)
            {
                Console.WriteLine($"Question N°{i}/{nbQuestion}");
                bonneReponse = PoserQuestion(NOMBRE_MIN, NOMBRE_MAX);

                if (bonneReponse)
                {
                    Console.WriteLine("BRAVO : c'était la bonne réponse");
                    points++;
                }
                else
                {
                    Console.WriteLine($"PERDU : ce n'était pas la bonne réponse, la bonne réponse était {reponse}");
                }

                Console.WriteLine();
            }

            Console.WriteLine($"nombre de points : {points}/{nbQuestion}");

            int moyenne = nbQuestion / 2;

            if (points == nbQuestion)
            {
                Console.WriteLine("Excellent !!!");
            }
            else if (points == 0)
            {
                Console.WriteLine("Révisez vos maths !!!");
            }
            else if (points > moyenne)
            {
                Console.WriteLine("Pas mal");
            }
            else
            {
                Console.WriteLine("Peux mieux faire");
            }
        }
    }
}
