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

  /*
  // Информация о чипе
  Serial.print("Chip ID: ");
  Serial.println(ESP.chipId());

  // Информация о CPU
  Serial.print("CPU Frequency: ");
  Serial.print(System.cpuFreq() / 1000000); // Convert to MHz
  Serial.println(" MHz");
 
  // Информация о Flash-памяти
  Serial.print("Flash Size: ");
  Serial.println(ESP.flashSize() / (1024 * 1024)); // Convert to MB
  */
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
  Serial.println("}");
  Serial.println("-------------------");
}
