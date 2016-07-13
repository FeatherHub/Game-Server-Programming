using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

/*
 * Data encoding & decoding 
 * 
 * Methods
 *  - Component Event Callbacks
 *  - 
 * 
 * 
 * Members
 * 
 * Network
 *  -Tcp Network
 *  -Packet Buffer Manager
 *  -Recv Packet Queue
 *  -Send Packet Queue
 * 
 * Thread
 *  -Is Network thread running
 *  -Network read thread
 *  -Network write thread
 * 
 * Dispatcher
 *  -Dispatcher packet process
 * 
 * Status
 *  -Client status
 */

namespace ChatClient1
{
    public partial class MainForm : Form
    {
        ClientSimpleTcp Network = new ClientSimpleTcp();
        PacketBufferManager PacketBuffer = new PacketBufferManager();

        Queue<PacketData> RecvPacketQueue = new Queue<PacketData>();
        Queue<byte[]> SendPacketQueue = new Queue<byte[]>();

        bool IsNetworkThreadRunning = false;
        System.Threading.Thread NetworkReadThread = null;
        System.Threading.Thread NetworkSendThread = null;
        
        System.Windows.Threading.DispatcherTimer dispatcherPacketProcess;

        CLIENT_STATUS ClientStatus = new CLIENT_STATUS();
        


        public MainForm()
        {
            InitializeComponent();
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            PacketBuffer.Init((8096 * 10), 4, 512);

            NetworkReadThread = new System.Threading.Thread(this.NetworkReadProcess);
            NetworkReadThread.Start();
            NetworkSendThread = new System.Threading.Thread(this.NetworkSendProcess);
            NetworkSendThread.Start();
            IsNetworkThreadRunning = true;

            dispatcherPacketProcess = new System.Windows.Threading.DispatcherTimer();
            dispatcherPacketProcess.Tick += new EventHandler(ReadPacketQueueProcess);
            dispatcherPacketProcess.Interval = new TimeSpan(0, 0, 0, 0, 100);
            dispatcherPacketProcess.Start();

            btnDisconnect.Enabled = false;

            PacketHandler.Init(this);

            DevLog.Write("프로그램 시작 !!!", LOG_LEVEL.INFO);
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            Network.Close();

            IsNetworkThreadRunning = false;
            NetworkReadThread.Join();
            NetworkSendThread.Join();
        }

        // 접속하기
        private void btnConnect_Click(object sender, EventArgs e)
        {
            string address = textBoxIP.Text;

            if (checkBoxLocalHostIP.Checked)
            {
                address = "127.0.0.1";
            }

            int port = Convert.ToInt32(textBoxPort.Text);

            if (Network.Connect(address, port))
            {
                labelStatus.Text = string.Format("{0}. 서버에 접속 중", DateTime.Now);
                btnConnect.Enabled = false;
                btnDisconnect.Enabled = true;
            }
            else
            {
                labelStatus.Text = string.Format("{0}. 서버에 접속 실패", DateTime.Now);
            }
        }

        // 접속 종료
        private void button3_Click(object sender, EventArgs e)
        {
            SetDisconnectd();
            Network.Close();
        }

        // 로그인 하기
        private void button1_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(textBoxUserID.Text))
            {
                MessageBox.Show("사용할 ID를 입력해 주세요");
                return;
            }

            SendPacketLogig((Int16)PACKET_ID.LOGIN_IN_REQ, textBoxUserID.Text, textBoxPW.Text);
        }

        // 로비 입장/나가기
        private void btnLobbyEnterLeave_Click(object sender, EventArgs e)
        {
            if (ClientStatus == CLIENT_STATUS.LOBBY)
            {
                //var request = new JsonPacketRequestLeaveLobby()
                //{
                //    LobbyID = textBoxLobbyID.Text.ToInt16(),
                //    UserID = textBoxUserID.Text
                //};
                
                //var bodyData = JsonEnDecode.Encode<JsonPacketRequestLeaveLobby>(request);
                //PostSendPacket((UInt16)PACKET_ID.REQUEST_LEAVE_LOBBY, bodyData);
                return;
            }


            if (ClientStatus == CLIENT_STATUS.LOGIN)
            {
                //var request = new JsonPacketRequestEnterLobby() 
                //{
                //    LobbyID = textBoxLobbyID.Text.ToInt16(),
                //    UserID = textBoxUserID.Text
                //};
                
                //var bodyData = JsonEnDecode.Encode<JsonPacketRequestEnterLobby>(request);
                //PostSendPacket((UInt16)PACKET_ID.REQUEST_ENTER_LOBBY, bodyData);
            }
            else
            {
                MessageBox.Show("로그인 상태가 아니거나 이미 로비에 입장한 상태입니다");
            }
        }

        // 채팅 보내기
        private void button5_Click(object sender, EventArgs e)
        {
            //if (ClientStatus != CLIENT_STATUS.LOBBY)
            //{
            //    MessageBox.Show("채팅은 로비에 입장해야만 가능합니다");
            //    return;
            //}

            //var request = new JsonPacketRequestChat() 
            //{
            //    LobbyID = textBoxLobbyID.Text.ToInt16(),
            //    UserID = textBoxUserID.Text,
            //    Chat = textBoxSendChat.Text
            //};
            //var bodyData = JsonEnDecode.Encode<JsonPacketRequestChat>(request);
            //PostSendPacket((UInt16)PACKET_ID.REQUEST_CHAT, bodyData);
        }


        public void SetClientStatus(CLIENT_STATUS status)
        {
            ClientStatus = status;

            if (status == CLIENT_STATUS.LOGIN)
            {
                btnLobbyEnterLeave.Text = "로비 입장";
            }
            else if (status == CLIENT_STATUS.LOBBY)
            {
                btnLobbyEnterLeave.Text = "로비 나가기";
            }
        }

        public void SetDisconnectd()
        {
            if (btnConnect.Enabled == false)
            {
                btnConnect.Enabled = true;
                btnDisconnect.Enabled = false;
                btnLobbyEnterLeave.Text = "로비 입장";
            }

            ClientStatus = CLIENT_STATUS.NONE;

            RecvPacketQueue.Clear();
            SendPacketQueue.Clear();

            labelStatus.Text = "서버 접속이 끊어짐";
        }

        public void ChatToUI(string userID, string chatMsg)
        {
            listBoxChat.Items.Add(string.Format("[{0}]: {1}", userID, chatMsg));
        }
        
        /*
        void PostSendPacket(UInt16 packetID, byte[] bodyData)
        {
            if (Network.IsConnected() == false)
            {
                MessageBox.Show("서버에 접속하지 않았습니다");
                return;
            }

       
            List<byte> dataSource = new List<byte>();
            dataSource.AddRange(BitConverter.GetBytes(packetID));
            dataSource.AddRange(BitConverter.GetBytes((UInt32)0));
            dataSource.AddRange(BitConverter.GetBytes((UInt32)0));
            dataSource.AddRange(BitConverter.GetBytes((UInt16)bodyData.Length));
            dataSource.AddRange(bodyData);

            SendPacketQueue.Enqueue(dataSource.ToArray());
        }
        */

        void SendPacketLogig(short packetID, string userID, string userPW)
        {
            if (Network.IsConnected() == false)
            {
                MessageBox.Show("서버에 접속하지 않았습니다");
                return;
            }
            
            var asciiUserID = System.Text.Encoding.Default.GetBytes(userID); 
            //유니코드 문자열 = System.Text.Encoding.Unicode.GetBytes
            var dataUserID = new byte[16+1];
            Buffer.BlockCopy(asciiUserID, 0, dataUserID, 0, asciiUserID.Length);

            var asciiUserPW = System.Text.Encoding.Default.GetBytes(userPW);
            var dataUserPW = new byte[16 + 1];
            Buffer.BlockCopy(asciiUserPW, 0, dataUserPW, 0, asciiUserPW.Length);

            List<byte> dataSource = new List<byte>();
            dataSource.AddRange(BitConverter.GetBytes(packetID));
            dataSource.AddRange(BitConverter.GetBytes((Int16)(dataUserID.Length + dataUserPW.Length)));
            dataSource.AddRange(dataUserID);
            dataSource.AddRange(dataUserPW);

            SendPacketQueue.Enqueue(dataSource.ToArray());
        }


        void NetworkReadProcess()
        {
            while (IsNetworkThreadRunning)
            {
                System.Threading.Thread.Sleep(32);

                if (Network.IsConnected() == false)
                {
                    continue;
                }

                var recvData = Network.Receive();

                if (recvData.Count > 0)
                {
                    PacketBuffer.Write(recvData.Array, recvData.Offset, recvData.Count);

                    while (true)
                    {
                        //Byte Array(segment)
                        var data = PacketBuffer.Read();
                        if (data.Count < 1)
                        {
                            break;
                        }

                        var packet = new PacketData();
                        //Packet Header
                        packet.PacketID = BitConverter.ToInt16(data.Array, data.Offset); //0
                        packet.DataSize = BitConverter.ToInt16(data.Array, data.Offset+2); //2
                        //Packet BodyData
                        packet.BodyData = new byte[packet.DataSize];
                        Buffer.BlockCopy(data.Array, (data.Offset + 4), packet.BodyData, 0, packet.DataSize); //4

                        lock (((System.Collections.ICollection)RecvPacketQueue).SyncRoot)
                        {
                            RecvPacketQueue.Enqueue(packet);
                        }
                    }
                }
                else
                {
                    var packet = new PacketData();
                    packet.PacketID = (short)PACKET_ID.SYSTEM_CLIENT_DISCONNECTD;
                    packet.DataSize = 0;
                    
                    lock (((System.Collections.ICollection)RecvPacketQueue).SyncRoot)
                    {
                        RecvPacketQueue.Enqueue(packet);
                    }
                }
            }
        }

        void NetworkSendProcess()
        {
            while (IsNetworkThreadRunning)
            {
                System.Threading.Thread.Sleep(32);

                if (Network.IsConnected() == false)
                {
                    continue;
                }

                lock (((System.Collections.ICollection)RecvPacketQueue).SyncRoot)
                {
                    if (SendPacketQueue.Count > 0)
                    {
                        var packet = SendPacketQueue.Dequeue();
                        Network.Send(packet);
                    }
                }
            }
        }

        void ReadPacketQueueProcess(object sender, EventArgs e)
        {
            ProcessLog();

            try
            {
                PacketData packet = new PacketData();

                lock (((System.Collections.ICollection)RecvPacketQueue).SyncRoot)
                {
                    if (RecvPacketQueue.Count() > 0)
                    {
                        packet = RecvPacketQueue.Dequeue();
                    }
                }

                if (packet.PacketID != 0)
                {
                    PacketHandler.Process(packet);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(string.Format("ReadPacketQueueProcess. error:{0}", ex.Message));
            }
        }

        private void ProcessLog()
        {
            // 너무 이 작업만 할 수 없으므로 일정 작업 이상을 하면 일단 패스한다.
            int logWorkCount = 0;

            while (true)
            {
                string msg;

                if (DevLog.GetLog(out msg))
                {
                    ++logWorkCount;

                    if (listBoxLog.Items.Count > 512)
                    {
                        listBoxLog.Items.Clear();
                    }

                    listBoxLog.Items.Add(msg);
                    listBoxLog.SelectedIndex = listBoxLog.Items.Count - 1;
                }
                else
                {
                    break;
                }

                if (logWorkCount > 8)
                {
                    break;
                }
            }
        }

        
    }


    public enum CLIENT_STATUS
    {
        NONE = 0,
        LOGIN = 1,
        LOBBY = 2,
    }
    
}
