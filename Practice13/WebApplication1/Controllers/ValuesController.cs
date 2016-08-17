using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Web.Http;

namespace WebApplication1.Controllers
{
    [Authorize]

    //엔드 포인트를 정의하려면 API Controller와 메서드를 정의한다

    //ASP.NET Web API의 API Controller!
    public class ValuesController : ApiController   //ApiController를 계승하지 않는다면 
    {                                               //IHttpController 인터페이스를 탑재
    
    //URL : http://localhost:XXXX/api/values/5

    /* 메서드 구성 = HTTP 요청 메서드(Get, Post 등)
     * [Get]
     * GetXXX
     * GETYYY
     * 
     * [Post]
     * PostXXX
     * PostYYY
     * 
     * - HTTP 요청 메서드(Get, Set)와 API 컨트롤러의 메서드 이름이 대응하면 맵핑된다
     */   
        
        // GET api/values
        public IEnumerable<string> Get() //id가 정의되지 않은 경우를 위한 메서드
        {
            return new string[] { "value1", "value2" };
        }

        // GET api/values/5
        public string Get(int id)
        {
            return "value";
        }

        // POST api/values
        public void Post([FromBody]string value) //FromBody : 인수의 값이 HTTP 요청의 Body 값
        {
        }

        // PUT api/values/5
        public void Put(int id, [FromBody]string value)
        {
        }

        // DELETE api/values/5
        public void Delete(int id)
        {
        }
    }
}
