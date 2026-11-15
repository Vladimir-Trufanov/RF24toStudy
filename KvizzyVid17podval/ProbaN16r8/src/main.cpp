#include <Arduino.h>

void setup() 
{
  Serial.begin(115200);
  delay(10000);
  Serial.print("Flash: ");
  Serial.print(ESP.getFlashChipSize() / (1024 * 1024));
  Serial.println(" MB");
  Serial.print("PSRAM: ");
  Serial.print(ESP.getPsramSize() / (1024 * 1024));
  Serial.println(" MB");
}

void loop() 
{
  // put your main code here, to run repeatedly:
}

