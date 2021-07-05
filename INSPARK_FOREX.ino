
/*
******************************************  INSPARK ***** PROJECT  ********************************************

              Project Title : Forex Rate Tracker
              
              Contributed by : Avi Singh(1NT18IS037) and Deepak Kumar Gupta(1NT18IS050)
              
******************************************                          *******************************************             
*/
// Loading in the libraries that are used.
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <BlynkSimpleEsp8266.h>//header file for using Blynk app sevices...

ESP8266WiFiMulti wifiMulti;

#define BLYNK_PRINT Serial

#define green D1
#define yellow D2
#define red D3

char auth[] = "4pyZKkHXVbLY9TNJA1D3zoEOCwaHoyCQ";// confidential and unique autentication key for connecting to Blynk app
char ssid[] = "DdeepakG";
char pass[] = "nitinG1105";

double myForexRate=0.0;
double preForexRate=0.0;
String myDataStr="0 (just started)";
String preDataStr="0";

BlynkTimer timer;

void myTimerEvent() //This is function which is called in every 5 seconds..., which sends data to Blynk app
{
  // You can send any value at any time.
  //Sending previous and current Forex Rates to Blynk App using virtual pin V1 and V5
  Blynk.virtualWrite(V1,preDataStr);
  Blynk.virtualWrite(V5, myDataStr);
}

// This is the start of the setup loop. Things that are within the setup loop run just once
void setup() 
{
  Serial.begin(9600);
  delay(500);    // wait half a second
  
  Serial.print("Connecting\t.....");
  delay(500);
  // disconnect any current WiFi connection... for good measure!
  WiFi.disconnect();
  // Connect/add any potential WiFi SSID's and passwords that your project may encounter
  wifiMulti.addAP("DdeepakG", "nitinG1105");
 // wifiMulti.addAP("SecondWifi", "SecondPassword");
 // wifiMulti.addAP("Nth_WIFI", "Nth_PASSWORD");

  // initializing ech of the digital pins as outputs...
  pinMode(green, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(red, OUTPUT);
  digitalWrite(green, 0);
  digitalWrite(yellow, 0);
  digitalWrite(red, 0); 

  Blynk.begin(auth, ssid, pass);
   // Setup a function to be called every 5second
  timer.setInterval(5000L, myTimerEvent);
  
// a loop that will print dots every half a second while the WiFi is NOT connected (WL_CONNECTED is likeeee an attribute of the Wifi library kinda)
  while(wifiMulti.run() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".....\t");
  }
  // the end of the loop. This will be reached when WL_CONNECTED becomes true, i.e. when the board has established a connection with the internet
  
  // Print out the Wifi name that was connected to, and give u a little validation
  Serial.print("\n WiFi SSID name:");
  Serial.println (WiFi.SSID());
  Serial.println("Successfully connected!!!");
  //Showing that wifi is connected by blinking leds 8 times...
  for(int i=0;i<8;i++)
  {
      digitalWrite(green, 1);
      digitalWrite(yellow, 0);
      digitalWrite(red, 0);
      delay(150);
      digitalWrite(green, 0);
      digitalWrite(yellow, 1);
      digitalWrite(red, 0);
      delay(150);
      digitalWrite(green, 0);
      digitalWrite(yellow, 0);
      digitalWrite(red, 1);
      delay(150);    
  }
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Netmask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());
  
  digitalWrite(green, 0);
  digitalWrite(yellow, 0);
  digitalWrite(red, 0); 
}
// This is the end of the setup loop


// This is the loop that DOES the stuff. It uses a function that is defined outside of this loop, at the very bottom of the code, for displaying on the Blynk app
// This loop goes forever and ever and ever and.....
void loop() 
{
    Blynk.run();
    timer.run(); // Initiates BlynkTimer
    
    // shortening HTTPClient to http...
    HTTPClient http;
    // This is my thingspeak API... U gotta put YOUR proper stuff in here, but the only thing that'll change is the 16 digit api key
    http.begin("api.thingspeak.com", 80, "/apps/thinghttp/send_request?api_key=SHGYCO73D3VSDMJS"); //HTTP  SHGYCO73D3VSDMJS
    // Do a GET (request)!
    int httpCode = http.GET();
    
    // httpCode will be negative if there's a problem... it'll be displayed (see the "else" statement below) if there's an error
    if(httpCode > 0) 
    {
      // If a file was found at the server (i.e. it's all g)...
      if(httpCode == HTTP_CODE_OK) 
      {      
      // THIS "myDataStr" is the string from the thinghttp URL, IT IS THE VARIABLE (As a string, i.e. text)
         preDataStr=myDataStr;
         myDataStr = http.getString();
         Serial.print("current : ");
         Serial.println(myDataStr);
         Serial.print("Previous : ");
         Serial.println(preDataStr); 
        //convert this string (text) to an float(floating point number)....    
         preForexRate = myForexRate; ///assigning last value to preForexRate to compare later.
         myForexRate = myDataStr.toFloat(); 
                 
      //..............CHECKING....FOR......DROP..........
         if(myForexRate < preForexRate) 
         {
            digitalWrite(green,0);
            digitalWrite(yellow,0);
            for(int x=0;x<12;x++)
            {
              digitalWrite(red,1);
              delay(200);  
              digitalWrite(red,0);
              delay(200);             
            }
            Serial.println("D.....R.....O.....P........");
         }
       //..............CHECKING....FOR.....RISE.........
         else if(myForexRate > preForexRate)
         {
            digitalWrite(yellow,0);
            digitalWrite(red,0);
            for(int x=0;x<12;x++)
            {
              digitalWrite(green,1);
              delay(200);
              digitalWrite(green,0);
              delay(200);             
            }
            Serial.println("...............R..I..S..E..");
         }
         //.........NO CHANGE..........
         else
         {
            digitalWrite(green,0);
            digitalWrite(red,0);
            for(int x=0;x<5;x++)
            {
              digitalWrite(yellow,1);
              delay(200);   
              digitalWrite(yellow,0);
              delay(200);          
            }
            Serial.println("........N-O - -C-H-A-N-G-E........");
         }

        delay(2000);

        digitalWrite(green,0);
        digitalWrite(yellow,0);
        digitalWrite(red,0);
       }
    }
  //............if there is a problem in retrieving data from cloud..........
    else
    {
     // Seeee, this is the "else" statement, which lets you know if the HTTP get request failed, and prints out the error code
       Serial.printf("[HTTP] GET... failed, error: %s", http.errorToString(httpCode).c_str());
       
    }
    //closing connection to reconnect again...
    http.end();
    // Wait 1 seconds and repeat this loop over again!
    delay(1000);
}
// THIS is the end of the "forever doing stuff loop... it will repeat forever and ever as long as there's power and there aren't any errors
