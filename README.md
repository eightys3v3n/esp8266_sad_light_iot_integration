ESP8266 Temperature Logger  

# What it does
Allows the integration of a Node ESP8266 into a BIOS S.A.D. light and the utilization of MQTT to turn it on and off.  

# Usage
## You Need:
- A Node ESP8266 (or another model but you might have to change pin names).  
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
Edit the Arduino project for your use case, note the MQTT and WiFi defines.  
Flash the ESP 8266, use serial to test everything works (it seems fine to plug in the USB And the BIOS living light with the ESP 8266 connected in between).  
If everything is working, close up the now smart light. I also made a hole in my enclosure because I plan on doing further software development and I don't want to take it apart every time.  
