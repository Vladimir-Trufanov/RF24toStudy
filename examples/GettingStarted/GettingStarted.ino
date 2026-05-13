/*
 * See documentation at https://nRF24.github.io/RF24
 * See License information at root directory of this library
 * Author: Brendan Doherty (2bndy5)
 */

/**
 * Простой пример передачи данных с одного приемопередатчика nRF24L01 на другой.
 *
 * Этот пример был написан для использования на 2 устройствах, действующих как "узлы".
 * Используйте последовательный монитор для изменения поведения каждого узла.
 */

#include <SPI.h>
#include "printf.h"
#include "RF24.h"

#define CE_PIN 6
#define CSN_PIN 7
RF24 radio(CE_PIN, CSN_PIN);

// Определяем адреса, которые будут использоваться для пары
uint8_t address[][6] = {"1Node", "2Node"};
// Определяем переменную, которая однозначно определяет, какой адрес эта радиостанция будет использовать для передачи
bool radioNumber = 1;  // 0 uses address[0] to transmit, 1 uses address[1] to transmit
// Используем для управления, является ли этот узел отправляющим или принимающим
bool role = false;     // true = TX role, false = RX role
// Определяем полезную нагрузку, как число с плавающей запятой, которое будет увеличиваться при каждой успешной передаче
float payload = 0.0;

void setup() 
{
  Serial.begin(115200);
  while (!Serial) 
  {
    // some boards need to wait to ensure access to serial over USB
  }
  // Инициализируем приемопередатчик по шине SPI
  if (!radio.begin()) 
  {
    Serial.println(F("Радиооборудование не отвечает!"));
    while (1) {}  // удержание в бесконечном цикле
  }
  // Выводим подсказку
  Serial.println(F("RF24/examples/Приступаем к работе"));
  // Устанавливаем номер радиостанции с помощью последовательного монитора при запуске
  Serial.println(F("Какой адрес для передачи использовать? '0' or '1'. По умолчанию '0'"));
  while (!Serial.available()) 
  {
    // wait for user input
  }
  char input = Serial.parseInt();
  radioNumber = input == 1;
  Serial.print(F("radioNumber = "));
  Serial.println((int)radioNumber);

  Serial.println(F("*** Введите 'T', чтобы начать передачу на другой узел"));

  // Устанавливаем наименьшую мощность передатчика, 
  // чтобы попытаться предотвратить проблемы, связанные с питанием, 
  // поскольку эти примеры, скорее всего, выполняются с узлами, расположенными 
  // в непосредственной близости друг от друга.
  radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default.
  // Экономим время передачи, настроивая радиостанцию на передачу только того количества байт, 
  // которое нужно для передачи числа с плавающей запятой
  radio.setPayloadSize(sizeof(payload));  // float datatype occupies 4 bytes

  // set the TX address of the RX node for use on the TX pipe (pipe 0)
  // !!! установите TX-адрес узла RX для использования в канале TX (канал 0).
  radio.stopListening(address[radioNumber]);  // put radio in TX mode
  // !!! set the RX address of the TX node into a RX pipe
  radio.openReadingPipe(1, address[!radioNumber]);  // using pipe 1
  // ----дополнительная настройка, специфичная для роли RX узла
  if (!role) 
  {
    radio.startListening();  // put radio in RX mode
  }

  // For debugging info
  // printf_begin();             // needed only once for printing details
  // radio.printDetails();       // (smaller) function that prints raw register values
  // radio.printPrettyDetails(); // (larger) function that prints human readable data
}  

void loop() 
{
  if (role) 
  {
    // This device is a TX node

    unsigned long start_timer = micros();                // start the timer
    bool report = radio.write(&payload, sizeof(float));  // transmit & save the report
    unsigned long end_timer = micros();                  // end the timer

    if (report) 
    {
      Serial.print(F("Transmission successful! "));  // payload was delivered
      Serial.print(F("Time to transmit = "));
      Serial.print(end_timer - start_timer);  // print the timer result
      Serial.print(F(" us. Sent: "));
      Serial.println(payload);  // print payload sent
      payload += 0.01;          // increment float payload
    } else {
      Serial.println(F("Transmission failed or timed out"));  // payload was not delivered
    }

    // to make this example readable in the serial monitor
    delay(1000);  // slow transmissions down by 1 second

  } 
  else 
  {
    // This device is a RX node

    uint8_t pipe;
    if (radio.available(&pipe)) {              // is there a payload? get the pipe number that received it
      uint8_t bytes = radio.getPayloadSize();  // get the size of the payload
      radio.read(&payload, bytes);             // fetch payload from FIFO
      Serial.print(F("Received "));
      Serial.print(bytes);  // print the size of the payload
      Serial.print(F(" bytes on pipe "));
      Serial.print(pipe);  // print the pipe number
      Serial.print(F(": "));
      Serial.println(payload);  // print the payload's value
    }
  }  // role

  if (Serial.available()) {
    // change the role via the serial monitor

    char c = toupper(Serial.read());
    if (c == 'T' && !role) {
      // Become the TX node

      role = true;
      Serial.println(F("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK"));
      radio.stopListening();

    } else if (c == 'R' && role) {
      // Become the RX node

      role = false;
      Serial.println(F("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK"));
      radio.startListening();
    }
  }

}  
