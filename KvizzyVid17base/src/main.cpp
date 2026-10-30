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
#include <with_nRF24L01.h>

RF24 radio(6,7);                  // указали номера выводов nRF24L01: CE и CSN
TnRF24L01 nRF(radio);

// ---
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

/* v1
#define DHT11_PIN  9 
DHT dht11(DHT11_PIN, DHT11);
*/


// датчик подключен к контакту №2
#define DHTPIN 9

// выбираем модель датчика, убрав признак комментария у нужной строки
#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

// создаем объект dht, с которым будем работать
DHT_Unified dht(DHTPIN, DHTTYPE);
// ---


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

  // ---
  // инициализируем датчика
  dht.begin();
  // ---

  Serial.println("Setup");       // отметили завершение Setup
}



unsigned long timeDHT = millis();

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


  // Считываем температуру и влажность не ранее чем 12 секунд
  if ((millis()-timeDHT)>12000)
  {

    /* v1

    // Считываем влажность
    float humi  = dht11.readHumidity();
    // Считываем температуру
    float tempC = dht11.readTemperature();
    // Проверяем, успешно ли выполнено считывание
    if (isnan(tempC) || isnan(humi)) 
    {
      Serial.println("Ошибка чтения датчика DHT11!");
    } 
    else 
    {
      Serial.print("Влажность: ");
      Serial.print(humi);
      Serial.print("%");
      Serial.print("  |  ");
      Serial.print("Tемпература: ");
      Serial.print(tempC);
      Serial.println("°C");
  
      // Вычисляем параметры и формируем json-сообщение для отправки на State 
      // - "{\"dht11\":{\"humi\":46,\"tempC\":248}}"
      //      jhumi = (int)humi;     
      //    jtempC = (int)(tempC*10); 
      */


    sensors_event_t event;
    // получаем значение температуры
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
        // в случае проблем с датчиком температуры выводим следующий текст
        Serial.println("Error reading temperature!");
    }
    else {
        // вывод в COM порт текста Temperature: xxx°C
        Serial.print("Temperature: ");
        Serial.print(event.temperature);
        Serial.println("°C");
    }
    // получаем значение влажности
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
        // в случае проблем с датчиком влажности выводим следующий текст
        Serial.println("Error reading humidity!");
    }
    else {
        // вывод в COM порт текста Humidity: xxx%
        Serial.print("Humidity: ");
        Serial.print(event.relative_humidity);
        Serial.println("%");
    }

      
      timeDHT = millis();
    
  }   
}

// ******************************************************** KvizzyVid17base ***
