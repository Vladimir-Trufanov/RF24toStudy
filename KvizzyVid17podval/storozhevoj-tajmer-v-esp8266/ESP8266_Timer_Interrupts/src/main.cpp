// ESP8266_Timer_Interrupts

#include <Arduino.h>
// Определяем контакт для встроенного светодиода
#define LED_built 2

// --------------------------------
// V1 - мигание светодиодом (инверсное)
// по срабатыванию сторожевого таймера

volatile boolean LED_state = false;

IRAM_ATTR void ISR () 
{                                
  // Таймер достиг нуля, перезагружаем его для нового отсчета
  timer0_write(ESP.getCycleCount() + 80000000L); // сбросили таймер для нового отсчета времени
  LED_state = !LED_state;                        
  digitalWrite(LED_built, LED_state);          
  if (LED_state) Serial.println("выключено");
  else Serial.println("ВКЛЮЧЕНО!");
}

void setup() 
{
  Serial.begin(115200);
  pinMode(LED_built, OUTPUT); // LED pin
  noInterrupts();                                // отключили прерывания со старыми настройками
  timer0_isr_init();                             // инициализируем нулевой таймер
  timer0_attachInterrupt(ISR);                   // подключили обработчик прерывания ISR к таймеру
  timer0_write(ESP.getCycleCount() + 80000000L); // назначили загрузку со значением времени в 1 секунду
  interrupts();                                  // включили прерывания
}

void loop() 
{ 
  delay(100000);                                
}

// --------------------------------
// V2 -  просто мигание светодиодом
/*
void setup() 
{
  Serial.begin(115200);
  pinMode(LED_built, OUTPUT);
}

void loop() {
  digitalWrite(LED_built, HIGH);
  Serial.println("выключено");
  delay(1000);
  
  digitalWrite(LED_built, LOW);
  Serial.println("ВКЛЮЧЕНО!");
  delay(1000);
}
*/