
#define C 4
#define X 5
#define Y 6
#define Z 7
  int n =0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  n =0;
  pinMode(C,OUTPUT);
  pinMode(X,INPUT);
  pinMode(Y,INPUT);
  pinMode(Z,INPUT);

}
int counter = 21;
int p12 = 50;  // 1000000/ 10000 /2  Half a period for 10kHz in microseconds
int reading  = 4;  //takes 4 microsces to do reading
long  x,y,z;

long doit() {
    long t = micros();
    x = 0;
    y = 0;
    z = 0;
    for (int c = 0; c < 21; c++)
    {
      digitalWrite(C,HIGH);
      delayMicroseconds(p12-reading);  // loop around for a while
      counter--;
      x << 1;
      x  |= digitalRead(X);
      y << 1; 
      y  |= digitalRead(Y);
      z << 1;
      z  |= digitalRead(Z);
      digitalWrite(C,LOW);
      delayMicroseconds(p12);
    }
    return micros() - t;
  
}

void loop() {
  // put your main code here, to run repeatedly:

  if ( n < 10){
    
  Serial.print ("\n\nStart ");
  Serial.print(micros());
  Serial.print("\nTime");
    long x =   doit();
    Serial.print(x);
    Serial.print ("\n");
    n++;
    Serial.print(micros());
  
  Serial.print("\nEND\n");
  }
}
