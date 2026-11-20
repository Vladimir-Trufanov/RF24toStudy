// ******************************************************** KvizzyVid17flat ***
// *                                                                          *
// *            Принимать json-сообщения о показаниях температуры, влажности, *
// *      уровня воды, напряжения питания с нижнего уровня (из подвала дома), *
// *         снимать значение напряжения на промежуточном уровне и передавать *
// *        по Internet WiFi на сайт (выполнять работу промежуточного уровня) * 
// *                                                                          *
// * v1.2.6, 19.11.2026                            Автор:       Труфанов В.Е. *
// * Copyright © 2026 tve                          Дата создания:  19.10.2026 *
// ****************************************************************************

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#include "v17WiFi.h"

//#include "Vid17flatDefine.h"
//#include "Vid17flatCommon.h"
//#include "Vid17flatHTTPS.h"

// Определяем задачи и готовим работу сторожевого таймера 
#include "v17WdtTask.h"        
// Подключаем задачи
#include "v17Base.h"   // 7-3307 принять сообщение нижнего уровня и отправить его в очередь на сайт
#include "v17Https.h"  // 6-5000 отправить сообщение из очереди на сайт
#include "v17Power.h"  // 5-3511 определить напряжение питания и отправить его в очередь на сайт

void setup() 
{
  Serial.begin(115200);
  vTaskDelay(5000/portTICK_PERIOD_MS);

  // Назначаем контакт A0 на приём показаний напряжения питания
  pinMode(A0, INPUT); 

  // Инициируем сторожевой таймер 
  iniWdt(); 

  // Назначаем сети для подключения по WiFi
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP("OPPO A9 2020", "b277a4ee84e8");
  wifiMulti.addAP("TP-Link_B394",  "18009217");
  wifiMulti.addAP("tve-DESKTOP",   "Ue18-647");
  wifiMulti.addAP("tve-MONOBLOCK", "Ue18-647");
  wifiMulti.addAP("linksystve",    "X93K6KQ6WF");
  wifiMulti.addAP("GoshaIMila",    "t1s2wde4bE");
    
  // Выполняем подключение к WiFi-сети
  WiFi_connect(wifiMulti); 

  // Включаем обработку события отсоединения с WiFi 
  // (для того, тобы обработать событие Wi-Fi через WiFi.onEvent() подключаем отдельную функцию, 
  // которая будет реагировать на конкретное событие, и регисрируем её с помощью метода onEvent().
  // Метод onEvent() принимает два аргумента. Функция-обработчик. Это функция, которая будет вызвана 
  // при возникновении события. Идентификатор события (опционально). Если указать конкретный тип события, 
  // обработчик вызовется только для него. Если не указывать — функция будет реагировать на все события.
  // Внутри функции-обработчика определяется, какое именно событие произошло, и какие действия должны выполниться)
  WiFi.onEvent(onWiFiEvent, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

  // Настраиваем интервал синхронизации, имя сервера, режим работы и часовой пояс
  initSNTP();
  // Ожидаем синхронизацию 
  wait4SNTP();
  // Изменяем данные о перезагрузках контроллера            
  //setReload(); 

  // 
  //setClock();

  // Инициируеи прием сообщений по nRF24L01
  iniradio();

  // Инициируем задачи
  xTaskCreatePinnedToCore(  // Принять сообщение нижнего уровня и отправить на сайт 
  vBase,                    // Task function
  "vBase",                  // Task name
  4096,                     // Stack size
  NULL,                     // Parameters passed to the task function
  7,                        // Priority
  NULL,                     // Task handle
  1); 
  
  xTaskCreatePinnedToCore(  // Принять сообщение нижнего уровня и отправить на сайт 
  vHttps,                   // Task function
  "vHttps",                 // Task name
  4096,                     // Stack size
  NULL,                     // Parameters passed to the task function
  6,                        // Priority
  NULL,                     // Task handle
  1); 
  
  xTaskCreatePinnedToCore(  // Определить напряжение питания и отправить на сайт 
  vPower,                   // Task function
  "vPower",                 // Task name
  4096,                     // Stack size
  NULL,                     // Parameters passed to the task function
  5,                        // Priority
  NULL,                     // Task handle
  1); 
} 
 
void saymem()
{
 Serial.print("Cвободная куча:  "); Serial.print(ESP.getFreeHeap());  Serial.print(" от ОЗУ   "); Serial.println(ESP.getHeapSize());
 Serial.print("FreePSRAM:      ");  Serial.print(ESP.getFreePsram()); Serial.print(" от FLASH "); Serial.println(ESP.getPsramSize());
}

void loop() 
{
  // Отмечаем начало нового цикла 
  iLoop++;
  uint32_t tTime=millis();

  // Считываем с последовательного порта целое число
  // (так как в зависимости от окружения за целым числом может следовать нулевое значение,
  // то отсекаем 0)
  if (Serial.available() > 0) 
  {
    int ii=Serial.parseInt();
    if (ii>0) inumber=ii;
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
   
  // Получаем значение напряжения источника питания схемы (Вольт) 
  float Vbattf = getVbattf();
  Serial.println(Vbattf, 3);
  
  // Перезагружаем контроллер в случае потери сети WiFi
  if (!isWiFi)
  {
    Serial.println("Перезагружаем контроллер после потери сети WiFi");
    ESP.restart();
  }

  /*   
  NetworkClientSecure *client = new NetworkClientSecure;
  if (client) 
  {
    client->setCACert(rootCACertificate);

    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before NetworkClientSecure *client is
      HTTPClient https;

      Serial.print("[HTTPS] begin...\n");
      
      // if (https.begin(*client, "https://jigsaw.w3.org/HTTP/connection.html")) 
      if (https.begin(*client, "https://kwinflat.ru/Vid17b/")) 
      {  // HTTPS
        Serial.print("[HTTPS] GET...\n");
        // start connection and send HTTP header
        int httpCode = https.GET();

        // httpCode will be negative on error
        if (httpCode > 0) 
        {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) 
          {
            String payload = https.getString();
            Serial.println("--------- Начало страницы --------------------------------------------");
            Serial.println(payload);
            Serial.println("--------- Завершение страницы ----------------------------------------");
          }
        } 
        else 
        {
          Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }

        https.end();
      } 
      else 
      {
        Serial.printf("[HTTPS] Unable to connect\n");
      }

      // End extra scoping block
    }

    delete client;
  } 
  else 
  {
    Serial.println("Unable to create client");
  }
  */

  Serial.println();
  saymem();
  vTaskDelay(1500/portTICK_PERIOD_MS);

  // Определяем время работы цикла задачи
  tLoop=millis()-tTime;
  // Отмечаем завершение задачи для сторожевого таймера
  finLoop = true;
  if (inumber == 1) MimicMCUhangEvent("Loop");   
}

// ******************************************************** KvizzyVid17flat ***
