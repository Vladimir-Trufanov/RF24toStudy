// tickerOSWatch.ino

#include <Ticker.h>
Ticker tickerOSWatch;

#define OSWATCH_RESET_TIME 10   // таймер будет разрешать 10 секунд на работу одного цикла
                                // иначе перезагрузка

static unsigned long last_loop = millis();

void ICACHE_RAM_ATTR osWatch(void) 
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

void setup() 
{
  Serial.begin(115200);
  Serial.println("\n\nESP.restart !!!");
  last_loop = millis();
  tickerOSWatch.attach_ms(((OSWATCH_RESET_TIME / 3) * 1000), osWatch);
}

void loop() 
{
  last_loop = millis();
  while(1) delay(1000); // это запустит osWatch
}
