using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;

using ProtoBuf;
using RgsClient;

namespace UnitTestClient
{
    [ProtoContract]
    struct Login
    {
        [ProtoMember(1)]
        public string id;

        [ProtoMember(2)]
        public string password;

        public static byte[] SerializedBytes(string id, string password)
        {
            Login login = new Login() { id = RgsClient.Protocol.Convert.ToUtf8String(id), password = RgsClient.Protocol.Convert.ToUtf8String(password) };

            MemoryStream serialize = new MemoryStream();
            ProtoBuf.Serializer.Serialize<Login>(serialize, login);

            return RgsClient.Protocol.ProtoBuf.MergeToHeader.Merge(1, serialize.ToArray());
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            const int maxThread = 1;
            Thread[] threads = new Thread[maxThread];
            for (int i = 0; i < maxThread; ++i)
            {
                threads[i] = new Thread(new ThreadStart(Stress));
                threads[i].Start();
            }

            Console.ReadLine();
        }

        static void Stress()
        {
            for (int i = 0; i < 10000; ++i)
            {
                ProtobufTest();
            }
        }

        static void ProtobufTest()
        {
            const int sessionNum = 5000;

            Session[] session = new Session[sessionNum];

            for (int i = 0; i < sessionNum; ++i)
            {
                session[i] = new Session(new RgsClient.Protocol.ProtoBuf.Protocol());
                session[i].Connect("127.0.0.1", 7000);
            }

            byte[] bytes = Login.SerializedBytes("111111", "2222222");

            for (int i = 0; i < sessionNum; ++i)
            {
                for (int j = 0; j < 20; ++j)
                {
                    session[i].Send(bytes);
                }
            }

            Thread.Sleep(400);

            for (int i = 0; i < sessionNum; ++i)
            {
                session[i].Disconnect();
            }

            Thread.Sleep(400);
        }

        static void JsonTest()
        {
            const int sessionNum = 5000;

            Session[] session = new Session[sessionNum];

            for (int i = 0; i < sessionNum; ++i)
            {   
                session[i] = new Session(new RgsClient.Protocol.Json.Protocol());
                session[i].Connect("127.0.0.1", 7000);
            }

            ulong count = 0;
            SimpleJson.JsonObject packet = new SimpleJson.JsonObject();
            packet["id"] = "CSHARP";
            packet["password"] = "CSHARP";

            for (int i = 0; i < sessionNum; ++i)
            {
                for (int j = 0; j < 10; ++j)
                {
                    packet["count"] = count++;
                    session[i].Send(RgsClient.Protocol.Json.Packet.SerializedBytes("login", packet));
                }
            }

            Thread.Sleep(500);

            for (int i = 0; i < sessionNum; ++i)
            {
                session[i].Disconnect();
            }

            Thread.Sleep(500);
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
    }
}
