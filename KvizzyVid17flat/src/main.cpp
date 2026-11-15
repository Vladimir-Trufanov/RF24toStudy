// ******************************************************** KvizzyVid17flat ***
// *                                                                          *
// *            Принимать json-сообщения о показаниях температуры, влажности, *
// *      уровня воды, напряжения питания с нижнего уровня (из подвала дома), *
// *         снимать значение напряжения на промежуточном уровне и передавать *
// *        по Internet WiFi на сайт (выполнять работу промежуточного уровня) * 
// *                                                                          *
// * v1.2.2, 15.11.2026                            Автор:       Труфанов В.Е. *
// * Copyright © 2026 tve                          Дата создания:  19.10.2026 *
// ****************************************************************************

#include <Arduino.h>

// Подключаем библиотеки для подключения камеры к WiFi
#include <WiFi.h>
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#include "v17WiFi.h"

#include "Vid17flatDefine.h"
#include "Vid17flatCommon.h"
#include "Vid17flatHTTPS.h"

// Определяем задачи и готовим работу сторожевого таймера 
#include "v17WdtTask.h"        
// Подключаем задачи
#include "v17Base.h"          // 4-3307 принять сообщение нижнего уровня и отправить на сайт
#include "v17Power.h"         // 4-3511 определить напряжение питания и отправить на сайт

void setup() 
{
  Serial.begin(115200);
  // Назначаем контакт A0 на приём показаний напряжения питания
  pinMode(A0, INPUT); 

  // Инициируем работу контроллера, как станции WiFi и с собственной сетью
  WiFi.mode(WIFI_MODE_STA);
  char* soft_apssid = soft_ap_ssid;      // не более 10 символов, латиница
  WiFi.softAP(soft_apssid, soft_apssid);
  // Включаем обработку события отсоединения с WiFi через безымянную лямбда-функцию
  // (так как это выполняется в отдельном потоке, то просто устанавливаем флаг,
  // чтобы его обнаружить в цикле loop и переподключить WiFi) 
  WiFiEventId_t eventID = WiFi.onEvent
  (
    [](WiFiEvent_t event, WiFiEventInfo_t info) 
    {
      Serial.print("Wi-Fi потерял связь. Причина: ");
      Serial.println(info.wifi_sta_disconnected.reason);
      isWiFi=false;  
    },
    WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED
  );
  // Привязываем возможные сети и подключаемся к WiFi
  wifiMulti.addAP("TP-Link_B394",  "18009217");
  wifiMulti.addAP("tve-DESKTOP",   "Ue18-647");
  wifiMulti.addAP("OPPO A9 2020",  "b277a4ee84e8");
  wifiMulti.addAP("tve-MONOBLOCK", "Ue18-647");
  wifiMulti.addAP("linksystve",    "X93K6KQ6WF");
  wifiMulti.addAP("GoshaIMila",    "t1s2wde4bE");
  Serial.println("Подключаемся к WiFi ... ");
  delay(100);
  if (wifiMulti.run() == WL_CONNECTED) 
  {
    Serial.println("WiFi подсоединен: ");
    isWiFi=true; 
  }
  // Настраиваем интервал синхронизации, имя сервера, режим работы и часовой пояс
  //initSNTP();
  // Ожидаем синхронизацию 
  //wait4SNTP();
  // Изменяем данные о перезагрузках контроллера            
  //setReload(); 
  // Готовим параметры сетей для CameraWebServer   
  //IniSayWiFi();
  
  // Инициируем задачи
  xTaskCreatePinnedToCore(  // Принять сообщение нижнего уровня и отправить на сайт 
  vBase,                    // Task function
  "vBase",                  // Task name
  1024,                     // Stack size
  NULL,                     // Parameters passed to the task function
  4,                        // Priority
  NULL,                     // Task handle
  0); 

  xTaskCreatePinnedToCore(  // Определить напряжение питания и отправить на сайт 
  vPower,                   // Task function
  "vPower",                 // Task name
  1024,                     // Stack size
  NULL,                     // Parameters passed to the task function
  4,                        // Priority
  NULL,                     // Task handle
  0); 

  // Инициируем сторожевой таймер 
  iniWdt(); 

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
  vTaskDelay(3000/portTICK_PERIOD_MS);

  //
  if (!isWiFi)
  {
    Serial.println("Подключается WiFi после потери сети");
    while (wifiMulti.run() != WL_CONNECTED) 
    { 
      Serial.print("."); delay(500);
    }
    Serial.println(" ");
    Serial.println("WiFi подсоединен!");
    //IniSayWiFi();
    isWiFi=true; 
  }

 
  // Определяем время работы цикла задачи
  tLoop=millis()-tTime;
  // Отмечаем завершение задачи для сторожевого таймера
  finLoop = true;
  if (inumber == 1) MimicMCUhangEvent("Loop");   
}

// ******************************************************** KvizzyVid17flat ***
