// Xamarin/C# app voor de besturing van een Arduino (Uno met Ethernet Shield) m.b.v. een socket-interface.
// Dit programma werkt samen met het Arduino-programma DomoticaServer.ino
// De besturing heeft betrekking op het aan- en uitschakelen van een Arduino pin, waar een led aan kan hangen of, 
// t.b.v. het Domotica project, een RF-zender waarmee een klik-aan-klik-uit apparaat bestuurd kan worden.
//
// De app heeft twee modes die betrekking hebben op de afhandeling van de socket-communicatie: "simple-mode" en "threaded-mode" 
// Wanneer het statement    //connector = new Connector(this);    wordt uitgecommentarieerd draait de app in "simple-mode",
// Het opvragen van gegevens van de Arduino (server) wordt dan met een Timer gerealisseerd. (De extra classes Connector.cs, 
// Receiver.cs en Sender.cs worden dan niet gebruikt.) 
// Als er een connector wordt aangemaakt draait de app in "threaded mode". De socket-communicatie wordt dan afgehandeld
// via een Sender- en een Receiver klasse, die worden aangemaakt in de Connector klasse. Deze threaded mode 
// biedt een generiekere en ook robuustere manier van communicatie, maar is ook moeilijker te begrijpen. 
// Aanbeveling: start in ieder geval met de simple-mode
//
// Werking: De communicatie met de (Arduino) server is gebaseerd op een socket-interface. Het IP- en Port-nummer
// is instelbaar. Na verbinding kunnen, middels een eenvoudig commando-protocol, opdrachten gegeven worden aan 
// de server (bijv. pin aan/uit). Indien de server om een response wordt gevraagd (bijv. led-status of een
// sensorwaarde), wordt deze in een 4-bytes ASCII-buffer ontvangen, en op het scherm geplaatst. Alle commando's naar 
// de server zijn gecodeerd met 1 char.
//
// Aanbeveling: Bestudeer het protocol in samenhang met de code van de Arduino server.
// Het default IP- en Port-nummer (zoals dat in het GUI verschijnt) kan aangepast worden in de file "Strings.xml". De
// ingestelde waarde is gebaseerd op je eigen netwerkomgeving, hier, en in de Arduino-code, is dat een router, die via DHCP
// in het segment 192.168.1.x IP-adressen uitgeeft.
// 
// Resource files:
//   Main.axml (voor het grafisch design, in de map Resources->layout)
//   Strings.xml (voor alle statische strings in het interface (ook het default IP-adres), in de map Resources->values)
// 
// De software is verder gedocumenteerd in de code. Tijdens de colleges wordt er nadere uitleg over gegeven.
// 
// Versie 1.2, 16/12/2016
// S. Oosterhaven
// W. Dalof (voor de basis van het Threaded interface)
//
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
    [Activity(Label = "@string/application_name", MainLauncher = true, Icon = "@drawable/icon", ScreenOrientation = Android.Content.PM.ScreenOrientation.Portrait)]

    public class MainActivity : Activity
    {
        Server server = new Server();
        // Variables (components/controls)
        // Controls on GUI
        ToggleButton AIbutton, lightsToggle;
        Button ForwardButton, LeftButton, RightButton, DownButton, UpdateButton;
        TextView MIPStatusText, HumidityValueText, LDRValueText, MIPSpeedValueText, TemperatureValueText;
        Spinner eyesSpinner, musicSpinner;
        SeekBar MipSpeedSlider;

        Timer EnergySaveTimer;

        bool EnergySparing = false;
        int EnergySparingTime = 0;
        int EnergySparingCooldown = 300;

        private List<KeyValuePair<string, int>> moods;
        private List<KeyValuePair<string, int>> sounds;

        protected override void OnCreate(Bundle bundle)
        {
            base.OnCreate(bundle);

            moods = new List<KeyValuePair<string, int>>
            {
                new KeyValuePair<string, int>("Happy", 3),
                new KeyValuePair<string, int>("Sad", 4),
                new KeyValuePair<string, int>("Angry", 5),
                new KeyValuePair<string, int>("Disgusted", 6),
                new KeyValuePair<string, int>("Scared", 7)
            };

            sounds = new List<KeyValuePair<string, int>>
            {
                new KeyValuePair<string, int>("Empty", 0),
                new KeyValuePair<string, int>("Mario", 1),
                new KeyValuePair<string, int>("Underworld", 2)
            };

            // Set our view from the "main" layout resource (strings are loaded from Recources -> values -> Strings.xml)
            SetContentView(Resource.Layout.ConnectedApp);
            
            // find and set the controls, so it can be used in the code
            eyesSpinner = FindViewById<Spinner>(Resource.Id.EyesDropdown);
            musicSpinner = FindViewById<Spinner>(Resource.Id.MusicDropdown);
            HumidityValueText = FindViewById<TextView>(Resource.Id.HumidityValue);
            LDRValueText = FindViewById<TextView>(Resource.Id.LDRDataValue);
            MIPStatusText = FindViewById<TextView>(Resource.Id.MIPStatusValue);
            TemperatureValueText = FindViewById<TextView>(Resource.Id.TempValue);
            lightsToggle = FindViewById<ToggleButton>(Resource.Id.LightsToggle);
            AIbutton = FindViewById<ToggleButton>(Resource.Id.AIToggle);
            MIPSpeedValueText = FindViewById<TextView>(Resource.Id.MipSpeedValue);
            MipSpeedSlider = FindViewById<SeekBar>(Resource.Id.MipSpeedSlider);
            ForwardButton = FindViewById<Button>(Resource.Id.moveUpButton);
            LeftButton = FindViewById<Button>(Resource.Id.moveLeftButton);
            RightButton = FindViewById<Button>(Resource.Id.moveRightButton);
            DownButton = FindViewById<Button>(Resource.Id.moveDownButton);
            UpdateButton = FindViewById<Button>(Resource.Id.UpdateButton);

            if(MipSpeedSlider != null)
            {
                MipSpeedSlider.ProgressChanged += (o, s) => MIPSpeedValueChanged(o, s);
            }
            if (ForwardButton != null)
            {
                ForwardButton.Click += (o, e) => MovementButton(o, e, 11);
            }
            if (DownButton != null)
            {
                DownButton.Click += (o, e) => MovementButton(o, e, 12);
            }
            if (LeftButton != null)
            {
                LeftButton.Click += (o, e) => MovementButton(o, e, 9);
            }
            if (RightButton != null)
            {
                RightButton.Click += (o, e) => MovementButton(o, e, 10);
            }
            if (AIbutton != null)
            {
                AIbutton.Click += (o, e) => OnAIToggle(o, e);
            }
            if(lightsToggle != null)
            {
                lightsToggle.Click += (o, e) => OnLightsToggle(o, e);
            }
            if(UpdateButton != null)
            {
                UpdateButton.Click += (o,e) => UpdateValues();
            }

            if (eyesSpinner != null)
            {
                List<string> moodNames = new List<string>();
                foreach (var item in moods)
                    moodNames.Add(item.Key);

                var adapter = new ArrayAdapter<string>(this, Android.Resource.Layout.SimpleSpinnerItem, moodNames);
                adapter.SetDropDownViewResource(Android.Resource.Layout.SimpleSpinnerDropDownItem);
                eyesSpinner.Adapter = adapter;

                eyesSpinner.ItemSelected += (sender, args) => Mood_ItemSelected(sender, args);

            }
            if (musicSpinner != null)
            {
                List<string> soundNames = new List<string>();
                foreach (var item in sounds)
                    soundNames.Add(item.Key);

                var adapter = new ArrayAdapter<string>(this, Android.Resource.Layout.SimpleSpinnerItem, soundNames);
                adapter.SetDropDownViewResource(Android.Resource.Layout.SimpleSpinnerDropDownItem);
                musicSpinner.Adapter = adapter;

                musicSpinner.ItemSelected += (sender, args) => Music_ItemSelected(sender, args);
            }

            EnergySaveTimer = new Timer(EnergySparingCooldown);
            EnergySaveTimer.Interval = 100;
            EnergySaveTimer.Elapsed += new ElapsedEventHandler(OnTimeEvent);
            EnergySaveTimer.Enabled = true;
            EnergySaveTimer.Start();

            // uncomment to use with server
            server.Connect(this); 
        }

        private void OnTimeEvent(object source, ElapsedEventArgs e)
        {
            RunOnUiThread(delegate
            {
                if (!EnergySparing) return;
                EnergySparingTime++;
                if (EnergySparingTime >= EnergySparingCooldown)
                {
                    EnergySaveTimer.Enabled = false;

                    Toast.MakeText(this, "Energy Saving Mode Enabled!", ToastLength.Long).Show();
                    SendStringToArduino("6", MIPStatusText);
                }                
            });
        }


        private void MIPSpeedValueChanged(object sender, EventArgs arg)
        {
            SeekBar slider = (SeekBar)sender;
            if (slider != null)
            {
                int value = slider.Progress;
                MIPSpeedValueText.Text = "MIP Speed = " + value;
                string sendText = "0" + value;
                SendStringToArduino(sendText, MIPStatusText);
                Toast.MakeText(this, sendText, ToastLength.Long).Show();
            }
        }

        private void MovementButton(object sender, EventArgs args, int val)
        {
            Button toggle = (Button)sender;
            if (toggle != null)
            {
                string sendText = "1" + val;
                SendStringToArduino(sendText, MIPStatusText);
                Toast.MakeText(this, sendText, ToastLength.Long).Show();
            }
        }

        private void Mood_ItemSelected(object sender, AdapterView.ItemSelectedEventArgs e)
        {
            Spinner spinner = (Spinner)sender;
            string name = spinner.GetItemAtPosition(e.Position).ToString();

            string toast = string.Format("Mood  = {0}; Value = {1}",
                spinner.GetItemAtPosition(e.Position), moods[e.Position].Value);
            Toast.MakeText(this, toast, ToastLength.Long).Show();

            string sendText = "2" + moods[e.Position].Value;
            SendStringToArduino(sendText, MIPStatusText);
            MIPStatusText.Text = spinner.GetItemAtPosition(e.Position).ToString();
        }

        private void Music_ItemSelected(object sender, AdapterView.ItemSelectedEventArgs e)
        {
            Spinner spinner = (Spinner)sender;
            string name = spinner.GetItemAtPosition(e.Position).ToString();

            string toast = string.Format("Music  = {0}; Value = {1}",
                spinner.GetItemAtPosition(e.Position), sounds[e.Position].Value);
            Toast.MakeText(this, toast, ToastLength.Long).Show();

            string sendText = "3" + sounds[e.Position].Value;
            SendStringToArduino(sendText, MIPStatusText);
        }

        public void DisplayHumidityValue(string value)
        {
            HumidityValueText.Text = value;
        }

        public void DisplayLDRValue(string value)
        {
            LDRValueText.Text = value;
        }
        public void DisplayTemperatureValue(string value)
        {
            LDRValueText.Text = value;
        }

        private void OnLightsToggle(object sender,  EventArgs args)
        {
            ToggleButton toggle = (ToggleButton)sender;
            if(toggle != null)
            {
                string sendText = "4" + Convert.ToInt32(toggle.Checked);

                SendStringToArduino(sendText, MIPStatusText);
                Toast.MakeText(this, sendText, ToastLength.Long).Show();
            }
        }

        private void OnAIToggle(object sender, EventArgs args)
        {
            ToggleButton toggle = (ToggleButton)sender;
            if(toggle != null)
            {
                string sendText = "5" + Convert.ToInt32(toggle.Checked);
                EnergySparing = !toggle.Checked;

                SendStringToArduino(sendText, MIPStatusText);
                Toast.MakeText(this, sendText, ToastLength.Long).Show();
            }
        }

        public void SendStringToArduino(string cmd, TextView text)
        {
            // uncomment to use with server
            if (!server.Send(cmd)) server.Connect(this);

            if (cmd == "6")
            {
                text.Text = "Sleeping";
                return;
            }
        }

        public void UpdateValues()
        {
            if (!server.Send("7")) server.Connect(this);

            string data = server.Receive();
            if (data == "-")
            {
                Toast.MakeText(this, "No Info Received", ToastLength.Short);
            }

            int ldrPos = data.IndexOf("LDR");
            int humidityPos = data.IndexOf("Humidity");
            int temperaturePos = data.IndexOf("Temperature");
            if (ldrPos>0)
            {
                ldrPos += 3;
                LDRValueText.Text = data.Substring(ldrPos, data.IndexOf("|") - ldrPos);
            }
            if (humidityPos>0)
            {
                humidityPos += 8;
                HumidityValueText.Text = data.Substring(humidityPos, data.IndexOf("}") - humidityPos);
            }
            if (temperaturePos>0)
            {
                temperaturePos += 11;
                TemperatureValueText.Text = data.Substring(temperaturePos, data.IndexOf(";") - temperaturePos);
            }
        }

        //Close the connection (stop the threads) if the application stops.
        protected override void OnStop()
        {
            base.OnStop();

            server.Stop();
        }

        //Close the connection (stop the threads) if the application is destroyed.
        protected override void OnDestroy()
        {
            base.OnDestroy();

            server.Stop();
        }

        //Prepare the Screen's standard options menu to be displayed.
        public override bool OnPrepareOptionsMenu(IMenu menu)
        {
            //Prevent menu items from being duplicated.
            menu.Clear();

            MenuInflater.Inflate(Resource.Menu.menu, menu);
            return base.OnPrepareOptionsMenu(menu);
        }
    }
}
