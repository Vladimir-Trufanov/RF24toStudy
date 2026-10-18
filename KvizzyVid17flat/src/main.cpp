#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <SPI.h>      // библиотека для обмена данными по протоколу SPI
#include <nRF24L01.h> // библиотека для nRF24L01+
#include <RF24.h>     // библиотека для радио модуля

static unsigned long last_loop = millis();

/*
#include <Ticker.h>
Ticker tickerOSWatch;

#define OSWATCH_RESET_TIME 41   // таймер будет разрешать 22 секунды на работу одного цикла
                                // иначе перезагрузка


//void ICACHE_RAM_ATTR osWatch(void) 
void IRAM_ATTR osWatch(void) 
{
  unsigned long t = millis();
  unsigned long last_run = t - last_loop; // abs(t - last_loop);
  if(last_run >= (OSWATCH_RESET_TIME * 1000)) 
  {
    // сохраните результат здесь в EEPROM или в памяти часов реального времени, если необходимо
    ESP.restart(); // обычная перезагрузка 
    //ESP.reset(); // аппаратный сброс
  }
}

*/


// "OPPO A9 2020"; "TP-Link_B394"; "tve-DESKTOP"; "linksystve"; "linksystve";
// "b277a4ee84e8"; "18009217"    ; "Ue18-647"   ; "x93k6kq6wf"; "X93K6KQ6WF";

// 17.10.2026 IP = 10.12.16.124
// const char* ssid     = "OPPO A9 2020";
// const char* password = "b277a4ee84e8";

// 17.10.2026 IP = 192.168.0.108 - включён в исключений у DRWEB
const char* ssid     = "TP-Link_B394";
const char* password = "18009217";

// Определяем контакт для синего светодиода
#define LED_built 2
// Инициируем nRF24L01+
RF24 radio(4,5);      // 4=CE, 5=CS
const uint8_t num_channels = 128;
uint8_t values[num_channels];
// Сбрасываем признак передачи через OTA
bool isOTA;

/*
int serial_putc(char c, FILE *) 
{
  Serial.write(c);
  return c;
}
void printf_begin(void) 
{
  fdevopen(&serial_putc, 0);
}
*/

void setup() 
{
  Serial.begin(115200);

  Serial.setDebugOutput(true);
  Serial.println(" ");

  Serial.println("\n\nESP.restart !!!");
  last_loop = millis();
  //tickerOSWatch.attach_ms(((OSWATCH_RESET_TIME / 3) * 1000), osWatch);


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

  //printf_begin();
  radio.begin();
  radio.setAutoAck(false);
  radio.startListening();  // включаем прием сигнала
  radio.printDetails();    // если правильно соединили, то увидите настройки модуля
  delay(10000);            // задержка на 10 секунд
  radio.stopListening();   // выключаем прием сигнала

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

  int i = 0;               // вывод заголовков всех 127 каналов
  // Выводим в строку экрана младшие разряды номеров каналов (0x00=0; 0x7F=127)
  // через сдвиг числа на четыре бита вправо
  while (i < num_channels) 
  {
    printf("%x",i>>4);
    ++i;
  }
  printf("\n\r");
  //Serial.println("\n***112***\n");
  // Выводим в строку экрана старшие разряды номеров каналов (0x00=0; 0x7F=127)
  // через битовое сложение номера канала и 0xF
  i = 0;
  while ( i < num_channels) 
  {
    printf("%x",i&0xF);
    ++i;
  }
  printf("\n\r");
}

// Задаем число прослушиваний каждого канала = 100
const int num_reps = 100; //100;
// Задаем время прослушивания канала за раз в микросекундах
const int num_delmks = 128; //128;

unsigned long imess = millis();

void _loop() 
{

  last_loop = millis();
  
  //while(1) delay(1000); // это запустит osWatch
  //delay(1000);
  //Serial.println("Прошла 21 секунда !!!");




  //if ((millis()-imess)>7000)
  //{
  //  Serial.println("Прошло 7 секунд!");
  //  imess = millis();
  //}

  //while(1){}
  //delay(50000);

  //Serial.println("Задержка");
  //delay(9000);
  //Serial.println("Проехали");

  unsigned long cikBegin = millis();
  
  // Пробегаем по каналам заданное число раз и 
  // подсчитываем, сколько раз был сигнал на каждом канале
  memset(values, 0, sizeof(values));
  int rep_counter = num_reps;
  while (rep_counter--) 
  {
    int i = num_channels;
    while (i--) 
    {
      radio.setChannel(i);
      radio.startListening(); // включаем прием сигнала
      delayMicroseconds(num_delmks);
      radio.stopListening();  // выключаем прием сигнала
      if (radio.testCarrier()) ++values[i];
      yield(); 
    }
  }

  int i = 0;
  while (i < num_channels) 
  {
    printf("%x", min(0xf, values[i] & 0xf));
    ++i;
  }
  printf("\n\r");

  Serial.print("delta = "); Serial.println((millis()-cikBegin));

  
  /*
  

  delay(2000);
  */
 
}

void loop() 
{
  ArduinoOTA.handle();
  if (isOTA==false) _loop();

  /*
  // Пробегаем по каналам заданное число раз и 
  // подсчитываем, сколько раз был сигнал на каждом канале
  memset(values, 0, sizeof(values));
  int rep_counter = num_reps;
  while (rep_counter--) 
  {
    int i = num_channels;
    while (i--) 
    {
      radio.setChannel(i);
      radio.startListening(); // включаем прием сигнала
      delayMicroseconds(num_delmks);
      radio.stopListening();  // выключаем прием сигнала
      if (radio.testCarrier()) ++values[i];
    }
  }

  int i = 0;
  while (i < num_channels) 
  {
    printf("%x", min(0xf, values[i] & 0xf));
    ++i;
  }
  printf("\n\r");
  */
}