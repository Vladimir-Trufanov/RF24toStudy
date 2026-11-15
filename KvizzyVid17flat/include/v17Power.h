/** Arduino, Esp32-C3 ****************************************** v17Power.h ***
 * 
 *                            Определить напряжение питания и отправить на сайт 
 * 
 * v1.0.0, 14.11.2026                                 Автор:      Труфанов В.Е.
 * Copyright © 2026 tve                               Дата создания: 14.11.2026
**/

#pragma once            
#include <Arduino.h>
#include "v17WdtTask.h"        

/*
void vPower(void* pvParameters) 
{
   for ( ;; )
   {
      // Отмечаем начало нового цикла задачи
      iPower++;
      uint32_t tTime=millis();
      // Выполняем задачу
      Serial.println("vPower");
      vTaskDelay(500/portTICK_PERIOD_MS);
      // Определяем время работы цикла задачи
      tPower=millis()-tTime;
      // Отмечаем завершение задачи для сторожевого таймера
      finPower = true;
      if (inumber == 3) MimicMCUhangEvent("vPower");   
   }
}
*/

// ************************************************************* v17Power.h ***
