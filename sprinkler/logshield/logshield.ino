#include <Wire.h>
#include "RTClib.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include "DHT.h"
#include <Streaming.h>
#include <SD.h>

#define DHTPIN 4     // DHT 22 data pin
#define SOLAR A2
#define BATTERY A1
#define INPUT3 A0
#define  PUMP1 7
#define  PUMP2 6
#define CS 10        // SD card chip select
#define BAUD   57600
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085); //set up barometric
DHT dht(DHTPIN, DHT22);
RTC_DS1307 rtc;  //Real time clock object



float get_pressure(){
  sensors_event_t event;
  bmp.getEvent(&event);
  return event.pressure;
}

float get_temperature(){
    float temperature;
    sensors_event_t event;
    bmp.getEvent(&event);
    bmp.getTemperature(&temperature);
    return temperature;

}

float get_solar(){
  int raw = analogRead(SOLAR);
  //Serial << raw << endl;
  return raw *5.0/206.0;
 }
 

void isotime() {
     DateTime nw = rtc.now(); 
     int s = nw.second();
     Serial << nw.year()  << '-' << nw.month()  << '-' << nw.day()    << ", ";
     Serial << nw.hour()  << ':' << nw.minute() << ':' << s << ", ";
}


void log(DateTime nw, float pressure,float temperature,float humidity,float solar){
     //log to file and serial (set to false to not log)
    File dataFile = SD.open("datalog.txt", FILE_WRITE);
    if (dataFile && true) {
      dataFile << nw.year()  << '-' << nw.month()  << '-' << nw.day()    << ' ';
      dataFile << nw.hour()  << ':' << nw.minute() << ':' << nw.second() << ',';
      dataFile << pressure   << ',' << humidity    << ',' << temperature << ',';
      dataFile << solar      <<  endl;  //',' << battery     << endl;    
      dataFile.close();
      // print the serial port too:
    }  else  {
    Serial << "error opening datalog.txt" << endl; } 
}

void measure(){
  
  float pressure = get_pressure();
  float temperature = get_temperature();
  float humidity = dht.readHumidity();
  float solar = get_solar();
  isotime();
  Serial << pressure << ", " << temperature << ",  " << humidity <<", "<< solar << "\n";
  log( rtc.now(),pressure,temperature,humidity,solar); 
}

void setup() {
  
   Serial.begin(BAUD); 
   Wire.begin();
   dht.begin();
   rtc.begin();
   bmp.begin();
   SD.begin(CS);
   //pinMode(13,OUTPUT);
   pinMode(SOLAR, INPUT);
   pinMode(BATTERY, INPUT);
   pinMode(PUMP1, OUTPUT);
   pinMode(PUMP2,OUTPUT);
   digitalWrite(PUMP1,HIGH);
   digitalWrite(PUMP2,HIGH);

   Serial << "Date, Time, Barometric,Temperature,humidity,solar\n";
}

void action ( DateTime &nw, int act, int hr, int mn, int dur) {
  long mins = nw.hour()*60 + nw.minute();
  int tm = hr*60 + mn;
  int  flag =  ((mins > tm) && mins < (tm + dur))?LOW:HIGH; 
  
  
  switch(act) {
   case PUMP1:
       digitalWrite(PUMP1,flag);
       Serial << "Pump1" << digitalRead(PUMP1) << endl;
       break;
    case PUMP2:
         digitalWrite(PUMP2,flag);
       break;
  }    
}



void loop(void) {
  static long ml = millis();


  if (millis() > ml) {
        measure();
        DateTime nw = rtc.now();
        action(nw, PUMP1, 22, 04, 5);
        action(nw, PUMP2, 10, 11, 10 );
        ml += 3000;
  }
    
}
 
 

