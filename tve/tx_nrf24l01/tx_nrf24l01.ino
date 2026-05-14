/** Arduino C/C++ ***************************************** tx_nrf24l01.ino ***
 *
 * ПЕРЕДАТЧИК - примерный эскиз для радиостанций nRF24L01+,
 * как передавать данные с одного узла на другой с обработкой сбоев.
 * 
 * v2.0.2, 14.05.2026                                 Автор:      Труфанов В.Е.
 * Copyright © 2026 tve                               Дата создания: 12.05.2026

  Радиоприемники nrf24l01+ являются достаточно надежными устройствами, но на макетных платах и т.д. с несогласованной проводкой сбои могут
  возникать случайным образом по прошествии многих часов, дней или недель. На этом примере показано, как справляться с различными сбоями и
  поддерживать радиоприемник в рабочем состоянии.

  К трем основным режимам неисправности радиоприемника относятся:
  Запись на радио: Радио не отвечает - исправлено путем добавления таймаута к внутренним функциям записи в RF24 (обработка сбоев)
  Чтение с радио: Доступно всегда возвращает значение true - Исправлено путем добавления таймаута к доступным функциям пользователем. Это реализовано внутри сети RF24Network.
  Потеряны настройки конфигурации радиостанции - исправлено путем отслеживания значения, отличающегося от значения по умолчанию, и повторной настройки радиостанции, если это значение возвращается к значению по умолчанию.

 * Arduino Nano
 * ------------
 * Скетч использует 25928 байт (80%) памяти устройства. Всего доступно 32256 байт.
 * Глобальные переменные используют 1415 байт (69%) динамической памяти, оставляя 633 байт для локальных переменных. Максимум: 2048 байт.
 * "C:\Users\Евгеньевич\AppData\Local\Arduino15\packages\arduino\tools\avrdude\8.0.0-arduino1/bin/avrdude" "-CC:\Users\Евгеньевич\AppData\Local\Arduino15\packages\arduino\tools\avrdude\8.0.0-arduino1/etc/avrdude.conf" -v -V -patmega328p -carduino "-PCOM15" -b115200 -D "-Uflash:w:C:\Users\Евгеньевич\AppData\Local\arduino\sketches\5EBB7E33EDD7F94B809681A5B41B83AC/Kvizzy900.ino.hex:i"
 * Avrdude version 8.0-arduino.1
 * Copyright see https://github.com/avrdudes/avrdude/blob/main/AUTHORS
 * 
 * v3.0.7, 10.12.2025:  26052 = 80% => 1544 => 504 [446]
 * v3.0.8, 06.04.2026:  25928 = 80% => 1415 => 633
 *
**/
#include <SPI.h>
#include "RF24.h"
#include "printf.h"

/*
Arduino Nano                       NRF24L01
-----------------------------------------------
13 [SCK]               ==>         [SCK]
12 [MISO]              ==>         [MISO]
11 [MOSI]              ==>         [MOSI]
7  [CE_PIN]            ==>         [CSN]
6  [CSN_PIN]           ==>         [CE]
*/

// Определяем переменные контакты контроллера для подключения к радиомодулю
#define CE_PIN 6   // номер контакта контроллера для подключения к пину CE радиомодуля
#define CSN_PIN 7  // номер контакта контроллера для подключения к пину CSN радиомодуля

// Назначаем радиомодуль на передачу
bool radioNumber = 1;   // true для 2 контроллера (передатчика)
// Подключаем локальные рабочие функции радиомодуля
#include "nRF24L01_tve.h"

void setup() 
{
  Serial.begin(115200);
  Serial.println(F("\nnRF24L01: стартовала передача!"));
  printf_begin();
  configureRadio();
}

void loop() 
{
  // Перезапускаем сеанс связи при обнаружении сбоя, проблемы с оборудованием  
  radio_failure_restarting();
  // Перепроверяем конфигурацию радиомодуля  
  verify_configuration_radio(); 

  radio.stopListening();  // First, stop listening so we can talk.
  Serial.println(F("Now sending"));
  unsigned long start_time = micros();  // Take the time, and send it.  This will block until complete
  if (!radio.write(&start_time, sizeof(unsigned long))) 
  {
    Serial.println(F("failed"));
  }
  radio.startListening();  // Now, continue listening

  unsigned long started_waiting_at = micros();  // Set up a timeout period, get the current microseconds
  bool timeout = false;                         // Set up a variable to indicate if a response was received or not

  while (!radio.available())  // While nothing is received
  {
    if (micros() - started_waiting_at > 200000)  // If waited longer than 200ms, indicate timeout and exit while loop
    {
      timeout = true;
      break;
    }
  }

  if (timeout) 
  {
    // Describe the results
    Serial.println(F("Failed, response timed out."));
  } 
  else 
  {
    // Grab the response, compare, and send to debugging spew
    unsigned long got_time;  // Variable for the received timestamp
    // Failure Handling
    uint32_t failTimer = millis();
    while (radio.available())  // If available() always returns true, there is a problem
    {
      if (millis() - failTimer > 250) 
      {
        radio.failureDetected = true;
        Serial.println("Radio available failure detected");
        break;
      }
      radio.read(&got_time, sizeof(unsigned long));
    }
    unsigned long end_time = micros();

    // Spew it
    Serial.print(F("Sent "));
    Serial.print(start_time);
    Serial.print(F(", Got response "));
    Serial.print(got_time);
    Serial.print(F(", Round-trip delay "));
    Serial.print(end_time - start_time);
    Serial.println(F(" microseconds"));
  }

  delay(1000);  // Try again 1s later
} 

// Arduino C/C++ ****************************************** tx_nrf24l01.ino ***
