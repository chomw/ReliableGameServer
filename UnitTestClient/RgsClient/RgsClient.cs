using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RgsClient
{
    public class Session
    {
        private NetClient client = new NetClient();
        private Protocol.Protocol protocol = null;

        public Session(Protocol.Protocol protocol)
        {
            this.protocol = protocol;

            this.client.Connected += new EventHandler<NetSocketConnectedEventArgs>(Connected);
            this.client.DataArrived += new EventHandler<NetSockDataArrivalEventArgs>(Received);
            this.client.Disconnected += new EventHandler<NetSocketDisconnectedEventArgs>(Disconnected);
            this.client.ErrorReceived += new EventHandler<NetSockErrorReceivedEventArgs>(ErrorReceived);
            this.client.StateChanged += new EventHandler<NetSockStateChangedEventArgs>(StateChanged);
        }

        //IO
        private void StateChanged(object sender, NetSockStateChangedEventArgs e)
        {
            Console.WriteLine("State: " + e.PrevState.ToString() + " -> " + e.NewState.ToString());
        }

        private void ErrorReceived(object sender, NetSockErrorReceivedEventArgs e)
        {
            if (e.Exception.GetType() == typeof(System.Net.Sockets.SocketException))
            {
                System.Net.Sockets.SocketException s = (System.Net.Sockets.SocketException)e.Exception;
                Console.WriteLine("Error: " + e.Function + " - " + s.SocketErrorCode.ToString() + "\r\n" + s.ToString());
            }
            else
                Console.WriteLine("Error: " + e.Function + "\r\n" + e.Exception.ToString());
        }

        private void Disconnected(object sender, NetSocketDisconnectedEventArgs e)
        {
            Console.WriteLine("Disconnected: " + e.Reason);
        }

        private void Received(object sender, NetSockDataArrivalEventArgs e)
        {
            protocol.ReadPacket(e.Data, e.Size);
        }

        private void Connected(object sender, NetSocketConnectedEventArgs e)
        {
            Console.WriteLine("Connected: " + e.SourceIP);
        }

        public void Disconnect()
        {
            this.client.Close("User forced");
        }

        public void Connect(string host, int port)
        {
            System.Net.IPEndPoint end = new System.Net.IPEndPoint(System.Net.IPAddress.Parse(host), port);
            this.client.Connect(end);
        }

        public void Send(byte[] msg)
        {
            if(msg == null)
            {
                return;
            }

            if(msg.Length <= 0)
            {
                return;
            }

            this.client.Send(msg);
        }
    }

}
