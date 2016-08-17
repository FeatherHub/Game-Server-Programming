using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
using System.Web.Http;
using Microsoft.Owin.Security.OAuth;
using Newtonsoft.Json.Serialization;

namespace WebApplication1
{
    public static class WebApiConfig
    {
        public static void Register(HttpConfiguration config)
        {
            // Web API configuration and services
            // Configure Web API to use only bearer token authentication.
            config.SuppressDefaultHostAuthentication();
            config.Filters.Add(new HostAuthenticationFilter(OAuthDefaults.AuthenticationType));

            // Web API routes
            config.MapHttpAttributeRoutes();

            //URL의 일부값 취득하기
            //라우팅 설정에서 Mapping protocol을 추가한다
            //Default routing 설정 코드 이전에 추가한다

            /*
             * 
             * config.Routes.MapHttpRoute(
             *  name:"AddressApi",
             *  routTemplate:"api/address/{country}/{preference}
             *  defaults:new{controller="address"}
             * );
             * 
             * /

            //라우팅 설정

            //MapHttpRoute 메서드의 인수인 routeTemplate에 지정된 문자열(URL template)에
            //해당하는 Request를 API의 컨트롤러의 메서드에 매핑한다

            //아래 라우팅 설정으로 인해 API 컨트롤러는
            //'지정 상황 + Controller' 형식에 맞게 이름을 달아야
            //지정 컨트롤러로써 인식된다 (= URL과 MAPPING된 API Controller)

            //"api/values/1"
            //위 URL로 HTTP GET 메서드를 호출한 경우
            //ValuesController Get 메서드가 호출되고 인수로 1을 전달한다
            config.Routes.MapHttpRoute(
                name: "DefaultApi",
                routeTemplate: "api/{controller}/{id}",
                defaults: new { id = RouteParameter.Optional } //Optional: 'id'는 생략가능하다
            );
        }
    }
}
