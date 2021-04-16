ESP8266 Temperature Logger  

# What it does
Allows the integration of a Node ESP8266 into a BIOS S.A.D. light and the utilization of MQTT or Home Assistant to turn it on and off.  

# Usage
## You Need:
- A NodeMCU 1.0 ESP8266 (or another model but you might have to change pin names).  
- A BIOS Seasonal Affective Disorder light.
- Soldering iron and bits.
- Wires.

## How To
You want to take apart the BIOS light and remove the main circuit board.  
Identify the ground, 3.3V, and On/Off testing pads.  
Solder a wire to each of those testing pads making sure to color them appropriately (or note down which is which as we can't see them when the board is screwed in).  
Put the circuit board back in the case, use tape to afix the ESP 8266 to the back of the LED saucer.  
Put the 3.3V wire onto the ESP 8266's 3.3V pin, ground to ground, and the On/Off wire to the D1 pin.  
Plug in the ESP 8266.  
Edit the espurna/code/espurna/config/credentials.h file according to the espurna/README.md.  
Flash ESPurna onto the ESP8266 by running espurna/code/build.sh from that directory.  
Scan your network to find the new WiFi device, login to it with the information in credentials.h and:  
- turn on MQTT  
- turn on HASS Discovery  
- Change the host name to something you like  

