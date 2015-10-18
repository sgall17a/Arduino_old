

const int redPin = 9;
const int greenPin = 10;
const int bluePin = 11;
 
void setup() {
// Start off with the LED off.

setColourRgb(0,0,0);
}
 
void loop() {
static float  p  = 3.14/200 ;
static int i = 0;
float th = p * i;
i++;
if (i>20) i = 0;
int red = cos(th*11) * 256;
if (red <0) red = 0;
int blue = cos(th *13) * 256;
if (blue < 0) blue =0;
int green = cos(th * 7 ) * 256;
if (green < 0) green = 0;


//int red = int(random(0,255));
//if (red < 50) red = 0;
//int green = int(random(0,255));
//if (green < 50) green = 0;
//int blue = int(random(0,255));
//if (blue < 50) blue = 0;

setColourRgb(red, green,blue);
//setColourRgb(random(0,255), 0, random(0,255));
delay(100);

}
 
void setColourRgb(unsigned int red, unsigned int green, unsigned int blue) {
analogWrite(redPin, red);
analogWrite(greenPin, green);
analogWrite(bluePin, blue);
} 
