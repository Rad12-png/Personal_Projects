# Personal Projects
My personal projects, including CAD, Arduino coding, Raspberry Pi, Python &amp; C++ scripts, etc.

## 2026 ESSS EC
* File #1:  ``PicoEarthquakeDetectionProgram.cpp``
  * The program I wrote for monitoring accelerometer data on a Raspberry Pico. It takes the accelerometer data and applies it to a scaling system I designed. 0 = no quake, 1 = moderate quake, and 2 = severe quake. Depending on the quake severity it triggers a unique lighting and sound procedure to warn residents of the disaster.
* File #2: `` UI_earthquake.py ``
  * This program is a simple UI that displays three graphs using data directly from the accelerometer. It shows the acceleration, velocity, displacement, and maximum experienced G-force of the building. By considering the magnitude of the acceleration by calculating the vector (not including gravity), I integrated the value to find velocity and displacement. I filtered out the drift noise from the system to keep each plot flat when no earthquake was induced. I calculated the G-force by multiplying the magnitude of the acceleration vector by the acceleration of gravity (9.8).
* File #3: ``UnoFourDigitDisplayAndLight.cpp``
  * This was what set our project apart in the competition. We included a 4 digit LCD display that outputted a countdown warning users of the time they needed to stay under cover. This countdown was dependent on the severity of the earthquake: 10s for moderate and 30S for severe. It also controls a secondary lighting system that is depended on the flashing warning lights from the Pico program. 
