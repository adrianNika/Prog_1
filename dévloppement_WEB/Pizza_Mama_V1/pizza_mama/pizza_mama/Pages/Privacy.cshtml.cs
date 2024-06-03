using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.RazorPages;
using Microsoft.Extensions.Logging;
using pizza_mama.Data;
using pizza_mama.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace pizza_mama.Pages
{
    public class PrivacyModel : PageModel
    {
        private readonly ILogger<PrivacyModel> _logger;
        DataContext dataContext1;

        public PrivacyModel(ILogger<PrivacyModel> logger, DataContext dataContext1)
        {
            _logger = logger;
            this.dataContext1 = dataContext1;
        }

        public void OnGet()
        {
            var pizza = new Pizza() { nom = "PizzaTest", prix = 5 };
            dataContext1.Pizzas.Add(pizza);
            dataContext1.SaveChanges();
        }
    }
}
