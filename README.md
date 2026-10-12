## RF24toStudy

Изначально репозитарий предназначался для изучение nRF24L01+ и библиотеки RF24. Но возникла практическая задача: передавать показания температуры, влажности, уровня воды из подвала дома и принимать данные с помощью этих устройств, а для разработки программного кода использовать PlatformIO (проект ***KvizzyVid17podval***).

### [Шаг1. Начальное знакомство с PlatformIO](PlatformIO_Study/PlatformIO_Study.md)

### [Шаг2. Перенос программного кода из IDE Arduino в PlatformIO](#)

#### [Скетч "Проверить подключение и работоспособность nRF24L01"](KvizzyVid17podval/proverka-rabotosposobnosti/src/main.cpp)

В скетче ***proverka-rabotosposobnosti*** (через PlatformIO) прослушиваются все 128 каналов nRF24L01 на Piranha Pro Mini (нижний уровень ***KvizzyVid17podval***, снимающий показания датчиков и передающий их на верхний уровень) и показывается их загруженность.

Настройки приложения в platformio.ini:

```
[env:pro16MHzatmega328]
platform = atmelavr
board = pro16MHzatmega328
framework = arduino
monitor_speed = 115200
lib_deps = symlink://C:/RF24toStudy

```
В настройках через ***lib_deps*** указывается подключение к единственной нештатной библиотеке RF24.

#### [Скетч "Показать информацию по плате ESP07"](KvizzyVid17podval/Esp8266-Info/src/main.cpp)

В скетче ***Esp8266-Info*** показываются данные ESP07 - платы верхнего уровня  ***KvizzyVid17podval***:  информация о чипе, MAC-адрес в двух представлениях, реальный размер флеш-памяти.

### Копия репозитария TMRh20 bump version to v1.6.0 для изучения

> ***Копия репозитария TMRh20 bump version to v1.6.0 для изучения
> https://github.com/nRF24/RF24/tree/master***

[![Arduino CLI build](https://github.com/nRF24/RF24/actions/workflows/build_arduino.yml/badge.svg)](https://github.com/nRF24/RF24/actions/workflows/build_arduino.yml)
[![Linux build](https://github.com/nRF24/RF24/actions/workflows/build_linux.yml/badge.svg)](https://github.com/nRF24/RF24/actions/workflows/build_linux.yml)
[![PlatformIO build](https://github.com/nRF24/RF24/actions/workflows/build_platformIO.yml/badge.svg)](https://github.com/nRF24/RF24/actions/workflows/build_platformIO.yml)
[![RP2xxx build](https://github.com/nRF24/RF24/actions/workflows/build_rp2xxx.yml/badge.svg)](https://github.com/nRF24/RF24/actions/workflows/build_rp2xxx.yml)
[![Documentation Status](https://readthedocs.org/projects/rf24/badge/?version=latest)](https://rf24.readthedocs.io/en/latest/?badge=latest)

# See http://nRF24.github.io/RF24 for all documentation


