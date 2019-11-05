using System;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Timers;
using Android.App;
using Android.Content;
using Android.Runtime;
using Android.Views;
using Android.Widget;
using Android.OS;
using Android.Content.PM;
using System.Text.RegularExpressions;
using System.Collections.Generic;
using Android.Graphics;
using System.Threading.Tasks;
using System.Collections;
using MIP;

namespace MIPAPP
{
    class Server
    {
        private const string IPADRESS = "192.168.1.3";
        private const string PORT = "80";
        Socket socket = null; // Socket   

        public bool IsConnected
        {
            get { return Convert.ToBoolean(socket.Available) && socket.Connected; }
        }

        public void Connect(MainActivity activity)
        {
            if (CheckValidIpAddress(IPADRESS) && CheckValidPort(PORT))
            {
                ConnectSocket(IPADRESS, PORT, activity);                
            }
        }

        public bool Send(string cmd)
        {
            if (socket == null) return false;

            bool hasSend = false;
            
            if (socket.Connected)
            {
                socket.Send(Encoding.ASCII.GetBytes(cmd));                 // Send toggle-command to the Arduino
                hasSend = true;
            }
            return hasSend;
        }

        public string Receive()
        {
            string result = "-";

            byte[] bytes = new byte[4096]; // response is always 4 bytes
            int bytesRec = socket.Receive(bytes);

            result = Encoding.ASCII.GetString(bytes, 0, bytesRec);

            return result;
        }

        // Connect to socket ip/prt (simple sockets)
        public void ConnectSocket(string ip, string prt, Activity activity)
        {
            activity.RunOnUiThread(() =>
            {
                if (socket == null)                                       // create new socket
                {
                    Toast.MakeText(activity, "Connecting...", ToastLength.Short).Show();
                    try  // to connect to the server (Arduino).
                    {
                        socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                        socket.Connect(new IPEndPoint(IPAddress.Parse(ip), Convert.ToInt32(prt)));
                        if (socket.Connected)
                        {
                            Toast.MakeText(activity, "Connected", ToastLength.Short).Show();
                        }
                    }
                    catch (Exception exception)
                    {
                        if (socket != null)
                        {
                            socket.Close();
                            socket = null;
                        }
                        Toast.MakeText(activity, exception.Message, ToastLength.Short).Show();
                    }
                }
                else // disconnect socket
                {
                    socket.Close(); socket = null;
                    Toast.MakeText(activity, "Disconnected", ToastLength.Short).Show();
                }
            });
        }

        //Check if the entered port is valid.
        private bool CheckValidPort(string port)
        {
            //Check if a value is entered.
            if (port != "")
            {
                Regex regex = new Regex("[0-9]+");
                Match match = regex.Match(port);

                if (match.Success)
                {
                    int portAsInteger = Int32.Parse(port);
                    //Check if port is in range.
                    return ((portAsInteger >= 0) && (portAsInteger <= 65535));
                }
                else return false;
            }
            else return false;
        }
        //Check if the entered IP address is valid.
        private bool CheckValidIpAddress(string ip)
        {
            if (ip != "")
            {
                //Check user input against regex (check if IP address is not empty).
                Regex regex = new Regex("\\b((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)(\\.|$)){4}\\b");
                Match match = regex.Match(ip);
                return match.Success;
            }
            else return false;
        }

        public void Stop()
        {
        }
    }
}