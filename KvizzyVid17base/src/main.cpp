#include <Arduino.h>

// ******************************************************** KvizzyVid17base ***
// *                                                                          *
// *   -------Проверить работоспособность модуля nrf24l01 и правильность подключения *   
// *                 ------ (установлена штатная библиотека RF24.h от TMRt20 v1.6.2: *
// *        ---- Optimized high speed nRF24L01+ driver class documentation v1.6.2, * 
// *                                                  ---- включающая nRF24L01.h) *
// *                                                                          *
// * v1.0.0, 19.10.2026                            Автор:       Труфанов В.Е. *
// * Copyright © 2026 tve                          Дата создания:  19.10.2026 *
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

RF24 radio(6,7);      // 6=CE, 7=CS
//TnRF24L01 nRF(radio);

void setup()
{
  Serial.begin(115200);
  while (!Serial);
  Serial.println("\n-------Проверка работоспособности nrf24l01");
    
  printf_begin();
  radio.begin();
  radio.setChannel(0x6f); // Устанавливаем канал
  radio.openWritingPipe(0x0123456789LL); // Открываем трубу для записи (адрес должен совпадать с приёмником!)
  
  /*
  radio.setAutoAck(false);
  radio.startListening();  // включаем прием сигнала
  radio.printDetails();    // если правильно соединили, то увидите настройки модуля
  delay(10000);            // задержка на 10 секунд
  radio.stopListening();   // выключаем прием сигнала
  */

  //nRF.Check_performance(true);
}

void loop()
{
  if (radio.available()) 
  {
    uint8_t data;
    uint8_t size = radio.read(data, sizeof(data));
    Serial.println("Получено:");
    for (int i = 0; i < size; i++) 
    {
      Serial.print(data[i]);
    }
    Serial.println();
    radio.write(data, size); // Отправляем данные обратно (подтверждение)
  }
  //nRF.Check_performance();
}

// ******************************************************** KvizzyVid17base ***
