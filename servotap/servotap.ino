
long nextmilli;
int night = 0;
int day = 0;

setup() {
  nextmilli = millis() + 60;
}

void throb() {
  static int night =0;
  static long mins = 0;

   if (analogRead(LSR) < 500) 
       night++;
   else
       night = 0;
       
   if (night > 20) {
       mins =0;
       night = 0;
   }
   
   mins++;

      
   if ((dy > 20) & ) 
   
   
  
  
}

loop() {
  if (nextmilli < millis()){
      throb();
      nextmilli = millis + 60;
     }      
}
