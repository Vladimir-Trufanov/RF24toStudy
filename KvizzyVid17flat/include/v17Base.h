/** Arduino, Esp32-C3 ******************************************* v17Base.h ***
 * 
 *                         Принять сообщение нижнего уровня и отправить на сайт 
 * 
 * v1.0.1, 15.11.2026                                 Автор:      Труфанов В.Е.
 * Copyright © 2026 tve                               Дата создания: 14.11.2026
**/

#pragma once            
#include <Arduino.h>
#include "v17WdtTask.h"        

void vBase(void* pvParameters) 
{
   for ( ;; )
   {
      // Отмечаем начало нового цикла задачи
      iBase++;
      uint32_t tTime=millis();
      // Выполняем задачу
      Serial.println("vBase");
      vTaskDelay(500/portTICK_PERIOD_MS);
      // Определяем время работы цикла задачи
      tBase=millis()-tTime;
      // Отмечаем завершение задачи для сторожевого таймера
      finBase = true;
      if (inumber == 2) MimicMCUhangEvent("vBase");   
   }
}

// ************************************************************** v17Base.h ***
