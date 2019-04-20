# CO2 driven ventilation using Brink Air 70 ventilation with Netatmo Weather Station

This project uses an [ESP32](https://www.espressif.com/en/products/hardware/esp32/overview) module 
to read the indoor CO2 levels via an 
[Netatmo Weather Station](https://www.netatmo.com/en-us/weather/weatherstation) through their API, 
to control a [Brink Air 70](https://www.brinkclimatesystems.nl/en-us/international/home/products/decentraal-ventileren/air-70) 
heat exchanging ventilation unit.

## Getting started

You can just open the Arduino project in Arduino IDE and compile/program for your ESP32 module. Change the [settings template](settings_template.h) filename
to settings.h and obviously change the settings accordingly.

## Used Arduino libraries

* [WiFiCLientSecure](https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFiClientSecure) v1.0.0 by Evandro Luis Copercini
* [ArduinoJson](https://arduinojson.org) v6.10.0 by Benoit Blanchon

## Relevant resources

* [Brink Air 70 installation manual](https://www.brinkclimatesystems.nl/getattachment/ea5685ab-a123-4766-9812-0aa5513de6b1), with some control options explained
* [Netatmo Weather API](https://dev.netatmo.com/resources/technical/reference/weather)
* [Netatmo Auth API](https://dev.netatmo.com/resources/technical/guides/authentication/clientcredentials)

## License

This project is licensed under The Unlicense License - see the [LICENSE](LICENSE) file for details
