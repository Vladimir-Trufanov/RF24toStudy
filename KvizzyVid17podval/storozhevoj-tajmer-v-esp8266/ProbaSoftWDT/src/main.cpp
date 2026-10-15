// ProbaSoftWDT
#include <Arduino.h>
void setup() 
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("\nЗапущен ESP07"); 
  // ESP.wdtDisable();
}
 
unsigned long imess = millis();

void loop() 
{ 
  // while(1){}   
  if ((millis()-imess)>2000)
  {
    Serial.println("\nПрошло 2 секунды!");
    imess = millis();
  }
  else delay(500);
}
