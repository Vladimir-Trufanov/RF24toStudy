
/*
  ПЕРЕДАТЧИК

  Примерный эскиз для радиостанций nRF24L01+
  Это очень простой пример того, как отправлять данные с одного узла на другой, но измененный, чтобы включить обработку сбоев.

  Радиоприемники nrf24l01+ являются достаточно надежными устройствами, но на макетных платах и т.д. с несогласованной проводкой сбои могут
  возникать случайным образом по прошествии многих часов, дней или недель. На этом примере показано, как справляться с различными сбоями и
  поддерживать радиоприемник в рабочем состоянии.

  К трем основным режимам неисправности радиоприемника относятся:
  Запись на радио: Радио не отвечает - исправлено путем добавления таймаута к внутренним функциям записи в RF24 (обработка сбоев)
  Чтение с радио: Доступно всегда возвращает значение true - Исправлено путем добавления таймаута к доступным функциям пользователем. Это реализовано внутри сети RF24Network.
  Потеряны настройки конфигурации радиостанции - исправлено путем отслеживания значения, отличающегося от значения по умолчанию, и повторной настройки радиостанции, если это значение возвращается к значению по умолчанию.

  Для радио #0 вывод сведений должен выглядеть следующим образом:

  STATUS         = 0x0e RX_DR=0 TX_DS=0 MAX_RT=0 RX_P_NO=7 TX_FULL=0
  RX_ADDR_P0-1   = 0x65646f4e31 0x65646f4e32
  RX_ADDR_P2-5   = 0xc3 0xc4 0xc5 0xc6
  TX_ADDR        = 0x65646f4e31
  RX_PW_P0-6     = 0x20 0x20 0x00 0x00 0x00 0x00
  EN_AA          = 0x3f
  EN_RXADDR      = 0x02
  RF_CH          = 0x4c
  RF_SETUP       = 0x03
  CONFIG         = 0x0f
  DYNPD/FEATURE  = 0x00 0x00
  Data Rate      = 1MBPS
  Model          = nRF24L01+
  CRC Length     = 16 bits
  PA Power       = PA_LOW

  Updated: 2019 by TMRh20
*/

#include <SPI.h>
#include "RF24.h"
#include "printf.h"

/****************** User Config ***************************/
#define CE_PIN 6
#define CSN_PIN 7
RF24 radio(CE_PIN, CSN_PIN);
byte addresses[][6] = {"1Node","2Node"};

// Set this radio as radio number 0 or 1
// Used to control whether this node is sending or receiving
bool radioNumber = 1;  // для 2 контроллера
bool role = 1;         // для 2 контроллера

/**********************************************************/

// Функция настройки радио
void configureRadio() 
{
  radio.begin();
  // Устанавливаем низкий уровень мощности, чтобы предотвратить проблемы, связанные с питанием, 
  // поскольку, скорее всего, устройства расположены близко. Значение по умолчанию - RF24_PA_MAX.
  radio.setPALevel(RF24_PA_LOW);
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

void setup() 
{
  Serial.begin(115200);
  Serial.println(F("RF24/examples/GettingStarted"));
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
