/** Esp8266-Arduino                                     *** with_nRF24L01.h ***
 * 
 * Обеспечить работу с радио-модулем nRF24L01+
 * 
 * v4.0.0, 18.10.2026                                 Автор:      Труфанов В.Е.
 * Copyright © 2026 tve                               Дата создания: 09.04.2026
**/

#pragma once

#include <Arduino.h>
#include <SPI.h>      // библиотека для обмена данными по протоколу SPI
#include <nRF24L01.h> // библиотека для nRF24L01+
#include <RF24.h>     // библиотека для радио модуля

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
    }
  }
  // Выводим количества принятых сигналов на каналах
  int i = 0;
  while (i < nrfCHANNELS) 
  {
    ivalue=min(0xf, values[i] & 0xf);
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




/*
#include "inimem.h"

// sm - префикс спрайта сообщений, поступающих в CYD
#define smLINEHEIGHT  16    // высота строки в спрайте (px)
#define smLINEOFFSET   2    // смещение текста от левого края спрайта (px)
#define smMAXLINE     13    // число строк в спрайте
#define smWIDTH      304    // ширина спрайта сообщений 
#define smHEIGHT     208    // высота спрайта
#define smTOP          0    // позиция по вертикали размещения спрайта на экране 
#define smLEFT        16    // позиция по горизонтали

TFT_eSprite stext3 = TFT_eSprite(&tft);

class TSprite_Main 
{
  public:

  TSprite_Main(); 
  void View(char* line0);
  void viewLine(char* line0);

  private:

  char lineIn[smLINESIZE];      // буфер входного сообщения
  char fill[smLINESIZE];        // заполнитель строки
  char line[smMAXLINE][smLINESIZE] =   // буфер всех сообщений экрана дисплея
  {
    "Text",
    "Привет Hello world",
    "Line 2",
    "0123456 10 123456 20 123456 30 123456 40",
    "04 Это пробный текст на русском языке для CYD",   
    "0123456 10 123456 20 123456 30 123456 40 123456 50 123456 60",
    "06 Это пробный текст на русском языке для CYD",   
    "0123456 10 123456 20 123456 30 123456 40 123456 50 123456 60",
    "08 Это пробный текст на русском языке для CYD",   
    "0123456 10 123456 20 123456 30 123456 40 123456 50 123456 60",
    "10 Это пробный текст на русском языке для CYD",   
    "11 0123456 10 123456 20 123456 30 123456 40 123456 50 123456 60",
    "12 0123456 10 123456 20 123456 30 123456 40 123456 50 123456 60",
  };
};

TSprite_Main::TSprite_Main() 
{
  // Чистим заполнитель
  memset(fill,32,smLINESIZE-1); 
  fill[smLINESIZE-1]='\0';
};

void TSprite_Main::View(char* line0)
{
  stext3.setColorDepth(8);
  stext3.createSprite(smWIDTH, smHEIGHT);
  if (stext3.created())
  {
    //getheap("Создан спрайт");
    // Заполняем буфер памяти, выделенный под спрайт, заданным цветом
    stext3.fillSprite(TFT_BLACK);
    // Отключаем перенос текста и по горизонтали и по вертикали 
    stext3.setTextWrap(false, false);
    // Определяем цвет текста с прозрачным фоном  
    stext3.setTextColor(TFT_WHITE,TFT_BLACK,true); 
    // Загружаем шрифт в память спрайта
    #ifdef FontFromFile
      stext3.loadFont(HuaweiSans16);   
    #else
      stext3.loadFont("HuaweiSans16");   
    #endif
    // Чистим заполнитель
    //memset(fill,32,smLINESIZE-1); 
    //fill[smLINESIZE-1]='\0';

    viewLine(line0);
    stext3.pushSprite(smLEFT,smTOP);
    // Выгружаем шрифт из памяти и удаляем спрайт
    stext3.unloadFont();         
    stext3.deleteSprite();
  }
  else
  {
    Serial.println("НЕ ПОЛУЧИЛОСЬ!");
  }
}
//
void TSprite_Main::viewLine(char* line0)
{
  // В окончательном варианте работать через семафор на формирование и вывод спрайта

  uint8_t ypoint;

  // Сдвигаем строки спрайта вниз
  for (uint8_t jline = smMAXLINE-1; jline > 0; jline--) 
  {
    memcpy(line[jline],line[jline-1], sizeof(line[jline-1]));
  }
  // Заполняем 0 строку
  memset(line[0],'\0',smLINESIZE); 
  memcpy(line[0],line0,smLINESIZE);

  // Размещаем строки в спрайте
  for (uint8_t jline = 0; jline < smMAXLINE; jline++) 
  {
    ypoint=jline*smLINEHEIGHT;
    stext3.setCursor(smLINEOFFSET,ypoint);
    stext3.print(fill);
    stext3.setCursor(smLINEOFFSET,ypoint);
    stext3.print(line[jline]);
  }
}

TSprite_Main ypsMain;
char Spase[smLINESIZE]; // буфер чистки строки

void taskMain (void *pvParameters) 
{
  // Заполняем буфер чистки строки
  memset(Spase,32,smLINESIZE-1); 
  Spase[smLINESIZE-1]='\0';
  while (1) 
  {
    TickType_t start = xTaskGetTickCount();
    // Если НЕ команда "Перейти в режим перепрошивки по OTA"
    if (inumber!=fOTA)
    {
      / *
      if (xSemaphoreTake(messMutex, (200 * portTICK_PERIOD_MS))) 
      { 
        // Выбираем сообщение единственный раз в интервале ожидания и приема сообщения
        // Serial.print("copyCalc==messCalc: "); Serial.print(copyCalc); Serial.print("="); Serial.println(messCalc);
        if (!(copyCalc==messCalc))
        {
          memcpy(CYD_message.line, CtrlMessage.line, smLINESIZE);
          //Serial.println("***");
          //Serial.println(CtrlMessage.line);
          Serial.println(CYD_message.line);
          //Serial.println("***");
          ypsMain.View(CYD_message.line);
          copyCalc=messCalc;
       }
        xSemaphoreGive(messMutex);  
      }
      else 
      {  
        //Serial.print ("Task 2: Mutex не захвачен ");
        //Serial.println (xTaskGetTickCount());
      }
      * /
      if (xSemaphoreTake(messMutex, (200 * portTICK_PERIOD_MS))) 
      { 
        tft.setCursor(17,17); tft.print(ipStr); tft.print(": "); tft.print(messCalc); tft.print(Spase);
        tft.setCursor(17,34); tft.print(CtrlMessage.line); tft.print(Spase);
        xSemaphoreGive(messMutex);  
      }
      else 
      {  
        //Serial.print ("Task 2: Mutex не захвачен ");
        //Serial.println (xTaskGetTickCount());
      }  
    }
    
    // Отмечаем завершение цикла задачи для сторожевого таймера
    flag[ftaskMain] = 1;
    TickType_t duration = xTaskGetTickCount() - start;
    //Serial.printf("Длительность taskMain(): %d ms\n", duration * portTICK_PERIOD_MS);
    // Если было введено число=fmessageReceived
    if (inumber == ftaskMain) MimicMCUhangEvent("taskMain");   
    vTaskDelay(1063);
  }
}
*/

// ******************************************************** with_nRF24L01.h ***
