using System;
using System.Net;
using System.Runtime.CompilerServices;

namespace programme_reseau // Note: actual namespace depends on the project name.
{
    internal class Program
    {
        static void Main(string[] args)
        {
            string url = "https://codeavecjonathan.com/res/papillon.jpg";

            var webClient = new WebClient();


            Console.WriteLine("Accès au réseau...");
            try 
            {
                webClient.DownloadFile(url, "papillon.jpg");
                
                Console.WriteLine("Téléchargement terminé");
            }
            catch (WebException ex)
            {
                if(ex.Response != null)
                {
                    var statecode = ((HttpWebResponse)ex.Response).StatusCode;
                    if (statecode == HttpStatusCode.NotFound)
                    {
                        Console.WriteLine("ERREUR RESEAU : Non trouvé");
                    }
                    else
                    {
                        Console.WriteLine($"ERREUR RESEAU : {statecode}");
                    }
                }
                else
                {
                    Console.WriteLine($"ERREUR RESEAU : {ex.Message}");
                }
                
            }
        }
    }
}