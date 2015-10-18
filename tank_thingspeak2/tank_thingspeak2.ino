
#include <ESP8266WiFi.h>
#include <pgmspace.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_HTU21DF.h>
#include <Streaming.h>

#define TANK 13
#define SDA 4
#define SCL 5
#define FORCEOFF  15 * 1000 
#define DATALOOP  10 * 1000

const char* PROGMEM  sensorkey  = "DL8GPP8A1A6DBKTC"; 
const char* PROGMEM  commandkey = "GXB2F76D1YTB0XYG";
const char* PROGMEM  logkey     = "MRPLOUPBR7HYRP04";
const char* PROGMEM  commandurl = "/talkbacks/2868/commands/execute?api_key=GXB2F76D1YTB0XYG";

//const char* PROGMEM  ssid       = "stephen2";
//const char* PROGMEM  password   = "muirbarkly";
const char* ssid                = "ii33FD43primary";
const char* password            = "5df5be4e";
const char* host                = "api.thingspeak.com"; 
unsigned long tankon            = 0;
unsigned long lastmillis        = 0;
unsigned long reconnects        = 0;

Adafruit_BMP085 bmp;
Adafruit_HTU21DF htu; // = Adafruit_HTU21DF();
WiFiClient client;

void setup(){
    delay(1000);  //This seems to stop a lot of resetting
    Serial.begin(115200);
    delay(100);
    Wire.begin(SDA,SCL);
    delay(100);
    
    Serial << "\n\nHumidity sensor";
    if (htu.begin()) Serial << " OK\n";
    
    Serial << "Barometric sensor ";
    if (bmp.begin()) Serial << "OK\n";

    lastmillis = millis();         
    tankon = millis();
    delay(1000);
      
    pinMode(TANK, OUTPUT);
    digitalWrite(TANK,LOW);  
      
   Serial << "First logon\n";   
   WiFi.begin(ssid, password);
   delay(10000);
   logon();
}

bool logon() {
  
   int tries = 0;   
   while(WiFi.status() != WL_CONNECTED) {
     Serial << "*";
     delay(500); 
     tries++;
     if (tries >100) break;
   }

   if ( WiFi.status() == WL_CONNECTED)  return true;
  	
   WiFi.disconnect();
   WiFi.begin(ssid, password);
   Serial << "Network disconnected\n";
}

void doTank(bool flag){
  yield();
  Serial << "\nTank " << flag << "\n";
  if (digitalRead(TANK)  != flag)
      digitalWrite(TANK, flag);
  tankon = millis();
  yield();
}


void executeCommand(){
      String command;
      if (!client.connect(host, 80)) {
           Serial << "Command connection failed\n";
           return; 
           }
      yield();
  
      client.print(String("GET ") + commandurl + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
               
      delay(100);
      
      String line;
      bool pumpon = false;
      while(client.available()){
        line = client.readStringUntil('\r');
        yield();
        //Dont print headers
      //  if (line.indexOf(":") == -1) 
     //   Serial << line << "\n";
        
        if (line.indexOf("pumpon") != -1) 
            doTank(true);
        if (line.indexOf("pumpoff") != -1) 
             doTank(false);
              
        line= "";
      }
      yield();
      client.stop();
}

void doSensors(){
    Serial << "\nSensors ";
    float temperature = bmp.readTemperature(); 
      yield();
    float pressure = bmp.readPressure()/100; 
      yield();
    float hum =  htu.readHumidity();  
      yield();
    float tank = 100; 
      yield();
    
    Serial << temperature << " " << pressure << "  " << hum ;
    yield();
      
    if (isnan(hum) || isnan(temperature)) {
          Serial << " Failed \n";
          return;
        }
     // make the datastring for posting to Thinkspeak
    String data = "&field1=";
      data += hum;
      data += "&field2=";
      data += pressure;
      data += "&field3=";
      data += temperature;
     
    Serial << "  -OK\n";
    yield();
    
    if (client.connect(host, 80))
    {    
      yield();
        client << "POST /update HTTP/1.1\n";
        client.print(String("Host: ")+ host + "\n");
        client.print("Connection: close\n");
        client.print(String("X-THINGSPEAKAPIKEY:") + sensorkey + "\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(data.length());
        client.print("\n\n");
        client.print(data);
      yield();
      Serial << " posted OK\n";
  }
  else 
    Serial << " Posting failed \n"; 
    
  yield();
}   
	     
void loop(){
  if (!logon()) {
      Serial << "*";
      return;
  }
  
  if ((millis() - tankon) > FORCEOFF)
        doTank(false);
        
  yield();
  
  if ((millis() - lastmillis) > DATALOOP){
        lastmillis = millis();
        doSensors();
        executeCommand();
     }
  delay(1000);
  Serial << "^";  
}
