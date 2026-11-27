#include <Arduino.h>

// ******************************************************** KvizzyVid17base ***
// *                                                                          *
// *                Передавать показания температуры, влажности, уровня воды, *
// *                      напряжения питания из подвала дома (нижний уровень) *   
// *                                                                          *
// * v1.2.1, 24.11.2026                            Автор:       Труфанов В.Е. *
// * Copyright © 2026 tve                          Дата создания:  19.10.2026 *
// ****************************************************************************

#define isDebug true              // true - режим отладки

#include <SPI.h>                  // подключили библиотеку SPI
#include <nRF24L01.h>             // подключили библиотеку nRF24L01
#include <RF24.h>                 // подключили библиотеку RF24

#include <GyverWDT.h>

//#include <with_nRF24L01.h>

#include <with_DHT1122.h>
#include <Vid17baseCommon.h> 


// Подключаем объекты для работы с радиомодулем
RF24 radio(6,7);                  // указали номера выводов nRF24L01: CE и CSN
//TnRF24L01 nRF(radio);
// Подключаем объект для чтения с датчика температуры и влажности
Tdht1122 dhtv;

// Определяем таймаут отправки показаний
#define timeoutDHT 13000          // 13 секунд
// Определяем таймаут для сброса счетчика сторожевого таймера
#define timeoutWDT 7000           // 7 секунд

// Устанавливаем адрес "трубы" - адрес передатчика и явно указываем компилятору, 
// что литерал имеет тип unsigned long (как минимум 32 бита) 
// для предотвращения неявного усечения значения
const uint32_t pipe = (uint32_t)0x7878787878; 

/*
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
*/

void setup() 
{
  Serial.begin(115200);          // установили скор.обмена данными 115200 бод

  Serial.println("\nПрограмма стартовала, ждём 5 секунд");
  delay(5000); 
  
  radio.begin();                 // инициировали работу nRF24L01
 
  // При необходимости инициируем проверку работоспособности
  // nRF.Check_performance(true);
   
  radio.setChannel(0x6f);        // установили канал
  radio.openWritingPipe(pipe);   // открыли "трубу" отправки данных
  radio.stopListening();         // прекратили прослушивание

  // По умолчанию модуль nRF24L01 настроен на автоматическую отправку пакета 
  // подтверждения приёма - ACK. Это механизм надёжной связи: передатчик ждёт 
  // ACK от приёмника, чтобы убедиться, что пакет принят корректно.
  // Команда radio.setAutoAck(false) отключает отправку пакета подтверждения приёма.
  // radio.setAutoAck(true);

  // Включаем сторожевую собаку, таймаут ~8с
  Watchdog.enable(RESET_MODE, WDT_PRESCALER_1024); 
  Serial.println("Сторожевой таймер включён на 8 секунд");
}

uint32_t timeDHT = millis();
uint32_t timeWDT = millis();
uint32_t iLoop=0;
int ipos=0;   // 0 => dht, 1 => water, 2 => power

// С интервалом отправляем сообщение в 32 символа
// (2026-05-08 было замечено, что если оторван провод CE, то
// команда "radio.write(&text, sizeof(text));" не завершается и
// контроллер зависает)
void loop() 
{
  // Сбрасываем счетчик сторожевого таймера
  if ((millis()-timeWDT)>timeoutWDT)
  {
    Watchdog.reset();
    timeWDT = millis();
  }

  // Прослушиваем каналы
  // nRF.Check_performance();  

  // Считываем температуру и влажность, уровень воды, напряжение питания
  // не ранее чем timeoutDHT секунд
  if ((millis()-timeDHT)>timeoutDHT)
  {
    iLoop++;
    //Serial.print("\nLoop = "); Serial.println(iLoop);  
    //Serial.print("\npos  = "); Serial.println(ipos);  

    /*
    if (ipos==0)
    {
      // Формируем и отправляем json-сообщение по температуре и влажности 
      makeHumiTemp(radio); 
      ipos=1;
    }
    else if (ipos==1)
    {
      // Формируем и отправляем сообщения по уровню воды  
      makeWater(radio); 
      ipos=2;
    }
    else
    {
      // Формируем и отправляем сообщения по напряжению питания 
    */
      makePower(radio); 
    //  ipos=0;
    //}
    timeDHT = millis();
  }  
  delay(500); 
}

// ******************************************************** KvizzyVid17base ***
