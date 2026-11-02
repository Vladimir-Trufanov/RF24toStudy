#include <Arduino.h>

// ******************************************************** KvizzyVid17base ***
// *                                                                          *
// *                Передавать показания температуры, влажности, уровня воды, *
// *                      напряжения питания из подвала дома (нижний уровень) *   
// *                                                                          *
// * v1.1.0, 02.11.2026                            Автор:       Труфанов В.Е. *
// * Copyright © 2026 tve                          Дата создания:  19.10.2026 *
// ****************************************************************************


#include <SPI.h>                  // подключили библиотеку SPI
#include <nRF24L01.h>             // подключили библиотеку nRF24L01
#include <RF24.h>                 // подключили библиотеку RF24
#include <with_nRF24L01.h>

#include <with_DHT1122.h>
#include <Vid17baseCommon.h> 
#include <iarduino_VCC.h> 

// Подключаем объекты для работы с радиомодулем
RF24 radio(6,7);                  // указали номера выводов nRF24L01: CE и CSN
TnRF24L01 nRF(radio);
// Подключаем объект для чтения с датчика температуры и влажности
Tdht1122 dhtv;

// Определяем таймаут чтения показаний температуры и влажности
#define timeoutDHT 13000          // 13 секунд

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

}

unsigned long timeDHT = millis();

// С интервалом в 5 секунд отправляем сообщение в 32 символа
// (2026-05-08 было замечено, что если оторван провод CE, то
// команда "radio.write(&text, sizeof(text));" не завершается и
// контроллер зависает)
void loop() 
{
  // Прослушиваем каналы
  // nRF.Check_performance();    

  // Считываем температуру и влажность, уровень воды, напряжение питания
  // не ранее чем timeoutDHT секунд
  if ((millis()-timeDHT)>timeoutDHT)
  {
    // Формируем и отправляем json-сообщение по температуре и влажности 
    makeHumiTemp(radio); 
    // Делаем задержку в полсекунды до следующего сообщения
    delay(500);
    // Формировуем и отправляем сообщения по уровню воды и напряжению питания 
    makeWaterPower(radio); 

    /*
    // Резервируем буфер сообщения на 31 символ + завершающий ноль
    char text[32] = "Hello МиР! -------------- 31";  // текст сообщения = 32 символа
    //Serial.println(text);  
  
      
    // Через 1 секунду считываем уровень воды в интервале от 0 до 1023
    delay(1000);
    int water = analogRead(A0);  
    Serial.print("Уровень воды:   "); Serial.print(water); Serial.println(" ед");

    // Чистим строку сообщения
    memset(text,'\0',32); 
    String s_WATER = "{\"water\":{\"level\":"+String(water)+"}}"; Serial.print(s_WATER+"\n ");  
    // Формируем прообраз json-сообщения с показанием уровня воды - "water":{"level":697}     
    strcat(text,"\"water\":{\"level\":"); 
    strcat(text,IntToChar(water));   
    strcat(text,"}");   
    // Отсылаем сообщение
    radio.write(&text, sizeof(text));  
    Serial.println(text);              // вывели текст в монитор порта

    // Через 1 секунду снимаем напряжение батареи
    delay(1000);
    float Vcc=analogRead_VCC();
    Serial.print("Напряжение:     "); Serial.print(Vcc); Serial.println(" В");
    // Округляем напряжение до 2 знаков после запятой
    Vcc=round(Vcc * 100.0) / 100.0; 
    // Отрезаем целое в сотых долях
    int jvcc=(int)(Vcc*100);    // "491", напряжение батареи в сотых долях вольта - "Напряжение = 4.91 В"       
    // Чистим строку сообщения
    memset(text,'\0',32); 
    String s_POWER = "{\"vcc\":{\"power\":"+String(jvcc)+"}}"; Serial.print(s_POWER+"\n ");  
    // Формируем прообраз json-сообщения с показанием уровня воды - "water":{"level":697}     
    strcat(text,"\"vcc\":{\"power\":"); 
    strcat(text,IntToChar(jvcc));   
    strcat(text,"}");   
    // Отсылаем сообщение
    radio.write(&text, sizeof(text)); 
    Serial.println(text);              // вывели текст в монитор порта
    */
   
    timeDHT = millis();
  }   
}

// ******************************************************** KvizzyVid17base ***
