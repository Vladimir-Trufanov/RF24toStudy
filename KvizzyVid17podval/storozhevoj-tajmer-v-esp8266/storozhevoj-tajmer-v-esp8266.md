## [Сторожевой таймер (Watchdog timer, WDT) в ESP8266](#)

### [Сторожевые таймеры в ESP8266](https://dzen.ru/a/Z2bTfzPm6jatK8Gg?ysclid=mv8i7l4u7665781619)

Основная функция сторожевого таймера заключается в том, чтобы при «затягивании» программы (для микроконтроллера это означает что произошло какое-то зависание)  произвести его перезагрузку.

ESP8266 имеет два сторожевых таймера, один аппаратный, второй – программный.

По умолчанию сторожевой таймер включен, т.е. после подачи питания на микросхему он начинает отсчет, после того как он досчитает до заданного значения, происходит перезагрузка. И если ничего не предпринимать, программа не будет работать нормально, периодически перезапускаясь после перезагрузки.

Значение, до которого «считает» сторожевой таймер в документации я не нашел, но эксперименты показали, что перезагрузка по программному сторожевому таймеру происходит примерно через 4 сек., по аппаратному, примерно через 7 сек.

### [Протокол сброса ESP07 по программному сторожевому таймеру](#)

```
[21:35:45]← 
[21:35:45]← --------------- CUT HERE FOR EXCEPTION DECODER ---------------
[21:35:45]← 
[21:35:45]← Soft WDT reset
[21:35:45]← 
[21:35:45]← Exception (4):
[21:35:45]← epc1=0x4000dd38 epc2=0x00000000 epc3=0x00000000 excvaddr=0x00000000 depc=0x00000000
[21:35:45]← 
[21:35:45]← >>>stack>>>
[21:35:45]← 
[21:35:45]← ctx: cont
[21:35:45]← sp: 3ffffe20 end: 3fffffd0 offset: 0160
[21:35:45]← 3fffff80:  4020136b 00000042 3ffee78b 40201362  
[21:35:45]← 3fffff90:  0000003a 000d0a00 3ffe89c4 00000001  
[21:35:45]← 3fffffa0:  5ffffe00 00000000 feefeffe 3ffeeac8  
[21:35:45]← 3fffffb0:  3fffdad0 00000000 3ffeea9c 40205b98  
[21:35:45]← 3fffffc0:  feefeffe feefeffe 3fffdab0 40101045  
[21:35:45]← <<<stack<<<
[21:35:45]← 
[21:35:45]← --------------- CUT HERE FOR EXCEPTION DECODER ---------------
[21:35:45]← 
[21:35:45]←  ets Jan  8 2013,rst cause:2, boot mode:(3,7)
[21:35:45]← 
[21:35:45]← load 0x4010f000, len 3424, room 16 
[21:35:45]← tail 0
[21:35:45]← chksum 0x2e
[21:35:45]← load 0x3fff20b8, len 40, room 8 
[21:35:45]← tail 0
[21:35:45]← chksum 0x2b
[21:35:45]← csum 0x2b
[21:35:45]← v0004f790
[21:35:45 
```

***Soft WDT reset*** — сигнал, что ESP8266 сбросился из-за срабатывания Watchdog Timer (WDT), а конкретнее — из-за Soft WDT reset (программный сброс). В выводе также указано Exception (4), что подтверждает программный характер сброса.

Soft WDT reset — таймер WDT был настроен на выполнение определённого кода, но программа не успела его завершить. Это частая причина нестабильности в долго работающих проектах.

***Exception (4)*** — номер исключения. Для WDT-сбросов это стандартно, и значение 4 помогает в отладке.

Используйте инструмент для декодирования. Самый эффективный способ — скопировать весь дамп (от строки ------------ CUT HERE FOR EXCEPTION DECODER --------------- до конца) и вставить в онлайн-декодер, например, Esp8266 Exception Decoder. Он проанализирует стек и укажет вероятную строку и даже имя функции, где случился сбой.

### [Watchdog Timer](https://wolles-elektronikkiste.de/en/watchdog-timer)

#### Сторожевой таймер ESP8266

По сравнению с микроконтроллерами AVR сторожевой таймер ESP8266 устроен принципиально иначе. Прежде всего, у него есть аппаратный и программный сторожевые таймеры. Программный сторожевой таймер активируется автоматически, без какого-либо дополнительного вмешательства. 

Загрузите следующий скетч на свой ESP8266 и посмотрите, что происходит на последовательном мониторе (настройте скорость передачи данных 115200 бод!)

```
// ProbaSoftWDT
#include <Arduino.h>
void setup() 
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("\nЗапущен ESP07"); 
  //ESP.wdtDisable();
}
  
void loop() 
{ 
  while(1){}             
}
```

Если после загрузки кода отключить GPIO0, запустить контроллер и подключиться к порту, то можно увидель, как ESP8266 перезапускается примерно каждые три секунды. Образно говоря, ***программный сторожевой таймер*** «кормит» аппаратный сторожевой таймер, чтобы тот не «кусался». 

```
[10:01:55]← Запущен ESP07
[10:01:58]← 
[10:01:58]← --------------- CUT HERE FOR EXCEPTION DECODER ---------------
[10:01:58]← 
[10:01:58]← Soft WDT reset
[10:01:58]← 
[10:01:58]← Exception (4):
[10:01:58]← epc1=0x40203f84 epc2=0x00000000 epc3=0x00000000 excvaddr=0x00000000 depc=0x00000000
[10:01:58]← 
[10:01:58]← >>>stack>>>
[10:01:58]← 
[10:01:58]← ctx: cont
[10:01:58]← sp: 3ffffe60 end: 3fffffd0 offset: 0160
[10:01:58]← 3fffffc0:  feefeffe feefeffe 3fffdab0 40100d15  
[10:01:58]← <<<stack<<<
[10:01:58]← 
[10:01:58]← --------------- CUT HERE FOR EXCEPTION DECODER ---------------
[10:01:58]← 
[10:01:58]←  ets Jan  8 2013,rst cause:2, boot mode:(3,6)
[10:01:58]← 
[10:01:58]← load 0x4010f000, len 3424, room 16 
[10:01:58]← tail 0
[10:01:58]← chksum 0x2e
[10:01:58]← load 0x3fff20b8, len 40, room 8 
[10:01:58]← tail 0
[10:01:58]← chksum 0x2b
[10:01:58]← csum 0x2b
[10:01:58]← v00041c70
...
[10:01:58]← Запущен ESP07
[10:02:01]← 
[10:02:01]← --------------- CUT HERE FOR EXCEPTION DECODER ---------------
[10:02:01]← 
[10:02:01]← Soft WDT reset
[10:02:01]← 
[10:02:01]← Exception (4):
[10:02:01]← epc1=0x40203f84 epc2=0x00000000 epc3=0x00000000 excvaddr=0x00000000 depc=0x00000000
[10:02:01]← 
[10:02:01]← >>>stack>>>
[10:02:01]← 
[10:02:01]← ctx: cont
[10:02:01]← sp: 3ffffe60 end: 3fffffd0 offset: 0160
[10:02:01]← 3fffffc0:  feefeffe feefeffe 3fffdab0 40100d15  
[10:02:01]← <<<stack<<<
[10:02:01]← 
[10:02:01]← --------------- CUT HERE FOR EXCEPTION DECODER ---------------
[10:02:01]← 
[10:02:01]←  ets Jan  8 2013,rst cause:2, boot mode:(3,6)
[10:02:01]← 
[10:02:01]← load 0x4010f000, len 3424, room 16 
[10:02:01]← tail 0
[10:02:01]← chksum 0x2e
[10:02:01]← load 0x3fff20b8, len 40, room 8 
[10:02:01]← tail 0
[10:02:01]← chksum 0x2b
[10:02:01]← csum 0x2b
[10:02:01]← v00041c70
 
```
Чтобы отключить программный сторожевой таймер, раскомментируйте функцию ***ESP.wdDisable();*** и посмотрите, что произойдет:

```
[10:13:32]← Запущен ESP07
[10:13:41]← 
[10:13:41]←  ets Jan  8 2013,rst cause:4, boot mode:(3,0)
[10:13:41]← 
[10:13:41]← wdt reset
[10:13:41]← load 0x4010f000, len 3424, room 16 
[10:13:41]← tail 0
[10:13:41]← chksum 0x2e
[10:13:41]← load 0x3fff20b8, len 40, room 8 
[10:13:41]← tail 0
[10:13:41]← chksum 0x2b
[10:13:41]← csum 0x2b
[10:13:41]← v00041c80
[10:13:41]← ~ld
...
[10:13:41]← Запущен ESP07
[10:13:49]← 
[10:13:49]←  ets Jan  8 2013,rst cause:4, boot mode:(3,0)
[10:13:49]← 
[10:13:49]← wdt reset
[10:13:49]← load 0x4010f000, len 3424, room 16 
[10:13:49]← tail 0
[10:13:49]← chksum 0x2e
[10:13:49]← load 0x3fff20b8, len 40, room 8 
[10:13:49]← tail 0
[10:13:49]← chksum 0x2b
[10:13:49]← csum 0x2b
[10:13:49]← v00041c80
[10:13:49]← ~ld
...
[10:13:49]← Запущен ESP07
[10:13:58]← 
[10:13:58]←  ets Jan  8 2013,rst cause:4, boot mode:(3,0)
[10:13:58]← 
[10:13:58]← wdt reset
[10:13:58]← load 0x4010f000, len 3424, room 16 
[10:13:58]← tail 0
[10:13:58]← chksum 0x2e
[10:13:58]← load 0x3fff20b8, len 40, room 8 
[10:13:58]← tail 0
[10:13:58]← chksum 0x2b
[10:13:58]← csum 0x2b
[10:13:58]← v00041c80
[10:13:58]← ~ld
```

Теперь сброс происходит примерно каждые восемь секунд, и на последовательном мониторе появляется сообщение "***wdt reset***". Это аппаратный сторожевой таймер. 

Если теперь ограничить время работы одного цикла двумя секундами, то контроллер будет работать постоянно.

```
// ProbaSoftWDT
#include <Arduino.h>
void setup() 
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("\nЗапущен ESP07"); 
  // ESP.wdtDisable();
}
 
unsigned long imess = millis();
 
void loop() 
{ 
  // while(1){}   
  if ((millis()-imess)>2000)
  {
    Serial.println("\nПрошло 2 секунды!");
    imess = millis();
  }
  else delay(500);
}

```

Каждые 2 секунды в последовательный порт будут выводиться сообщения:

```
[10:56:00]← Прошло 2 секунды!
[10:56:02]← 
[10:56:02]← Прошло 2 секунды!
[10:56:04]← 
[10:56:04]← Прошло 2 секунды!
[10:56:07]← 
[10:56:07]← Прошло 2 секунды!
[10:56:09]← 
[10:56:09]← Прошло 2 секунды!
[10:56:11]← 
[10:56:11]← Прошло 2 секунды!
[10:56:14]← 
```



---

#### Сторожевой таймер (Watchdog timer, WDT) в ESP8266 

— это механизм защиты от зависаний программы. Если код «зацикливается» и долго не выполняет нужные операции (например, перестаёт отвечать в WiFi или уходит в долгий цикл), таймер принудительно перезагружает модуль. В ESP8266 таких таймера два: программный и аппаратный. 
microsin.ru
dzen.ru
wikihandbk.com
Внешний сторожевой таймер на NE555
Как это работает
Программный сторожевой таймер. Реализован на базе таймера MAC и управления задачами. Его задача — периодически «скармливать» (сбрасывать) таймер при выполнении «лёгких» задач. Если программа надолго «зависнет» и не будет вызывать эту функцию, таймер сработает и перезагрузит модуль. По умолчанию он активен. 
microsin.ru
dzen.ru
Аппаратный сторожевой таймер. Это «последний рубеж обороны». Он включается, если программный таймер был отключён (например, через system_soft_wdt_stop()), не сработал или его вообще не было. Он срабатывает гарантированно, если программа не сбросила его в течение примерно 8 секунд. 
microsin.ru
wikihandbk.com
Интервалы срабатывания
Интервал прерывания аппаратного таймера — около 1638 мс.
Если программа не сбросила аппаратный таймер в течение интервала обработки прерывания (6553,6 мс), срабатывает аппаратный сброс.
Если программа не сбрасывала таймер в течение 8192 мс после прерывания, сброс произойдёт гарантированно.
Программный таймер срабатывает примерно через 4 секунды, аппаратный — примерно через 7 секунд.
 
microsin.ru
dzen.ru
Как с этим работать
В SDK Espressif для управления программным таймером есть три функции:
system_soft_wdt_stop() — остановить программный таймер. Важно: не останавливайте его дольше чем на 6 секунд, иначе сработает аппаратный.
system_soft_wdt_restart() — перезапустить программный таймер (можно вызвать только после остановки).
system_soft_wdt_feed() — сбросить программный таймер (только если он запущен).
 
dzen.ru
Для платформы Arduino эти функции дублируются методами: ESP.wdtDisable(), ESP.wdtEnable(timeout_ms) и ESP.wdtFeed(). Обратите внимание: в Arduino-версии аргумент timeout_ms в ESP.wdtEnable() на практике не используется — внутренняя реализация не позволяет его задать. 
dzen.ru
Хорошая новость: многие библиотеки ESP неявно сбрасывают сторожевой таймер в своих функциях, поэтому часто вам не придётся управлять им вручную. 
dzen.ru
Как понять, что именно вызвало перезагрузку
После перезагрузки в монитор порта (UART) выводятся сообщения, которые помогают определить причину:
При перезапуске программным таймером в логах часто есть трассировка стека — это самый надёжный способ найти строку кода, вызвавшую зависание.
При перезапуске аппаратным таймером трассировки может не быть. В этом случае поможет добавление отладочных сообщений (Serial.print()) в код — проследите, какие сообщения появляются перед сбросом.
 
wikihandbk.com
Важное замечание
Не все загрузчики (bootloader) корректно работают со сторожевыми таймерами. 
drive2.ru
Когда это полезно
Сторожевые таймеры особенно ценны в проектах, где стабильность критична: например, в удалённых датчиках, IoT-устройствах, системах с реле или в сценариях, где зависание может привести к потере данных или сбою в работе подключённого оборудования. 
github.com
wikihandbk.com
Если планируете использовать WDT в своём проекте, советую начать с минимального интервала и внимательно следить за тем, чтобы не блокировать таймер надолго. Если возникнут сложности с отладкой — пишите, подскажу!

---



#### [espressif.com FAQ ESP8266](https://microsin.ru/programming/arm/espressifcom-faq-esp8266.html?ysclid=mv8htkh8uy556075861)

Какова роль сторожевого таймера ESP8266 (watchdog)?
Для повышения надежности системы в условиях высоких помех, в микросхему ESP8266 интегрирован двухуровневый сторожевой механизм (watchdog) включающий программный watchdog и аппаратный watchdog. По умолчанию оба этих сторожевых таймера разрешены.

Каков интервал таймаута watchdog? Что произойдет при событии таймаута?
Интервал прерывания hardware watchdog составляет 0.8*2048 мс (1638.4 мс). Интервал обработки прерывания составляет 0.8*8192 мс (6553.6 мс). Интервал обработки прерывания это предел времени, когда нужно сбросить watchdog после того, как произошло прерывание. Если истек интервал обработки прерывания, и аппаратный сторожевой таймер не был сброшен, то срабатывает аппаратный сброс (hardware watchdog reset). Таким образом, если программа не выполнила сброс сторожевого таймера в течение 6553.6 мс, то может привести к аппаратному сбросу системы. Если же программа в течение 8192 не выполнила сброс аппаратного сторожевого таймера, то это гарантированно приведет к watchdog reset.

Программный сторожевой таймер (software watchdog) основан на таймере MAC и управлении задачами (task arrangement). Интервал прерывания составляет 1600 мс, и он равен интервалу обработки прерывания. В результате, когда работают оба сторожевых таймера, и программа работала в течение 1600 мс, то это может привести к программному сбросу (software watchdog). Если же программа работала свыше 3200 мс, то это приведет к гарантированному сбросу от сторожевого таймера.

Моему приложению не нужен watchdog, можно его запретить?
Текущая версия SDK позволяет запретить только software watchdog. Могут быть предприняты следующие способы избежать watchdog reset, когда программа пользователя занимает CPU слишком долго:

1. Если Ваша подпрограмма нуждается в окне времени между software watchdog reset и hardware watchdog reset, то Вы можете использовать функцию system_soft_wdt_stop(), чтобы запретить software watchdog. После выполнения Вашего кода можно перезапустить software watchdog с помощью функции system_soft_wdt_restart().

2. Вы можете сбрасывать watchdog в циклах своего кода с помощью функции system_soft_wdt_feed(), чтобы watchdog обновлялся до того, как сгенерирует сброс.

Как лучше всего организовать в моей программе 10-секундную задержку, или опрос входа?
Таймер watchdog не позволяет выполнить пустые бесконечные циклы. Как упоминалось выше, переполнение watchdog приведет к сбросу, если программа пользователя слишком долго зависнет на опросе какого-то события, или будет использовать циклы для формирования задержек. Разработчики рекомендуют для формирования задержек использовать callback-функции (так называемые функции обратного вызова) и API таймера.   

Для опроса события, если это возможно, всегда используйте прерывания или функции API таймера, чтобы осуществлять частые проверки. Большинство событий можно привязать к callback-функциям из SDK, и таким образом опроса почти всегда можно избежать.

#### [esp8266-watchdog](https://github.com/pxgo/esp8266-watchdog/tree/main)

#### [Алиса - Работа с Программный сторожевой таймер в esp8266]

Программный сторожевой таймер (software watchdog) в ESP8266 — это механизм, который помогает предотвратить зависание устройства, автоматически перезапуская его, если программа долго не выполняет нужных действий. По умолчанию он активен, но требует аккуратного обращения. 
wolles-elektronikkiste.de
dzen.ru
Как это работает
Программный таймер основан на механизме управления задачами (task arrangement) и использует таймер MAC. Его интервал прерывания составляет около 1600 мс. Если программа «застревает» и не вызывает функцию сброса таймера в течение этого времени, срабатывает программный сброс. 
microsin.ru
Важно: если программный таймер отключить (например, надолго остановить), в дело вступает аппаратный сторожевой таймер, который также инициирует перезагрузку. 
dzen.ru
Ключевые функции для работы
В SDK Espressif для управления таймером доступны три основные функции:
system_soft_wdt_stop() — останавливает программный сторожевой таймер. Крайне важно не останавливать его надолго (более 6 секунд), иначе сработает аппаратный таймер.
system_soft_wdt_restart() — перезапускает таймер. Эту функцию можно вызывать только после того, как таймер был остановлен.
system_soft_wdt_feed() — сбрасывает программный таймер. Вызывайте эту функцию регулярно, пока таймер активен.
 
dzen.ru
Для платформы Arduino эти функции имеют прямые аналоги в библиотеке ESP8266:
ESP.wdtDisable() — отключает программный сторожевой таймер.
ESP.wdtEnable(uint32_t timeout_ms) — включает программный сторожевой таймер. Аргумент timeout_ms на практике не используется.
ESP.wdtFeed() — сбрасывает таймер.
 
dzen.ru
Практические рекомендации
Не отключайте таймер без необходимости. Это повышает риск зависания и срабатывания аппаратного сторожевого таймера.
Регулярно сбрасывайте таймер. Если ваша программа выполняет длительные операции (например, в обработчиках прерываний или в блокирующих вызовах), вызывайте ESP.wdtFeed() как можно чаще.
Анализируйте логи при перезагрузке. После сброса через программный таймер в мониторе порта можно увидеть трассировку стека. Это помогает быстро найти строку кода, вызвавшую зависание.
Учитывайте работу библиотек. Многие функции стандартной библиотеки ESP неявно сбрасывают сторожевой таймер, поэтому в большинстве случаев вам не придётся делать это вручную.
 
dzen.ru
wikihandbk.com
Пример использования
Вот как можно организовать работу с таймером в простом скетче:
void setup() {
  Serial.begin(115200);
  // Включаем программный сторожевой таймер
  ESP.wdtEnable(0);
}

void loop() {
  // Ваш код здесь

  // Регулярно сбрасываем таймер, чтобы не вызвать сброс
  ESP.wdtFeed();

  // Имитация длительной операции
  delay(1000);
}
В этом примере delay(1000) не даёт процессору зависнуть, но при этом регулярно «кормит» программный таймер.
Главное — не допускайте ситуаций, когда программа надолго «застревает» в одном месте, и не забывайте про сброс таймера. Это надёжная страховка от зависаний.

#### [ESP8266 Watchdogs in Arduino](https://sigmdel.ca/michel/program/esp8266/arduino/watchdogs_en.html)

angl

```
While the hardware and software watchdog timers of the ESP8266 are essential, they are not sufficient to ensure the kind of reliability needed in an IoT device. In the next post, I discuss how to implement yet a third watchdog to further improve the dependability of the firmware programmed into this chip. In the mean time, I thought it would be useful to discuss watchdogs in general and to delve into some of the details of the ESP8266 watchdogs.

Table of Contents

What is a Watchdog Timer?
ESP8266 Hardware Watchdog
ESP8266 Software Watchdog
ESP8266 Watchdog Timeouts
Care and Feeding of the ESP8266 Watchdogs
Recovery
toc
What is a Watchdog Timer?
If the man stops kicking the dog, the dog will take advantage of the hesitation and bite the man.
Niall Murphy, Watchdog Timers.

If you are upset with kicking software dogs, you could substitute a "feed" metaphor which I think is even more eloquent.

The role of a watchdog timer (abbreviated WDT) is to react to a hardware or software malfunction in a timely manner by returning a device to normal operation most often by performing a reset.

When resetting a frozen computer with a three-finger salute or by pulling the power plug or by removing the battery or whatever other incantation we have stumbled upon, we are the watchdog. In embedded systems such as IoT devices where human intervention cannot be relied upon for correcting errors, a well-designed WDT should be designed to be the ultimate defensive measure. Granted, "ultimate" is an ambiguous term, but it is warranted in this case. A watchdog is often the last line of defence against hardware or software malfunction. And as it most often resets the device, it is an extreme measure which may or may not result in recovery.

A watchdog can be viewed as a counter that is regularly decremented. Properly functioning software will "feed" the watchdog which involves resetting the counter value. Presumably, runaway software will not do that and the watchdog counter will reach zero, when it is said to "bite" by resetting the device.

Better watchdog timers are actual integrated circuits that are independent of the CPU or micro-controllers that they monitor.Examples include the Texas Instrument UCC3946, Maxim MAX823/4/5 and the STMicroelectronics STWD100 chips. They operate on the following principle.

external watchdog circuit

On a regular basis, the micro-controller must toggle the watchdog input pin wdi connected to one of its gpio pin. If this is not done, the watchdog times out and asserts its output pin wdo which can be wired to the micro-controller reset pin. There are more sophisticated versions of these chips less likely to be spoofed by runaway software (see Jack Ganssle Great Watchdog Timers for Embedded Systems).

When using an off-the-shelf device without an independent watchdog circuit, it will be necessary to rely on the built-in watchdog of its micro-controller. The watchdog timer works pretty much in the same manner. An internal register is decremented at regular intervals. If it is not reset, the processor will reboot when the register reaches zero.

References:

Barr, Michael (2001), Introduction to Watchdog Timers.
Murphy, Niall (2000), Watchdog Timers.
Ganssle, Jack (2016), Great Watchdog Timers for Embedded Systems.
toc
ESP8266 Hardware Watchdog
The ESP8266 software watchdog, which has a time-out period about half as long as that of the ESP8266 hardware watchdog, feeds the latter. The following sketch makes the hardware watchdog trigger a reset by suspending the software watchdog and then throwing the ESP into an endless empty loop.

void setup() {
 
  ESP.wdtDisable();
  while (1) {};

}
 
void loop() {}
ESP is an EspClass object declared in ESP.h which is part of the ESP8266/Arduino core included in every Arduino sketch.

Open the serial monitor and upload the sketch. Wait about 8 seconds, and see something like this displayed in the serial monitor window:

initial hardware watchdog timeout

And then... nothing happens. The system is frozen! This is not good. Actually, this is a well-known problem that only occurs on the first watchdog reset after a firmware download by serial link (see esp8266/Arduino FAQ). Manually reset the ESP8266 and from then on the hardware watchdog timeouts and resets the ESP without fail.

hardware watchdog timeout

This initial failure of the hardware watchdog is not a big deal. Presumably an ESP based device will be turned off after uploading its Arduino sketch with a computer and will be powered up again when it is installed in its final position. If uploading occurred in situ as it were, then remember to turn the ESP off and then back on or to reset it manually. It is said that the initial failure does not occur after an OTA upload of a sketch. I have yet to test this.

At first blush, one might desire that there be a way to hook the hardware watchdog timeout routine to save information in non-volatile memory before the reset is performed. As far as I know, this is not easily done. On second thought, only those of us that create bug-free software at first try should lament this lack of flexibility. For my part, I am glad the hardware watchdog works dependably no matter what I might do.

Since there is really nothing much more that can be done with the hardware watchdog, time to go on to the software watchdog.

Reference:

Santos, Nuno (2017), ESP8266: Watchdog functions.
The hardware watchdog biting code can be found on many sites on the Web. I happen to like Nuno Santos' clear exposition.

toc
ESP8266 Sofware Watchdog
Getting the software watchdog to bite on the ESP8266 is even easier.

void setup() {
 
  while (1) {};

}
 
void loop() {}
Again the system will freeze if the sketch is downloaded to the ESP over a serial link. After an initial power cycle or reset, the device will be locked in an endless loop of resets cause by the software watchdog every 3.2 seconds or so.

software watchdog timeout

The EspClass object has a watchdog enable routine with a timeout parameter. Here is part of the class definition that will be useful in this blog.

class EspClass {
   public:
      // TODO: figure out how to set WDT timeout
      void wdtEnable(uint32_t timeout_ms = 0);
      // note: setting the timeout value is not implemented at the moment
      void wdtEnable(WDTO_t timeout_ms = WDTO_0MS);

      void wdtDisable();
      void wdtFeed();

      String getResetReason();
      String getResetInfo();
      struct rst_info * getResetInfoPtr();       

      void reset();
      void restart();
But as the comment in the code says, the watchdog enable routine does not implement the timeout value: wdtEnable(0) and wdtEnable(20000) have exactly the same effect. They both enable the software watchdog with the same default timeout period.

There is not much one can do with the software watchdog. It can be enabled, disabled and fed with wdtFeed. We can use the latter to try to get the length of time before the watchdogs bite.

toc
ESP8266 Watchdogs Timeout
There seems to be conflicting information about the timeout period for the ESP8266 software watchdog. Dave Evans reports a software watchdog timeout occurs between 1.5 and 2.5 seconds on an Adafruit Huzzah ESP8286. My own test with similar code but run on a Wemos D1 mini, puts it at between 1.62 seconds and 1.63 seconds. However pasko_zh (paško from Zurich, I am sorry but that contributor's real name is not readily available to credit him properly) is adamant that the software watchdog timeout period is 3.2 seconds. It's a value that I have seen elsewhere, so I decided to investigate the matter a bit further.

I used the following sketch (available for download: esp_watchdog_timing.ino) to obtain the period of the hardware and software watchdogs.

/*
 * Test to establish the timeout period of the ESP8266 watchdog timers 
 */

//#define SOFT_WATCHDOG     // comment out to test hardware watchdog time-out

#ifdef SOFT_WATCHDOG
  #define LOOPS 15
  #define STEP 10
  unsigned long interval = 3150;
#else
  #define LOOPS 10
  #define STEP 100
  unsigned long interval = 7900;
#endif

unsigned long etime;

void setup() {
  Serial.begin(115200);
  Serial.printf("\n\nSdk version: %s\n", ESP.getSdkVersion());
  Serial.printf("Core Version: %s\n", ESP.getCoreVersion().c_str());
  Serial.printf("Boot Version: %u\n", ESP.getBootVersion());
  Serial.printf("Boot Mode: %u\n", ESP.getBootMode());
  Serial.printf("CPU Frequency: %u MHz\n", ESP.getCpuFreqMHz());
  Serial.printf("Reset reason: %s\n", ESP.getResetReason().c_str());
  Serial.println("---------------------------");
  Serial.println("-- WATCHDOG TIMING TESTS --");
  Serial.println("---------------------------\n");

  while (1) {
  
    interval += STEP;
    
    Serial.print("  Testing ");
    #ifdef SOFT_WATCHDOG
      Serial.print("software");
    #else
      Serial.print("hardware");
    #endif  
    Serial.printf(" watchdog with interval of %u ms - ", interval);
    Serial.flush();

    ESP.wdtDisable();
    #ifdef SOFT_WATCHDOG
      ESP.wdtEnable(0);
    #endif  
      
    for (int i = 0; i < LOOPS; i++) {
  
      ESP.wdtFeed();
            
      ESP.wdtDisable();
      #ifdef SOFT_WATCHDOG
        ESP.wdtEnable(0);
      #endif  
        
      etime = millis();
      while (millis() - etime < interval) { }

      ESP.wdtDisable();
      ESP.wdtEnable(0);
      ESP.wdtFeed();
      Serial.printf("%d ", i+1);
    }  
    Serial.println("done.");
  }
}

void loop() { }
The idea is simple enough. Do an empty loop for a length of time set with the interval variable in milliseconds:

  etime = millis();
  while (millis() - etime < interval) { }
During that time the watchdog is not being fed. If the watchdog does not bite, increase the interval and try again until the watchdog does reset the ESP. Since the value of the interval variable is printed to the serial monitor before the beginning of the empty loop, it will be easy to figure out the length of the watchdog time-out period. There are two complications. First, each interval is tested a number of times because the ESP may perform background task which makes timing somewhat unpredictable. The number of iterations of the empty loop test is set with the LOOPS directive. The second complication is the care and feeding of the watchdogs at the end of each iteration, just before printing the iteration number to the serial monitor. It seems that the Serial library interacts with the watchdogs in unpredictable fashion. This is not too surprising as both transmission and reception of serial data is interrupt-driven (see ESP8266 Arduino Core documentation.)
Here is the Arduino serial monitor output of the sketch when run on a Wemos D1 mini to test the hardware watchdog:

hardware watchdog timeout

This is the output when the SOFT_WATCHDOG directive is defined:

software watchdog timeout

The 8.2 seconds timeout for the hardware watchdog and 3.2 seconds for the software watchdog timeout, concord with the times reported by pasko_zh.

Before starting an empty loop to see if the watchdog will timeout, both pasko_zh and I feed and restart the software watchdog, while Dave Evans only feeds it. Note that pasko_zh uses the lower level C functions in user_interface.h so that he begins each test with

  system_soft_wdt_feed();
  system_soft_wdt_restart();  
The same result can be obtained with the methods in ESPclass:
  ESP.wdtFeed();
  ESP.wdtEnable(0);  
Indeed, I modified test1 of the Evans sketch (available for download: evans_soft.ino) to verify that not enabling the software watchdog is the reason it bites when not fed it for 1.6 to 1.8 seconds. These timing results could lead us to conclude that the initial watchdog timer value set when restarting the watchdog is greater than the value set by a watchdog feed routine. However, changing the order in which the watchdog is fed and restarted does not change the timing results.
I did not care to push my investigation further. The lesson seems to be that the software watchdog should not be starved for much more than a second to avoid its bite.

References:

Evans, Dave (2016), Watchdog Timer Tests.
paško from Zurich (pasko_zh) (2016), I'm getting these Watchdog Resets "wdt reset" - How can I avoid them?.
toc
Care and Feeding of the ESP8266 Watchdogs
In his discussion of the ESP watchdogs, pasko_zh makes an important point: ESP.wdtFeed() (or system_soft_wdt_feed()) actually feeds both the hardware and the software watchdogs. So the two watchdogs are not cascaded. They are two independent watchdogs with different (and non modifiable) timeout periods that happen to be fed by the same low level function.

In most Arduino sketches, the feeding of the watchdogs is done implicitly. It is done at the beginning of each iteration of the program loop. It is done in the yield() and delay() functions (but not the delayMicroseconds() function). Certain blocking functions, such as those found in the WiFi module also feed the watchdogs. So it would appear that the 'Arduino way' of preventing software and hardware resets is to ensure that any loop in a sketch lasts less than say 1 second (1.5 second at most) or, if necessary, that it calls regularly on yield() or delay() as necessary.

Calls to yield() or delay() have a salutary side effect. They are integrated with the WiFi module so that a connection to a WiFi network is not lost as long as the software watchdog is fed. Unfortunately, I have noticed that things are not quite as smooth with regard to the MQTT module pubsub. So in those sketches that use that module, I call on local_yield() or local_delay() as defined below if needs be.

/*
   Need a local yield that that calls yield() and mqttClient.loop()
   The system yield() routine does not call mqttClient.loop()
*/

void local_yield()
{
  yield();
  mqttClient.loop();
}

/*
   Need a local delay calls yield() and mqttClient.loop()
   The system delay() routine does not call mqttClient.loop()
*/
void local_delay(unsigned long millisecs)
{
  unsigned long start = millis();
  local_yield();
  if (millisecs > 0)
  {
    while (elapsed_time(start) < millisecs)
    {
      local_yield();
    }
  }
}
Note that the following declarations are part of the sketch that contains the above functions

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

WiFiClient    espClient;
PubSubClient  mqttClient(espClient);
toc
Recovery
That is a rather ambiguous if not ambitious title. As it happens, it is possible to obtain the reason why the ESP8266 has come to be started or restarted at the start of a sketch. By recovery, I just mean that it is therefore possible to take different actions based on the cause of the restart.

Quoting the Espressif document ESP8266 Reset Causes and Common Fatal Exception Causes, "Each time [the] ESP8266 reboots, the ROM code will print out a number corresponding to the reset cause." It also prints out something called the boot mode which indicates from where the code was taken. This was visible on the screen shots of the serial monitor window:

   ets Jan 8 2013, rst cause:2, boot mode: (1,6)
 or
   ets Jan 8 2013, rst cause:4, boot mode: (3,6)
To the right of "rst cause", the 2 indicates a software watchdog reset and the 4 marks a hardware watchdog reset. As for the first digit of the boot mode, a 1 indicates that the code was just uploaded to the ESP, a 3 indicates that the code is from the SPI flash memory.
The explanation for the boot mode comes from a document by Max Filippov on the web called Boot Process. It states that the ESP boot mode is in the lower three bits of the first number of the boot mode. Perhaps calling this the "boot mode" is unfortunate. Others use "boot device". Actually, the values of the three bits correspond to three pins of the integrated circuit and determine the source of the code run by the ESP8266. As the following table taken from the document shows, there are three possibilities:


GPIO15	GPIO0	GPIO2	Dec.	Device	Description
0	0	1	1	UART	Code downloaded from UART (programming)
0	1	1	3	Flash	Boot from code in SPI Flash (normal)
1	x	x	4-7	SDIO	Boot from SD-card
Of course, to boot from code on an SD card, it would be necessary to add an external card reader to the ESP8266.

While this information is useful when programming an ESP8266 from the Arduino IDE, the information must be retrieved if the sketch is to make use of it. The class EspClass contains a method that returns the cause of the reset as a string. The method, ESP.getResetReason(), was used in the initial setup block of the watchdog timing sketch. There is another function, named ESP.getResetInfoPtr(), that returns a pointer to a structure with information about system resets including the reason for the last one, which may be more useful than the string function.

As far as I can ascertain, there is no EspClass method to get the boot device. It can be retrieved from memory. A contributor to the ESP8266 Community Forum, who goes by the moniker 0ff, posted an assembler routine that does that.

The following sketch (available for download: esp_boot.ino) retrieves the boot information in the setup section and then goes on to restart the ESP in a specific manner.

/*
 * esp_boot.ino
 * 
 */
#define BM_WDT_SOFTWARE 0
#define BM_WDT_HARDWARE 1
#define BM_ESP_RESTART 2
#define BM_ESP_RESET 3

#define BOOT_MODE BM_WDT_SOFTWARE

extern "C" {
#include "user_interface.h"
}

struct bootflags
{
  unsigned char raw_rst_cause : 4;
  unsigned char raw_bootdevice : 4;
  unsigned char raw_bootmode : 4;

  unsigned char rst_normal_boot : 1;
  unsigned char rst_reset_pin : 1;
  unsigned char rst_watchdog : 1;

  unsigned char bootdevice_ram : 1;
  unsigned char bootdevice_flash : 1;
};

struct bootflags bootmode_detect(void) {
  int reset_reason, bootmode;
  asm (
    "movi %0, 0x60000600\n\t"
    "movi %1, 0x60000200\n\t"
    "l32i %0, %0, 0x114\n\t"
    "l32i %1, %1, 0x118\n\t"
    : "+r" (reset_reason), "+r" (bootmode) /* Outputs */
    : /* Inputs (none) */
    : "memory" /* Clobbered */
  );

  struct bootflags flags;

  flags.raw_rst_cause = (reset_reason & 0xF);
  flags.raw_bootdevice = ((bootmode >> 0x10) & 0x7);
  flags.raw_bootmode = ((bootmode >> 0x1D) & 0x7);

  flags.rst_normal_boot = flags.raw_rst_cause == 0x1;
  flags.rst_reset_pin = flags.raw_rst_cause == 0x2;
  flags.rst_watchdog = flags.raw_rst_cause == 0x4;

  flags.bootdevice_ram = flags.raw_bootdevice == 0x1;
  flags.bootdevice_flash = flags.raw_bootdevice == 0x3;

  return flags;
}


void setup() {
  Serial.begin(115200);
  Serial.println("\n\nSketch Setup\n");

  rst_info* rinfo = ESP.getResetInfoPtr();
  
  Serial.printf("rinfo->reason:   %d, %s\n", rinfo->reason, ESP.getResetReason().c_str());
  Serial.printf("rinfo->exccause: %d\n", rinfo->exccause);
  Serial.printf("rinfo->epc1:     %d\n", rinfo->epc1);
  Serial.printf("rinfo->epc2:     %d\n", rinfo->epc2);
  Serial.printf("rinfo->epc3:     %d\n", rinfo->epc3);
  Serial.printf("rinfo->excvaddr: %d\n", rinfo->excvaddr);
  Serial.printf("rinfo->depc:     %d\n", rinfo->depc);

  struct bootflags bflags = bootmode_detect();

  Serial.printf("\nbootflags.raw_rst_cause: %d\n", bflags.raw_rst_cause);
  Serial.printf("bootflags.raw_bootdevice: %d\n", bflags.raw_bootdevice);
  Serial.printf("bootflags.raw_bootmode: %d\n", bflags.raw_bootmode);
  
  Serial.printf("bootflags.rst_normal_boot: %d\n", bflags.rst_normal_boot);
  Serial.printf("bootflags.rst_reset_pin: %d\n", bflags.rst_reset_pin);
  Serial.printf("bootflags.rst_watchdog: %d\n", bflags.rst_watchdog);
  
  Serial.printf("bootflags.bootdevice_ram: %d\n", bflags.bootdevice_ram);
  Serial.printf("bootflags.bootdevice_flash: %d\n", bflags.bootdevice_flash);

  Serial.printf("\n\nrinfo->reason=%d\n\n", ESP.getResetInfoPtr()->reason);

  if (bflags.raw_bootdevice == 1) {
    Serial.println("The sketch has just been uploaded over the serial link to the ESP8266");
    Serial.println("Beware: the device will freeze after it reboots in the following step."); 
    Serial.println("It will be necessary to manually reset the device or to power cycle it");
    Serial.println("and thereafter the ESP8266 will continuously reboot.");
  }
  
#if BOOT_MODE == BM_ESP_RESTART
  Serial.println("\n\nRebooting with ESP.restart()");
  ESP.restart();
#elif BOOT_MODE == BM_ESP_RESET
  Serial.println("\n\nRebooting with ESP.reset()");
  ESP.reset();
#else 
  #if BOOT_MODE == BM_WDT_HARDWARE      
    Serial.println("\n\nRebooting with hardware watchdog timeout reset");
    ESP.wdtDisable();
  #else
    Serial.println("\n\nRebooting with sofware watchdog timeout reset");
  #endif;  
  while (1) {}  // timeout!
#endif;  
}

void loop() { }
The way the ESP8266 is rebooted is determined by the preprocessor directive BOOT_MODE directive which must be set to one of four values:


Value	Description
BM_WDT_SOFTWARE	Software watchdog timeout
BM_WDT_HARDWARE	Hardware watchdog timeout
BM_ESP_RESTART	Software restart with ESP.restart()
BM_ESP_RESET	Software restart with ESP.reset()
Running the sketch confirms that in all four ways of provoking a reboot of the ESP, it will hang on the first reboot after the sketch is downloaded. Consequently, the values in the following table show the result of running the sketch only after that first manual reset after downloading it to the ESP. That explains why boot mode is always (3, 6) in the table.


ESP.restart()	ESP.reset()	soft WDT	hard WDT
rst cause	2	2	2	4
boot mode	(3,6)	(3,6)	(3,6)	(3,6)
rinfo->reason	4	4	3	1
rfinfo->exccause	0	0	4	4
raw_rst_cause	2	2	2	4
raw_bootdevice	3	3	3	3
raw_bootmode	6	6	6	6
The first two rows of results are the "ets" values sent to UART0 at the start of the reboot, before the sketch in flash memory is run. The next two rows show the partial content of the rst_info struct returned by the EspClass method ESP.getResetInfoPtr(). The last three rows are the values read from the ESP memory using the code from 0ff. They correspond exactly to the "ets" start up message.

Clearly, rinfo->reason and bootflags.raw_bootdevice is most useful to decide on how to start a sketch. I will end this discussion of the standard ESP8266 watchdogs with an example sketch (download: esp_bootex.ino) showing how this could be done.

/*
 * esp_bootex.ino
 * 
 */
 
extern "C" {
#include "user_interface.h"
}

int getBootDevice(void) {
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

void setup() {
  Serial.begin(115200);
  Serial.println();

  if ( getBootDevice() == 1 ) {
    Serial.println("\nThis sketch has just been uploaded over the UART.");
    Serial.println("The ESP8266 will freeze on the first restart.");
    Serial.println("Press the reset button or power cycle the ESP");
    Serial.println("and operation will be resumed thereafter.");
  }
  
  char buff[32];

  switch (ESP.getResetInfoPtr()->reason) {
    
    case REASON_DEFAULT_RST: 
      // do something at normal startup by power on
      strcpy_P(buff, PSTR("Power on"));
      break;
      
    case REASON_WDT_RST:
      // do something at hardware watch dog reset
      strcpy_P(buff, PSTR("Hardware Watchdog"));     
      break;
      
    case REASON_EXCEPTION_RST:
      // do something at exception reset
      strcpy_P(buff, PSTR("Exception"));      
      break;
      
    case REASON_SOFT_WDT_RST:
      // do something at software watch dog reset
      strcpy_P(buff, PSTR("Software Watchdog"));
      break;
      
    case REASON_SOFT_RESTART: 
      // do something at software restart ,system_restart 
      strcpy_P(buff, PSTR("Software/System restart"));
      break;
      
    case REASON_DEEP_SLEEP_AWAKE:
      // do something at wake up from deep-sleep
      strcpy_P(buff, PSTR("Deep-Sleep Wake"));
      break;
      
    case REASON_EXT_SYS_RST:
      // do something at external system reset (assertion of reset pin)
      strcpy_P(buff, PSTR("External System"));
      break;
      
    default:  
      // do something when reset occured for unknown reason
      strcpy_P(buff, PSTR("Unknown"));     
      break;
  }
  Serial.printf("\n\nReason for reboot: %s\n", buff);
  Serial.println("----------------------------------------------");
}

char user_input() {
  while (1) {
    Serial.println();
    Serial.println("To select how to reset the ESP, press");
    Serial.println("  1 - for ESP.restart()");
    Serial.println("  2 - for ESP.reset()");
    Serial.println("  3 - for hardware watchdog timeout");
    Serial.println("  4 - for software watchdog timeout");
    Serial.println("or press the reset button.");
    Serial.println("In any case, press the reset button if");
    Serial.println("if this sketch has just been uploaded.");
    while (!Serial.available()) {
      delay(1); 
    }
    char ch = Serial.read();
    if (ch >= '1' && ch <= '4') { 
      return ch; 
    } else {
      Serial.println("\nInvalid entry");
    }
  }  
}

void loop() {
  switch (user_input()) {
    case '1':
      Serial.println("\n\nRebooting with ESP.restart()");
      ESP.restart();
      break;
    case '2':  
      Serial.println("\n\nRebooting with ESP.reset()");
      ESP.reset();
      break;
    case '3':  
      Serial.println("\n\nRebooting with hardware watchdog timeout reset");
      ESP.wdtDisable();
      while (1) {} // timeout
      break;
    case '4':
      Serial.println("\n\nRebooting with sofware watchdog timeout reset");
      while (1) {} // timeout
      break;
  }  
}
As shown the sketch is kind of useless since all it does is replicate the EspClass method getResetReason(). Of course, the idea is that something more useful than preparing a string for a print statement of the cause of the reset should be done in the switch/case code.
```


rus
```
Несмотря на то, что аппаратные и программные сторожевые таймеры ESP8266 очень важны, их недостаточно для обеспечения надежности, необходимой для устройства интернета вещей. В следующем посте я расскажу, как реализовать третий сторожевой таймер, чтобы еще больше повысить надежность прошивки, запрограммированной в этом чипе. А пока я подумал, что было бы полезно поговорить о сторожевых таймерах в целом и подробнее остановиться на сторожевых таймерах ESP8266.

Оглавление

Что такое сторожевой таймер?
Аппаратный сторожевой таймер ESP8266
Программный сторожевой таймер ESP8266
Таймауты сторожевых таймеров ESP8266
Уход за сторожевыми таймерами ESP8266 и их обслуживание
Восстановление
toc
Что такое сторожевой таймер?
Если человек перестанет пинать собаку, она воспользуется его нерешительностью и укусит его.
Найл Мерфи, Сторожевые таймеры.

Если вам не нравится метафора с пинком, вы можете заменить ее на «кормление», что, на мой взгляд, звучит еще более красноречиво.

Роль сторожевого таймера (сокращенно WDT) заключается в том, чтобы своевременно реагировать на аппаратные или программные сбои и возвращать устройство в нормальное рабочее состояние, чаще всего путем перезагрузки.

When resetting a frozen computer with a three-finger salute or by pulling the power plug or by removing the battery or whatever other incantation we have stumbled upon, we are the watchdog. In embedded systems such as IoT devices where human intervention cannot be relied upon for correcting errors, a well-designed WDT should be designed to be the ultimate defensive measure. Granted, "ultimate" is an ambiguous term, but it is warranted in this case. A watchdog is often the last line of defence against hardware or software malfunction. And as it most often resets the device, it is an extreme measure which may or may not result in recovery.

A watchdog can be viewed as a counter that is regularly decremented. Properly functioning software will "feed" the watchdog which involves resetting the counter value. Presumably, runaway software will not do that and the watchdog counter will reach zero, when it is said to "bite" by resetting the device.

Better watchdog timers are actual integrated circuits that are independent of the CPU or micro-controllers that they monitor.Examples include the Texas Instrument UCC3946, Maxim MAX823/4/5 and the STMicroelectronics STWD100 chips. They operate on the following principle.

external watchdog circuit

On a regular basis, the micro-controller must toggle the watchdog input pin wdi connected to one of its gpio pin. If this is not done, the watchdog times out and asserts its output pin wdo which can be wired to the micro-controller reset pin. There are more sophisticated versions of these chips less likely to be spoofed by runaway software (see Jack Ganssle Great Watchdog Timers for Embedded Systems).

When using an off-the-shelf device without an independent watchdog circuit, it will be necessary to rely on the built-in watchdog of its micro-controller. The watchdog timer works pretty much in the same manner. An internal register is decremented at regular intervals. If it is not reset, the processor will reboot when the register reaches zero.

References:

Barr, Michael (2001), Introduction to Watchdog Timers.
Murphy, Niall (2000), Watchdog Timers.
Ganssle, Jack (2016), Great Watchdog Timers for Embedded Systems.
toc
ESP8266 Hardware Watchdog
The ESP8266 software watchdog, which has a time-out period about half as long as that of the ESP8266 hardware watchdog, feeds the latter. The following sketch makes the hardware watchdog trigger a reset by suspending the software watchdog and then throwing the ESP into an endless empty loop.

void setup() {
 
  ESP.wdtDisable();
  while (1) {};

}
 
void loop() {}
ESP is an EspClass object declared in ESP.h which is part of the ESP8266/Arduino core included in every Arduino sketch.

Open the serial monitor and upload the sketch. Wait about 8 seconds, and see something like this displayed in the serial monitor window:

initial hardware watchdog timeout

And then... nothing happens. The system is frozen! This is not good. Actually, this is a well-known problem that only occurs on the first watchdog reset after a firmware download by serial link (see esp8266/Arduino FAQ). Manually reset the ESP8266 and from then on the hardware watchdog timeouts and resets the ESP without fail.

hardware watchdog timeout

This initial failure of the hardware watchdog is not a big deal. Presumably an ESP based device will be turned off after uploading its Arduino sketch with a computer and will be powered up again when it is installed in its final position. If uploading occurred in situ as it were, then remember to turn the ESP off and then back on or to reset it manually. It is said that the initial failure does not occur after an OTA upload of a sketch. I have yet to test this.

At first blush, one might desire that there be a way to hook the hardware watchdog timeout routine to save information in non-volatile memory before the reset is performed. As far as I know, this is not easily done. On second thought, only those of us that create bug-free software at first try should lament this lack of flexibility. For my part, I am glad the hardware watchdog works dependably no matter what I might do.

Since there is really nothing much more that can be done with the hardware watchdog, time to go on to the software watchdog.

Reference:

Santos, Nuno (2017), ESP8266: Watchdog functions.
The hardware watchdog biting code can be found on many sites on the Web. I happen to like Nuno Santos' clear exposition.

toc
ESP8266 Sofware Watchdog
Getting the software watchdog to bite on the ESP8266 is even easier.

void setup() {
 
  while (1) {};

}
 
void loop() {}
Again the system will freeze if the sketch is downloaded to the ESP over a serial link. After an initial power cycle or reset, the device will be locked in an endless loop of resets cause by the software watchdog every 3.2 seconds or so.

software watchdog timeout

The EspClass object has a watchdog enable routine with a timeout parameter. Here is part of the class definition that will be useful in this blog.

class EspClass {
   public:
      // TODO: figure out how to set WDT timeout
      void wdtEnable(uint32_t timeout_ms = 0);
      // note: setting the timeout value is not implemented at the moment
      void wdtEnable(WDTO_t timeout_ms = WDTO_0MS);

      void wdtDisable();
      void wdtFeed();

      String getResetReason();
      String getResetInfo();
      struct rst_info * getResetInfoPtr();       

      void reset();
      void restart();
Но, как сказано в комментарии к коду, процедура включения сторожевого таймера не использует значение тайм-аута: wdtEnable(0) и wdtEnable(20000) имеют одинаковый эффект. Они оба включают программный сторожевой таймер с одинаковым периодом ожидания по умолчанию.

С программным сторожевым таймером мало что можно сделать. Его можно включить, отключить и настроить с помощью wdtFeed. С помощью последнего мы можем попытаться определить время до срабатывания сторожевого таймера.

toc
Сторожевой таймер ESP8266: тайм-аут
Информация о тайм-ауте программного сторожевого таймера ESP8266 противоречива. Дэйв Эванс сообщает, что на плате Adafruit Huzzah ESP8286 программный сторожевой таймер срабатывает через 1,5–2,5 секунды. Мой собственный тест с аналогичным кодом, но на плате Wemos D1 mini, показал время от 1,62 до 1,63 секунды. Однако pasko_zh (пашко из Цюриха, извините, но настоящее имя этого участника не указано, чтобы я мог отдать ему должное) утверждает, что время ожидания сторожевого таймера составляет 3,2 секунды. Это значение я встречал и в других источниках, поэтому решил изучить вопрос подробнее.

Я использовал следующий скетч (доступен для скачивания: esp_watchdog_timing.ino) для определения периода аппаратного и программного сторожевого таймера.

/*
 * Тест для определения периода ожидания сторожевых таймеров ESP8266 
 */

//#define SOFT_WATCHDOG // закомментируйте, чтобы проверить аппаратный сторожевой таймер

#ifdef SOFT_WATCHDOG
  #define LOOPS 15
  #define STEP 10
  unsigned long interval = 3150;
#else
  #define LOOPS 10
  #define STEP 100
  unsigned long interval = 7900;
#endif

unsigned long etime;

void setup() {
  Serial.begin(115200);
  Serial.printf("\n\nSdk version: %s\n", ESP.getSdkVersion());
  Serial.printf("Core Version: %s\n", ESP.getCoreVersion().c_str());
  Serial.printf("Boot Version: %u\n", ESP.getBootVersion());
  Serial.printf("Boot Mode: %u\n", ESP.getBootMode());
  Serial.printf("CPU Frequency: %u MHz\n", ESP.getCpuFreqMHz());
  Serial.printf("Reset reason: %s\n", ESP.getResetReason().c_str());
  Serial.println("---------------------------");
  Serial.println("-- WATCHDOG TIMING TESTS --");
  Serial.println("---------------------------\n");

  while (1) {
  
    interval += STEP;
    
    Serial.print("  Testing ");
    #ifdef SOFT_WATCHDOG
      Serial.print("software");
    #else
      Serial.print("hardware");
    #endif  
    Serial.printf(" watchdog with interval of %u ms - ", interval);
    Serial.flush();

    ESP.wdtDisable();
    #ifdef SOFT_WATCHDOG
      ESP.wdtEnable(0);
    #endif  
      
    for (int i = 0; i < LOOPS; i++) {
  
      ESP.wdtFeed();
            
      ESP.wdtDisable();
      #ifdef SOFT_WATCHDOG
        ESP.wdtEnable(0);
      #endif  
        
      etime = millis();
      while (millis() - etime < interval) { }

      ESP.wdtDisable();
      ESP.wdtEnable(0);
      ESP.wdtFeed();
      Serial.printf("%d ", i+1);
    }  
    Serial.println("done.");
  }
}

void loop() { }
Идея достаточно проста. Выполните пустой цикл в течение времени, заданного переменной interval в миллисекундах:

  etime = millis();
  while (millis() - etime < interval) { }
В течение этого времени сторожевой таймер не срабатывает. Если сторожевой таймер не срабатывает, увеличьте интервал и повторите попытку, пока сторожевой таймер не сбросит ESP. Поскольку значение переменной interval выводится на последовательный монитор перед началом пустого цикла, будет легко определить продолжительность тайм-аута сторожевого таймера. Есть два сложных момента. Во-первых, каждый интервал проверяется несколько раз, поскольку ESP может выполнять фоновые задачи, что делает время срабатывания несколько непредсказуемым. Количество итераций теста пустого цикла задается с помощью директивы LOOPS. Вторая сложность заключается в том, что сторожевые таймеры нужно запускать в конце каждой итерации, непосредственно перед выводом номера итерации на последовательный монитор. Похоже, что библиотека Serial взаимодействует со сторожевыми таймерами непредсказуемым образом. Это неудивительно, поскольку и передача, и прием последовательных данных осуществляются с помощью прерываний (см. документацию по ESP8266 Arduino Core.)
Вот вывод скетча в последовательном мониторе Arduino при запуске на Wemos D1 mini для проверки аппаратного сторожевого таймера:

 аппаратный сторожевой таймер

Вот вывод при определении директивы SOFT_WATCHDOG:

software watchdog timeout

The 8.2 seconds timeout for the hardware watchdog and 3.2 seconds for the software watchdog timeout, concord with the times reported by pasko_zh.

Before starting an empty loop to see if the watchdog will timeout, both pasko_zh and I feed and restart the software watchdog, while Dave Evans only feeds it. Note that pasko_zh uses the lower level C functions in user_interface.h so that he begins each test with

  system_soft_wdt_feed();
  system_soft_wdt_restart();  
Такого же результата можно добиться с помощью методов, описанных в ESPclass:
  ESP.wdtFeed();
  ESP.wdtEnable(0);  
Действительно, я изменил test1 скетч Эванса (доступен для скачивания: evans_soft.ino), чтобы убедиться, что именно из-за того, что программный сторожевой таймер не включен, он срабатывает, если его не активировать в течение 1,6–1,8 секунды. Эти результаты могут привести нас к выводу, что начальное значение сторожевого таймера, установленное при перезапуске сторожевого таймера, больше значения, установленного процедурой активации сторожевого таймера. Однако изменение порядка подачи питания на сторожевой таймер и его перезапуска не влияет на результаты измерения времени.
I did not care to push my investigation further. The lesson seems to be that the software watchdog should not be starved for much more than a second to avoid its bite.

References:

Evans, Dave (2016), Watchdog Timer Tests.
paško from Zurich (pasko_zh) (2016), I'm getting these Watchdog Resets "wdt reset" - How can I avoid them?.
toc
Care and Feeding of the ESP8266 Watchdogs
In his discussion of the ESP watchdogs, pasko_zh makes an important point: ESP.wdtFeed() (or system_soft_wdt_feed()) actually feeds both the hardware and the software watchdogs. So the two watchdogs are not cascaded. They are two independent watchdogs with different (and non modifiable) timeout periods that happen to be fed by the same low level function.

В большинстве скетчей для Arduino сторожевые таймеры запускаются неявно. Это происходит в начале каждой итерации программного цикла. Это происходит в функциях yield() и delay() (но не в функции delayMicroseconds()). Некоторые блокирующие функции, например в модуле WiFi, также запускают сторожевые таймеры. Таким образом, «ардуиновский способ» предотвращения программных и аппаратных перезагрузок заключается в том, чтобы любой цикл в скетче длился не более, скажем, 1 секунды (максимум 1,5 секунды) или, при необходимости, регулярно вызывал yield() или delay() по мере надобности.

Вызовы yield() или delay() имеют полезный побочный эффект. Они интегрированы с модулем Wi-Fi, поэтому соединение с сетью Wi-Fi не прерывается, пока работает программный сторожевой таймер. К сожалению, я заметил, что с модулем MQTT pubsub. не все так гладко. Поэтому в тех скетчах, где используется этот модуль, я при необходимости вызываю local_yield() или local_delay() как указано ниже.

/*
 Нужен локальный yield, который вызывает yield() и mqttClient.loop()
 Системная процедура yield() не вызывает mqttClient.loop()
*/

void local_yield()
{
  yield();
  mqttClient.loop();
}

/*
 Нужна локальная задержка, вызывающая yield() и mqttClient.loop()
 Системная функция delay() не вызывает mqttClient.loop()
*/
void local_delay(unsigned long millisecs)
{
  unsigned long start = millis();
  local_yield();
  if (millisecs > 0)
  {
    while (elapsed_time(start) < millisecs)
    {
      local_yield();
    }
  }
}
Note that the following declarations are part of the sketch that contains the above functions

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

WiFiClient    espClient;
PubSubClient  mqttClient(espClient);
toc
Recovery
That is a rather ambiguous if not ambitious title. As it happens, it is possible to obtain the reason why the ESP8266 has come to be started or restarted at the start of a sketch. By recovery, I just mean that it is therefore possible to take different actions based on the cause of the restart.

Quoting the Espressif document ESP8266 Reset Causes and Common Fatal Exception Causes, "Each time [the] ESP8266 reboots, the ROM code will print out a number corresponding to the reset cause." It also prints out something called the boot mode which indicates from where the code was taken. This was visible on the screen shots of the serial monitor window:

   ets Jan 8 2013, rst cause:2, boot mode: (1,6)
 or
   ets Jan 8 2013, rst cause:4, boot mode: (3,6)
Цифра 2 справа от «первой причины» указывает на программный сброс сторожевого таймера, а цифра 4 — на аппаратный сброс. Что касается первой цифры режима загрузки, то 1 означает, что код только что был загружен в ESP, а 3 — что код взят из флэш-памяти SPI.
Объяснение режима загрузки можно найти в документе Макса Филиппова под названием Boot Process. В нем говорится, что режим загрузки ESP определяется тремя младшими битами первого числа режима загрузки. Возможно, называть это «режимом загрузки» не совсем корректно. Другие используют термин «загрузочное устройство». На самом деле значения трех битов соответствуют трем контактам интегральной схемы и определяют источник кода, выполняемого ESP8266. Как видно из следующей таблицы, взятой из документа, есть три варианта:


GPIO15	GPIO0	GPIO2	Dec.	Device	Description
0	0	1	1	UART	Code downloaded from UART (programming)
0	1	1	3	Flash	Boot from code in SPI Flash (normal)
1	x	x	4-7	SDIO	Boot from SD-card
Разумеется, чтобы загрузиться с кода на SD-карте, к ESP8266 необходимо подключить внешний кардридер.

Хотя эта информация полезна при программировании ESP8266 в Arduino IDE, ее необходимо получить, чтобы использовать в скетче. Класс EspClass содержит метод, который возвращает причину сброса в виде строки. Метод ESP.getResetReason() использовался в начальном setup блоке скетча для отслеживания времени. Есть еще одна функция с именем ESP.getResetInfoPtr(), которая возвращает указатель на структуру с информацией о системных сбросах, включая причину последнего сброса, что может быть полезнее, чем строковая функция.

Насколько я могу судить, не существует EspClass способа получить загрузочное устройство. Его можно извлечь из памяти. Участник форума сообщества ESP8266 под ником 0ff опубликовал программу на ассемблере, которая это делает.

Следующий скетч (доступен для скачивания: esp_boot.ino) извлекает загрузочную информацию из раздела setup и затем перезапускает ESP особым образом.

/*
 * esp_boot.ino
 * 
 */
#define BM_WDT_SOFTWARE 0
#define BM_WDT_HARDWARE 1
#define BM_ESP_RESTART 2
#define BM_ESP_RESET 3

#define BOOT_MODE BM_WDT_SOFTWARE

extern "C" {
#include "user_interface.h"
}

struct bootflags
{
  unsigned char raw_rst_cause : 4;
  unsigned char raw_bootdevice : 4;
  unsigned char raw_bootmode : 4;

  unsigned char rst_normal_boot : 1;
  unsigned char rst_reset_pin : 1;
  unsigned char rst_watchdog : 1;

  unsigned char bootdevice_ram : 1;
  unsigned char bootdevice_flash : 1;
};

struct bootflags bootmode_detect(void) {
  int reset_reason, bootmode;
  asm (
    "movi %0, 0x60000600\n\t"
    "movi %1, 0x60000200\n\t"
    "l32i %0, %0, 0x114\n\t"
    "l32i %1, %1, 0x118\n\t"
    : "+r" (reset_reason), "+r" (bootmode) /* Outputs */
    : /* Inputs (none) */
    : "memory" /* Clobbered */
  );

  struct bootflags flags;

  flags.raw_rst_cause = (reset_reason & 0xF);
  flags.raw_bootdevice = ((bootmode >> 0x10) & 0x7);
  flags.raw_bootmode = ((bootmode >> 0x1D) & 0x7);

  flags.rst_normal_boot = flags.raw_rst_cause == 0x1;
  flags.rst_reset_pin = flags.raw_rst_cause == 0x2;
  flags.rst_watchdog = flags.raw_rst_cause == 0x4;

  flags.bootdevice_ram = flags.raw_bootdevice == 0x1;
  flags.bootdevice_flash = flags.raw_bootdevice == 0x3;

  return flags;
}


void setup() {
  Serial.begin(115200);
  Serial.println("\n\nНастройка скетча\n");

  rst_info* rinfo = ESP.getResetInfoPtr();
  
  Serial.printf("rinfo->reason: %d, %s\n", rinfo->reason, ESP.getResetReason().c_str());
  Serial.printf("rinfo->exccause: %d\n", rinfo->exccause);
  Serial.printf("rinfo->epc1: %d\n", rinfo->epc1);
  Serial.printf("rinfo->epc2: %d\n", rinfo->epc2);
  Serial.printf("rinfo->epc3: %d\n", rinfo->epc3);
  Serial.printf("rinfo->excvaddr: %d\n", rinfo->excvaddr);
  Serial.printf("rinfo->depc: %d\n", rinfo->depc);

  struct bootflags bflags = bootmode_detect();

  Serial.printf("\nbootflags.raw_rst_cause: %d\n", bflags.raw_rst_cause);
  Serial.printf("bootflags.raw_bootdevice: %d\n", bflags.raw_bootdevice);
  Serial.printf("bootflags.raw_bootmode: %d\n", bflags.raw_bootmode);
  
  Serial.printf("bootflags.rst_normal_boot: %d\n", bflags.rst_normal_boot);
  Serial.printf("bootflags.rst_reset_pin: %d\n", bflags.rst_reset_pin);
  Serial.printf("bootflags.rst_watchdog: %d\n", bflags.rst_watchdog);
  
  Serial.printf("bootflags.bootdevice_ram: %d\n", bflags.bootdevice_ram);
  Serial.printf("bootflags.bootdevice_flash: %d\n", bflags.bootdevice_flash);

  Serial.printf("\n\nrinfo->reason=%d\n\n", ESP.getResetInfoPtr()->reason);

  if (bflags.raw_bootdevice == 1) {
    Serial.println("Эскиз только что загружен по последовательной ссылке на ESP8266");
    Serial.println("Осторожно: устройство зависнет после перезагрузки на следующем шаге".); 
    Serial.println("Необходимо будет вручную перезагрузить устройство или включить его");
    Serial.println("после этого ESP8266 будет постоянно перезагружаться".);
  }
  
#if BOOT_MODE == BM_ESP_RESTART
  Serial.println("Перезагрузка с помощью ESP.restart()");
  ESP.перезагрузка();
#elif BOOT_MODE == BM_ESP_RESET
  Serial.println("\n\nПерезагрузка с помощью ESP.reset()");
  ESP.reset();
#else 
  #if BOOT_MODE == BM_WDT_HARDWARE      
    Serial.println("\n\nRebooting with hardware watchdog timeout reset");
    ESP.wdtDisable();
  #else
    Serial.println("\n\nRebooting with sofware watchdog timeout reset");
  #endif;  
  while (1) {}  // timeout!
#endif;  
}

void loop() { }
The way the ESP8266 is rebooted is determined by the preprocessor directive BOOT_MODE directive which must be set to one of four values:


Value	Description
BM_WDT_SOFTWARE	Software watchdog timeout
BM_WDT_HARDWARE	Hardware watchdog timeout
BM_ESP_RESTART	Software restart with ESP.restart()
BM_ESP_RESET	Software restart with ESP.reset()
Запуск скетча подтверждает, что при всех четырех способах перезагрузки ESP он зависнет при первой перезагрузке после загрузки скетча. Следовательно, значения в следующей таблице показывают результат запуска скетча только после первого ручного сброса после его загрузки в ESP. Это объясняет, почему boot mode всегда (3, 6) в таблице.


ESP.restart()	ESP.reset()	soft WDT	hard WDT
rst cause	2	2	2	4
boot mode	(3,6)	(3,6)	(3,6)	(3,6)
rinfo->reason	4	4	3	1
rfinfo->exccause	0	0	4	4
raw_rst_cause	2	2	2	4
raw_bootdevice	3	3	3	3
raw_bootmode	6	6	6	6
Первые две строки результатов — это значения ets, отправленные на UART0 в начале перезагрузки, до запуска скетча из флэш-памяти. Следующие две строки показывают частичное содержимое структуры rst_info, возвращаемой методом EspClass ESP.getResetInfoPtr(). Последние три строки — это значения, считанные из памяти ESP с помощью кода из 0ff. Они в точности соответствуют стартовому сообщению «ets».

Очевидно, что rinfo->reason и bootflags.raw_bootdevice наиболее полезны для определения способа запуска скетча. Я завершу обсуждение стандартных сторожевых таймеров ESP8266 примером скетча (скачать: esp_bootex.ino), показывающим, как это можно сделать.

/*
 * esp_bootex.ino
 * 
 */
 
extern "C" {
#include "user_interface.h"
}

int getBootDevice(void) {
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

void setup() {
  Серийный номер.начало(115200);
  Серийный номер.println();

  if ( getBootDevice() == 1 ) {
    Serial.println("\nЭтот скетч только что был загружен через UART.");
    Serial.println("ESP8266 зависнет при первом перезапуске.");
    Serial.println("Нажмите кнопку сброса или выключите и снова включите ESP");
    Serial.println("после чего работа возобновится.");
  }
  
  char buff[32];

  switch (ESP.getResetInfoPtr()->reason) {
    
    case REASON_DEFAULT_RST: 
      // do something at normal startup by power on
      strcpy_P(buff, PSTR("Power on"));
      break;
      
    case REASON_WDT_RST:
      // do something at hardware watch dog reset
      strcpy_P(buff, PSTR("Hardware Watchdog"));     
      break;
      
    case REASON_EXCEPTION_RST:
      // сделать что-то при сбросе исключения
      strcpy_P(buff, PSTR("Exception"));      
      break;
      
    case REASON_SOFT_WDT_RST:
      // сделать что-то при сбросе программного сторожевого таймера
      strcpy_P(buff, PSTR("Software Watchdog"));
      break;
      
    case REASON_SOFT_RESTART: 
      // сделать что-то при программном перезапуске ,system_restart 
      strcpy_P(buff, PSTR("Перезагрузка программного обеспечения/системы"));
      break;
      
    case REASON_DEEP_SLEEP_AWAKE:
      // сделать что-то при выходе из режима глубокого сна
      strcpy_P(buff, PSTR("Выход из режима глубокого сна"));
      break;
      
    case REASON_EXT_SYS_RST:
      // сделать что-то при внешнем системном сбросе (при замыкании контакта сброса)
      strcpy_P(buff, PSTR("Внешняя система"));
      break;
      
    default:  
      // сделать что-то, если сброс произошел по неизвестной причине
      strcpy_P(buff, PSTR("Unknown"));     
      break;
  }
  Serial.printf("\n\nПричина перезагрузки: %s\n", buff);
  Serial.println("----------------------------------------------");
}

char user_input() {
  while (1) {
    Serial.println();
    Serial.println("Чтобы выбрать способ перезагрузки ESP, нажмите");
    Serial.println(" 1 — для ESP.restart()");
    Serial.println(" 2 — для ESP.reset()");
    Serial.println(" 3 — для аппаратного тайм-аута сторожевого таймера");
    Serial.println(" 4 — для программного тайм-аута сторожевого таймера");
    Serial.println("или нажмите кнопку сброса.");
    Serial.println("В любом случае нажмите кнопку сброса, если");
    Serial.println("если этот скетч был загружен только что.");
    while (!Serial.доступно()) {
      задержка(1); 
    }
    char ch = Serial.прочитать();
    если (ch >= '1' && ch <= '4') { 
      return ch; 
    } else {
      Serial.println("\nInvalid entry");
    }
  }  
}

void loop() {
  переключатель (user_input()) {
    case '1':
      Serial.println("\n\nПерезагрузка с помощью ESP.restart()");
      ESP.restart();
      break;
    case '2':  
      Serial.println("\n\nПерезагрузка с помощью ESP.reset()");
      ESP.reset();
      break;
    case '3':  
      Serial.println("\n\nПерезагрузка с помощью аппаратного сброса по таймеру");
      ESP.wdtDisable();
      while (1) {} // timeout
      break;
    case '4':
      Serial.println("\n\nПерезагрузка с сбросом тайм-аута программного сторожевого таймера");
      while (1) {} // timeout
      break;
  }  
}
Как видно, этот скетч практически бесполезен, поскольку все, что он делает, — это дублирует метод EspClass getResetReason(). Конечно, идея в том, что в коде switch/case должно быть что-то более полезное, чем подготовка строки для вывода причины сброса.

```

#### [Как настроить интервал срабатывания сторожевого таймера?](#)

Алиса
В ESP8266 нельзя произвольно задать интервал срабатывания сторожевого таймера — он жёстко задан архитектурой и прошивкой. Но можно подстроиться под него и правильно управлять сбросом.

Что фиксировано, а что можно менять
Программный WDT (soft WDT)
Интервал прерывания: примерно 1638 мс.
Лимит на обработку/сброс: примерно 6554 мс (то есть если не сбросить таймер в течение ~6,5 с после прерывания — будет сброс).
Настройка интервала: недоступна. Эти значения зашиты в SDK. Функции вроде system_soft_wdt_feed() просто говорят системе «я ещё жив», но не меняют таймаут.
В Arduino-обёртке ESP.wdtEnable(timeout_ms) аргумент timeout_ms фактически игнорируется: внутренняя реализация не позволяет переопределить интервал.

Аппаратный WDT
Его параметры тоже не настраиваются программно: он срабатывает, если программный таймер не был обслужен вовремя. Это «страховочный» уровень.

Как на практике «настроить» поведение под свои задачи
Вместо изменения интервала вы управляете частотой сброса (feed) так, чтобы таймер никогда не истекал:

Вызывайте сброс регулярно. Например, в loop() или в конце каждой крупной задачи:
cpp
void loop() {
  // ваш код
  ESP.wdtFeed();  // или system_soft_wdt_feed() для SDK
}
Для длительных операций (сетевые запросы, чтение файлов, сложные вычисления) разбивайте их на части и сбрасывайте WDT между шагами.
Если нужно временно отключить программный WDT, используйте:
cpp
ESP.wdtDisable();        // или system_soft_wdt_stop()
// ... короткий блок кода ...
ESP.wdtEnable();         // или system_soft_wdt_restart()
Но не держите его выключенным дольше ~6 секунд — сработает аппаратный WDT.
Важные нюансы именно для ESP8266
Сетевые и WiFi-операции сами по себе могут занимать много времени и неявно сбрасывать WDT внутри SDK, но полагаться на это рискованно: лучше явно делать ESP.wdtFeed() после долгих вызовов.
Serial.print() в больших объёмах и долгие delay() без сброса WDT — частая причина сбросов.
В логах при сбросе по WDT будет сообщение вроде Soft WDT reset или Hard WDT reset. Это помогает понять, какой именно таймер сработал.
Если нужен другой интервал
Единственный способ получить «свой» интервал — не трогать встроенный WDT, а реализовать собственный watchdog на таймере/millis:

cpp
unsigned long lastFeed = 0;
const unsigned long wdtTimeout = 10000; // 10 секунд

void loop() {
  if (millis() - lastFeed > wdtTimeout) {
    // Здесь можно сделать что-то своё: перезагрузить через ESP.restart(),
    // отправить отчёт, мигнуть светодиодом и т.д.
    ESP.restart();
  }

  // ваш код
  lastFeed = millis(); // «сброс» своего таймера
}
Это не заменит системный WDT, но даст вам контролируемый интервал для собственных действий.

Внешние решения
Если критически важно иметь строго заданный интервал и полный контроль, используют внешний WDT (например, на NE555 или специализированной микросхеме), который сбрасывается сигналом с GPIO и сбрасывает чип по своему таймауту.

Учитывая, что вы раньше интересовались работой с nRF24L01 и стабильностью связи на ESP8266, частая проблема — долгие ожидания ответа от радиомодуля. Если скажете, какой стиль кода вы используете (Arduino, чистый SDK, FreeRTOS), подскажу конкретный шаблон, где и как безопасно вставлять ESP.wdtFeed() рядом с работой nRF24L01.

