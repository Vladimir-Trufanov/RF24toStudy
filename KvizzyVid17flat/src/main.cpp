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

#include <SPI.h>      
#include <nRF24L01.h> 
#include <RF24.h>     

// Для NodeMCU-32S
RF24 radio(22,5);      // указали номера выводов nRF24L01: CE и CSN

// Устанавливаем адрес "трубы" - адрес передатчика и явно указываем компилятору, 
// что литерал имеет тип unsigned long (как минимум 32 бита) 
// для предотвращения неявного усечения значения
const uint32_t pipeIn = (uint32_t)0x7878787878; 


//#include "Vid17flatDefine.h"
//#include "Vid17flatCommon.h"
//#include "Vid17flatHTTPS.h"

// Определяем задачи и готовим работу сторожевого таймера 
#include "v17WdtTask.h"        
// Подключаем задачи
#include "v17Base.h"          // 4-3307 принять сообщение нижнего уровня и отправить на сайт
#include "v17Power.h"         // 4-3511 определить напряжение питания и отправить на сайт

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
  //initSNTP();
  // Ожидаем синхронизацию 
  //wait4SNTP();
  // Изменяем данные о перезагрузках контроллера            
  //setReload(); 
  // Готовим параметры сетей для CameraWebServer   
  //IniSayWiFi();

   Serial.println("Начало прослушивания !!!");      
   radio.begin();            // инициировали работу nRF24L01
   radio.setChannel(0x6f);   // установили канал

   radio.openReadingPipe(0, pipeIn); // открыли "трубу" чтения
   radio.startListening();           // начали прослушивать
   Serial.println("");

  
  // Инициируем задачи
  xTaskCreatePinnedToCore(  // Принять сообщение нижнего уровня и отправить на сайт 
  vBase,                    // Task function
  "vBase",                  // Task name
  4096,                     // Stack size
  NULL,                     // Parameters passed to the task function
  5,                        // Priority
  NULL,                     // Task handle
  1); 
  
  xTaskCreatePinnedToCore(  // Определить напряжение питания и отправить на сайт 
  vPower,                   // Task function
  "vPower",                 // Task name
  4096,                     // Stack size
  NULL,                     // Parameters passed to the task function
  4,                        // Priority
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
  saymem();
  vTaskDelay(3000/portTICK_PERIOD_MS);
  
  // Перезагружаем контроллер в случае потери сети WiFi
  if (!isWiFi)
  {
    Serial.println("Перезагружаем контроллер после потери сети WiFi");
    ESP.restart();
  }


   // Если модуль получил информацию, считываем буфер до 32 символов
   if (radio.available()) 
   { 
      char text[34] = {0};             // переменная, хранящая текст сообщения
      yield(); 
      radio.read(&text, sizeof(text)); // считали текст сообщения
      yield(); 
      Serial.println(text);            // вывели текст в монитор порта
   }
   // Прерываем блокировку процессора ESP8266 (ESP32)
   // (на мгновение передаём управление фоновым функциям Wi-Fi и предотвращаем перезагрузку
   // от сторожевого таймера)
   yield(); 




   
  // Определяем время работы цикла задачи
  tLoop=millis()-tTime;
  // Отмечаем завершение задачи для сторожевого таймера
  finLoop = true;
  if (inumber == 1) MimicMCUhangEvent("Loop");   
}

// ******************************************************** KvizzyVid17flat ***
