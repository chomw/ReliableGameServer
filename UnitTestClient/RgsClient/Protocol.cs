using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using ProtoBuf;
using System.IO;
using System.Runtime.InteropServices;
using SimpleJson;

namespace RgsClient.Protocol
{
    public abstract class Protocol
    {
        public abstract void ReadPacket(byte[] data, int size);
    }

    public class Convert
    {
        public static string ToUtf8String(string str)
        {
            return Encoding.UTF8.GetString(ToUtf8Bytes(str));
        }

        public static byte[] ToUtf8Bytes(string str)
        {
            byte[] defaultBytes = Encoding.Default.GetBytes(str);
            byte[] utf8Bytes = Encoding.Convert(Encoding.Default, Encoding.UTF8, defaultBytes);

            return utf8Bytes;
        }        
    }

    public class Byte
    {
        public static byte[] Merge(byte[] arg1, byte[] arg2)
        {
            byte[] tmp = new byte[arg1.Length + arg2.Length];
            for (int i = 0; i < arg1.Length; i++)
            {
                tmp[i] = arg1[i];
            }
            for (int j = 0; j < arg2.Length; j++)
            {
                tmp[arg1.Length + j] = arg2[j];
            }
            return tmp;
        }
    }
    

    [StructLayout(LayoutKind.Sequential)]
    struct Integer
    {
        [MarshalAs(UnmanagedType.I4)]
        public int value;

        // Calling this method will return a byte array with the contents
        // of the struct ready to be sent via the tcp socket.
        public byte[] Serialize()
        {
            // allocate a byte array for the struct data
            var buffer = new byte[Marshal.SizeOf(typeof(Integer))];

            // Allocate a GCHandle and get the array pointer
            var gch = GCHandle.Alloc(buffer, GCHandleType.Pinned);
            var pBuffer = gch.AddrOfPinnedObject();

            // copy data from struct to array and unpin the gc pointer
            Marshal.StructureToPtr(this, pBuffer, false);
            gch.Free();

            return buffer;
        }

        // this method will deserialize a byte array into the struct.
        public void Deserialize(ref byte[] data)
        {
            var gch = GCHandle.Alloc(data, GCHandleType.Pinned);
            this = (Integer)Marshal.PtrToStructure(gch.AddrOfPinnedObject(), typeof(Integer));
            gch.Free();
        }

        public static byte[] SerializedBytes(int value)
        {
            Integer data = new Integer() { value = value };
            return data.Serialize();
        }

        public static int SizeOf()
        {
            return Marshal.SizeOf(typeof(int));
        }
    }
}

namespace RgsClient.Protocol.ProtoBuf
{
    public class MergeToHeader
    {
        public static byte[] Merge(int packetId, byte[] body)
        {
            //Header Serializing
            byte[] header = PacketHeader.SerializedBytes(packetId, body.Length);

            byte[] packet = new byte[body.Length + header.Length];
            packet = Byte.Merge(header, body);

            return packet;
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    struct PacketHeader
    {
        [MarshalAs(UnmanagedType.I4)]
        public int packetId;

        [MarshalAs(UnmanagedType.I4)]
        public int size;


        // Calling this method will return a byte array with the contents
        // of the struct ready to be sent via the tcp socket.
        public byte[] Serialize()
        {
            // allocate a byte array for the struct data
            var buffer = new byte[Marshal.SizeOf(typeof(PacketHeader))];

            // Allocate a GCHandle and get the array pointer
            var gch = GCHandle.Alloc(buffer, GCHandleType.Pinned);
            var pBuffer = gch.AddrOfPinnedObject();

            // copy data from struct to array and unpin the gc pointer
            Marshal.StructureToPtr(this, pBuffer, false);
            gch.Free();

            return buffer;
        }

        // this method will deserialize a byte array into the struct.
        public void Deserialize(ref byte[] data)
        {
            var gch = GCHandle.Alloc(data, GCHandleType.Pinned);
            this = (PacketHeader)Marshal.PtrToStructure(gch.AddrOfPinnedObject(), typeof(PacketHeader));
            gch.Free();
        }

        public static byte[] SerializedBytes(int packetId, int size)
        {
            PacketHeader header = new PacketHeader() { packetId = packetId, size = size };
            return header.Serialize();
        }
    }

    public class Protocol : RgsClient.Protocol.Protocol
    {
        protected MemoryStream buffer = new MemoryStream();

        public override void ReadPacket(byte[] data, int size)
        {
        }
    }
}

namespace RgsClient.Protocol.Json
{
    public class Packet
    {
        public string name;
        public JsonObject packet;

        public static byte[] SerializedBytes(string packetName, JsonObject packet)
        {
            byte[] header = Convert.ToUtf8Bytes(packetName);
            byte[] body = Convert.ToUtf8Bytes(packet.ToString());

            byte[] bytePacket = Byte.Merge(
                Byte.Merge(Integer.SerializedBytes(header.Length), header),
                Byte.Merge(Integer.SerializedBytes(body.Length), body)
                );

            return bytePacket;
        }
    }

    public class Protocol : RgsClient.Protocol.Protocol
    {
        protected MemoryStream buffer = new MemoryStream();

        private object sync = new object();
        private Queue<Packet> queue = new Queue<Packet>();

        private void Enqueue(Packet packet)
        {
            lock (sync)
            {
                queue.Enqueue(packet);
            }
        }

        public Packet Dequeue()
        {
            lock (sync)
            {
                return queue.Dequeue();
            }
        }

        public bool IsEmpty()
        {
            lock (sync)
            {
                return queue.Count > 0;
            }
        }



        private int ReadInt()
        {
            byte[] bytes = new byte[Integer.SizeOf()];
            this.buffer.Read(bytes, 0, Integer.SizeOf());

            Integer integer = new Integer();
            integer.Deserialize(ref bytes);

            return integer.value;
        }

        private string ReadString(int length)
        {
            byte[] bytes = new byte[length];
            this.buffer.Read(bytes, 0, length);

            return Encoding.UTF8.GetString(bytes);
        }

        public override void ReadPacket(byte[] data, int size)
        {
            buffer.Write(data, 0, size);
            buffer.Position = 0;

            while (true)
            {
                //header size
                if (Integer.SizeOf() + buffer.Position > buffer.Length)
                {
                    break;
                }

                int headerSize = ReadInt();

                //header
                if (headerSize + buffer.Position > buffer.Length)
                {
                    break;
                }

                string header = ReadString(headerSize);

                //body size
                if (Integer.SizeOf() + buffer.Position > buffer.Length)
                {
                    break;
                }

                int bodySize = ReadInt();

                //body
                if (bodySize + this.buffer.Position > this.buffer.Length)
                {
                    break;
                }

                string body = ReadString(bodySize);

                //Console.WriteLine("header size : " + headerSize);
                //Console.WriteLine("header : " + header);
                //Console.WriteLine("body size : " + bodySize);
                Console.WriteLine("body : " + body);
            }

            buffer.SetLength(0);
            buffer.Capacity = data.Length;
        }
    }
}