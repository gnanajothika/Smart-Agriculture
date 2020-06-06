#include <DHT.h>   // Including library for dht
#include <ESP8266WiFi.h>    // esp library
 

String apiKey = "YOUR_API_KEY";    //  Enter your Write API key from ThingSpeak
const char *ssid = "YOUR_WIFI_NAME";    // replace with your wifi ssid and wpa2 key
const char *pass = "YOUR_WIFI_PASSWORD";
const char *server = "api.thingspeak.com";

int sensor_pin = A0;      //pin A0 to  moisture sensor
int output_value ;

#define DHTPIN D2          //pin where the dht11 is connected
DHT dht(DHTPIN, DHT11);

#define RELAY D1           //water pump relay

WiFiClient client;
 
void setup()
{
       Serial.begin(9600);
       delay(10);
       
       dht.begin();       // start DHT11 library

       pinMode(RELAY, OUTPUT);
       
       Serial.println("");
       Serial.println("Connecting to ");
       Serial.println(ssid);
       WiFi.begin(ssid, pass);
 
      while (WiFi.status() != WL_CONNECTED)
     {
            delay(500);
            Serial.print("...");
     }
      Serial.println("");
      Serial.println("WiFi connected");
     
      Serial.println("Reading From the Sensor ...");
      Serial.println("Calibrating Sensor Data ...");
      Serial.println("Wait for few sec ...");
     
        int i=0;
        for(i=0; i<=30; i++)
        {
        String op = Serial.readStringUntil(':');
        String ku= Serial.readStringUntil(',');
        String hi= Serial.readStringUntil('W');
        Serial.println(i +".");
        }                                         /////////for rejecting unwanted data
      Serial.println("Calibration Done ...");
 
}
 
void loop()
{
     
        //PH level//////////////////////////////////////////////////////////////////////////////////////        
        float PH=0.00;
        String x = Serial.readStringUntil(':');
        String ch= Serial.readStringUntil(',');
        String y= Serial.readStringUntil('W');
        float phlevel = ch.toFloat();
        if(phlevel <=10.00)
        {
          PH = phlevel;
          //Serial.print("PH Level : ");
          //Serial.println(PH);
          delay(500);
        }/////////////////////////////////////////////////////////////////////////////////////////

 
      float aH = dht.readHumidity();
      float aT = dht.readTemperature();

     output_value= (100 - ((analogRead(sensor_pin)/1023)*100));
     //output_value = map(output_value,550,0,0,100);
     output_value = output_value;
     float sM = output_value;
     if (sM < 0)
     {
      sM = 0;
      }
      else
      {
       sM = output_value;
      }


      int wM = 0;
      if(sM <=90)
      {
        digitalWrite( RELAY,HIGH);
        wM=1;
      }
      if( sM >= 90)
      {
        digitalWrite( RELAY,LOW);
        wM=0;
      }

     
           
                  if (client.connect(server,80))   //   api.thingspeak.com
                      {  
                           
                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(aT);
                             postStr +="&field2=";
                             postStr += String(aH);
                             postStr +="&field3=";
                             postStr += String(sM);
                             postStr +="&field4=";
                             postStr += String(PH);
                             postStr +="&field5=";
                             postStr += String(wM);
                             postStr += "\r\n\r\n\r\n";
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n\n");
                             client.print(postStr);
 
                             Serial.print("AirTemperature: ");
                             Serial.print(aT);
                             Serial.print("Â°C   ");
                             Serial.print("AirHumidity: ");
                             Serial.print(aH);
                             Serial.print("%    ");  
                             Serial.print("SoilMositure : ");
                             Serial.print(sM);
                             Serial.print("%    ");
                             Serial.print("PH Level : ");
                             Serial.print(PH);
                             Serial.print("ph   ");
                             Serial.print("WaterPump : ");
                             if(wM==1)
                             {
                              Serial.print("ON");
                             }
                             else
                             Serial.print("OFF");
           
                             
                        }
          client.stop();
 
          Serial.println(" ");
  // thingspeak needs minimum 15 sec delay between updates, i've set it to 30 seconds
  delay(10000);
}