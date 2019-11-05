
//All methods
/*
 * SendRFData
 * ReceiveRFData
 * WireSend
 * ReadTH
 * ReadLight
 * LED1M
 * LED2M
 * Sounds
 * Sing
 * Buzz
 */

//Including libraries
#include <NewRemoteTransmitter.h>
#include <NewRemoteReceiver.h>
#include <Wire.h>
#include <dht.h>
#include "pitches.h"

//Declaring pins
#define TH 3
#define LED1 6
#define LED2 5
#define spkr 8
#define LDR 16

//Declaring some variables
bool sleep = false;
bool AION = true;
float lightvalue;
float THvalue;
int THtemp;
int THhumid;
String song = "empty";

//Declaring some hardware
dht DHT;

//Declaring song arrays
int mario_melody[] =
		{
				NOTE_E7,
				NOTE_E7,
				0,
				NOTE_E7,
				0,
				NOTE_C7,
				NOTE_E7,
				0,
				NOTE_G7,
				0,
				0,
				0,
				NOTE_G6,
				0,
				0,
				0,
				NOTE_C7,
				0,
				0,
				NOTE_G6,
				0,
				0,
				NOTE_E6,
				0,
				0,
				NOTE_A6,
				0,
				NOTE_B6,
				0,
				NOTE_AS6,
				NOTE_A6,
				0,
				NOTE_G6,
				NOTE_E7,
				NOTE_G7,
				NOTE_A7,
				0,
				NOTE_F7,
				NOTE_G7,
				0,
				NOTE_E7,
				0,
				NOTE_C7,
				NOTE_D7,
				NOTE_B6,
				0,
				0,
				NOTE_C7,
				0,
				0,
				NOTE_G6,
				0,
				0,
				NOTE_E6,
				0,
				0,
				NOTE_A6,
				0,
				NOTE_B6,
				0,
				NOTE_AS6,
				NOTE_A6,
				0,
				NOTE_G6,
				NOTE_E7,
				NOTE_G7,
				NOTE_A7,
				0,
				NOTE_F7,
				NOTE_G7,
				0,
				NOTE_E7,
				0,
				NOTE_C7,
				NOTE_D7,
				NOTE_B6,
				0,
				0,
};

int mario_tempo[] =
		{
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				9,
				9,
				9,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				9,
				9,
				9,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
				12,
};

int underworld_melody[] =
		{
				NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4, NOTE_AS3, NOTE_AS4, 0, 0, NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4, NOTE_AS3, NOTE_AS4, 0, 0,
				NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4, NOTE_DS3, NOTE_DS4, 0, 0, NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4, NOTE_DS3, NOTE_DS4, 0, 0,
				NOTE_DS4, NOTE_CS4, NOTE_D4, NOTE_CS4, NOTE_DS4, NOTE_DS4, NOTE_GS3, NOTE_G3, NOTE_CS4, NOTE_C4, NOTE_FS4, NOTE_F4, NOTE_E3,
				NOTE_AS4, NOTE_A4, NOTE_GS4, NOTE_DS4, NOTE_B3, NOTE_AS3, NOTE_A3, NOTE_GS3, 0, 0, 0};

int underworld_tempo[] =
		{
				12, 12, 12, 12, 12, 12, 6, 3, 12, 12, 12, 12, 12, 12, 6, 3, 12, 12, 12, 12, 12, 12, 6, 3, 12, 12, 12, 12, 12, 12, 6, 6, 18,
				18, 18, 6, 6, 6, 6, 6, 6, 18, 18, 18, 18, 18, 18, 10, 10, 10, 10, 10, 10, 3, 3, 3};

int jingle_melody[] =
		{
				NOTE_e, NOTE_e, NOTE_e, NOTE_e, NOTE_e, NOTE_e, NOTE_e, NOTE_g, NOTE_c, NOTE_d, NOTE_e, 0,
				NOTE_f, NOTE_f, NOTE_f, NOTE_f, NOTE_f, NOTE_e, NOTE_e, NOTE_e, NOTE_e, NOTE_d, NOTE_d, NOTE_e, NOTE_d, NOTE_g};

int jingle_beats[] =
		{
				1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2};

void setup()
{
	Wire.begin();
	Serial.begin(9600);
	Serial.println("Arduino UNO 1 start");

	//Seting up some hardware
	NewRemoteReceiver::init(0, 2, ReceiveRFData);

	//Initializing pins
	pinMode(LED1, OUTPUT);
	pinMode(LED2, OUTPUT);
	pinMode(spkr, OUTPUT);
	pinMode(LDR, INPUT);
	pinMode(TH, INPUT);
	digitalWrite(LED1, HIGH);

	WireSend(1);
	delay(100);
	WireSend(0);
	delay(100);
	WireSend(1);
	delay(100);
	WireSend(0);
	delay(100);
}

void loop()
{
	Serial.println("-------------------------------------------------");
	Serial.println();
	Serial.println("Restart loop");
	Serial.println();

	ReadLight();
	Serial.println();
	ReadTH();
	Serial.println();
	Sounds();
	Serial.println();
	SendRFData();
}

void SendRFData()
{
	int val;

	val = String(7 + String(lightvalue)).toInt();
	NewRemoteTransmitter transmitter(val, 11, 266);
	transmitter.sendUnit(5, 1);

	val = String(8 + String(THhumid)).toInt();
	NewRemoteTransmitter transmitter2(val, 11, 266);
	transmitter2.sendUnit(5, 1);

	val = String(9 + String(THtemp)).toInt();
	NewRemoteTransmitter transmitter3(val, 11, 266);
	transmitter3.sendUnit(5, 1);

	delay(1000);
}

//Method for rerceiving lient data
void ReceiveRFData(NewRemoteCode receivedCode)
{
	Serial.print("Getting Data");
	char cmd = String(receivedCode.address).charAt(0);
	int val = String(receivedCode.address).substring(1).toInt();

	Serial.print("cmd = ");
	Serial.print(cmd);
	Serial.print(" val = ");
	Serial.println(val);

	if (cmd != 6)
	{
		sleep = false;
		WireSend(14);
	}

	if (cmd == '0') //Speed toggle
	{
		int mipspeed = val;

		if (mipspeed > 15)
		{
			WireSend(mipspeed);
		}
	}

	if (cmd == '1') //Motor toggle
	{								/*
    int mtrbool = bool(val);

    if(!mtrbool)
    {
      if(mtrdireccmd > 0)
      {
        int mtrdirec = val;

        Serial.print("Send motor command");

        WireSend(mtrdirec);
      }
    }

    else
    {
      Serial.print("Send motor command");

      WireSend(2);
    }
  */
	}

	if (cmd == '2') //Mood toggle
	{
		Serial.print("Set mood to: ");
		Serial.println(val);

		WireSend(val);
	}

	if (cmd == '3') //Song toggle
	{
		if (val == 0)
			song = "empty";
		if (val == 1)
			song = "mario";
		if (val == 2)
			song = "underworld";

		Serial.print("Change song to: ");
		Serial.println(song);
	}

	if (cmd == '4') // Lights toggle
	{
		Serial.print("Set LED1 to: ");
		Serial.println(val);

		LED1M(val);
	}

	if (cmd == '5') //AI toggle
	{
		Serial.print("Set AI to: ");
		Serial.println(val);

		if (val == 0)
		{
			WireSend(13);
			AION = false;
		}

		if (val == 1)
		{
			WireSend(2);
			AION = true;
		}
	}

	if (cmd == '6') //Sleep toggle
	{
		Serial.println("Mip Went to bed... Good night");

		sleep = true;
		WireSend(8);
	}
}

//Method for sending Wire
void WireSend(int var)
{
	/*Wire.beginTransmission(9);
  Wire.write(var);
  Wire.endTransmission();*/
}

//Method for measuring temperature & humidity
void ReadTH()
{
	THvalue = DHT.read11(TH);

	THtemp = DHT.temperature;
	Serial.print("Temperature = ");
	Serial.println(THtemp);

	THhumid = DHT.humidity;
	Serial.print("Humidity = ");
	Serial.println(THhumid);
}

//Method for reading LDR and turning on/of LEDs
void ReadLight()
{
	lightvalue = analogRead(LDR);

	Serial.print("Light = ");
	Serial.println(lightvalue);

	if (lightvalue >= 200)
	{
		LED2M(HIGH);
	}

	else if (lightvalue < 200)
	{
		LED2M(LOW);
	}
}

//Method for controlling LED batch 1
void LED1M(int LEDvalue)
{
	if (LEDvalue == 0)
	{
		digitalWrite(LED1, LOW);
	}

	if (LEDvalue == 1)
	{
		digitalWrite(LED1, HIGH);
	}
}

//Method for controlling LED batch 2
void LED2M(byte b)
{
	digitalWrite(LED2, b);
}

//Method for the making of sounds
void Sounds()
{
	Sing(song);
}

void Sing(String song)
{
	Serial.print("Currently playing song: ");
	Serial.println(song);

	if (song == "empty")
	{
		delay(1500);
	}

	if (song == "mario")
	{
		int size = sizeof(mario_melody) / sizeof(int);

		for (int i = 0; i < size; i++)
		{
			int noteDuration = 1000 / mario_tempo[i];

			Buzz(mario_melody[i], noteDuration);

			int noteDelay = noteDuration * 1.30;
			delay(noteDelay);

			Buzz(0, noteDuration);
		}
	}

	if (song == "underworld")
	{
		int size = sizeof(underworld_melody) / sizeof(int);

		for (int i = 0; i < size; i++)
		{
			int noteDuration = 1000 / underworld_tempo[i];

			Buzz(underworld_melody[i], noteDuration);

			int noteDelay = noteDuration * 1.30;
			delay(noteDelay);

			Buzz(0, noteDuration);
		}
	}

	if (song == "jingle")
	{
		int size = sizeof(jingle_melody) / sizeof(int);

		for (int i = 0; i < size; i++)
		{
			int noteDuration = 1000 / jingle_beats[i];

			Tone(jingle_melody[i], noteDuration);

			int noteDelay = noteDuration * 1.30;
			delay(noteDelay);

			Tone(0, noteDuration);
		}
	}
}

void Buzz(long frequency, long length)
{
	long delayValue = 1000000 / frequency / 2;
	long Cycles = frequency * length / 1000;

	for (long i = 0; i < Cycles; i++)
	{
		digitalWrite(spkr, HIGH);
		delayMicroseconds(delayValue);
		digitalWrite(spkr, LOW);
		delayMicroseconds(delayValue);
	}
}

void Tone(int Freq, int duration)
{
	int uSdelay = 1000000 / Freq;
	int on_time = uSdelay * 0.7;
	int off_time = uSdelay * 0.3;

	unsigned long ending = millis() + duration;
	while (millis() < ending)
	{
		digitalWrite(spkr, HIGH);
		delayMicroseconds(on_time);
		digitalWrite(spkr, LOW);
		delayMicroseconds(off_time);
	}
}
