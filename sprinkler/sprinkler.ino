// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#include "DHT.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include "RTClib.h"
#include <Streaming.h>
#include <SPI.h>
//#include <SD.h>

#define DHTPIN 5      // HDHT2 humidity connection 
#define DHTTYPE DHT22 // DHT 22  (AM2302) for normal 16mhz Arduino
#define CS 10         // Chip select for SD card
#define LED 13        // Flashing LED
#define SOLAR A6      // Solar panel voltage
#define BATTERY A7    // Battery voltage
#define MOISTURE A2
#define PUMP1 3
#define PUMP2 4
#define START2_F 30
#define START2_T  5
#define FIRSTTIME  2014, 9, 12, 10, 0, 0  // Datetime to start system going
#define FREQUENCY  20 //3600 * 24 * 2  // How often we water 2 days as seconds
#define WATERTIME  5       // Ten minutes currently
#define MEASURE   5               // every 5 secs 

//global variables
DHT dht(DHTPIN, DHTTYPE);  //Set up humidity
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085); //set up barometric
RTC_DS1307 rtc;  //Real time clock object
long next_watering;
long flash;
long next_measure;
long nextpump2;
   
void setup() {
  
  //setup up pins for input output etc
  pinMode(LED,    OUTPUT); 
  pinMode(PUMP1,  OUTPUT);
  pinMode(PUMP2,  OUTPUT);
  pinMode(CS,     OUTPUT);  //Serial.print("Initializing SD card...");
  pinMode(BATTERY,INPUT);
  pinMode(SOLAR,  INPUT);
  pinMode(MOISTURE,INPUT);
  
  digitalWrite(PUMP1,HIGH);
  digitalWrite(PUMP2,LOW);

  delay (200);  // let everything settle down a bit
  
  Serial.begin(57600); 
  Serial << "Starting" << endl;
  
  Wire.begin();
  dht.begin();
  rtc.begin();
  
  if (! rtc.isrunning())  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  //This is to start this system
  DateTime first_time (FIRSTTIME);
  next_watering = first_time.unixtime() ;
  nextpump2 = first_time.unixtime();
  long nw = rtc.now().unixtime();
  next_measure = nw + MEASURE;
  flash = nw + 5;
   
  if(!bmp.begin()) Serial << "Problem with BMO085" << endl;
  
}

 

void loop() {
   DateTime nw = rtc.now(); 
   long     ut = nw.unixtime();

   // Serial << '.';
    if (ut > next_measure){
      measure(nw);
      next_measure += MEASURE;
    }
    
    if (ut > flash){
      //Serial << "flashs\n";
       digitalWrite(LED,HIGH);
    }
    if (ut > flash +1){
          digitalWrite(LED,LOW);
          flash += 5;
    }
    
    if (ut > next_watering) {
      digitalWrite(PUMP1,LOW);  //relay is a sink so LOW
    }
      
      //turn it off WATERTIME seconds after starting
     if (ut > next_watering + WATERTIME) {
          next_watering += FREQUENCY;
          digitalWrite (PUMP1,HIGH);
        }
    
    if (ut > nextpump2) {
      digitalWrite(PUMP2,LOW);  //relay is a sink so LOW
    }
      
      //turn it off WATERTIME seconds after starting
     if (ut > nextpump2 + START2_T) {
          nextpump2 += START2_T;
          digitalWrite (PUMP2,HIGH);
        }

 }

void measure(DateTime &nw) {
  
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
 //   File dataFile = SD.open("datalog.txt", FILE_WRITE);
//         Serial << "error opening datalog.txt" << endl; }
       
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









