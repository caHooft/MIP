
//All methods
/*
 * SendDataToApp
 * ReceiveRFData
 * SendRFInfo
 */

//Inlduding libraries
#include <NewRemoteTransmitter.h>
#include <NewRemoteReceiver.h>
#include <SPI.h>
#include <Ethernet.h>

//Declaring pins

//Declaring some variables
IPAddress ip(192, 168, 1, 3);
bool connected =false;
float lightvalue;
float THvalue;
int THtemp;
int THhumid;

//Declaring some hardware
EthernetServer server(80);

//Declaring some arrays
byte mac[] = 
{ 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED 
};

void setup() 
{
  //Serial.begin(9600);  
  Serial.begin(115200);  
  Serial.println("Arduino UNO 3 start");
 
  //Setting up some hardware
  NewRemoteReceiver::init(0, 2, ReceiveRFData);
  Ethernet.begin(mac, ip);
  Serial.print("Current IP: ");
  Serial.println(Ethernet.localIP());
  server.begin();
  connected  = true;  
}

void loop() 
{
  if(!connected) return;
  EthernetClient ethernetClient = server.available();
 
  if(!ethernetClient)
  { 
    Serial.println("No Connection"); 
    return;
  }
  
  while(ethernetClient.connected())
  {
    char buffer[128];
    int count =0;

    while(ethernetClient.available())
    {
      buffer[count ++] = ethernetClient.read();
    }

    buffer[count]  = '\0';

    if(count > 0)
    {
      SendDataToApp(ethernetClient, buffer);
    }
  }
}

void SendDataToApp(EthernetClient client, char buffer[128])
{  
  String bufferString = String(buffer);
  
   Serial.print(bufferString);
   if(bufferString == String("7"))
   {
    String cmd = "data";
    cmd += "LDR"+ (String)lightvalue + "|";
    cmd += "Humidity"+ (String)THhumid +"}"; 
    cmd += "Temperature" + (String)THtemp +";";
    client.print(cmd);
    Serial.println(cmd); 
    return;  
   }

   SendRFInfo(bufferString.toInt());
}

void ReceiveRFData(NewRemoteCode receivedCode) 
{
  Serial.print("cmd  = "); // command
  
  char cmd = String(receivedCode.address).charAt(0);
  Serial.print(cmd);
  Serial.print(" val = ");
  int val = String(receivedCode.address).substring(1).toInt();
  Serial.println(val);
  
  if(cmd == '7') lightvalue = val;
  if(cmd == '8') THhumid = val;
  if(cmd == '9') THtemp = val;
}

void SendRFInfo(int cmd)
{  
  Serial.print("Sending Data = ");
  Serial.println(cmd);

  NewRemoteTransmitter transmitter(cmd, 9, 266);  
  transmitter.sendUnit(5, 1); 
}
