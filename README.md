# ESP8266 Door sensor on battery

This sensor use the deep sleep ability of the ESP8266. The ESP is awaked when the door state is changing. Then it connects to WiFi, and call an URL to inform the domotic box of the new state of the door. It stay awake about 5 seconds only and then go back to deep sleep.<br>
Deep sleep duration is limited to 207 minutes. So if for any reason, the domotic box has missed the new door state, it will be send again later.

![Alt text](schematic.png?raw=true "Schematic")

## Step by step

1. Print PCB
   <a href="https://easyeda.com/pico12/ESP12-door-sensor-on-battery">https://easyeda.com/pico12/ESP12-door-sensor-on-battery</a>.<br>
   This PCB has a connector with Tx, Rx, Input0, Vcc, Gnd, to allow you to program the ESP.

2. Buy components (you can easiliy find it on eBay or AliExpress) :
   - ESP12
   - CD4077
   - Magnetic door sensor
   - 2 resistors 100k
   - 1 capacitor 1uF
   - USB to ESP8266 ESP-01 Serial Adapter CH340 (to program it)

3. Compile and download software
   - Download <a href="https://www.arduino.cc/en/main/software">Arduino IDE</a>
   - In Arduino IDE, File, Preferences, Additional Boards Manager URLs, add "http://arduino.esp8266.com/staging/package_esp8266com_index.json"
   - In Tools, Board, Boards manager, install "esp8266 by ESP8266 Community"
   - Clone this repository
   - Open .ino file with Arduino IDE

4. Configure it using serial link or WiFi
   To configure the ESP, the input 0 has to be in low state (like to program it).<br>
   You can configure it immediately after program it, or you can put input0 on low state 3 seconds after power up.<br>
   Using serial link you can write command like "param 1 MySSID". You can also use the configuration web page.<br>
   I recommend to avoid using DHCP because it is 5 seconds slower than using a static IP address.

5. DONE !
