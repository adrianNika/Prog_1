// c# 10 - global using
// See https://aka.ms/new-console-template for more information

using cs10_global_using;
using Newtonsoft.Json;

var p = new Personne { nom = "Toto", age = 20 };
var json_p = JsonConvert.SerializeObject(p);
Console.WriteLine(json_p);
Console.WriteLine(p.GetJson());
