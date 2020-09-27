# SoE
## Serial Over Ethernet Device

This is the firmware for a device that can communicate to an RS232 device over Wi-Fi using a ESP8266 Node MCU 12E device and a NulSom Inc. Ultra Compact RS232 to TTL Converter with Male DB9 (3.3V to 5V).

### Hardware

1. **HiLetgo 1PC ESP8266 NodeMCU CP2102 ESP-12E Internet WiFi Development Board** (https://www.amazon.com/gp/product/B010O1G1ES)
2. **NulSom Inc. Ultra Compact RS232 to TTL Converter with Male DB9 (3.3V to 5V)** (https://www.amazon.com/gp/product/B00OPU2QJ4/)

### Software

1. If your device software is capable of communicating directly over an Ethernet port, simply configure the IP address of the device on your network and the port number (defaults to 3001).
2. If your software only knows how to communicate over a COM port then you will need to download a Virtual COM port driver. I use the NetBurner driver which is free. This can be downloaded from https://www.netburner.com/download/virtual-comm-port-driver-windows-xp-10/

The images below show how to connect the RS232 to TTL Converter to the ESP8266.

![](https://raw.githubusercontent.com/porrey/soe/master/Fritzing/SoE_Breadboard.png)

![](https://raw.githubusercontent.com/porrey/soe/master/Fritzing/SoE_Schematic.png)

### Configuration

Follow the instructions for the Virtual COM port driver from NetBurner. My settings for the device I built are shown below.

The image below shows the port after it has been created.
![](https://raw.githubusercontent.com/porrey/soe/master/Images/NetBurner1.png)

This image shows the properties for the defined virtual port.
![](https://raw.githubusercontent.com/porrey/soe/master/Images/NetBurner2.png)

This image shows the COM port in the Devices manager under Windows 10.
![](https://raw.githubusercontent.com/porrey/soe/master/Images/NetBurner3.png)