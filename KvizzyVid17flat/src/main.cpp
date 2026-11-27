// ******************************************************** KvizzyVid17flat ***
// *                                                                          *
// *            Принимать json-сообщения о показаниях температуры, влажности, *
// *      уровня воды, напряжения питания с нижнего уровня (из подвала дома), *
// *         снимать значение напряжения на промежуточном уровне и передавать *
// *        по Internet WiFi на сайт (выполнять работу промежуточного уровня) * 
// *                                                                          *
// * v1.2.9, 21.11.2026                            Автор:       Труфанов В.Е. *
// * Copyright © 2026 tve                          Дата создания:  19.10.2026 *
// ****************************************************************************

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#include "v17WiFi.h"
#include "ResetReason.h"

// ============================================== 1. Инициировать использование очереди ===
// Подключаем файлы обеспечения передачи и приёма сообщений через очередь                //
#include "QueChar.h"     // заголовочный файл класса TQueMessage                         //
// Назначаем объект работы с сообщениями через очередь                                   //
TQue queMessa;                                                                           //
// ========================================================================================                                                                                         

#include "v17Define.h"

// Определяем задачи и готовим работу сторожевого таймера 
#include "v17WdtTask.h"        
// Подключаем задачи
#include "v17Base.h"   // 7-3307 принять сообщение нижнего уровня и отправить его в очередь на сайт
#include "v17Https.h"  // 6-5000 отправить сообщение из очереди на сайт
#include "v17Power.h"  // 5-3511 определить напряжение питания и отправить его в очередь на сайт

// Определяем объект для синхронизации времени 
#include "AttachSNTP.h"
TAttachSNTP oSNTP;

void setup() 
{
  Serial.begin(115200);

  // Получаем значение напряжения источника питания схемы (Вольт) 
  Serial.println("\n---------------- v1.2.9, 21.11.2026 ----------------");
  float Vbattf = getVbattf();
  Serial.println(Vbattf, 3);

  vTaskDelay(5000/portTICK_PERIOD_MS);
  
  // Публикуем причину предыдущего сброса
  Serial.print("Причина сброса CPU0: "); print_reset_reason(rtc_get_reset_reason(0));
  Serial.print("Причина сброса CPU1: "); print_reset_reason(rtc_get_reset_reason(1));
  
  // Назначаем контакт A0 на приём показаний напряжения питания
  pinMode(A0, INPUT); 

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

  // Синхронизируем время с интервалом по умолчанию 60 мин
  oSNTP.Create();

  // Изменяем данные о перезагрузках контроллера в NVS
  // (после синхронизации времени)   
  setReload(); 
  
  // Инициируеи прием сообщений по nRF24L01
  iniradio();
  
  // Инициируем сторожевой таймер 
  iniWdt(); 
    
  // ====================================== 2. Создать очередь и подключить передатчик ===
  // Создаем очередь                                                                    //
  String inMess=queMessa.Create();                                                      //
  // Если не получилось, сообщаем "Очередь не была создана и не может использоваться"   // 
  if (inMess==tQueueNotCreate) Serial.println(tQueueNotCreate);                         //
  // Если очередь получилась, то отмечаем  "Очередь сформирована"                       //
  else Serial.println(tQueueBeformed);                                                  //
  // Подключаем функцию передачи сообщения на периферию                                 //
  queMessa.attachFunction(transQue);                                                    //
  // ===================================================================================== 

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
  //float Vbattf = getVbattf();
  //Serial.println(Vbattf, 3);
  
  // Перезагружаем контроллер в случае потери сети WiFi
  if (!isWiFi)
  {
    Serial.println("Перезагружаем контроллер после потери сети WiFi");
    ESP.restart();
  }

  //Serial.println();
  //saymem();
  vTaskDelay(1500/portTICK_PERIOD_MS);

  // Определяем время работы цикла задачи
  tLoop=millis()-tTime;
  // Отмечаем завершение задачи для сторожевого таймера
  finLoop = true;
  if (inumber == 1) MimicMCUhangEvent("Loop");   
}

// ******************************************************** KvizzyVid17flat ***
