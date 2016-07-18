using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net.Sockets;
using System.IO;
using System.Net;

/*
-Initialization
  Make socket
  Register endpoint 
  Connect
-Network Functions
  Send
  Receive 
*/

//네트워킹의 가장 하부에 있는 모듈이다
//byte 단위의 수신/전송을 담당한다

//패킷
//로직은 이 모듈을 사용하여 byte stream를 보내고 얻는다
//로직은 byte를 양자 간에 약속한 절단/채취 방법에 따라 가공한다

namespace ChatClient1
{
    class ClientSimpleTcp
    {
        public Socket Sock = null;
        public string LatestErrorMsg;
        byte[] ReadBuffer = new byte[4096];

        //소켓연결        
        public bool Connect(string ip, int port)
        {
            try
            {
                //host byte order -> net byte order
                IPAddress serverIP = IPAddress.Parse(ip); 
                int serverPort = port;

                Sock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

                //EndPoint = IP + Port
                Sock.Connect(new IPEndPoint(serverIP, serverPort));

                if (Sock == null || Sock.Connected == false)
                {
                    return false;
                }

                return true;
            }
            catch (Exception ex)
            {
                LatestErrorMsg = ex.Message;
                return false;
            }
        }

        //왜 반환하지? 
        //반환되는 것은 새로운 변수 
        public ArraySegment<byte> Receive()
        {
            try
            {
                var nRecv = Sock.Receive(ReadBuffer, 0, ReadBuffer.Length, SocketFlags.None);

                if (nRecv == 0)
                {
                    return new ArraySegment<byte>();
                }

                return new ArraySegment<byte>(ReadBuffer, 0, nRecv);
            }
            catch (SocketException se)
            {
                LatestErrorMsg = se.Message;
            }

            return new ArraySegment<byte>();
        }

        //스트림에 쓰기
        public void Send(byte[] sendData)
        {
            try
            {
                //클라인데 소켓이 통신 중에 제거되는 일도 있나?
                //(closesocket이 통신 중에 호출되는 일)
                if (Sock != null && Sock.Connected) //연결상태 유무 확인
                {
                    Sock.Send(sendData, 0, sendData.Length, SocketFlags.None);
                }
                else
                {
                    LatestErrorMsg = "먼저 채팅서버에 접속하세요!";
                }
            }
            catch (SocketException se)
            {
                LatestErrorMsg = se.Message;
            }
        }

        //소켓과 스트림 닫기
        public void Close()
        {
            if (Sock != null && Sock.Connected)
            {
                Sock.Shutdown(SocketShutdown.Both);
                Sock.Close();
            }
        }

        public bool IsConnected()
        {
            if (Sock != null && Sock.Connected == true)
            {
                return true;
            }
            else
            {
                return false;
            }

            //return (Sock != null && Sock.Connected) ? true : false;
        }
    }
}
