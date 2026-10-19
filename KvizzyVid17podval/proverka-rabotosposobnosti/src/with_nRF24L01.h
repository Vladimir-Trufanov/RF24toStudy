/** Esp8266-Arduino                                     *** with_nRF24L01.h ***
 * 
 * Обеспечить работу с радио-модулем nRF24L01+
 * 
 * v4.0.2, 19.10.2026                                 Автор:      Труфанов В.Е.
 * Copyright © 2026 tve                               Дата создания: 09.04.2026
**/

#pragma once

#include <Arduino.h>
#include <RF24.h>         // библиотека для радио модуля

#define nrfCHANNELS 128   // общее число радио-каналов

class TnRF24L01 
{
  public:
  
  TnRF24L01(RF24 inradio, int numreps = 100, int numdelmks = 128); 
  void Check_performance(bool Title=false);

  private:
  
  // Отделить заголовок от данных чертой
  void dash();
  
  RF24 radio;
  uint8_t values[nrfCHANNELS];      // активность канала (количество сигналов)
  char infoline[nrfCHANNELS+1];     // инфо-строка по каналам + '\0'
  char infochar[1];                 // байт информации по каналу
  int ivalue = 0;                   // значение, связанное с каналом

  int num_reps = 100;               // число прослушиваний каждого канала
  int num_delmks = 128;             // время прослушивания канала за раз в микросекундах
};

TnRF24L01::TnRF24L01(RF24 inradio, int numreps, int numdelmks) 
{
  radio = inradio; 
  num_reps = numreps;               // число прослушиваний каждого канала
  num_delmks = numdelmks;           // время прослушивания канала за раз в микросекундах
  // Чистим инфо-строку по каналам
  memset(infoline,'\0',nrfCHANNELS+1); 
};

// ****************************************************************************
// *   Проверить работоспособность модуля nrf24l01 и правильность подключения *   
// *                 (установлена штатная библиотека RF24.h от TMRt20 v1.6.2: *
// *        Optimized high speed nRF24L01+ driver class documentation v1.6.2, * 
// *                                                   включающая nRF24L01.h) *
// *                                       https://dzen.ru/a/YYIaUt9Bz32SgEZy *
// ****************************************************************************
void TnRF24L01::Check_performance(bool Title)
{
  // Если требуется, то выводим заголовок проверочной таблицы с номерами каналов
  if (Title)
  {
    Serial.println("\nПроверка работоспособности nrf24l01+");
    // Отделяем заголовок чертой
    dash();
    // Выводим в строку экрана младшие разряды номеров каналов (0x00=0; 0x7F=127)
    // через сдвиг числа на четыре бита вправо
    int i = 0; 
    while (i < nrfCHANNELS) 
    {
      ivalue=i>>4;
      infoline[i]=char(48+ivalue); // printf("%x",i>>4);
      ++i;
    }
    Serial.println(infoline);
    // Выводим в строку экрана старшие разряды номеров каналов (0x00=0; 0x7F=127)
    // через битовое сложение номера канала и 0xF
    i = 0;
    while ( i < nrfCHANNELS ) 
    {
      ivalue = i&0xF;
      if (ivalue>9) infoline[i]=char(55+ivalue); // printf("%x",i&0xF)
      else infoline[i]=char(48+ivalue);          // printf("%x",i&0xF)
      ++i;
    }
    Serial.println(infoline);
    // Отделяем заголовок от данных чертой
    dash();
  }
  // Пробегаем по каналам заданное число раз и 
  // подсчитываем, сколько раз был сигнал на каждом канале
  memset(values, 0, sizeof(values));
  int rep_counter = num_reps;
  while (rep_counter--) 
  {
    int i = nrfCHANNELS;
    while (i--) 
    {
      radio.setChannel(i);
      radio.startListening(); // включаем прием сигнала
      delayMicroseconds(num_delmks);
      radio.stopListening();  // выключаем прием сигнала
      if (radio.testCarrier()) ++values[i];
      // Прерываем блокировку процессора ESP8266
      // (на мгновение передаём управление фоновым функциям Wi-Fi и предотвращаем перезагрузку
      // от сторожевого таймера)
      yield(); 
    }
  }
  // Выводим количества принятых сигналов на каналах
  int i = 0;
  while (i < nrfCHANNELS) 
  {
    // Если более 9 принятых сигналов на канале, то готовим вывод символа "Z"
    ivalue=values[i];
    if (ivalue>9) ivalue=42;
    // Вкладываем число сигналов в строку
    infoline[i]=char(48+ivalue); // printf("%x",i>>4);
    ++i;
  }
  Serial.println(infoline);
}
// ****************************************************************************
// *                     Отделить заголовок от данных чертой                  *
// ****************************************************************************
void TnRF24L01::dash()
{
  uint8_t i = 0;
  while (i < nrfCHANNELS) 
  {
    infoline[i] = char(45); // "-"; 
    ++i;
  }
  Serial.println(infoline);
}

// ******************************************************** with_nRF24L01.h ***
