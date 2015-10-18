#include <Adafruit_HTU21DF.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <pgmspace.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <Streaming.h>

#define TANK 3
#define DHTPIN 2
#define SDA 4
#define SCL 5
#define FORCEOFF  20000
#define DATALOOP 10000


const char* PROGMEM  sensorkey  = "DL8GPP8A1A6DBKTC";
const char* PROGMEM  commandkey = "GXB2F76D1YTB0XYG";
const char* PROGMEM  logkey     = "MRPLOUPBR7HYRP04";
const char* PROGMEM  writeurl   = "http://api.thingspeak.com/update";
const char* PROGMEM  commandurl = "http://api.thingspeak.com/talkbacks/2868/commands/execute";
//const char* PROGMEM  ssid     = "iiFD43primary";
const char* PROGMEM  ssid       = "stephen2";
const char* PROGMEM  password   = "muirbarkly";
const char* PROGMEM  pumpon     = "pumpon";
const char* PROGMEM  pumpoff    = "pumpoff";

unsigned long tankon = 0;
unsigned long lastmillis = 0;
float hum = 0;
float tank = 0;
int sda = 4;
int scl =5;


Adafruit_BMP085 bmp;
Adafruit_HTU21DF htu = Adafruit_HTU21DF();
WiFiClient client;

/*String sendData(float temperature, float pressure, float humidity) 
  {
  String url = "http://api.thingspeak.com/update?key=DL8GPP8A1A6DBKTC";
  url += "&field1=";
  url += temperature;
  url += "&field2=";
  url +=  pressure;
  url += "field3=";
  url += humidity;
  
  client.connect(url.c_str(),80); 
}
*/

void post(String url = "api.thingspeak.com", String data="")
{
  int failedCounter;
  yield();
  if (client.connect(url.c_str(), 80))
  {         
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: "+ url + "\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: DL8GPP8A1A6DBKTC\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(data.length());
    client.print("\n\n");
      
    client.print(data);
    
  //  lastConnectionTime = millis();
    
    if (client.connected())
    {
      Serial.println("Connecting to ThingSpeak...");
      Serial.println();
      
      failedCounter = 0;
      yield();
    }
    else
    {
      failedCounter++;
  
      Serial.println("Not connected to ThingSpeak failed ("+String(failedCounter, DEC)+")");   
      Serial.println();
    }
    
  }
  else
  {
    failedCounter++;
    
    Serial.println("Connection to ThingSpeak Failed ("+String(failedCounter, DEC)+")");   
    Serial.println();
    yield();
    
    //lastConnectionTime = millis(); 
  }
}

/*
String do_action(const char* action, const char* apiKey, String data ){
	
	
    if (client.connect("api.thingspeak.com",80)) { 
    Serial << "Connected\n";
    Serial << action << " " << apiKey << " "  << data;
    yield(); 
    String postStr = apiKey;
    postStr += data;
    postStr += "\r\n\r\n";
		
    //client.print (action);
    client << action <<" /update HTTP/1.1\n"; 
    client << "Host: api.thingspeak.com\n"; 
    client << "Connection: close\n"; 
    client << "X-THINGSPEAKAPIKEY: " << apiKey << "\n";
    client << "Content-Type: application/x-www-form-urlencoded\n"; 
    client << "Content-Length: " << postStr.length() << "\n\n"; 
    client << postStr;
    
    }

    String s;
    if (client.available() > 0) {
      // read the bytes incoming from the client:
      s += client.read();
    }
    Serial << s << "\n";
    client.stop();
    Serial << "done connect\n";
    return s;
}

void log(String s) {
  String x = "?key=";
	x += logkey;
	x += "&field1=";
	x += s;
  
  do_action("POST",writeurl,x);
}
*/
String executeCommand(){
  //POST https://api.thingspeak.com/talkbacks/2868/commands/execute
    String api_key="GXB2F76D1YTB0XYG";
    String data = String("?api_key=") + commandkey;  
    return post("http://api.thingspeak.com", data);
}



void doSensors(){
    float temperature = bmp.readTemperature(); 
    yield();
    float pressure = bmp.readPressure();
    yield();
    hum =  htu.readHumidity();
    yield();
    tank = 100;
    Serial << temperature << " " << pressure << "  " << hum << "\n";
    yield();
    if (isnan(hum) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    
    String x = "&field1=";
      x += hum;
      x += "&field2=";
      x += pressure;
      x += "&field3=";
      x += temperature;
    yield();  
    updateThingSpeak(x);
   //do_action("POST", writeurl,x);
}   
	  
	  
void setup(){
	    Serial.begin(115200);
	    delay(10);
	    WiFi.begin(ssid, password);

	    Serial << "\n\nConnecting to " << ssid << "\n";
   
	    WiFi.begin(ssid, password);
   
	    while (WiFi.status() != WL_CONNECTED) {
	      delay(500);
	      Serial.print(".");
	    }
		
	    Serial.println("\nWiFi connected");
	    tankon = millis();
            yield();
            Wire.begin(sda,scl);
            delay(100);
            
            if (!htu.begin()) 
              Serial.println("Couldn't find sensor!");
            else
              Serial << "Got humidity\n";
              
            delay(100);
            
            if (!bmp.begin())
                Serial << "Couldnt open BMP\n"; 
            else
                Serial << "BMP open\n";
            yield();
            lastmillis = millis();
	    hum  = 0;
        delay(100);
}
	  
	     
void loop(){

  if ((millis() - tankon) > FORCEOFF){
        digitalWrite(TANK,false);
     //   log("emergency_Tank off");
        tankon = millis();
    }

  if ((millis() - lastmillis) > DATALOOP){
        lastmillis = millis();
        
        doSensors();
        
      //  String command = executeCommand();
     //   bool flag = (command == String(pumpon));
        bool flag = false;
        digitalWrite(TANK,flag);
        
	if (flag) tankon = millis();
		
     }
}
