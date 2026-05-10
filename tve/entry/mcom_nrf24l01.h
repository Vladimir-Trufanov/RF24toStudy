/** Arduino C/C++ ***************************************** mcom_nrf24l01.h ***
 *
 * Выполнить контрольные действия проверки работоспособности nRF24L01+ 
 * по команде из последовательного порта 
 * 
 * v1.0.1, 09.05.2026                                 Автор:      Труфанов В.Е.
 * Copyright © 2026 tve                               Дата создания: 08.05.2026
 *
**/

#ifndef MCOMNRF24L01
#define MCOMNRF24L01
#pragma once  

#include <SPI.h>      // библиотека для обмена данными по протоколу SPI
#include <nRF24L01.h> // библиотека для nRF24L01+
#include <RF24.h>     // библиотека для радио модуля

// Проверить работоспособность модуля Check_nRF24L01+ и правильность подключения    
void Check_nRF24L01();

RF24 radio(6,7);    
const uint8_t num_channels = 128;
uint8_t values[num_channels];

// ****************************************************************************
// *  Проверить работоспособность модуля nrf24l01 и правильность подключения  *   
// *         (установлена штатная библиотека RF24.h от TMRt20 v1.6.0:         *
// *     Optimized high speed nRF24L01+ driver class documentation v1.6.0,    * 
// *                              включающая nRF24L01.h)                      *
// ****************************************************************************
int serial_putc(char c, FILE *) 
{
  Serial.write(c);
  return c;
}
void printf_begin(void) 
{
  fdevopen(&serial_putc, 0);
}
void Check_nRF24L01()
{
  Serial.println(""); 
  Serial.println("РАДИОМОДУЛЬ nRF24L01+"); 
  Serial.println("---------------------"); 
  printf_begin();
  radio.begin();
  radio.setAutoAck(false);
  radio.startListening();  // включили прием сигнала
  radio.printDetails();    // если правильно соединили, то увидим настройки модуля
  delay(5000);             // задержка на 5 секунд
  radio.stopListening();   // выключили прием сигнала
  Serial.println(""); 
}

#endif 

// ******************************************************** mcom_nrf24l01.h ***

