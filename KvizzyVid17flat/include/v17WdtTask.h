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

// Определяем задачи, их флаги для WDT, счетчики времен работы задач и циклов задач, число зацикливания задачи
/*
void vBase(void* pvParameters);  volatile bool finBase=false;  uint32_t tBase=millis();  uint32_t iBase=0;  // inumber == 2
void vPower(void* pvParameters); volatile bool finPower=false; uint32_t tPower=millis(); uint32_t iPower=0; // inumber == 3
*/
/* loop(); */                    volatile bool finLoop=false;  uint32_t tLoop=millis();  uint32_t iLoop=0;  // inumber == 3

// Инициировать задачи
void IniWdtTask();
// Имитировать зависание задачи         
void MimicMCUhangEvent(String NameTask);

// ***************************************************************************
// *                               Инициировать задачи                       *
// ***************************************************************************
void IniWdtTask()
{
  /*  
  xTaskCreatePinnedToCore(
  vBase,                    // Task function
  "vBase",                  // Task name
  1024,                     // Stack size
  NULL,                     // Parameters passed to the task function
  4,                        // Priority
  NULL,                     // Task handle
  0); 

  xTaskCreatePinnedToCore(
  vPower,                   // Task function
  "vPower",                 // Task name
  1024,                     // Stack size
  NULL,                     // Parameters passed to the task function
  4,                        // Priority
  NULL,                     // Task handle
  0); 
  */
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
