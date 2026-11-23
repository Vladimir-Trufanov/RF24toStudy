/** Arduino, Esp32-C3 ****************************************** v17Https.h ***
 * 
 *                                       Отправить сообщение из очереди на сайт  
 * 
 * v1.0.1, 20.11.2026                                 Автор:      Труфанов В.Е.
 * Copyright © 2026 tve                               Дата создания: 19.11.2026
**/

#pragma once            
#include <Arduino.h>
#include "v17WdtTask.h"  

#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <NetworkClientSecure.h>

const char *rootCACertificate = R"string_literal(
-----BEGIN CERTIFICATE-----
MIIFgzCCA2ugAwIBAgIORea7A4Mzw4VlSOb/RVEwDQYJKoZIhvcNAQEMBQAwTDEg
MB4GA1UECxMXR2xvYmFsU2lnbiBSb290IENBIC0gUjYxEzARBgNVBAoTCkdsb2Jh
bFNpZ24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMTQxMjEwMDAwMDAwWhcNMzQx
MjEwMDAwMDAwWjBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSNjET
MBEGA1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjCCAiIwDQYJ
KoZIhvcNAQEBBQADggIPADCCAgoCggIBAJUH6HPKZvnsFMp7PPcNCPG0RQssgrRI
xutbPK6DuEGSMxSkb3/pKszGsIhrxbaJ0cay/xTOURQh7ErdG1rG1ofuTToVBu1k
ZguSgMpE3nOUTvOniX9PeGMIyBJQbUJmL025eShNUhqKGoC3GYEOfsSKvGRMIRxD
aNc9PIrFsmbVkJq3MQbFvuJtMgamHvm566qjuL++gmNQ0PAYid/kD3n16qIfKtJw
LnvnvJO7bVPiSHyMEAc4/2ayd2F+4OqMPKq0pPbzlUoSB239jLKJz9CgYXfIWHSw
1CM69106yqLbnQneXUQtkPGBzVeS+n68UARjNN9rkxi+azayOeSsJDa38O+2HBNX
k7besvjihbdzorg1qkXy4J02oW9UivFyVm4uiMVRQkQVlO6jxTiWm05OWgtH8wY2
SXcwvHE35absIQh1/OZhFj931dmRl4QKbNQCTXTAFO39OfuD8l4UoQSwC+n+7o/h
bguyCLNhZglqsQY6ZZZZwPA1/cnaKI0aEYdwgQqomnUdnjqGBQCe24DWJfncBZ4n
WUx2OVvq+aWh2IMP0f/fMBH5hc8zSPXKbWQULHpYT9NLCEnFlWQaYw55PfWzjMpY
rZxCRXluDocZXFSxZba/jJvcE+kNb7gu3GduyYsRtYQUigAZcIN5kZeR1Bonvzce
MgfYFGM8KEyvAgMBAAGjYzBhMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8EBTAD
AQH/MB0GA1UdDgQWBBSubAWjkxPioufi1xzWx/B/yGdToDAfBgNVHSMEGDAWgBSu
bAWjkxPioufi1xzWx/B/yGdToDANBgkqhkiG9w0BAQwFAAOCAgEAgyXt6NH9lVLN
nsAEoJFp5lzQhN7craJP6Ed41mWYqVuoPId8AorRbrcWc+ZfwFSY1XS+wc3iEZGt
Ixg93eFyRJa0lV7Ae46ZeBZDE1ZXs6KzO7V33EByrKPrmzU+sQghoefEQzd5Mr61
55wsTLxDKZmOMNOsIeDjHfrYBzN2VAAiKrlNIC5waNrlU/yDXNOd8v9EDERm8tLj
vUYAGm0CuiVdjaExUd1URhxN25mW7xocBFymFe944Hn+Xds+qkxV/ZoVqW/hpvvf
cDDpw+5CRu3CkwWJ+n1jez/QcYF8AOiYrg54NMMl+68KnyBr3TsTjxKM4kEaSHpz
oHdpx7Zcf4LIHv5YGygrqGytXm3ABdJ7t+uA/iU3/gKbaKxCXcPu9czc8FB10jZp
nOZ7BN9uBmm23goJSFmH63sUYHpkqmlD75HHTOwY3WzvUy2MmeFe8nI+z1TIvWfs
pA9MRf/TuTAjB0yPEL+GltmZWrSZVxykzLsViVO6LAUP5MSeGbEYNNVMnbrt9x+v
JJUEeKgDu+6B5dpffItKoZB0JaezPkvILFa9x8jvOOJckvB595yEunQtYQEgfn7R
8k8HWV+LLUNS60YMlOH1Zkd5d9VUWx+tJDfLRVpOoERIyNiwmcUVhAn21klJwGW4
5hpxbqCo8YLoRT5s1gLXCmeDBVrJpBA=
-----END CERTIFICATE-----
)string_literal";

char tBuffer[42]; // буфер сообщения на 41 символ и завершающий ноль

void vHttps(void* pvParameters) 
{
   for ( ;; )
   {
      // Отмечаем начало нового цикла задачи
      iHttps++;
      uint32_t tTime=millis();
      printTime(); 

      // Выбираем сообщение из очереди
      strcpy(tBuffer, queMessa.Receive());
      Serial.print("Прием: ");
      Serial.println(tBuffer);
      String addr="https://kwinflat.ru/Vid17b/";
      //String sql=

      /*
      NetworkClientSecure *client = new NetworkClientSecure;
      if (client) 
      {
        client->setCACert(rootCACertificate);
        {
          HTTPClient https;
          Serial.println("[HTTPS] начат сеанс ...");
          if (https.begin(*client, "https://kwinflat.ru/Vid17b/")) 
          {  
            Serial.println("[HTTPS] GET-запрос отправляется ...");
            // Запускаем соединение и отправляем HTTP-заголовки
            int httpCode = https.GET();
            // Если ответ положительный (HTTP-запрос обработан сервером и получен ответ от него)
            if (httpCode > 0) 
            {
              Serial.printf("[HTTPS] код ответа... code: %d\n", httpCode);
              // 200 или 301, хорошо
              if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) 
              {
                String payload = https.getString();
                Serial.println("--------- Начало страницы --------------------------------------------");
                Serial.println(payload);
                Serial.println("--------- Завершение страницы ----------------------------------------");
              }
            } 
            // Если ответ отрицательный (ошибка)
            else 
            {
              Serial.printf("[HTTPS] GET... не прошёл, ошибка: %s\n", https.errorToString(httpCode).c_str());
            }
            https.end();
          } 
          else 
          {
            Serial.printf("[HTTPS] не удалось подключиться\n");
          }
        }
        delete client;
      } 
      else 
      {
        Serial.println("Не удалось создать защищенного клиента");
      }
      */
     
      vTaskDelay(5000/portTICK_PERIOD_MS);
      // Определяем время работы цикла задачи
      tHttps=millis()-tTime;
      // Отмечаем завершение задачи для сторожевого таймера
      finHttps = true;
      if (inumber == 4) MimicMCUhangEvent("vHttps");   
   }
}



// Not sure if NetworkClientSecure checks the validity date of the certificate.
// Setting clock just to be sure...
/*
void setClock() 
{
  configTime(0, 0, "pool.ntp.org");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) 
  {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print(F("Current time: "));
  char buf[26];
  Serial.print(asctime_r(&timeinfo, buf));
}
*/

// ************************************************************* v17Https.h ***
