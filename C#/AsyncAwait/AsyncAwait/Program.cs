using System;

namespace AsyncAwait // Note: actual namespace depends on the project name.
{
    internal class Program
    {
        static async Task Main(string[] args)
        {
            string url1 = "https://codeavecjonathan.com/res/dummy.txt";
            string url2 = "https://codeavecjonathan.com/res/pizzas1.json";




            Console.Write("Tléchargement...");
            var displayTask = DisplayProgress();
            var downloadTask1 = DownloadData(url1);
            var downloadTask2 = DownloadData(url2);

            //await downloadTask1;
            //await downloadTask2;
            await Task.WhenAll(downloadTask1, downloadTask2);

            Console.WriteLine("FIN DU PROGRAMME");
        }


        static async Task DownloadData(string url)
        {
            var httpCLient = new HttpClient();

            var resultat = await httpCLient.GetStringAsync(url);

            Console.WriteLine($"OK -> {url}");
            //Console.WriteLine(resultat);
        }

        static async Task DisplayProgress()
        {
            while (true)
            {
                await Task.Delay(50);
                Console.Write(".");
            }
        }
    }
}