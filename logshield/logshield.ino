#include <Wire.h>
#include "RTClib.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include "DHT.h"
#include <Streaming.h>
#include <SD.h>
#include <LiquidCrystal_I2C.h>

#define INPUT3 A0
#define BATTERY A1
#define SOLAR A2
#define SDA A4 
#define SCL A5

#define DHTPIN 4     // DHT 22 data pin
#define  PUMP2 6
#define  PUMP1 7

#define CS 10  // SD card chip select
#define MOSI 11
#define MISO 12
#define SCK  13     

#define BAUD   9600

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085); //set up barometric
DHT dht(DHTPIN, DHT22);
//Real time clock object
RTC_DS1307 rtc; 
// Set the LCD I2C address
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 


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
 
float get_battery(){
  int raw = analogRead(BATTERY);
  //Serial << raw << endl;
  //return raw *5.0/206.0;
  return 12.0;
 }

void isotime() {
     DateTime nw = rtc.now(); 
     int s = nw.second();
     lcd << nw.year()  << '-' << nw.month()  << '-' << nw.day()    << " ";
     lcd << nw.hour()  << ':' << nw.minute();
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
    lcd << "error opening datalog.txt" << endl; } 
}

void measure(){
  static int row = 0;
  int pressure = get_pressure();
  int temperature = get_temperature();
  int humidity = dht.readHumidity();
  int solar = get_solar();
  int battery = get_battery();
  lcd.clear();
  lcd.setCursor(0,0);
  isotime();
  lcd.setCursor(0,1);
  switch(row)
  {
  case 0:
    lcd << pressure  << " hPa " << temperature << " C";
    row = 1;
    break;
  case 1:
    lcd << humidity <<"% Hum Sol " << solar << "v";
    row =2;
    break;
  case 2:
    lcd << "Bat "<< battery << " v";
    row = 0; 
   }
    
  log( rtc.now(),pressure,temperature,humidity,solar); 
}

void action ( DateTime &nw, int act, int hr, int mn, int dur) {
  long mins = nw.hour()*60 + nw.minute();
  int tm = hr*60 + mn;
  int  flag =  ((mins > tm) && mins < (tm + dur))?LOW:HIGH; 
  
  
  switch(act) {
   case PUMP1:
       digitalWrite(PUMP1,flag);
       //lcd << "Pump1" << digitalRead(PUMP1) << endl;
       break;
    case PUMP2:
         digitalWrite(PUMP2,flag);
       break;
  }    
}

void setup() {
  
   Serial.begin(BAUD); 
   Wire.begin();
   dht.begin();
  
   bmp.begin();
   SD.begin(CS);
   lcd.begin(16,2);         // initialize the lcd for 20 chars 4 lines and turn on backlight
   rtc.begin();
   if (! rtc.isrunning()) {
     lcd << "RTC is NOT running!";
     rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
   }
  
   //pinMode(13,OUTPUT);
   pinMode(SOLAR, INPUT);
   pinMode(BATTERY, INPUT);
   pinMode(PUMP1, OUTPUT);
   pinMode(PUMP2,OUTPUT);
   digitalWrite(PUMP1,HIGH);
   digitalWrite(PUMP2,HIGH);
  
   //lcd << "Date, Time, Barometric,Temperature,humidity,solar\n";
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
 
 

