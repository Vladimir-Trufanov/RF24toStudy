#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <SPI.h>      // библиотека для обмена данными по протоколу SPI
#include <nRF24L01.h> // библиотека для nRF24L01+
#include <RF24.h>     // библиотека для радио модуля

#include "with_nRF24L01.h"

static unsigned long last_loop = millis();

// "OPPO A9 2020"; "TP-Link_B394"; "tve-DESKTOP"; "linksystve"; "linksystve";
// "b277a4ee84e8"; "18009217"    ; "Ue18-647"   ; "x93k6kq6wf"; "X93K6KQ6WF";

// 17.10.2026 IP = 10.12.16.124
const char* ssid     = "OPPO A9 2020";
const char* password = "b277a4ee84e8";

// 17.10.2026 IP = 192.168.0.108 - включён в исключений у DRWEB
// const char* ssid     = "TP-Link_B394";
// const char* password = "18009217";

// Определяем контакт для синего светодиода
#define LED_built 2
// Инициируем nRF24L01+
RF24 radio(4,5);      // 4=CE, 5=CS
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

// Сбрасываем признак передачи через OTA
bool isOTA;

void setup() 
{
  Serial.begin(115200);

  Serial.setDebugOutput(true);
  Serial.println(" ");

  Serial.println("\n\nESP.restart !!!");
  last_loop = millis();
  
  // Сбрасываем признак передачи через OTA
  isOTA = false;
  // Подсоединяем управление синим светодиодом и выключаем его
  pinMode(LED_built, OUTPUT); 
  digitalWrite(LED_built, true);          
  
  delay(5000);
  Serial.print("Будем подключаться из PlatformIO к WiFi-сети "); Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) 
  {
    Serial.println("Ошибка соединения! Перезагрузка через 5 секунд ...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() 
  {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) 
    {
      type = "sketch";
    } 
    else 
    {  // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Начало обновления " + type);
    // Включаем синий светодиод
    digitalWrite(LED_built, false);  
    // Устанавливаем признак передачи через OTA
    isOTA = true;
  });
  ArduinoOTA.onEnd([]() 
  {
    Serial.println("\nОбновление завершено");
    // Выключаем синий светодиод
    digitalWrite(LED_built, true);          
    // Сбрасываем признак передачи через OTA
    isOTA = false;
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) 
  {
    Serial.printf("Обновлено: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) 
  {
    Serial.printf("Ошибка обновления[%u]: ", error);
    if (error == OTA_AUTH_ERROR) 
    {
      Serial.println("OTA_AUTH_ERROR");
    } 
    else if (error == OTA_BEGIN_ERROR) 
    {
      Serial.println("OTA_BEGIN_ERROR");
    } 
    else if (error == OTA_CONNECT_ERROR) 
    {
      Serial.println("OTA_CONNECT_ERROR");
    } 
    else if (error == OTA_RECEIVE_ERROR) 
    {
      Serial.println("OTA_RECEIVE_ERROR");
    } 
    else if (error == OTA_END_ERROR) 
    {
      Serial.println("OTA_END_ERROR");
    }
  });

  ArduinoOTA.begin();
  Serial.print("Контроллер подключен к WiFi-сети "); Serial.println(ssid);
  Serial.print("IP адрес:                        "); Serial.println(WiFi.localIP());

  radio.begin();                  // инициировали работу nRF24L01
  // При необходимости инициируем проверку работоспособности
  // nRF.Check_performance(true);
  radio.setChannel(0x6f);         // установили канал
  radio.openReadingPipe(0, pipe); // открыли "трубу" чтения
  radio.startListening();         // начали прослушивать
  Serial.println("");
}

unsigned long imess = millis();

void _loop() 
{
  last_loop = millis();
  // Прослушиваем каналы
  // nRF.Check_performance(); 
  // Если модуль получил информацию, считываем буфер до 32 символов
  if (radio.available()) 
  { 
    char text[34] = {0};             // переменная, хранящая текст сообщения
    yield(); 
    radio.read(&text, sizeof(text)); // считали текст сообщения
    yield(); 
    Serial.println(text);            // вывели текст в монитор порта
  }
  // Прерываем блокировку процессора ESP8266
  // (на мгновение передаём управление фоновым функциям Wi-Fi и предотвращаем перезагрузку
  // от сторожевого таймера)
  yield(); 
  
  if ((millis()-imess)>8000)
  {
    Serial.println("Прошло 8 секунд!");
    imess = millis();
  }
}

void loop() 
{
  ArduinoOTA.handle();
  if (isOTA==false) _loop();
}