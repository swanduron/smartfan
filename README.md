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

# Chips
- RP2040
- MAX7219

# Board picture
![Board][Media\IMG_1095.HEIC]
