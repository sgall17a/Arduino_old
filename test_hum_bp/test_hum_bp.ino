

#include <Adafruit_HTU21DF.h>
#include <ESP8266WiFi.h>
//#include <DHT.h>
#include <pgmspace.h>
#include <Wire.h>
#include <BMP085.h>
#include <Streaming.h>

/*
   Connections
   ===========
   Connect SCL to analog 5
   Connect SDA to analog 4
   Connect VDD to 3.3V DC
   Connect GROUND to common ground
*/

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
//const char* PROGMEM  ssid       = "iiFD43primary";
const char* PROGMEM  ssid       = "stephen2";
const char* PROGMEM  password   = "muirbarkly";
const char* PROGMEM  pumpon     = "pumpon";
const char* PROGMEM  pumpoff    = "pumpoff";

unsigned long tankon = 0;
unsigned long lastmillis = 0;
int32_t temperature = 0;
int32_t barometric =  0;
float humidity = 0;
float tank = 0;

Adafruit_HTU21DF htu = Adafruit_HTU21DF();
//WiFiClient client;
BMP085 dps = BMP085();      // Digital Pressure Sensor 
/*
String do_action(const char* action, const char* apiKey, String data ){
	
	
    if (client.connect("api.thingspeak.com",80)) {  
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
    Serial << s;
   // client.stop();
    return s;
}


void log(String s) {
  String x = "?key=";
	x += logkey;
	x += "&field1=";
	x += s;
  
  do_action("POST",writeurl,x);
}

String executeCommand(){
    String data = String("?api_key=") + commandkey;  
    return do_action("POST", commandurl, data);
}

*/
void doSensors(){
    dps.getTemperature(&temperature); 
    dps.getPressure(&barometric);
    humidity =  htu.readHumidity();
    tank = 100;
    
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    
    String x = "key";
      x += sensorkey;
      x += "&field1=";
      x += humidity;
      x += "&field2=";
      x += barometric;
      x += "&field3=";
      x += temperature;
      
     Serial << "Hum= " << humidity << "  BP= " << barometric << " Temp= " << temperature << "\n";
     Serial << "Data " << x << "\n";
      
  // do_action("POST", writeurl,x);
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
	   // tankon = millis();

            Wire.begin(4,5);
            Serial << "Wire started \n";

            if (!htu.begin()) {
              Serial << "Couldn't find humidity sensor!\n";
           }
            delay(1000);
            dps.init();
            Serial << "BMP mode " << dps.getMode() << "\n";
	    lastmillis = millis();
	    temperature = 0;
	    barometric   = 0;
	    humidity  = 0;
            delay(1);
}
	  
	     
void loop(){

 // if ((millis() - tankon) > FORCEOFF){
 //       digitalWrite(TANK,false);
 //       log("emergency_Tank off");
 //       tankon = millis();
 //   }
  delay(1);
  if ((millis() - lastmillis) > DATALOOP){
        lastmillis = millis();
        Serial << "\nDoing  loop\n";
        doSensors();
        
       // String command = executeCommand();
       // bool flag = (command == String(pumpon));
       // digitalWrite(TANK,flag);
       // if (flag) tankon = millis();
		
     }
}
