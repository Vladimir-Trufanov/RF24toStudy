/** Arduino, Esp32-C3 ******************************************* v17Base.h ***
 * 
 *                                  Принять сообщение по радио с нижнего уровня 
 *                                            и отправить его в очередь на сайт
 * 
 * v1.0.2, 19.11.2026                                 Автор:      Труфанов В.Е.
 * Copyright © 2026 tve                               Дата создания: 14.11.2026
**/

#pragma once            
#include <Arduino.h>
#include "v17WdtTask.h"  
#include <RF24.h>     
#include <Regexp.h>
#include <ArduinoJson.h>
 
// Для NodeMCU-32S
RF24 radio(22,5);      // указали номера выводов nRF24L01: CE и CSN

// Устанавливаем адрес "трубы" - адрес передатчика и явно указываем компилятору, 
// что литерал имеет тип unsigned long (как минимум 32 бита) 
// для предотвращения неявного усечения значения
const uint32_t pipeIn = (uint32_t)0x7878787878; 

char text[34];  // буфер радиосообщения
int num;        // идентификационный номер запроса

// Выполнить выборку всех json-сообщений из строки
void Deser(String Text);
// Принять сообщение по nRF24L01
void getradiomess(); 
// Инициировать прием сообщений по nRF24L01
void iniradio();
// Разобрать json-сообщение 
void match_callback(const char * match,const unsigned int length,const MatchState & ms);

void vBase(void* pvParameters) 
{
   for ( ;; )
   {
      // Отмечаем начало нового цикла задачи
      iBase++;
      uint32_t tTime=millis();

      // Принимаем сообщение по nRF24L01
      getradiomess(); 
      String jsonMess="{"+String(text)+"}"; 
      // Обрабатываем только поступившие сообщения
      if (jsonMess.length()>2)  
      {
        Serial.print("Радиоприём: "); Serial.println(jsonMess);        
        // Выполняем разборку json для определения типа сообщения
        Deser(jsonMess);
        // num=6 - влажность и температура с датчика DHT22 в подвале
        // ---------------------------------------------------------------------
        // http://localhost:100/State/?cycle=114&num=6&ctrl=207&sjson={"dht22":{"humi":87,"tempC":264}}   <-запрос
        // <State>{"cycle":114}{"num":6}{"ctrl":207}{"sjson":{dht22":{"humi":87,"tempC":264}}}</State>    <-ответ
        String parm="?cycle="+String(iBase)+"&num="+String(num)+"&ctrl=207&sjson="+jsonMess;
        String retMess=queMessa.Send(parm);
        if (retMess!=tisOk) Serial.println(retMess); 
      }
      vTaskDelay(7500/portTICK_PERIOD_MS);
      // Определяем время работы цикла задачи
      tBase=millis()-tTime;
      // Отмечаем завершение задачи для сторожевого таймера
      finBase = true;
      if (inumber == 2) MimicMCUhangEvent("vBase");   
   }
}

// ****************************************************************************
// *               Выполнить выборку всех json-сообщений из строки            *
// ****************************************************************************
void Deser(String Text)
{
  // Преобразовываем String-ответ в chars-ответ для парсинга
  int str_len = Text.length()+1; 
  char json[str_len];
  Text.toCharArray(json, str_len);
  // Создаем объект поиска соответствий
  MatchState ms(json);
  // Формируем регулярное выражение ("соответствие") для поиска json-сообщений:
  // <Lead>{"led4":{"light":25,"time":1996},"intrv":{"mode4":6900,"img":1001,"tempvl":3003,"lumin":2002,"bar":5005}}</Lead>
  const char * match="{[{\"a-zA-Z0-9:,%[}%]]*}";
  // Выполняем поиск по соответствию
  //int count = 
  ms.GlobalMatch(match,match_callback);
  //Serial.print("count: "); Serial.println(count);
}
// ***************************************************************************
// *                       Принять сообщение по nRF24L01                     *
// ***************************************************************************
void getradiomess() 
{
   // Если модуль получил информацию, считываем буфер до 32 символов
   if (radio.available()) 
   { 
      memset(text,'\0',34); 
      yield(); 
      radio.read(&text, sizeof(text)); // считали текст сообщения
      yield(); 
   }
   // Прерываем блокировку процессора ESP8266 (ESP32)
   // (на мгновение передаём управление фоновым функциям Wi-Fi и предотвращаем перезагрузку
   // от сторожевого таймера)
   yield(); 
}
// ***************************************************************************
// *                   Инициировать прием сообщений по nRF24L01              *
// ***************************************************************************
void iniradio() 
{
   Serial.println("Начало прослушивания !!!");      
   radio.begin();                    // инициировали работу nRF24L01
   radio.setChannel(0x6f);           // установили канал

   radio.openReadingPipe(0, pipeIn); // открыли "трубу" чтения
   radio.startListening();           // начали прослушивать
   Serial.println("");
}

// String s_DHT11 = "{\"dht22\":{\"humi\":"+String(jhumi)+",\"tempC\":"+String(jtempC)+"}}";  
// String s_WATER = "{\"water\":{\"level\":"+String(water)+"}}";
// String s_POWER = "{\"vcc\":{\"power\":"+String(jvcc)+"}}"; 

// ****************************************************************************
// *                          Разобрать json-сообщение                        *
// ****************************************************************************
void match_callback(const char * match,const unsigned int length,const MatchState & ms)
{
  String sjson = String(match);
  sjson = sjson.substring(0,length);
  // Трассируем выбранный json
  // Serial.print("sjson: "); Serial.println(sjson); 

  JsonDocument doc;
  deserializeJson(doc, sjson);

  String water=doc["water"];
  if (water != "null")
  {
   num=7;  
   /*
    inlight = doc["led4"]["light"];
    Prefs.putInt("inlight",inlight);
    // Serial.print("Prefs.inlight: "); Serial.println(Prefs.getInt("inlight"));

    intime = doc["led4"]["time"];
    Prefs.putInt("intime",intime);
    // Serial.print("Prefs.intime: "); Serial.println(Prefs.getInt("intime"));
   */
  }
  String vcc=doc["vcc"];
  if (vcc != "null")
  {
   num=6;  
   /*
    mode4 = doc["intrv"]["mode4"];
    Prefs.putInt("mode4",mode4);
    // Serial.print("Prefs.mode4: "); Serial.println(Prefs.getInt("mode4"));

    img = doc["intrv"]["img"];
    Prefs.putInt("img",img);
    // Serial.print("Prefs.img: "); Serial.println(Prefs.getInt("img"));
   */
  }
  String dht22=doc["dht22"];
  if (dht22 != "null")
  {
   num=8;  
  }
}

// ************************************************************** v17Base.h ***
