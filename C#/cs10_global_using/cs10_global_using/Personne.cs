
using Newtonsoft.Json;

namespace cs10_global_using
{
    class Personne
    {
        public string nom { get; set; }
        public int age { get; set; }

        public string GetJson()
        {
            return JsonConvert.SerializeObject(this);
        }
    }
}
