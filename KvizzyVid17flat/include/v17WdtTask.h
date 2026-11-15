/** Arduino, Esp32-C3 **************************************** v17WdtTask.h ***
 * 
 *                    Определить задачи и обеспечить работу сторожевого таймера 
 * 
 * v1.0.0, 15.11.2026                                 Автор:      Труфанов В.Е.
 * Copyright © 2026 tve                               Дата создания: 15.11.2026
**/

#pragma once            
#include <Arduino.h>

// Определяем число, которое будет считываться в основном цикле
// с последовательного порта для зацикливания задач
volatile int inumber;

// Определяем задачи, их флаги для WDT, счетчики времен работы задач и циклов задач, числа зацикливания задачи
void vBase(void* pvParameters);  volatile bool finBase=false;  uint32_t tBase=millis();  uint32_t iBase=0;  // inumber == 2
void vPower(void* pvParameters); volatile bool finPower=false; uint32_t tPower=millis(); uint32_t iPower=0; // inumber == 3
/* loop(); */                    volatile bool finLoop=false;  uint32_t tLoop=millis();  uint32_t iLoop=0;  // inumber == 1

// Определяем заголовок для объекта таймера
hw_timer_t *timer = NULL;
// Инициируем спинлок критической секции в обработчике таймерного прерывания
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void MimicMCUhangEvent(String NameTask); // имитирование зависания задачи  

// ***************************************************************************
// *                  Обработать прерывание сторожевого таймера              *
// ***************************************************************************
void IRAM_ATTR onTimer() 
{
   portENTER_CRITICAL_ISR(&timerMux);
   // Если время работы сторожевого таймера завершено, все задачи завершены
   // хотя бы один раз (флаги завершения всех задач установлены), 
   // то сбрасываем флаги задач и снова устанавливаем счетчик сторожевого таймера
   if (finBase == true && finPower == true && finLoop == true) 
   {
      // Сбрасываем флаги задач
      finBase = false;
      finPower = false;
      finLoop = false;
      // "Пинаем собаку" - устанавливаем заново счетчик сторожевого таймера
      timerWrite(timer, 0);
   }
   // Иначе перезагружаем контроллер
   else 
   {
      ESP.restart();
   }
   portEXIT_CRITICAL_ISR(&timerMux);
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

// *********************************************************** v17WdtTask.h ***
