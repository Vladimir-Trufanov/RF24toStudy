/** ESP32-Esp8266-Arduino                                *** with_DHT1122.h ***
 * 
 * Обеспечить работу с датчиками температуры и влажности DHT11-DHT22
 * 
 * v1.0.1, 31.10.2026                                 Автор:      Труфанов В.Е.
 * Copyright © 2026 tve                               Дата создания: 29.10.2026
**/

#pragma once

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// Определяем возможные модели датчика, убрав признак комментария у нужной строки
#define DHTTYPE11  DHT11  // DHT 11
#define DHTTYPE22  DHT22  // DHT 22 (AM2302)
#define DHTTYPE21  DHT21  // DHT 21 (AM2301)

// Настраиваем статичные параметры класса
// ----------------------------------------------------------------------------
// Определяем пин для подключения датчика
#define DHTPIN 9
// Определяем модель датчика
#define DHTTYPE DHTTYPE22
// Определяем объект, через который будем работать с датчиком
DHT_Unified dhtx(DHTPIN, DHTTYPE);
// ----------------------------------------------------------------------------

class Tdht1122 
{
  public:
  
  Tdht1122(); 
  // Снять показание влажности
  float getHumi(bool isSerial=false);  
  // Снять показание температуры        
  float getTemp(bool isSerial=false);       
  
  private:
  
  sensors_event_t event;  // событие показания датчика
  float Humi;             // показание влажности
  float TempC;            // показание температуры
};

Tdht1122::Tdht1122() 
{
  // Инициализируем датчик
  dhtx.begin();
};

// ****************************************************************************
// *                          Снять показание влажности                       *
// ****************************************************************************
float Tdht1122::getHumi(bool isSerial) 
{
  float Humi=-1;
  dhtx.humidity().getEvent(&event);
  Humi=event.relative_humidity; 
  if (isnan(Humi)) 
  {
    // в случае проблем с датчиком влажности выводим следующий текст
    Serial.println("Ошибка считывания влажности!");
  }
  else 
  {
    // При необходимости трассируем считанное значение
    if (isSerial)
    {
      Serial.print("Влажность:   "); Serial.print(Humi); Serial.println("%");
    }
  }
  return Humi;
}
// ****************************************************************************
// *                          Снять показание температуры                     *
// ****************************************************************************
float Tdht1122::getTemp(bool isSerial) 
{ 
  float TempC=-111;
  dhtx.temperature().getEvent(&event);
  TempC=event.temperature; 
  if (isnan(TempC)) 
  {
    // в случае проблем с датчиком температуры выводим следующий текст
    Serial.println("Ошибка считывания температуры!");
  }
  else 
  {
    // При необходимости трассируем считанное значение
    if (isSerial)
    {
      Serial.print("Температура: "); Serial.print(TempC); Serial.println("°C");
    }
  }
  return TempC;
}

// ********************************************************* with_DHT1122.h ***

