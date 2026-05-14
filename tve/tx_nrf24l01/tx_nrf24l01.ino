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

// Назначаем радиомодуль на передачу
bool radioNumber = 1;   // true для 2 контроллера (передатчика)
// Подключаем локальные рабочие функции радиомодуля
#include "nRF24L01_tve.h"

/*
// Функция настройки радио
void configureRadio() 
{
  radio.begin();
  // Устанавливаем низкий уровень мощности (PA — Power Amplifier), чтобы предотвратить проблемы, 
  // связанные с питанием, поскольку устройства расположены близко. Значение по умолчанию - RF24_PA_MAX.
  // Константы, соответствующие разным уровням мощности в дБм: RF24_PA_MIN — −18 дБм; RF24_PA_LOW — −12 дБм; 
  // RF24_PA_HIGH — -6 дБм; RF24_PA_MAX — 0 дБм (максимальная мощность).
  radio.setPALevel(RF24_PA_MIN);
  // По умолчанию модуль NRF24L01 работает на передающем канале 76h. 
  // Частотный диапазон модуля NRF24L01 разбит на 128 каналов с шагом 1 МГц: от 2,4 ГГц до 2,527 ГГц. 
  // Например, каналу 0 соответствует частота 2,4 ГГц, а каналу 37 — частота 2,437 ГГц. 
  radio.setChannel(0x6F); // установили канал

  // Open a writing and reading pipe on each radio, with opposite addresses
  if (radioNumber) 
  {
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1, addresses[0]);
  } 
  else 
  {
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1, addresses[1]);
  }
  // Start the radio listening for data
  radio.startListening();
  radio.printDetails();
}
*/

void setup() 
{
  Serial.begin(115200);
  Serial.println(F("\nnRF24L01: стартовала передача!"));
  printf_begin();
  configureRadio();
}

uint32_t configTimer = millis();

void loop() 
{
  if (radio.failureDetected) 
  {
    radio.failureDetected = false;
    delay(250);
    Serial.println("Radio failure detected, restarting radio");
    configureRadio();
  }
  // Every 5 seconds, verify the configuration of the radio. This can be
  // done using any setting that is different from the radio defaults.
  if (millis() - configTimer > 5000) 
  {
    configTimer = millis();
    if (radio.getDataRate() != RF24_1MBPS) 
    {
      radio.failureDetected = true;
      Serial.print("Radio configuration error detected");
    }
  }

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
