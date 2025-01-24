# smartfan

# Introduction
This project is used to make a controller that is used to make the Juniper SRX320 firmware more silent. The device has two 4cm fans and it's very noisy when the device is under a heavy workload.</br>
I attempted to use other silence fans to replace the old one but the cooling efficiency is not good.

# Base design
The project is based on the Micropython project and RP2040 chip, the total cost is ~$8. The project needs to meet the following requirements:

- Control the 4-wire fan speed
- The speed curve can be changed
- The setting can be saved
- The easy way to identify the FAN or sensors which one of them has hardware failure

# Usage
The board can be used below ways:
1. GPU fan speed management
2. Pump speed of the external liquid cooling system
3. The same usage is familiar to me

# Board picture
![Board](Media/IMG_1095.jpg)

# Illustration

![Block](Media/functions.jpg)

# How to use this board
0. The MODE button can change the board between FIX mode and AUTO mode, and the MODESEL LED will be on/off.
    - Push this button will trigger a forcibly time.sleep in 6 seconds to ensure that all configurations are saved.
1. In AUTO mode, user can set the temperature range through "HIGH" button and "LOW" button.
    - HIGH button can change the upper level in 40 degrees to 70 degrees.
    - LOW button can change the lower level in 0 degrees to 30 degrees.
    - The PWM duty will be recalculated with the new temperature range.
2. In FIX mode, user can set the PWM duty through  "HIGH" button and "LOW" button.
    - Push those 2 buttons, and the PWM duty will be recalculated with the new range.
3. The system checks the configuration every 5 seconds, and any configuration will be saved.
4. The board has 2 ways to detect temperature through 18B20. System will align the PWM duty to the higher temperature and the temperature range.
5. Any sensors getting offline doesn't affect the system running.
6. If all sensors losing, FAN having no tachometer, and the PWM duty reaching to 100%, the Buzzer and all LEDs will alert you.
7. Each exception will be recoreded to the internal FLASH memory.
8. The FAKESIGNAL outputs 4K 50% duty PWM pulse to feed the device tachometer

# Reference project
I appreciate the above contributors. The project can't be finished without their help.
- pdwerryhouse/max7219_8digit: https://github.com/pdwerryhouse/max7219_8digit
- ubidefeo/MicroPython-Button: https://github.com/ubidefeo/MicroPython-Button