using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Microsoft.Owin.Hosting;
using System.Net.Http;




namespace WebAPI_SelfHosting
{
    class Program
    {
        static void Main(string[] args)
        {
            string baseAddress = "http://localhost:19000/";
            //
            using (WebApp.Start<Startup>(url: baseAddress))
            {
                var client = new HttpClient();
                //
                var response = client.GetAsync(baseAddress + "api/values").Result;
                //
                Console.WriteLine(response);
                Console.WriteLine(response.Content.ReadAsStringAsync().Result);

                Console.ReadLine();
            }
        }
    }
}
