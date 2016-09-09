using System;
using System.Collections.Generic;
using System.Text;

using System.Net;
using System.Net.Sockets;

using System.Threading;
using System.Runtime.InteropServices;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;


namespace RgsClient
{
    #region Enums
    public enum SocketState
    {
        Closed,
        Closing,
        Connected,
        Connecting,
        Listening,
    }
    #endregion

    #region Event Args
    public class NetSocketConnectedEventArgs : EventArgs
    {
        public IPAddress SourceIP;
        public NetSocketConnectedEventArgs(IPAddress ip)
        {
            this.SourceIP = ip;
        }
    }

    public class NetSocketDisconnectedEventArgs : EventArgs
    {
        public string Reason;
        public NetSocketDisconnectedEventArgs(string reason)
        {
            this.Reason = reason;
        }
    }

    public class NetSockStateChangedEventArgs : EventArgs
    {
        public SocketState NewState;
        public SocketState PrevState;
        public NetSockStateChangedEventArgs(SocketState newState, SocketState prevState)
        {
            this.NewState = newState;
            this.PrevState = prevState;
        }
    }

    public class NetSockDataArrivalEventArgs : EventArgs
    {
        public byte[] Data;
        public int Size = 0;
        public NetSockDataArrivalEventArgs(byte[] data, int size)
        {
            this.Data = data;
            this.Size = size;
        }
    }

    public class NetSockErrorReceivedEventArgs : EventArgs
    {
        public string Function;
        public Exception Exception;
        public NetSockErrorReceivedEventArgs(string function, Exception ex)
        {
            this.Function = function;
            this.Exception = ex;
        }
    }

    public class NetSockConnectionRequestEventArgs : EventArgs
    {
        public Socket Client;
        public NetSockConnectionRequestEventArgs(Socket client)
        {
            this.Client = client;
        }
    }
    #endregion

    #region Socket Classes
    public abstract class NetBase
    {
        #region Fields
        /// <summary>Current socket state</summary>
        protected SocketState state = SocketState.Closed;
        /// <summary>The socket object, obviously</summary>
        protected Socket socket;

        const int MaxBufferSize = 8192;

        private long nextSendTime = 0;

        /// <summary>Queue of objects to be sent out</summary>
        protected MemoryStream sendBuffer = new MemoryStream();

        /// <summary>Store incoming bytes to be processed</summary>
        protected byte[] byteBuffer = new byte[MaxBufferSize];

        /// <summary>Threaded timer checks if socket is busted</summary>
        protected Timer connectionTimer;
        /// <summary>Interval for socket checks (ms)</summary>
        protected int ConnectionCheckInterval = 1000;
        #endregion

        #region Public Properties
        /// <summary>Current state of the socket</summary>
        public SocketState State { get { return this.state; } }

        /// <summary>Port the socket control is listening on.</summary>
        public int LocalPort
        {
            get
            {
                try
                {
                    return ((IPEndPoint)this.socket.LocalEndPoint).Port;
                }
                catch
                {
                    return -1;
                }
            }
        }

        /// <summary>IP address enumeration for local computer</summary>
        public static string[] LocalIP
        {
            get
            {
                IPHostEntry h = Dns.GetHostEntry(Dns.GetHostName());
                List<string> s = new List<string>(h.AddressList.Length);
                foreach (IPAddress i in h.AddressList)
                    s.Add(i.ToString());
                return s.ToArray();
            }
        }
        #endregion

        #region Events
        /// <summary>Socket is connected</summary>
        public event EventHandler<NetSocketConnectedEventArgs> Connected;
        /// <summary>Socket connection closed</summary>
        public event EventHandler<NetSocketDisconnectedEventArgs> Disconnected;
        /// <summary>Socket state has changed</summary>
        /// <remarks>This has the ability to fire very rapidly during connection / disconnection.</remarks>
        public event EventHandler<NetSockStateChangedEventArgs> StateChanged;
        /// <summary>Recived a new object</summary>
        public event EventHandler<NetSockDataArrivalEventArgs> DataArrived;
        /// <summary>An error has occurred</summary>
        public event EventHandler<NetSockErrorReceivedEventArgs> ErrorReceived;
        #endregion

        #region Constructor
        /// <summary>Base constructor sets up buffer and timer</summary>
        public NetBase()
        {
            this.connectionTimer = new Timer(
                new TimerCallback(this.connectedTimerCallback),
                null, Timeout.Infinite, Timeout.Infinite);
        }
        #endregion

        #region CopyBytes
        static unsafe void Copy(byte[] source, int sourceOffset, byte[] target,
        int targetOffset, int count)
        {
            // If either array is not instantiated, you cannot complete the copy.
            if ((source == null) || (target == null))
            {
                throw new System.ArgumentException();
            }

            // If either offset, or the number of bytes to copy, is negative, you
            // cannot complete the copy.
            if ((sourceOffset < 0) || (targetOffset < 0) || (count < 0))
            {
                throw new System.ArgumentException();
            }

            // If the number of bytes from the offset to the end of the array is 
            // less than the number of bytes you want to copy, you cannot complete
            // the copy. 
            if ((source.Length - sourceOffset < count) ||
                (target.Length - targetOffset < count))
            {
                throw new System.ArgumentException();
            }

            // The following fixed statement pins the location of the source and
            // target objects in memory so that they will not be moved by garbage
            // collection.
            fixed (byte* pSource = source, pTarget = target)
            {
                // Set the starting points in source and target for the copying.
                byte* ps = pSource + sourceOffset;
                byte* pt = pTarget + targetOffset;

                // Copy the specified number of bytes from source to target.
                for (int i = 0; i < count; i++)
                {
                    *pt = *ps;
                    pt++;
                    ps++;
                }
            }
        }
        #endregion

        #region Send

        private long Now()
        {
            return DateTime.Now.Ticks / TimeSpan.TicksPerMillisecond;
        }

        /// <summary>Send data</summary>
        /// <param name="bytes">Bytes to send</param>
        public void Send(byte[] data)
        {
            try
            {
                if (data == null)
                    throw new NullReferenceException("data cannot be null");
                else if (data.Length == 0)
                    throw new NullReferenceException("data cannot be empty");
                else
                {
                    List<ArraySegment<byte>> send = new List<ArraySegment<byte>>(1);
                    send.Add(new ArraySegment<byte>(data));
                    this.socket.BeginSend(send, SocketFlags.None, new AsyncCallback(this.SendCallback), this.socket);

                    //if (sendBuffer.Length + data.Length >= MaxBufferSize)
                    //{
                    //    Send();

                    //    //보낸 시간 저장
                    //    nextSendTime = Now() + 16;
                    //}

                    //sendBuffer.Write(data, 0, data.Length);
                    //if (nextSendTime <= Now())
                    //{
                    //    Send();

                    //    //보낸 시간 저장
                    //    nextSendTime = Now() + 16;
                    //}
                }
            }
            catch (Exception ex)
            {
                this.OnErrorReceived("Send", ex);
            }
        }

        private void Send()
        {
            List<ArraySegment<byte>> send = new List<ArraySegment<byte>>(1);
            send.Add(new ArraySegment<byte>(ReadToBytes(ref sendBuffer)));
            this.socket.BeginSend(send, SocketFlags.None, new AsyncCallback(this.SendCallback), this.socket);
        }

        static byte[] ReadToBytes(ref MemoryStream stream)
        {
            stream.Position = 0;
            byte[] byteData = new byte[stream.Length];
            stream.Read(byteData, 0, (int)stream.Length);

            stream.SetLength(0);
            stream.Position = 0;

            return byteData;
        }

        /// <summary>Callback for BeginSend</summary>
        /// <param name="ar"></param>
        private void SendCallback(IAsyncResult ar)
        {
            try
            {
                Socket sock = (Socket)ar.AsyncState;
                int didSend = sock.EndSend(ar);
                if (this.socket != sock)
                {
                    this.Close("Async Connect Socket mismatched");
                    return;
                }
            }
            catch (ObjectDisposedException)
            {
                return;
            }
            catch (SocketException ex)
            {
                if (ex.SocketErrorCode == SocketError.ConnectionReset)
                    this.Close("Remote Socket Closed");
                else
                    throw;
            }
            catch (Exception ex)
            {
                this.Close("Socket Send Exception");
                this.OnErrorReceived("Socket Send", ex);
            }
        }
        #endregion

        #region Close
        /// <summary>Disconnect the socket</summary>
        /// <param name="reason"></param>
        public void Close(string reason)
        {
            try
            {
                if (this.state == SocketState.Closing || this.state == SocketState.Closed)
                    return; // already closing/closed

                this.OnChangeState(SocketState.Closing);

                if (this.socket != null)
                {
                    this.socket.Close();
                    this.socket = null;
                }
            }
            catch (Exception ex)
            {
                this.OnErrorReceived("Close", ex);
            }

            try
            {
                this.OnChangeState(SocketState.Closed);
                if (this.Disconnected != null)
                    this.Disconnected(this, new NetSocketDisconnectedEventArgs(reason));
            }
            catch (Exception ex)
            {
                this.OnErrorReceived("Close Cleanup", ex);
            }
        }
        #endregion

        #region Receive
        /// <summary>Receive data asynchronously</summary>
        protected void Receive()
        {
            try
            {
                lock (this.byteBuffer)
                {
                    this.socket.BeginReceive(this.byteBuffer, 0, this.byteBuffer.Length, SocketFlags.None, new AsyncCallback(this.ReceiveCallback), this.socket);
                }
            }
            catch (Exception ex)
            {
                this.OnErrorReceived("Receive", ex);
            }
        }

        /// <summary>Callback for BeginReceive</summary>
        /// <param name="ar"></param>
        private void ReceiveCallback(IAsyncResult ar)
        {
            try
            {
                lock (this.byteBuffer)
                {
                    Socket sock = (Socket)ar.AsyncState;
                    int size = sock.EndReceive(ar);

                    if (this.socket != sock)
                    {
                        this.Close("Async Receive Socket mismatched");
                        return;
                    }

                    if (size < 1)
                    {
                        this.Close("No Bytes Received");
                        return;
                    }

                    if (this.DataArrived != null)
                    {
                        this.DataArrived(this, new NetSockDataArrivalEventArgs(this.byteBuffer, size));
                    }
                    this.socket.BeginReceive(this.byteBuffer, 0, this.byteBuffer.Length, SocketFlags.None, new AsyncCallback(this.ReceiveCallback), this.socket);
                }
            }
            catch (ObjectDisposedException)
            {
                return; // socket disposed, let it die quietly
            }
            catch (SocketException ex)
            {
                if (ex.SocketErrorCode == SocketError.ConnectionReset)
                    this.Close("Remote Socket Closed");
                else
                    throw;
            }
            catch (Exception ex)
            {
                this.Close("Socket Receive Exception");
                this.OnErrorReceived("Socket Receive", ex);
            }
        }

        /// <summary>Find first position the specified byte within the stream, or -1 if not found</summary>
        /// <param name="ms"></param>
        /// <param name="find"></param>
        /// <returns></returns>
        private int IndexOfByteInStream(MemoryStream ms, byte find)
        {
            int b;
            do
            {
                b = ms.ReadByte();
            } while (b > -1 && b != find);

            if (b == -1)
                return -1;
            else
                return (int)ms.Position - 1; // position is +1 byte after the byte we want
        }

        /// <summary>Find first position the specified bytes within the stream, or -1 if not found</summary>
        /// <param name="ms"></param>
        /// <param name="find"></param>
        /// <returns></returns>
        private int IndexOfBytesInStream(MemoryStream ms, byte[] find)
        {
            int index;
            do
            {
                index = this.IndexOfByteInStream(ms, find[0]);

                if (index > -1)
                {
                    bool found = true;
                    for (int i = 1; i < find.Length; i++)
                    {
                        if (find[i] != ms.ReadByte())
                        {
                            found = false;
                            ms.Position = index + 1;
                            break;
                        }
                    }
                    if (found)
                        return index;
                }
            } while (index > -1);
            return -1;
        }
        #endregion

        #region OnEvents
        protected void OnErrorReceived(string function, Exception ex)
        {
            if (this.ErrorReceived != null)
                this.ErrorReceived(this, new NetSockErrorReceivedEventArgs(function, ex));
        }

        protected void OnConnected(Socket sock)
        {
            if (this.Connected != null)
                this.Connected(this, new NetSocketConnectedEventArgs(((IPEndPoint)sock.RemoteEndPoint).Address));
        }

        protected void OnChangeState(SocketState newState)
        {
            SocketState prev = this.state;
            this.state = newState;
            if (this.StateChanged != null)
                this.StateChanged(this, new NetSockStateChangedEventArgs(this.state, prev));

            if (this.state == SocketState.Connected)
                this.connectionTimer.Change(0, this.ConnectionCheckInterval);
            else if (this.state == SocketState.Closed)
                this.connectionTimer.Change(Timeout.Infinite, Timeout.Infinite);
        }
        #endregion

        #region Connection Sanity Check
        private void connectedTimerCallback(object sender)
        {
            try
            {
                if (this.state == SocketState.Connected &&
                    (this.socket == null || !this.socket.Connected))
                    this.Close("Connect Timer");
            }
            catch (Exception ex)
            {
                this.OnErrorReceived("ConnectTimer", ex);
                this.Close("Connect Timer Exception");
            }
        }
        #endregion
    }

    public class NetClient : NetBase
    {
        #region Constructor
        public NetClient() : base() { }
        #endregion

        #region Connect
        /// <summary>Connect to the computer specified by Host and Port</summary>
        public void Connect(IPEndPoint endPoint)
        {
            if (this.state == SocketState.Connected)
                return; // already connecting to something

            try
            {
                if (this.state != SocketState.Closed)
                    throw new Exception("Cannot connect socket is " + this.state.ToString());

                this.OnChangeState(SocketState.Connecting);

                if (this.socket == null)
                    this.socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

                this.socket.BeginConnect(endPoint, new AsyncCallback(this.ConnectCallback), this.socket);
            }
            catch (Exception ex)
            {
                this.OnErrorReceived("Connect", ex);
                this.Close("Connect Exception");
            }
        }

        /// <summary>Callback for BeginConnect</summary>
        /// <param name="ar"></param>
        private void ConnectCallback(IAsyncResult ar)
        {
            try
            {
                Socket sock = (Socket)ar.AsyncState;
                sock.EndConnect(ar);

                if (this.socket != sock)
                {
                    this.Close("Async Connect Socket mismatched");
                    return;
                }

                if (this.state != SocketState.Connecting)
                    throw new Exception("Cannot connect socket is " + this.state.ToString());

                this.socket.ReceiveBufferSize = this.byteBuffer.Length;
                this.socket.SendBufferSize = this.byteBuffer.Length;

                this.OnChangeState(SocketState.Connected);
                this.OnConnected(this.socket);

                this.Receive();
            }
            catch (Exception ex)
            {
                this.Close("Socket Connect Exception");
                this.OnErrorReceived("Socket Connect", ex);
            }
        }
        #endregion
    }
    #endregion
}