/** Arduino, ESP32, C/C++ *************************************** v17WiFi.h ***
 * 
 *                                         Обслужить действия, связанные с WiFi

 * 
 * v1.1.2, 20.11.2026                                 Автор:      Труфанов В.Е.
 * Copyright © 2026 tve                               Дата создания: 26.02.2026
 * 
**/

#pragma once

// Обеспечиваем хранение в NVS (счетчик перезапусков, дата и время)
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
// Подключаем библиотеку для связи с сервером SNTP, которая является 
// библиотекой ядра ESP32 по умолчанию и не требуют установки
#include "esp_sntp.h"

static bool isWiFi=false;          // true - WiFi подключен

// Настроить интервал синхронизации, имя сервера, режим работы и часовой пояс
void initSNTP(); 
// Отработать момент синхронизации времени с SNTP-сервером 
void notify(struct timeval* t); 
// Обработать событие WiFi
void onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info); 
// Вывести причину разрыва связи WiFi
void onWiFiSay(uint8_t discon_reason);
// Извлечь информацию о текущем времени и вывести ее в отформатированном виде
void printTime(); 
// Настроить часовой пояс                         
void setTimezone(); 
// Ожидать синхронизацию 
void wait4SNTP(); 
// Подключиться к одной из назначенных сетей 
void WiFi_connect(WiFiMulti wifiMulti, const char* mess="\nОжидание подключения к WiFi: ..."); 

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
   // Указываем функцию уведомления (callback), которая вызывается при каждой 
   // синхронизации. В данном коде для этой цели определяем функцию notify(), 
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
// *                           Обработать событие WiFi                        *
// ****************************************************************************
void onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) 
{
  switch (event) 
  {
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      Serial.println("Подключено к точке доступа");
      break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.print("Получен IP-адрес: ");
      Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.print("Связь с Wi-Fi потеряна. Причина: ");
      onWiFiSay(info.wifi_sta_disconnected.reason);
      isWiFi=false;  
      break;
    default:
      break;
  }
}
// ****************************************************************************
// *                 Вывести причину разрыва связи WiFi                       *
// ****************************************************************************
void onWiFiSay(uint8_t discon_reason) 
{
  Serial.print((uint8_t)discon_reason); Serial.print(", ");
  switch (discon_reason) 
  {
    case WIFI_REASON_MIC_FAILURE:
      Serial.println("Cбой проверки целостности сообщения");
      break;
    default:
      Serial.println("Cбой ещё не зарегистрирован");
      break;
  }
  /*
  Поле info.wifi_sta_disconnected.reason в обработчиках событий Wi-Fi на ESP32 (например, в Arduino IDE или ESP-IDF) 
  содержит числовой код, который указывает на причину разрыва соединения со станцией (STA). 

  "esp_witi_types_generic.h"

  typedef enum {
    WIFI_REASON_UNSPECIFIED                        = 1,     < Unspecified reason 
    WIFI_REASON_AUTH_EXPIRE                        = 2,     < Authentication expired 
    WIFI_REASON_AUTH_LEAVE                         = 3,     < Deauthentication due to leaving 
    WIFI_REASON_ASSOC_EXPIRE                       = 4,     < Deprecated, will be removed in next IDF major release 
    WIFI_REASON_DISASSOC_DUE_TO_INACTIVITY         = 4,     < Disassociated due to inactivity 
    WIFI_REASON_ASSOC_TOOMANY                      = 5,     < Too many associated stations 
    WIFI_REASON_NOT_AUTHED                         = 6,     < Deprecated, will be removed in next IDF major release 
    WIFI_REASON_CLASS2_FRAME_FROM_NONAUTH_STA      = 6,     < Class 2 frame received from nonauthenticated STA 
    WIFI_REASON_NOT_ASSOCED                        = 7,     < Deprecated, will be removed in next IDF major release 
    WIFI_REASON_CLASS3_FRAME_FROM_NONASSOC_STA     = 7,     < Class 3 frame received from nonassociated STA 
    WIFI_REASON_ASSOC_LEAVE                        = 8,     < Deassociated due to leaving 
    WIFI_REASON_ASSOC_NOT_AUTHED                   = 9,     < Association but not authenticated 
    WIFI_REASON_DISASSOC_PWRCAP_BAD                = 10,    < Disassociated due to poor power capability 
    WIFI_REASON_DISASSOC_SUPCHAN_BAD               = 11,    < Disassociated due to unsupported channel 
    WIFI_REASON_BSS_TRANSITION_DISASSOC            = 12,    < Disassociated due to BSS transition 
    WIFI_REASON_IE_INVALID                         = 13,    < Invalid Information Element (IE) 
    WIFI_REASON_MIC_FAILURE                        = 14,    < MIC failure 
    WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT             = 15,    < 4-way handshake timeout 
    WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT           = 16,    < Group key update timeout 
    WIFI_REASON_IE_IN_4WAY_DIFFERS                 = 17,    < IE differs in 4-way handshake 
    WIFI_REASON_GROUP_CIPHER_INVALID               = 18,    < Invalid group cipher 
    WIFI_REASON_PAIRWISE_CIPHER_INVALID            = 19,    < Invalid pairwise cipher 
    WIFI_REASON_AKMP_INVALID                       = 20,    < Invalid AKMP 
    WIFI_REASON_UNSUPP_RSN_IE_VERSION              = 21,    < Unsupported RSN IE version 
    WIFI_REASON_INVALID_RSN_IE_CAP                 = 22,    < Invalid RSN IE capabilities 
    WIFI_REASON_802_1X_AUTH_FAILED                 = 23,    < 802.1X authentication failed 
    WIFI_REASON_CIPHER_SUITE_REJECTED              = 24,    < Cipher suite rejected 
    WIFI_REASON_TDLS_PEER_UNREACHABLE              = 25,    < TDLS peer unreachable 
    WIFI_REASON_TDLS_UNSPECIFIED                   = 26,    < TDLS unspecified 
    WIFI_REASON_SSP_REQUESTED_DISASSOC             = 27,    < SSP requested disassociation 
    WIFI_REASON_NO_SSP_ROAMING_AGREEMENT           = 28,    < No SSP roaming agreement 
    WIFI_REASON_BAD_CIPHER_OR_AKM                  = 29,    < Bad cipher or AKM 
    WIFI_REASON_NOT_AUTHORIZED_THIS_LOCATION       = 30,    < Not authorized in this location 
    WIFI_REASON_SERVICE_CHANGE_PERCLUDES_TS        = 31,    < Service change precludes TS 
    WIFI_REASON_UNSPECIFIED_QOS                    = 32,    < Unspecified QoS reason 
    WIFI_REASON_NOT_ENOUGH_BANDWIDTH               = 33,    < Not enough bandwidth 
    WIFI_REASON_MISSING_ACKS                       = 34,    < Missing ACKs 
    WIFI_REASON_EXCEEDED_TXOP                      = 35,    < Exceeded TXOP 
    WIFI_REASON_STA_LEAVING                        = 36,    < Station leaving 
    WIFI_REASON_END_BA                             = 37,    < End of Block Ack (BA) 
    WIFI_REASON_UNKNOWN_BA                         = 38,    < Unknown Block Ack (BA) 
    WIFI_REASON_TIMEOUT                            = 39,    < Timeout 
    WIFI_REASON_PEER_INITIATED                     = 46,    < Peer initiated disassociation 
    WIFI_REASON_AP_INITIATED                       = 47,    < AP initiated disassociation 
    WIFI_REASON_INVALID_FT_ACTION_FRAME_COUNT      = 48,    < Invalid FT action frame count 
    WIFI_REASON_INVALID_PMKID                      = 49,    < Invalid PMKID 
    WIFI_REASON_INVALID_MDE                        = 50,    < Invalid MDE 
    WIFI_REASON_INVALID_FTE                        = 51,    < Invalid FTE 
    WIFI_REASON_TRANSMISSION_LINK_ESTABLISH_FAILED = 67,    < Transmission link establishment failed 
    WIFI_REASON_ALTERATIVE_CHANNEL_OCCUPIED        = 68,    < Alternative channel occupied 

    WIFI_REASON_BEACON_TIMEOUT                     = 200,   < Beacon timeout 
    WIFI_REASON_NO_AP_FOUND                        = 201,   < No AP found 
    WIFI_REASON_AUTH_FAIL                          = 202,   < Authentication failed 
    WIFI_REASON_ASSOC_FAIL                         = 203,   < Association failed 
    WIFI_REASON_HANDSHAKE_TIMEOUT                  = 204,   < Handshake timeout 
    WIFI_REASON_CONNECTION_FAIL                    = 205,   < Connection failed 
    WIFI_REASON_AP_TSF_RESET                       = 206,   < AP TSF reset 
    WIFI_REASON_ROAMING                            = 207,   < Roaming 
    WIFI_REASON_ASSOC_COMEBACK_TIME_TOO_LONG       = 208,   < Association comeback time too long 
    WIFI_REASON_SA_QUERY_TIMEOUT                   = 209,   < SA query timeout 
    WIFI_REASON_NO_AP_FOUND_W_COMPATIBLE_SECURITY  = 210,   < No AP found with compatible security 
    WIFI_REASON_NO_AP_FOUND_IN_AUTHMODE_THRESHOLD  = 211,   < No AP found in auth mode threshold 
    WIFI_REASON_NO_AP_FOUND_IN_RSSI_THRESHOLD      = 212,   < No AP found in RSSI threshold 
  } wifi_err_reason_t;
  */
}
// ****************************************************************************
// *   Извлечь информацию о текущем времени и вывести ее в отформатированном  *
// *   виде с помощью struct tm структуры данных:                             *
// *                               https://cplusplus.com/reference/ctime/tm/  *
// ****************************************************************************
/*
 "%A, %B %d %Y %H:%M:%S" - это спецификаторы формата,  которые определяют,  как
 в struct tm timeinfo; будет отформатирован текст, а члены tm struct следующие:
  
  function strftime() - format time as string:
  https://learn.microsoft.com/ru-ru/cpp/c-runtime-library/reference/strftime-wcsftime-strftime-l-wcsftime-l?view=msvc-170

*/
void printTime() 
{
  struct tm timeinfo;
  getLocalTime(&timeinfo);
  Serial.println(&timeinfo, "%d.%m.%Y %H:%M:%S");
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
// *               Подключиться к одной из назначенных сетей                  *
// ****************************************************************************
void WiFi_connect(WiFiMulti wifiMulti, const char* mess) 
{
  Serial.print(mess);
  isWiFi=false; 
  while ((wifiMulti.run() != WL_CONNECTED)) 
  {
    Serial.print(".");
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
  Serial.println(" ");
  Serial.print("Контроллер подключен к WiFi-сети "); Serial.println(WiFi.SSID());
  Serial.print("IP адрес:                        "); Serial.println(WiFi.localIP());
  isWiFi=true;
  vTaskDelay(500/portTICK_PERIOD_MS);
}

/*
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
