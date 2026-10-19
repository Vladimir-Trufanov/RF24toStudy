#include <Arduino.h>

// ********************************************* proverka-rabotosposobnosti ***
// *                                                                          *
// *   Проверить работоспособность модуля nrf24l01 и правильность подключения *   
// *                 (установлена штатная библиотека RF24.h от TMRt20 v1.6.2: *
// *        Optimized high speed nRF24L01+ driver class documentation v1.6.2, * 
// *                                                   включающая nRF24L01.h) *
// *                                       https://dzen.ru/a/YYIaUt9Bz32SgEZy *
// *                                                                          *
// * v3.1.0, 18.10.2026                            Автор:       Труфанов В.Е. *
// * Copyright © 2026 tve                          Дата создания:  09.04.2026 *
// ****************************************************************************

#include <SPI.h>      // библиотека для обмена данными по протоколу SPI
#include <nRF24L01.h> // библиотека для nRF24L01+
#include <RF24.h>     // библиотека для радио модуля

#include "with_nRF24L01.h"

// Настраиваем вывод в поток 
#if defined(__AVR__)
  // для AVR (классические Arduino)
  int serial_putc(char c, FILE *) 
  {
    Serial.write(c);
    return c;
  }
  void printf_begin( ) 
  {
    fdevopen(&serial_putc, 0);
  }
#elif defined(ESP8266)
  // для ESP8266
  void printf_begin( ) 
  {
    Serial.setDebugOutput(true);
  }
#else
  // другой код
  void printf_begin( ) 
  {
  }
#endif

//RF24 radio(6,7);      // 6=CE, 7=CS
RF24 radio(4,5);      
TnRF24L01 nRF(radio);

//const uint8_t num_channels = 128;
//uint8_t values[num_channels];
//char infoline[128+1]; char infochar[1]; int ivalue = 0;

void setup(void)
{
  Serial.begin(115200);
  //Serial.println("\nПроверка работоспособности nrf24l01");
    
  printf_begin();
  radio.begin();
  radio.setAutoAck(false);
  radio.startListening();  // включаем прием сигнала
  radio.printDetails();    // если правильно соединили, то увидите настройки модуля
  delay(10000);            // задержка на 10 секунд
  radio.stopListening();   // выключаем прием сигнала

  // Готовим переменные для вывода заголовков всех 127 каналов
  //int i = 0; 
  //memset(infoline,'\0',129); 

  /*
  // Выводим в строку экрана младшие разряды номеров каналов (0x00=0; 0x7F=127)
  // через сдвиг числа на четыре бита вправо
  while (i < num_channels) 
  {
    ivalue=i>>4;
    infoline[i]=char(48+ivalue); // printf("%x",i>>4);
    ++i;
  }
  Serial.println(infoline);
  // Выводим в строку экрана старшие разряды номеров каналов (0x00=0; 0x7F=127)
  // через битовое сложение номера канала и 0xF
  i = 0;
  while ( i < num_channels ) 
  {
    ivalue = i&0xF;
    if (ivalue>9) infoline[i]=char(55+ivalue); // printf("%x",i&0xF)
    else infoline[i]=char(48+ivalue);          // printf("%x",i&0xF)
    ++i;
  }
  Serial.println(infoline);
  // Отделяем заголовок от данных чертой
  i = 0;
  while ( i < num_channels ) 
  {
    infoline[i] = char(45); // "-"; 
    ++i;
  }
  Serial.println(infoline);
  */
  nRF.Check_performance(true);
}

// Задаем число прослушиваний каждого канала = 100
//const int num_reps = 100;
// Задаем время прослушивания канала за раз в микросекундах
//const int num_delmks = 128;



void loop(void)
{

  /*
  // Пробегаем по каналам заданное число раз и 
  // подсчитываем, сколько раз был сигнал на каждом канале
  memset(values, 0, sizeof(values));
  int rep_counter = num_reps;
  while (rep_counter--) 
  {
    int i = num_channels;
    while (i--) 
    {
      radio.setChannel(i);
      radio.startListening(); // включаем прием сигнала
      delayMicroseconds(num_delmks);
      radio.stopListening();  // выключаем прием сигнала
      if (radio.testCarrier()) ++values[i];
    }
  }
  // Выводим количества принятых сигналов на каналах
  int i = 0;
  while (i < num_channels) 
  {
    ivalue=min(0xf, values[i] & 0xf);
    infoline[i]=char(48+ivalue); // printf("%x",i>>4);
    //printf("%x", min(0xf, values[i] & 0xf));
    ++i;
  }
  Serial.println(infoline);
  */

  nRF.Check_performance();
  //Serial.println("                                                                           infoline");

}

// ********************************************* proverka-rabotosposobnosti ***
