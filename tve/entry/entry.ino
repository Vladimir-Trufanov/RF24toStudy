// ******************************** ------proverka-rabotosposobnosti-nrf24l01.ino ***
// *                                                                          *
// *   ------Проверить работоспособность модуля nrf24l01 и правильность подключения *   
// *                 (установлена штатная библиотека RF24.h от TMRt20 v1.6.0: *
// *        Optimized high speed nRF24L01+ driver class documentation v1.6.0, * 
// *                                                   включающая nRF24L01.h) *
// *                                       https://dzen.ru/a/YYIaUt9Bz32SgEZy *
// *                                                                          *
// * v2.0.2, 09.05.2026                            Автор:       Труфанов В.Е. *
// * Copyright © 2024 tve                          Дата создания:  09.04.2024 *
// ****************************************************************************

#include <SPI.h>      // библиотека для обмена данными по протоколу SPI
#include <nRF24L01.h> // библиотека для nRF24L01+
#include <RF24.h>     // библиотека для радио модуля

// Подключаем прием команд из последовательного порта
#include "gcom_Serial.h"    // приём команд из последовательного порта
#include "mcom_nrf24l01.h"  // выполнение команд модулем nRF24L01+

void setup(void)
{
  Serial.begin(9600);
  Serial.println("\n\r=== entry.ino ==="); 


  /*
  int i = 0;               // вывод заголовков всех 127 каналов
  // Выводим в строку экрана младшие разряды номеров каналов (0x00=0; 0x7F=127)
  // через сдвиг числа на четыре бита вправо
  while (i < num_channels) 
  {
    printf("%x",i>>4);
    ++i;
  }
  printf("\n\r");
  // Выводим в строку экрана старшие разряды номеров каналов (0x00=0; 0x7F=127)
  // через битовое сложение номера канала и 0xF
  i = 0;
  while ( i < num_channels ) 
  {
    printf("%x",i&0xF);
    ++i;
  }
  printf("\n\r");
  */


}

// Задаем число прослушиваний каждого канала = 100
const int num_reps = 100;
// Задаем время прослушивания канала за раз в микросекундах
const int num_delmks = 128;

void loop(void)
{
  // Если команды из последовательного порта разрешают
  // выполнение фонового цикла, то выполняем цикл
  if (gcSerial()<0)
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

    int i = 0;
    while (i < num_channels) 
    {
      printf("%x", min(0xf, values[i] & 0xf));
      ++i;
    }
    printf("\n\r");
    */
  
    Serial.print("."); 
    delay(1000);
  }
}

// ******************************** proverka-rabotosposobnosti-nrf24l01.ino ***
