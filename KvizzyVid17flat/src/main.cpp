// ******************************************************** KvizzyVid17flat ***
// *                                                                          *
// *            Принимать json-сообщения о показаниях температуры, влажности, *
// *      уровня воды, напряжения питания с нижнего уровня (из подвала дома), *
// *         снимать значение напряжения на промежуточном уровне и передавать *
// *        по Internet WiFi на сайт (выполнять работу промежуточного уровня) * 
// *                                                                          *
// * v1.2.2, 15.11.2026                            Автор:       Труфанов В.Е. *
// * Copyright © 2026 tve                          Дата создания:  19.10.2026 *
// ****************************************************************************

#include <Arduino.h>

// Подключаем библиотеки для подключения камеры к WiFi
#include <WiFi.h>
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#include "v17WiFi.h"

#include "Vid17flatDefine.h"
#include "Vid17flatCommon.h"
#include "Vid17flatHTTPS.h"

// Определяем задачи и готовим работу сторожевого таймера 
#include "v17WdtTask.h"        
// Подключаем задачи
#include "v17Base.h"          // 4-3307 принять сообщение нижнего уровня и отправить на сайт
#include "v17Power.h"         // 4-3511 определить напряжение питания и отправить на сайт


// Явная функция-обработчик
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
      //Serial.println("Связь с точкой доступа потеряна");
      // Здесь можно добавить код для попытки повторного подключения
      Serial.print("Wi-Fi потерял связь. Причина: ");
      Serial.println(info.wifi_sta_disconnected.reason);
      isWiFi=false;  
      break;
    default:
      break;
  }
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




void setup() 
{
  Serial.begin(115200);
  vTaskDelay(5000/portTICK_PERIOD_MS);

  // Назначаем контакт A0 на приём показаний напряжения питания
  pinMode(A0, INPUT); 

  // Инициируем сторожевой таймер 
  iniWdt(); 

  // Назначаем сети для подключения по WiFi
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP("OPPO A9 2020", "b277a4ee84e8");
  wifiMulti.addAP("TP-Link_B394",  "18009217");
  wifiMulti.addAP("tve-DESKTOP",   "Ue18-647");
  wifiMulti.addAP("tve-MONOBLOCK", "Ue18-647");
  wifiMulti.addAP("linksystve",    "X93K6KQ6WF");
  wifiMulti.addAP("GoshaIMila",    "t1s2wde4bE");
  
  // Выполняем подключение к WiFi-сети
  WiFi_connect(wifiMulti); 

  /*
  Serial.print("mess");
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
  */
 
  // Включаем обработку события отсоединения с WiFi через безымянную лямбда-функцию
  // (так как это выполняется в отдельном потоке, то просто устанавливаем флаг,
  // чтобы его обнаружить в цикле loop и переподключить WiFi) 

  /*
  Чтобы обработать событие Wi-Fi через WiFi.onEvent() и сделать это явно, нужно создать отдельную функцию, которая будет реагировать на конкретное событие, и зарегистрировать её с помощью метода onEvent(). 
github.com
docs.espressif.com
Как это работает
Метод onEvent() принимает два аргумента:
Функция-обработчик. Это функция, которая будет вызвана при возникновении события.
Идентификатор события (опционально). Если указать конкретный тип события, обработчик вызовется только для него. Если не указывать — функция будет реагировать на все события.
 
github.com
docs.espressif.com
Внутри функции-обработчика вы определяете, какое именно событие произошло, и пишете код, который должен выполниться в этом случае
*/

  // Регистрируем обработчик gjnthb ctnb WiFi
  WiFi.onEvent(onWiFiEvent, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);




  /*
  WiFiEventId_t eventID = WiFi.onEvent
  (
    [](WiFiEvent_t event, WiFiEventInfo_t info) 
    {
      Serial.print("Wi-Fi потерял связь. Причина: ");
      Serial.println(info.wifi_sta_disconnected.reason);
      isWiFi=false;  
    },
    WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED
  );
  */

  /*
  // Привязываем возможные сети и подключаемся к WiFi
  wifiMulti.addAP("TP-Link_B394",  "18009217");
  wifiMulti.addAP("tve-DESKTOP",   "Ue18-647");
  wifiMulti.addAP("OPPO A9 2020",  "b277a4ee84e8");
  wifiMulti.addAP("tve-MONOBLOCK", "Ue18-647");
  wifiMulti.addAP("linksystve",    "X93K6KQ6WF");
  wifiMulti.addAP("GoshaIMila",    "t1s2wde4bE");
  Serial.println("Подключаемся к WiFi ... ");
  delay(100);
  if (wifiMulti.run() == WL_CONNECTED) 
  {
    Serial.println("WiFi подсоединен: ");
    isWiFi=true; 
  }
  */

  // Настраиваем интервал синхронизации, имя сервера, режим работы и часовой пояс
  //initSNTP();
  // Ожидаем синхронизацию 
  //wait4SNTP();
  // Изменяем данные о перезагрузках контроллера            
  //setReload(); 
  // Готовим параметры сетей для CameraWebServer   
  //IniSayWiFi();
  
  // Инициируем задачи
  xTaskCreatePinnedToCore(  // Принять сообщение нижнего уровня и отправить на сайт 
  vBase,                    // Task function
  "vBase",                  // Task name
  4096,                     // Stack size
  NULL,                     // Parameters passed to the task function
  5,                        // Priority
  NULL,                     // Task handle
  1); 
  
  xTaskCreatePinnedToCore(  // Определить напряжение питания и отправить на сайт 
  vPower,                   // Task function
  "vPower",                 // Task name
  4096,                     // Stack size
  NULL,                     // Parameters passed to the task function
  4,                        // Priority
  NULL,                     // Task handle
  1); 
} 
  

void loop() 
{
  // Отмечаем начало нового цикла 
  iLoop++;
  uint32_t tTime=millis();

  // Считываем с последовательного порта целое число
  // (так как в зависимости от окружения за целым числом может следовать нулевое значение,
  // то отсекаем 0)
  if (Serial.available() > 0) 
  {
    int ii=Serial.parseInt();
    if (ii>0) inumber=ii;
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
   
  // Получаем значение напряжения источника питания схемы (Вольт) 
  float Vbattf = getVbattf();
  Serial.println(Vbattf, 3);
  vTaskDelay(3000/portTICK_PERIOD_MS);
  
  //
  
  if (!isWiFi)
  {
    Serial.println("Перезагружаем контроллер после потери сети WiFi");
    ESP.restart();
    
    /*
    Serial.println("Подключается WiFi после потери сети");
    wifiMulti.addAP("TP-Link_B394",  "18009217");
   
    // Подключаемя к одной из назначенных сетей
    WiFi_connect(wifiMulti); 
    */
  }
   
  // Определяем время работы цикла задачи
  tLoop=millis()-tTime;
  // Отмечаем завершение задачи для сторожевого таймера
  finLoop = true;
  if (inumber == 1) MimicMCUhangEvent("Loop");   
}

// ******************************************************** KvizzyVid17flat ***
