/** Arduino, Esp32-C3 ****************************************** v17Https.h ***
 * 
 *                                       Отправить сообщение из очереди на сайт  
 * 
 * v1.0.0, 19.11.2026                                 Автор:      Труфанов В.Е.
 * Copyright © 2026 tve                               Дата создания: 19.11.2026
**/

#pragma once            
#include <Arduino.h>
#include "v17WdtTask.h"  

void vHttps(void* pvParameters) 
{
   for ( ;; )
   {
      // Отмечаем начало нового цикла задачи
      iHttps++;
      uint32_t tTime=millis();
      // Выполняем задачу
      Serial.println("vHttps");
      vTaskDelay(500/portTICK_PERIOD_MS);
      // Определяем время работы цикла задачи
      tHttps=millis()-tTime;
      // Отмечаем завершение задачи для сторожевого таймера
      finHttps = true;
      if (inumber == 4) MimicMCUhangEvent("vHttps");   
   }
}

// ************************************************************* v17Https.h ***
