# MCVE_5019
MCVE for https://github.com/esp8266/Arduino/issues/5019

Project created in Visual Studio with Visual Micro extension.

Fails to compile, yet toolchain installed? Try Tools - Visual Micro- Rescan Toolchains and Libraries.

IDE Settings:

![Image of ESP8266 Settings](https://raw.githubusercontent.com/gojimmypi/MCVE_5019/master/images/ESP8266_Settings.PNG)

See application settings in GlobalDefine.h

When connecting to a normal WiFi AP, this app works just fine. When first connecting to a guest WiFi and needing to "Accept Terms and Conditions", the `client.connect(DASHBOARD_HOST, httpPort)` fails with an error.
