from ds18x20 import DS18X20
from onewire import OneWire

from machine import Pin, SPI, Timer, PWM, WDT
from neopixel import NeoPixel
from mp_button import Button
import max7219_8digit
from components import *
from utils import *
import time

### predefined data struct
dataBuffer = {
    "sensors": {"sensor1": {"sensorStatus": True, "temp": 26.0},
                "sensor2": {"sensorStatus": True, "temp": 20}},
    "levels": {"highLevel": 70, "lowLevel": 10},
    "gauges": {"fan1": 0},
    "dotColor": [15, 15, 15]
}

### Merge config and init the timer to check the difference of config

fileIns = fileObj(dataBuffer)
fileIns.init()

### Start the peripheries setting

t1OK = Pin(2, Pin.OUT)
t2OK = Pin(3, Pin.OUT)

speedPin = Pin(18, Pin.OUT)
speedLine = NeoPixel(speedPin, 10)

tempsetPin = Pin(19, Pin.OUT)
buttonLine = NeoPixel(tempsetPin, 8)

sysStatus = Pin(20, Pin.OUT)
statPoint = NeoPixel(sysStatus, 1)

spi = SPI(1, 10_000_000)
chipSSPin = Pin(9, Pin.OUT)
ledDisplay = max7219_8digit.Display(spi, chipSSPin)

ledDisplay.write_to_buffer_with_dots('12343679')
ledDisplay.display()
ledDisplay.set_intensity(1)

sensors = tempSensor(Pin(1), Pin(0), dataBuffer)
sensors.init()

fans = fanObj(16, dataBuffer)
fans.init()

upBtn = Button(4, callback=button_action, dataBuffer=dataBuffer)  # lowlevel
downBtn = Button(6, callback=button_action, dataBuffer=dataBuffer)  # highlevel

dot = statusDot(dataBuffer)
dot.init()

# pwm pin
fanPwmPin = PWM(Pin(17))
fanPwmPin.freq(25000)
fanPwmPin.duty_u16(0xffff // 2)

fakePwmPin = PWM(Pin(21))
fakePwmPin.freq(4000)
fakePwmPin.duty_u16(0xffff // 2)

while True:
    m = (dataComposer(dataBuffer))
    displayEngine(m, ledDisplay, speedLine, buttonLine, statPoint, t1OK, t2OK)
    upBtn.update()
    downBtn.update()

    pwmPercent = int(rangeMapping(m['lowLevel'], m['highLevel'], m['temp']) * 100)
    fanPwmPin.duty_u16(int(0xFFFF * pwmPercent // 100))


