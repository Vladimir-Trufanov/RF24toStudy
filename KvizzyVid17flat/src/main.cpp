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

  // Создаём объект таймера, устанавливаем его частоту отсчёта (1Mhz)
  timer = timerBegin(1000000);
  // Подключаем функцию обработчика прерывания от таймера - onTimer
  timerAttachInterrupt(timer, &onTimer);
  // Настраиваем таймер: интервал перезапуска - 20 секунд (20000000 микросекунд),
  // всегда повторяем перезапуск (третий параметр = true), неограниченное число 
  // раз (четвертый параметр = 0) 
  timerAlarm(timer, 20000000, true, 0);

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
 
  // Определяем время работы цикла задачи
  tLoop=millis()-tTime;
  // Отмечаем завершение задачи для сторожевого таймера
  finLoop = true;
  if (inumber == 1) MimicMCUhangEvent("Loop");   
}

// ******************************************************** KvizzyVid17flat ***
