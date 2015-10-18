
//Define pins
#define CP 4
#define XP 5
#define YP 6
#define ZP 7
#define BITNUM 21
#define READFREQ
#define BAUDRATE 9600
int n =0;

int p12 = 50;  // 1000000/ 10000 /2  Half a period for 10kHz in microseconds
int reading  = 4;  //takes 4 microsces to do reading
//These will be recalculated dynamically

class Dro{
  long val;
  long maxx;
  long minx;
  long sumx;
  long cnt;
  public:
  void clear();
  long value();
  void append();
  void addbit(int b);
};
  void Dro::clear() {
   val = 0;
   cnt  =0;
   minx =  999999999999999;
   maxx  =0;
  }

  long Dro::value(){
   return (sumx - maxx - minx ) / (cnt-2);
  }
  
  void Dro::addbit(int b){
    val << 1;
    val |= b;
  }
  
  void Dro::append(){
   sumx += val;
   minx = min(val,minx);
   maxx = max(val,maxx);
   cnt++;
  };
  
  Dro X = Dro();
  Dro Y = Dro();
  Dro Z = Dro ();

void do_read(){
    for (int c = 0; c < BITNUM; c++)
    {
      digitalWrite(CP,HIGH);
      delayMicroseconds(p12-reading);  // loop around for a while
      X.addbit(digitalRead(XP));
      Y.addbit(digitalRead(YP));
      Z.addbit(digitalRead(ZP));
      digitalWrite(CP,LOW);
      delayMicroseconds(p12);
    }
  
}


  void prepare_and_read() {
    //Separate initialisations etc so we can time properly
    X.clear();
    Y.clear();
    Z.clear();
      do_read();
    X.append();
    Y.append();
    Z.append();
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(BAUDRATE);
  pinMode(CP,OUTPUT);
  pinMode(XP,INPUT);
  pinMode(YP,INPUT);
  pinMode(ZP,INPUT);
  
  p12 =1;  // work out timings without delays. Actualy need one microsend otherwise
    //get strange readings.
  reading =0;
  long t = micros();
    do_read();
  t = micros() - t;
  

  Serial.print("\nTimes ");
  Serial.print(t);
  int p12 = 1000000 / READFREQ /2; //Half a period for READFREQ or 10 kHz 
  int calc_time = t/ BITNUM; // There are BITNUM bits to read
  reading = p12 -calc_time;
  Serial.print("\nHalf period ");
  Serial.print(p12);
  Serial.print ("\n Reading ");
  Serial.print(reading);

}


void loop() {
  // put your main code here, to run repeatedly:

}
