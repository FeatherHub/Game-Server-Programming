using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChatClient1
{
    public enum PACKET_ID : ushort
    {
        INVALID = 0,

        SYSTEM_CLIENT_CONNECT = 11,
        SYSTEM_CLIENT_DISCONNECTD = 12,

        LOGIN_IN_REQ = 21,
        LOGIN_IN_RES = 22,
       
        
        //REQUEST_LOGIN = 1101,
        //RESPONSE_LOGIN = 1102,

        //REQUEST_ENTER_LOBBY = 1111,
        //RESPONSE_ENTER_LOBBY = 1112,

        //REQUEST_LEAVE_LOBBY = 1121,
        //RESPONSE_LEAVE_LOBBY = 1122,

        //REQUEST_CHAT = 1131,
        //NOTICE_CHAT = 1132,



        // SERVER SIDE
        //DB_REQUEST_LOGIN = 2001,
        //DB_RESPONSE_LOGIN = 2002,

        //INNER_NOTIFY_ENTER_LOBBY = 2012,
        //INNER_ROLL_BACK_ENTER_LOBBY = 2014,
        //INNER_NOTIFY_LEAVE_LOBBY = 2016,
    }

    public enum ERROR_CODE : ushort
    {
        NONE = 0,

        // 로그인 
        USER_MGR_ID_DUPLICATION = 211,
        USER_MGR_MAX_USER_COUNT = 212,
        USER_MGR_INVALID_SESSION_INDEX = 213,
        USER_MGR_NOT_CONFIRM_USER = 214,
        USER_MGR_REMOVE_INVALID_SESSION = 221,

        

        ENTER_LOBBY_ALREADY_LOBBY = 10011,
        ENTER_LOBBY_INVALID_LOBBY_ID = 10012,
        ENTER_LOBBY_LOBBY_FULL = 10013,
        ENTER_LOBBY_ADD_USER_DUPLICATION_SESSION = 10014,
        ENTER_LOBBY_ADD_USER_DUPLICATION_USERID = 10015,
        LEAVE_LOBBY_DO_NOT_ENTER_LOBBY = 10021,
        LEAVE_LOBBY_NO_LOBBY = 10022,
        LOBBY_CHAT_DO_NOT_ENTER_LOBBY = 10031,

    }


#pragma warning disable 649
    struct PacketData
    {
        public Int16 PacketID;
        public Int16 DataSize;
        public byte[] BodyData;
    }

    public struct JsonPacketRequestLogin
    {
        public string ID;
        public string PW;
    }

    public struct JsonPacketResponseLogin
    {
        public ERROR_CODE Result;
    }


    public struct JsonPacketRequestEnterLobby
    {
        public string UserID;
        public short LobbyID;
    }

    public struct JsonPacketResponseEnterLobby
    {
        public ERROR_CODE Result;
        public short LobbyID;
    }

    public struct JsonPacketInnerNotifyEnterLobby
    {
        public ERROR_CODE Result;
        public string UserID;
        public short LobbyID;
    }

    public struct JsonPacketInnerRollBackEnterLobby
    {
        public string UserID;
        public short LobbyID;
    }


    public struct JsonPacketRequestLeaveLobby
    {
        public string UserID;
        public short LobbyID;
    }

    public struct JsonPacketResponseLeaveLobby
    {
        public ERROR_CODE Result;
    }

    public struct JsonPacketInnerNotifyLeaveLobby
    {
        public ERROR_CODE Result;
        public string UserID;
        public short LobbyID;
    }


    public struct JsonPacketRequestChat
    {
        public string UserID;
        public short LobbyID;
        public string Chat;
    }

    public struct JsonPacketNoticeChat
    {
        public string UserID;
        public string Chat;
    }
#pragma warning restore 649
}
