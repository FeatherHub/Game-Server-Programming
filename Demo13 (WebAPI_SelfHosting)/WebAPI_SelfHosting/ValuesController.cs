using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Web.Http;


namespace WebAPI_SelfHosting
{
    public class ValuesController : ApiController
    {
        //Get api/values
        public IEnumerable<string> Get()
        {
            return new string[] { "value1", "value2" };
        }

        //Get api/values/5
        public string Get(int id)
        {
            return "value";
        }

        //POST api/values
        public void Post([FromBody]string value)
        {

        }

        //PUT api/values/5
        public void Put(int id, [FromBody]string value)
        {

        }

        //DELETE api/vlaues/5
        public void Delete(int id)
        {

        }


    }
}
