
//----------------------------------------------------------------------------------
//  Prints last reset reason of ESP32
//  https://github.com/espressif/arduino-esp32/blob/master/libraries/ESP32/examples/ResetReason/ResetReason2/ResetReason2.ino

//  Здесь используется механизм IDF, который сохраняет причины сбоя при перезагрузке.
//  See https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/misc_system_api.html#software-reset
//
//  This example uses esp_reset_reason() instead of rtc_get_reset_reason(0).
//  This example forces a WDT timeout in 10s.
//
//  Note enableLoopWDT() cannot be used here because it also takes care of
//  resetting WDT.  See cores/esp32/main.cpp::loopTask().
//
//  Author: David McCurley
//  Public Domain License
//----------------------------------------------------------------------------------
//  2023.04.21 r1.0 Initial release
//----------------------------------------------------------------------------------

#include "esp_task_wdt.h"

//Converts reason type to a C string.
//Type is located in /tools/sdk/esp32/include/esp_system/include/esp_system.h
const char *resetReasonName(esp_reset_reason_t r) 
{
  switch (r) 
  {
    case ESP_RST_UNKNOWN:   return "Unknown";
    case ESP_RST_POWERON:   return "PowerOn";    //Power on or RST pin toggled
    case ESP_RST_EXT:       return "ExtPin";     //External pin - not applicable for ESP32
    case ESP_RST_SW:        return "Reboot";     //esp_restart()
    case ESP_RST_PANIC:     return "Crash";      //Exception/panic
    case ESP_RST_INT_WDT:   return "WDT_Int";    //Interrupt watchdog (software or hardware)
    case ESP_RST_TASK_WDT:  return "WDT_Task";   //Task watchdog
    case ESP_RST_WDT:       return "WDT_Other";  //Other watchdog
    case ESP_RST_DEEPSLEEP: return "Sleep";      //Reset after exiting deep sleep mode
    case ESP_RST_BROWNOUT:  return "BrownOut";   //Brownout reset (software or hardware)
    case ESP_RST_SDIO:      return "SDIO";       //Reset over SDIO
    default:                return "Undefine";
  }
}

void PrintResetReason(void) 
{
  esp_reset_reason_t r = esp_reset_reason();
  if (r == ESP_RST_POWERON) 
  {
    delay(6000);  // Wait for serial monitor to connect
  }
  Serial.printf(" [%i - %s]\r\n", r, resetReasonName(r));
}



/*
  https://github.com/espressif/arduino-esp32/blob/master/libraries/ESP32/examples/ResetReason/ResetReason/ResetReason.ino 
                                                         
  Print last reset reason of ESP32
*  =================================
*
* Используйте любой из методов print_reset_reason
  или verbose_print_reset_reason, чтобы отобразить
  причину последнего сброса этого устройства.
*  Author:
*  Evandro Luis Copercini - 2017
*/

#include <Arduino.h>

#if CONFIG_IDF_TARGET_ESP32  // ESP32/PICO-D4
#include "esp32/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C2
#include "esp32c2/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C6
#include "esp32c6/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32P4
#include "esp32p4/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C5
#include "esp32c5/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C61
#include "esp32c61/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S31
#include "esp32s31/rom/rtc.h"
#else
#error Target CONFIG_IDF_TARGET is not supported
#endif

void print_reset_reason(int reason) 
{
  switch (reason) 
  {
    case 1:  Serial.print("POWERON_RESET - cброс при включении питания"); break;           // Vbat power on reset
    case 3:  Serial.print("SW_RESET - программный сброс ядра контроллера"); break;         // Software reset digital core
    case 4:  Serial.print("OWDT_RESET - сброс ядра по устаревшему таймеру"); break;        // Legacy watch dog reset digital core
    case 5:  Serial.print("DEEPSLEEP_RESET - сброс в режиме глубокого сна"); break;        // Deep Sleep reset digital core
    case 6:  Serial.print("SDIO_RESET - сброс ядра с помощью модуля SLC"); break;          // Reset by SLC module, reset digital core
    case 7:  Serial.print("TG0WDT_SYS_RESET - сброс по 0-му сторожевому таймеру"); break;  // Timer Group0 Watch dog reset digital core
    case 8:  Serial.print("TG1WDT_SYS_RESET - сброс по 1-му сторожевому таймеру"); break;  // Timer Group1 Watch dog reset digital core
    case 9:  Serial.print("RTCWDT_SYS_RESET - сброс ядра по RTC Watchdog"); break;         // RTC Watch dog Reset digital core
    case 10: Serial.print("INTRUSION_RESET - проверка перезагрузки"); break;               // Instrusion tested to reset CPU
    case 11: Serial.print("TGWDT_CPU_RESET - сброс по аппаратному таймеру"); break;        // Time Group reset CPU
    case 12: Serial.print("SW_CPU_RESET - программный сброс процессора "); break;          // Software reset CPU
    case 13: Serial.print("RTCWDT_CPU_RESET - сброс по срабатывания таймера RTC"); break;  // RTC Watch dog Reset CPU
    case 14: Serial.print("EXT_CPU_RESET - сброс APP CPU основным процессором"); break;    // for APP CPU, reset by PRO CPU
    case 15: Serial.print("RTCWDT_BROWN_OUT_RESET - просадка напряжения"); break;          // Reset when the vdd voltage is not stable
    case 16: Serial.print("RTCWDT_RTC_RESET - сброс системы и модуля RTC"); break;         // RTC Watch dog reset digital core and rtc module
    default: Serial.print("NO_MEAN - неучтенный сброс процессора");
  }
  PrintResetReason();
}

/*
  Example Serial Log:
  ====================

rst:0x10 (RTCWDT_RTC_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0x00
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:1
load:0x3fff0008,len:8
load:0x3fff0010,len:160
load:0x40078000,len:10632
load:0x40080000,len:252
entry 0x40080034
CPU0 reset reason:
RTCWDT_RTC_RESET
RTC Watch dog reset digital core and rtc module
CPU1 reset reason:
EXT_CPU_RESET
for APP CPU, reset by PRO CPU
Going to sleep
ets Jun  8 2016 00:22:57

rst:0x5 (DEEPSLEEP_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0x00
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:1
load:0x3fff0008,len:8
load:0x3fff0010,len:160
load:0x40078000,len:10632
load:0x40080000,len:252
entry 0x40080034
CPU0 reset reason:
DEEPSLEEP_RESET
Deep Sleep reset digital core
CPU1 reset reason:
EXT_CPU_RESET
for APP CPU, reset by PRO CPU
Going to sleep
ets Jun  8 2016 00:22:57
*/

