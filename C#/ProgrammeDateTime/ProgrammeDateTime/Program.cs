using System;
using System.Globalization;

namespace ProgrammeDateTime // Note: actual namespace depends on the project name.
{
    internal class Program
    {
        static void Main(string[] args)
        {
            DateTime date = DateTime.Now;

            //Console.WriteLine(date);

            CultureInfo culture = CultureInfo.GetCultureInfo("fr-FR");

            Console.WriteLine(date.ToString("ddddd dd MMMM yyyy HH:mm:ss", culture));

            DateTime dateDemain = date.AddDays(1);

            Console.WriteLine($"Demain : {dateDemain.ToString("ddddd dd MMMM yyyy HH:mm:ss")} ", culture);

            var diff = dateDemain - date;

            Console.WriteLine($"Différence jour : {diff.TotalDays}");

        }
    }
}