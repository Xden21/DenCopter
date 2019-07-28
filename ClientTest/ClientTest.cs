using System;
using System.Net.Sockets;
using System.Text;

namespace ClientTest
{
    class ClientTest
    {
        void StartComm()
        {
            TcpClient client = new TcpClient("192.168.100.1", 8560);
            NetworkStream stream = client.GetStream();

                byte[] buffer = new byte[1];
                buffer[0] = 1;
                stream.Write(buffer, 0, buffer.Length);
                byte[] rbuffer = new byte[256];
                int amount = stream.Read(rbuffer, 0, 256);
                byte[] datareceived = new byte[amount];
                Array.Copy(rbuffer, 0, datareceived, 0, amount);
                string message = Encoding.UTF8.GetString(rbuffer, 0, amount);               
                Console.WriteLine(message);
             
        }


        static void Main(string[] args)
        {
            Console.WriteLine("Starting a client");
            ClientTest clientTest = new ClientTest();
            clientTest.StartComm();
        }
    }
}
