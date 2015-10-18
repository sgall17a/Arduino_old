#include "DHT.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include "RTClib.h"
#include <Streaming.h>
#include <SD.h>


#define DHTPIN 5      // HDHT2 humidity connection 
#define DHTTYPE DHT22 // DHT 22  (AM2302) for normal 16mhz Arduino
#define CS 10         // Chip select for SD card
#define LED 13        // Flashing LED
#define SOLAR A0      // Solar panel voltage
#define BATTERY A1    // Battery voltage
#define MOISTURE A2
#define PUMP1 3
#define PUMP2 4
#define BAUDRATE 57600

typedef int (*pfunc)(int, bool);

class Ticker {
private:
	 long next;
	 int freq;
         int pin;
	 pfunc CB; 
 public:
	// void bool flag() CB;
	 void tick(long t)
	 {
		 if (t > next) {
			CB(pin,true);
			 next += freq;		 
	           }
          else CB(pin,false);
   }

Ticker( int pin,long nxt,int f, pfunc tCB);
};

Ticker::Ticker(int pin, long nxt, int f, pfunc tCB ){
 this->next = next -f;
 this->freq = f;
 this->CB = tCB;
 this->pin = pin;

 digitalWrite(pin,LOW);
 if (pin){
   pinMode(pin,OUTPUT);
   digitalWrite(pin,HIGH);
 }
}

int pump(int pin, bool f){
    static bool oldf;
   if (f != oldf) {
	digitalWrite(pin,f);
        oldf = f;
   }
	return 0;
}

int measure(int pin, bool f){
 
    // read sensors
    sensors_event_t event;
    float temperature;
    float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA; 
    //bmp (barometric), rtc (clock), dht are inialised global objects
    float humidity = dht.readHumidity();
    bmp.getEvent(&event);
    bmp.getTemperature(&temperature);
    float pressure = event.pressure;
    //if (!event.pressure) log("Sensor error");
    int  battery   = digitalRead(BATTERY);
    int  solar     = digitalRead(SOLAR);
    int  moisture  = digitalRead(MOISTURE);
    
    //log to file and serial (set to false to not log)
    File dataFile = SD.open("datalog.txt", FILE_WRITE);
    if (dataFile && true) {
      dataFile << nw.year()  << '-' << nw.month()  << '-' << nw.day()    << ' ';
      dataFile << nw.hour()  << ':' << nw.minute() << ':' << nw.second() << ',';
      dataFile << pressure   << ',' << humidity    << ',' << temperature << ',';
      dataFile << solar      << ',' << battery     << endl;    
      dataFile.close();
      // print the serial port too:
    }  else  {
        Serial << "error opening datalog.txt" << endl; }
       
    //log to serial  (set to false to not log)    
    if (true) {
      Serial << nw.year()  << '-' << nw.month()  << '-' << nw.day()    << ' ';
      Serial << nw.hour()  << ':' << nw.minute() << ':' << nw.second() << ',';
      Serial << pressure    << ',';
      Serial << humidity    << ',';
      Serial << temperature << ',';
      Serial << solar       << ',';
      Serial << battery     << ',';
      Serial << moisture    << endl;
    }
}
Ticker Pump1(4, 1000,10,pump);
Ticker Pump2(5, 1000,10,pump);
Ticker Measure(0,1000,600,measure);
long t;

void setup(){

  pinMode(BATTERY,INPUT);
  pinMode(SOLAR,  INPUT);
  pinMode(MOISTURE,INPUT);
  
  Serial.begin(BAUDRATE); 
  Serial << "Starting" << endl;
  
  Wire.begin();
  dht.begin();
  
  
  rtc.begin();
  if (! rtc.isrunning())  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  //This is to start this system
  DateTime first_time (FIRSTTIME);
  t = first_time.unixtime();
   
  if(!bmp.begin()) Serial << "Problem with BMO085" << endl;
  
  pinMode(CS,     OUTPUT);  //Serial.print("Initializing SD card...");
  if (!SD.begin(CS)) 
    Serial.println("Card failed, or not present");
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  dataFile << "Time,Barometric,Humidity,Temperature,Solar,Battery,Moist" << endl;
  Serial << "Time,Barometric,Humidity,Temperature,Solar,Battery,Moist" << endl;
  dataFile.close();

}

void loop(){
    long t =10000;
    Pump1.tick(t);
    Pump2.tick(t);
    Measure.tick(t);
}
