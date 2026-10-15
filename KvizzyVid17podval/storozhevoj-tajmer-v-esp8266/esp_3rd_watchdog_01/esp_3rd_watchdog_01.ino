
/*
 * esp_3rd_watchdog_01.ino
 * 
 * Example sketch to show need for third watchdog.
 * 
 * Michel Deslierres
 * August 26, 2017
 */

extern "C" 
{
  #include "user_interface.h"
}

int getBootDevice(void) 
{
  int bootmode;
  asm (
    "movi %0, 0x60000200\n\t"
    "l32i %0, %0, 0x118\n\t"
    : "+r" (bootmode) /* Output */
    : /* Inputs (none) */                
    : "memory" /* Clobbered */           
  );
  return ((bootmode >> 0x10) & 0x7); 
}

void setup() 
{
  
  Serial.begin(115200);
  delay(5000);
  Serial.println("\nНачало!!!");
  
  Serial.printf("\n\nПричина перезагрузки: %s\n", ESP.getResetReason().c_str());
  Serial.println("----------------------------------------------");

  if ( getBootDevice() == 1 ) 
  {
    Serial.println("\nЭтот скетч только что был загружен через UART.");
    Serial.println("ESP8266 зависнет при первом перезапуске.");
    Serial.println("Теперь нажмите кнопку сброса или включите ESP повторно");
    Serial.println("и после этого работа будет возобновлена.");
    while (1) { yield(); }
  }
}

char user_input() 
{
  while (1) 
  {
    Serial.println();
    Serial.println("Это проверка сторожевых псов ESP8266. В верхнем текстовом поле введите число (1, 2, или 3)");
    Serial.println("  1 - чтобы пройти бесконечный цикл, который не кормит обоих сторожевых псов ESP");
    Serial.println("  2 - чтобы пройти бесконечный цикл, который не питает программный ESP watchdog");
    Serial.println("  3 - чтобы пройти бесконечный цикл, который действительно питает ESP watchdog");
    Serial.println("а затем нажмите клавишу Enter или нажмите на кнопку [Отправить]");
    Serial.println();  
    while (!Serial.available()) 
    {
      delay(1); 
    }
    char ch = Serial.read();
    if (ch >= '1' && ch <= '3') 
    { 
      return ch; 
    } 
    else if (ch != '\n') 
    {
      Serial.println("\nInvalid entry");
    }
  }  
}

void loop() 
{
  switch (user_input()) 
  {
    case '1':
      Serial.println("\n\nЗапуск бесконечного цикла без подачи питания обоим сторожевым псам ESP");
      Serial.println("Программный сторожевой таймер должен отключиться через 3 секунды и сбросить ESP...");
      while (1) {}  // software watchdog timeout
      break;
    case '2':
      Serial.println("\n\nЗапуск бесконечного цикла без подключения к программному обеспечению ESP watchdogs");
      Serial.println("Аппаратный сторожевой таймер должен отключиться через 7 секунд и сбросить ESP...");
      ESP.wdtDisable();
      while (1) {} // hardware watchdog timeout
      break;
    case '3':  
      Serial.println("\n\nЗапуск бесконечного цикла с кормлением ESP-сторожевых псов");
      Serial.println("Подождите не менее 10 секунд, чтобы убедиться, что сторожевые псы не срабатывают,");
      Serial.println("а затем нажмите любую клавишу сброса, чтобы выйти из этого цикла!");
      while (1) { delay(2); };
      break;
    defaut:
      delay(1000);
      break;   
  }  
}
