#include <Arduino.h>

// transmitter_8266_minimum.ino 

/*
Чем этот скетч отличается от самого простого?

Возвращаемое значение esp_now_init() показывает, завершился ли процесс без ошибок.
Аналогичным образом мы используем возвращаемое значение esp_now_add_peer(). 
Важно: функция не проверяет, действительно ли узел доступен. Но, например, вы получите сообщение об ошибке,
если превышено максимальное количество узлов. Подробнее см. в документации по API.

С помощью esp_now_register_send_cb((esp_now_send_cb_t)messageSent); мы регистрируем функцию messageSent, 
которая вызывается после отправки сообщения. Параметры предопределены.
Проверка status в функции messageSent и проверка result в качестве возвращаемого значения esp_now_send()
проверяют разные критерии. Подробности — в документации по API. 
*/

#include <espnow.h>
#include "ESP8266WiFi.h"
/*
[7]   ESP32-2432S028 (CYD)         MAC-Address: B8:D6:1A:5C:F9:54
                    Formated: {0xB8, 0xD6, 0x1A, 0x5C, 0xF9, 0x54} 

[13]  13_ESP32_2432S028_CYD        MAC_ADDRESS: A4:F0:0F:5A:B5:54   
    uint8_t  MAC_FORMATED[] = {0xA4, 0xF0, 0x0F, 0x5A, 0xB5, 0x54};
*/

  uint8_t receiverAddress[] = {0xB8, 0xD6, 0x1A, 0x5C, 0xF9, 0x54};
//uint8_t receiverAddress[] = {0xA4, 0xF0, 0x0F, 0x5A, 0xB5, 0x54};

void messageSent(uint8_t *macAddr, uint8_t status) 
{
  Serial.print("Send status: ");
  if(status == 0)
  {
    Serial.println("Success");
  }
  else
  {
    Serial.println("Error");
  }
}

void setup()
{
  Serial.begin(115200);
  delay(2000); // uncomment if your serial monitor is empty
  Serial.println("\n");

  WiFi.mode(WIFI_STA);
  if (esp_now_init() == 0) 
  {
    Serial.println("ESPNow Init success");
  }
  else 
  {
    Serial.println("ESPNow Init fail");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  // Добавляем устройство в список сопряжённыз для протокола ESP-NOW
  // до отправки данных на это устройство. Без добавления в список 
  // система не позволит отправить данные на указанный MAC-адрес.
  // Отправка готовится по 0 (текущему) каналу без шифрования (NULL,0) 
  uint8_t result = esp_now_add_peer(receiverAddress, ESP_NOW_ROLE_CONTROLLER, 0, NULL, 0);
  if(result != 0)
  {
    Serial.println("Failed to add peer");
  }
  // Регистрируем callback-функцию, которая будет вызываться каждый раз после того, 
  // как данные успешно отправлены с одного устройства ESP-NOW на другое.
  esp_now_register_send_cb(messageSent);  
}

uint16_t i=0;
 
void loop()
{
  uint16_t remainder = i % 2;
  char charArray[8];       // Буфер должен быть достаточно большим
  if (remainder==0)
  {
    ultoa(i,charArray,10); // 10 — основание (десятичная система)
    esp_now_send(receiverAddress, (uint8_t *) charArray, sizeof(charArray)-1); // -1 to not send the NULL terminator
    Serial.println(charArray);
  }
  else
  {
    //char message[] = "Привет от ESP8266 (Esp01) Hello! 0123456789";
    char message[] = "Привет от ESP8266(Esp01) Hello! 123=37";
    esp_now_send(receiverAddress, (uint8_t *) message, sizeof(message)-1); // -1 to not send the NULL terminator
    Serial.println(message);
  }
  i++;
  delay(5000);
}
