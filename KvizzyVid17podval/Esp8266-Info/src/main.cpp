#include <Arduino.h>
#include <ESP8266WiFi.h>

void ViewInfo();

void setup() 
{
  Serial.begin(115200);
  while (!Serial) 
  {
    // Wait for serial port to connect. Needed for native USB port only
    delay(100);
  }
}

void loop() 
{
  delay(5000);
  ViewInfo();
}

void ViewInfo() 
{
  Serial.println("\nESP8266 Parameters:");
  Serial.println("-------------------");
  
  // Информация о чипе
  Serial.print("Chip ID: ");
  Serial.println(ESP.getChipId());
  
  // MAC-адрес
  Serial.print("MAC-Address: ");
  String mac = WiFi.macAddress();
  Serial.println(mac);

  Serial.print("Formated:    ");
  Serial.print("{");
  int index = 0;
  for(int i=0; i<6; i++)
  {
    Serial.print("0x");
    Serial.print(mac.substring(index, index+2));
    if(i<5)
    {
      Serial.print(", ");
    }
    index += 3;
  }
  Serial.println(" ");

   
  uint32_t realSize = ESP.getFlashChipRealSize();
  Serial.printf("Реальный размер флеш-памяти: %u байт\n", realSize);

  Serial.println("}");
  Serial.println("-------------------");
}
