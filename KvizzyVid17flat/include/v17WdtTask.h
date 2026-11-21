/** Arduino, Esp32-C3 **************************************** v17WdtTask.h ***
 * 
 *                    Определить задачи и обеспечить работу сторожевого таймера 
 * 
 * v1.0.2, 21.11.2026                                 Автор:      Труфанов В.Е.
 * Copyright © 2026 tve                               Дата создания: 15.11.2026
**/

#pragma once            
#include <Arduino.h>
#include "ArduinoNvs.h"

static uint32_t boot_count;  // счетчик перезапусков

// Параметры локальной и собственной сети контроллера для передачи в html
//char hssid[12];     // не более 11 символов, латиница
////char hlocalIP[18];
//char hsoftAPIP[18];
// Данные о последней перезагрузке контроллера для передачи в html
// <tr><td class="tnet" id="dnreload"> Дата     </td><td class="tcam" id="dvreload"> 28.03.2026 </td></tr> 
// <tr><td class="tnet" id="tnreload"> Время    </td><td class="tcam" id="tvreload"> 08-36-58   </td></tr> 
// <tr><td class="tnet" id="nnreload"> Загрузка </td><td class="tcam" id="nvreload"> 12         </td></tr> 
//char nvreload[8];
//char dvreload[12];
//char tvreload[10];


// Определяем число, которое будет считываться в основном цикле
// с последовательного порта для зацикливания задач
volatile int inumber;

// Определяем задачи, их флаги для WDT, счетчики времен работы задач и циклов задач, числа зацикливания задачи
void vBase(void* pvParameters);  volatile bool finBase=false;  uint32_t tBase=millis();  uint32_t iBase=0;  // inumber == 2
void vHttps(void* pvParameters); volatile bool finHttps=false; uint32_t tHttps=millis(); uint32_t iHttps=0; // inumber == 3
void vPower(void* pvParameters); volatile bool finPower=false; uint32_t tPower=millis(); uint32_t iPower=0; // inumber == 4
/* loop(); */                    volatile bool finLoop=false;  uint32_t tLoop=millis();  uint32_t iLoop=0;  // inumber == 1

// Определяем заголовок для объекта таймера
hw_timer_t *timer = NULL;
// Инициируем спинлок критической секции в обработчике таймерного прерывания
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

// Инициировать сторожевой таймер 
void iniWdt();                           
// Имитировать зависания задачи 
void MimicMCUhangEvent(String NameTask); 
// Изменить данные о перезагрузках контроллера 
void setReload(); 

// ***************************************************************************
// *                  Обработать прерывание сторожевого таймера              *
// ***************************************************************************
void IRAM_ATTR onTimer() 
{
   portENTER_CRITICAL_ISR(&timerMux);
   // Если время работы сторожевого таймера завершено, все задачи завершены
   // хотя бы один раз (флаги завершения всех задач установлены), 
   // то сбрасываем флаги задач и снова устанавливаем счетчик сторожевого таймера
   if (finBase == true && finHttps == true && finPower == true && finLoop == true) 
   {
      // Сбрасываем флаги задач
      finBase = false;
      finHttps = false;
      finPower = false;
      finLoop = false;
      // "Пинаем собаку" - устанавливаем заново счетчик сторожевого таймера
      timerWrite(timer, 0);
   }
   // Иначе перезагружаем контроллер
   else 
   {
      Serial.println("Перезагрузка по счетчику ----------------------- !!!");
      delay(500);
      ESP.restart();
   }
   portEXIT_CRITICAL_ISR(&timerMux);
}
// ***************************************************************************
// *                       Инициировать сторожевой таймер                    *
// ***************************************************************************
void iniWdt() 
{
  // Создаём объект таймера, устанавливаем его частоту отсчёта (1Mhz)
  timer = timerBegin(1000000);
  // Подключаем функцию обработчика прерывания от таймера - onTimer
  timerAttachInterrupt(timer, &onTimer);
  // Настраиваем таймер: интервал перезапуска - 20 секунд (20000000 микросекунд),
  // всегда повторяем перезапуск (третий параметр = true), неограниченное число 
  // раз (четвертый параметр = 0) 
  timerAlarm(timer, 20000000, true, 0);
}

// ***************************************************************************
// *                        Имитировать зависание задачи                     *
// ***************************************************************************
void MimicMCUhangEvent(String NameTask)
{
   while (true)
   {
      Serial.print(NameTask);
      Serial.println(": зависание задачи!!!");
      vTaskDelay(1000/portTICK_PERIOD_MS);
   }
}
// ****************************************************************************
// *              Изменить данные о перезагрузках контроллера                 *
// ****************************************************************************
void setReload() 
{
  // Инициируем NVS
  NVS.begin();
  // После каждой перезагрузки увеличиваем счётчик загрузок и выводим в терминал.
  if (NVS.getInt("boot_count")) 
  {
    boot_count = NVS.getInt("boot_count"); 
    boot_count++;
  }
  else 
  {
    Serial.println("Еще нет данных о перезагрузках контроллера!");
    boot_count = 1; 
  }
  NVS.setInt("boot_count", boot_count);
  // Выбираем данные о текущем времени и дате
  struct tm timeinfo;
  getLocalTime(&timeinfo);
  // Формируем и записываем дату
  char dvreload[12];
  strftime(dvreload, sizeof(dvreload), "%d.%m.%Y", &timeinfo);
  String datereload=String(dvreload);
  NVS.setString("datereload", datereload);
  // Формируем и записываем время
  char tvreload[10];
  strftime(tvreload, sizeof(tvreload), "%H:%M:%S", &timeinfo);
  String timereload=String(tvreload);
  NVS.setString("timereload", timereload);

  // Выводим обновленную информацию

  // Выбираем номер последней перезагрузки
  char nvreload[8];
  //sprintf(nvreload, "%d", NVS.getInt("boot_count")); 
  sprintf(nvreload, "%lld", NVS.getInt("boot_count")); 
  // Выбираем дату последней перезагрузки
  //String datereload = NVS.getString("datereload");
  //datereload.toCharArray(dvreload,datereload.length()+1); 
  // Инициируем время последней перезагрузки
  //String timereload = NVS.getString("timereload");
  //timereload.toCharArray(tvreload,timereload.length()+1); 

  //Serial.print("IP - собственной сети:  ");  Serial.print(hsoftAPIP); Serial.print(" "); Serial.println(soft_ap_ssid);
  //Serial.print("IP - рабочей станции:   ");  Serial.print(hlocalIP);  Serial.print(" "); Serial.println(hssid);
  
  Serial.print("Последняя перезагрузка: ");  Serial.print(nvreload);  
  Serial.print(" ["); Serial.print(dvreload); Serial.print(", "); Serial.print(tvreload); Serial.println("]");
}

// *********************************************************** v17WdtTask.h ***
