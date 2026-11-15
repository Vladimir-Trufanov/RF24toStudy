/** Arduino, ESP32, C/C++ *************************************** v17WiFi.h ***
 * 
 *                                         Обслужить действия, связанные с WiFi

 * 
 * v1.0.1, 30.03.2026                                 Автор:      Труфанов В.Е.
 * Copyright © 2026 tve                               Дата создания: 26.02.2026
 * 
**/

#pragma once

// Обеспечиваем хранение в NVS (счетчик перезапусков, дата и время)
#include <Arduino.h>
//#include "ArduinoNvs.h"

static bool isWiFi=false;          // true - WiFi подключен
// static uint32_t boot_count;        // счетчик перезапусков

/*
// Настроить интервал синхронизации, имя сервера, режим работы и часовой пояс
void initSNTP(); 
// Отработать момент синхронизации времени с SNTP-сервером 
void notify(struct timeval* t); 
// Извлечь информацию о текущем времени и вывести ее в отформатированном виде
void printTime(); 
// Изменить данные о перезагрузках контроллера 
void setReload(); 
// Настроить часовой пояс                         
void setTimezone(); 
// Ожидать синхронизацию 
void wait4SNTP(); 

// ****************************************************************************
// * Настроить интервал синхронизации, имя сервера, режим работы и часовой пояс
// ****************************************************************************
void initSNTP() 
{ 
   // Определяем, как часто синхронизировать внутренние часы ESP32 с сервером 
   // SNTP. Интервал указывается в микросекундах. Интервал в 60*60*1000UL 
   // микросекунд означает синхронизацию каждый час. Разумные интервалы запросов 
   // обычно составляют от одного-двух раз в день до 5 раз в час. 
   sntp_set_sync_interval(1 * 59 * 60 * 1000UL);  // 59 минут 
   // Указываем функцию уведомления (callback), которая вызывается при каждой с
   // инхронизации. В данном коде для этой цели определяем функцию notify(), 
   // которая просто выводит «synchronized». 
   sntp_set_time_sync_notification_cb(notify);
   // Устанавливаем режим работы: ESP_SNTP_OPMODE_POLL — просто опрашивать
   // сервер SNTP (есть также ESP_SNTP_OPMODE_LISTENONLY)
   esp_sntp_setoperatingmode(ESP_SNTP_OPMODE_POLL);
   // Указываем имя/адрес сервера. При желании можно указать несколько серверов.
   // Например: "ntp.msk-ix.ru"
   //   esp_sntp_setservername(0, "pool.ntp.org");
   //   esp_sntp_setservername(1, "de.pool.ntp.org");
   //   esp_sntp_setservername(2, "time.nist.gov");
   esp_sntp_setservername(0, "ntp.msk-ix.ru");
   // Запускаем службу SNTP с указанными выше параметрами
   esp_sntp_init();
   // Устанавливаем часовой пояс, поскольку сервер SNTP 
   // возвращает время в формате UTC
   setTimezone();
}
// ****************************************************************************
// *         Отработать момент синхронизации времени с SNTP-сервером          *
// ****************************************************************************
void notify(struct timeval* t) 
{
  delay(700); // задержка, чтобы проскочить вывод точек при ожидании синхронизации
  Serial.println("Время синхронизировано!");
}
// ****************************************************************************
// *   Извлечь информацию о текущем времени и вывести ее в отформатированном  *
// *   виде с помощью struct tm структуры данных:                             *
// *                               https://cplusplus.com/reference/ctime/tm/  *
// ****************************************************************************
/ *
 "%A, %B %d %Y %H:%M:%S" - это спецификаторы формата,  которые определяют,  как
 в struct tm timeinfo; будет отформатирован текст, а члены tm struct следующие:
  
  function strftime() - format time as string:
  https://learn.microsoft.com/ru-ru/cpp/c-runtime-library/reference/strftime-wcsftime-strftime-l-wcsftime-l?view=msvc-170

* /
void printTime() 
{
  struct tm timeinfo;
  getLocalTime(&timeinfo);
  Serial.println(&timeinfo, "%d.%m.%Y %H:%M:%S");
}
// ****************************************************************************
// *              Изменить данные о перезагрузках контроллера                 *
// ****************************************************************************
void setReload() 
{
  // Инициируем NVS
  NVS.begin();
  // После каждой перезагрузки увеличиваем счётчик загрузок и выводим в терминал.
  if (NVS.getInt("boot_count")) 
  {
    boot_count = NVS.getInt("boot_count"); 
    boot_count++;
  }
  else 
  {
    Serial.println("Еще нет данных о перезагрузках контроллера!");
    boot_count = 1; 
  }
  NVS.setInt("boot_count", boot_count);
  // Выбираем данные о текущем времени и дате
  struct tm timeinfo;
  getLocalTime(&timeinfo);
  // Формируем и записываем дату
  char divreload[12];
  strftime(divreload, sizeof(divreload), "%d.%m.%Y", &timeinfo);
  String datereload=String(divreload);
  NVS.setString("datereload", datereload);
  // Формируем и записываем время
  char tivreload[10];
  strftime(tivreload, sizeof(tivreload), "%H:%M:%S", &timeinfo);
  String timereload=String(tivreload);
  NVS.setString("timereload", timereload);
}
// ****************************************************************************
// *                           Настроить часовой пояс                         *
// ****************************************************************************
void setTimezone() 
{ 
   // Здесь устанавливается стандартное время для региона - Европа/Москва
   // https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
   setenv("TZ", "MSK-3", 1);
   tzset();
}
// ****************************************************************************
// *                            Ожидать синхронизацию                         *
// ****************************************************************************
void wait4SNTP() 
{
  Serial.println("Ожидание синхронизации ");
  while (sntp_get_sync_status() != SNTP_SYNC_STATUS_COMPLETED) 
  {
    delay(450);
    Serial.print(".");
  }
  Serial.println(" ");
}


// ****************************************************************************
// *            Подготовить параметры сетей для CameraWebServer               *
// * (инициировать работу контроллера,как станции WiFi и с собственной сетью) *
// ****************************************************************************
void IniSayWiFi()
{
  // Инициируем hssid, hlocalIP, hsoftAPIP - параметры локальной и собственной 
  // сети контроллера для передачи в html
  String(WiFi.SSID()).toCharArray(hssid,String(WiFi.SSID()).length()+1); 
  String ipaddr=WiFi.localIP().toString(); ipaddr.toCharArray(hlocalIP,ipaddr.length()+1); 
  ipaddr=WiFi.softAPIP().toString(); ipaddr.toCharArray(hsoftAPIP,ipaddr.length()+1); 
  // Инициируем номер последней перезагрузки
  NVS.begin();
  sprintf(nvreload, "%d", NVS.getInt("boot_count")); 
  // Инициируем дату последней перезагрузки
  String datereload = NVS.getString("datereload");
  datereload.toCharArray(dvreload,datereload.length()+1); 
  // Инициируем время последней перезагрузки
  String timereload = NVS.getString("timereload");
  timereload.toCharArray(tvreload,timereload.length()+1); 

  Serial.print("IP - собственной сети:  ");  Serial.print(hsoftAPIP); Serial.print(" "); Serial.println(soft_ap_ssid);
  Serial.print("IP - рабочей станции:   ");  Serial.print(hlocalIP);  Serial.print(" "); Serial.println(hssid);
  Serial.print("Последняя перезагрузка: ");  Serial.print(nvreload);  Serial.print(" ["); Serial.print(dvreload); Serial.print(", "); Serial.print(tvreload); Serial.println("]");
}

*/

// ************************************************************** v17WiFi.h ***
