/** ESP32-Esp8266-Arduino                             *** Vid17baseCommon.h ***
 * 
 * Выделенные общие функции нижнего уровня
 * 
 * v1.0.0, 02.11.2026                                 Автор:      Труфанов В.Е.
 * Copyright © 2026 tve                               Дата создания: 02.11.2026
**/

#pragma once

#include <Arduino.h>
#include <RF24.h>                 // подключили библиотеку RF24
#include <iarduino_VCC.h> 
#include <with_DHT1122.h>

// Подключаем объект для чтения с датчика температуры и влажности
Tdht1122 dht;

// Преобразовать беззнаковое  целое в строку символов
char* IntToChar(uint32_t numbIn); 
// Сформировать и отправить json-сообщение по температуре и влажности
void makeHumiTemp(RF24 radio); 
// Сформировать и отправить сообщения по уровню воды и напряжению питания 
void makeWaterPower(RF24 radio); 

// ****************************************************************************
// *            Преобразовать беззнаковое  целое в строку символов            *
// ****************************************************************************
char charNumby[10]; // char[9]+'\0'
char* IntToChar(uint32_t numbIn) 
{
  uint32_t numby=numbIn;
  memset(charNumby,'\0',10); 
  if (numby>999999999) numby=999999999;
  String(numby).toCharArray(charNumby,10);
  return charNumby;
}
// ****************************************************************************
// *    Сформировать и отправить json-сообщение по температуре и влажности    *
// ****************************************************************************
void makeHumiTemp(RF24 radio) 
{
  // Резервируем буфер сообщения на 31 символ + завершающий ноль
  char text[32] = "Hello МиР! -------------- 31";  // текст сообщения = 32 символа
  //Serial.println(text);  
  // Округляем влажность до целого
  float humi = dht.getHumi(true); humi = round(humi);
  // Отрезаем целое
  int jhumi=(int)humi;         // "46", процент влажности - "Влажность = 46%"
  // Округляем температуру до десятых долей
  float tempC = dht.getTemp(true); tempC = round(tempC*10.0)/10.0;
  // Отрезаем целое десятых долей 
  int jtempC=(int)(tempC*10);  // "248", температура в десятых градусах цельсия - "Температура = 24.8°C"       
  // Чистим строку сообщения
  memset(text,'\0',32); 
  String s_DHT11 = "{\"dht22\":{\"humi\":"+String(jhumi)+",\"tempC\":"+String(jtempC)+"}}"; Serial.print(s_DHT11+"\n ");  
  // Формируем прообраз json-сообщения с показаниями температуры и влажности - "dht22":{"humi":46,"tempC":248}     
  strcat(text,"\"dht22\":{\"humi\":"); 
  strcat(text,IntToChar(jhumi));   
  strcat(text,",\"tempC\":"); 
  strcat(text,IntToChar(jtempC));   
  strcat(text,"}");   
  // Отсылаем сообщение
  radio.write(&text, sizeof(text));  
  Serial.println(text);              // вывели текст в монитор порта
}
// ****************************************************************************
// *  Сформировать и отправить сообщения по уровню воды и напряжению питания  *
// ****************************************************************************
void makeWaterPower(RF24 radio) 
{
  // Резервируем буфер сообщения на 31 символ + завершающий ноль
  char text[32] = "Hello МиР! -------------- 31";  // текст сообщения = 32 символа
  //Serial.println(text);  

  // Считываем уровень воды в интервале от 0 до 1023
  int water = analogRead(A0);  
  Serial.print("Уровень воды:   "); Serial.print(water); Serial.println(" ед");
  // Чистим строку сообщения
  memset(text,'\0',32); 
  String s_WATER = "{\"water\":{\"level\":"+String(water)+"}}"; Serial.print(s_WATER+"\n ");  
  // Формируем прообраз json-сообщения с показанием уровня воды - "water":{"level":697}     
  strcat(text,"\"water\":{\"level\":"); 
  strcat(text,IntToChar(water));   
  strcat(text,"}");   
  // Отсылаем сообщение
  radio.write(&text, sizeof(text));  
  Serial.println(text); // вывели текст в монитор порта
  // Делаем задержку в полсекунды до следующего сообщения
  delay(500);

  // Снимаем напряжение батареи
  delay(1000);
  float Vcc=analogRead_VCC();
  Serial.print("Напряжение:     "); Serial.print(Vcc); Serial.println(" В");
  // Округляем напряжение до 2 знаков после запятой
  Vcc=round(Vcc * 100.0) / 100.0; 
  // Отрезаем целое в сотых долях
  int jvcc=(int)(Vcc*100); // "491", напряжение батареи в сотых долях вольта - "Напряжение = 4.91 В"       
  // Чистим строку сообщения
  memset(text,'\0',32); 
  String s_POWER = "{\"vcc\":{\"power\":"+String(jvcc)+"}}"; Serial.print(s_POWER+"\n ");  
  // Формируем прообраз json-сообщения с показанием уровня воды - "water":{"level":697}     
  strcat(text,"\"vcc\":{\"power\":"); 
  strcat(text,IntToChar(jvcc));   
  strcat(text,"}");   
  // Отсылаем сообщение
  radio.write(&text, sizeof(text)); 
  Serial.println(text); // вывели текст в монитор порта
}

// ****************************************************** Vid17baseCommon.h ***

