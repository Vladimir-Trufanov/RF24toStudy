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


#include <SPI.h>                  // подключили библиотеку SPI
#include <nRF24L01.h>             // подключили библиотеку nRF24L01
#include <RF24.h>                 // подключили библиотеку RF24

#include "with_nRF24L01.h"

RF24 radio(6,7);                  // указали номера выводов nRF24L01: CE и CSN
TnRF24L01 nRF(radio);

// Устанавливаем адрес "трубы" - адрес передатчика и явно указываем компилятору, 
// что литерал имеет тип unsigned long (как минимум 32 бита) 
// для предотвращения неявного усечения значения
const uint32_t pipe = (uint32_t)0x7878787878; 

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

void setup() 
{
  Serial.begin(115200);          // установили скор.обмена данными 115200 бод
  while (!Serial);               // для Leonardo и т.п. плат, ждёт пока откроется монитор порта
  printf_begin();                // включили вывод в поток дополнительной информации
  radio.begin();                 // инициировали работу nRF24L01
 
  // При необходимости инициируем проверку работоспособности
  // nRF.Check_performance(true);
   
  radio.setChannel(0x6f);        // установили канал
  radio.openWritingPipe(pipe);   // открыли "трубу" отправки данных
  radio.stopListening();         // прекратили прослушивание
  Serial.println("Setup");       // отметили завершение Setup
}

// С интервалом в 5 секунд отправляем сообщение в 32 символа
// (2026-05-08 было замечено, что если оторван провод CE, то
// команда "radio.write(&text, sizeof(text));" не завершается и
// контроллер зависает)
void loop() 
{
  // const char text[] = "Hello World 234567 20 2345678 31"; // Текст сообщения = 32 символа
  // const char text[] =    "Hello МИР! 234567 20 2345 31";  // Текст сообщения = 32 символа
  const char text[] =    "Hello МиР! -------------- 31";  // Текст сообщения = 32 символа
  radio.write(&text, sizeof(text));  // отослали текст
  Serial.println(text);              // вывели текст в монитор порта
  
  delay(5000);                      // задержка 
  // Прослушиваем каналы
  // nRF.Check_performance();    
}

// ******************************************************** KvizzyVid17base ***
