from ds18x20 import DS18X20
from onewire import OneWire

from machine import Pin, SPI, Timer, PWM, WDT
from mp_button import Button
import json
import time
import max7219_8digit
import asyncio
import os
import random


# data sample
# '''{
#     "temp": 22.22,  # the highest temp of all temp sensors
#     "sensorActive": 1,  # which sensor is believed
#     "speed": 2321,  # fan speed updated by class fanObj()
#     "lowLevel": 20,   # temp low level, updated by button event
#     "highLevel": 60, # temp low level, updated by button event
#     "alarmBit": True # depict the emergency status of sensor disabled or fan stop
# }'''
#

# {
#     "sensors": {"sensor1": {"sensorStatus": True, "temp": 50},
#                 "sensor2": {"sensorStatus": True, "temp": 50}},  # updated by sensor test
#     "levels": {"highLevel": 50, "lowLevel": 10},  # updated by button event
#     "gauges": {"fan1": 0}  # updated by gauge test
# }

# {
#     "temp": 22.22,
#     "sensorActive": 1,
#     "highLevel": 50,
#     "lowLevel": 20,
#     "speed": 2321,
#     "alarmBit": True
# }

# @brief Enable the system fan speed control and update the speed gauge
# @param def __init__(self, fanPin, dataBuffer):
#   fanPin, provide the speed pin that connects to the pin3 of the fan connector
#   dataBuffer, the global data struct that uses to storage the information of the system
# @return No explicit update. This class updates the fan speed automatically

class fanObj():

    def __init__(self, fanPin, dataBuffer):
        self.value = 0
        self.speed = 0
        self.fanPin = Pin(fanPin, Pin.IN, Pin.PULL_UP)
        self.dataBuffer = dataBuffer

    def init(self):
        self.fanPin.irq(trigger=Pin.IRQ_RISING, handler=self.handleInterrupt)
        self.fanTimer = Timer()
        self.fanTimer.init(period=1000, mode=Timer.PERIODIC, callback=self.fanSpeed)

    def handleInterrupt(self, a):
        self.value += 1

    def fanSpeed(self, a):
        self.speed = self.value * 30
        self.value = 0
        self.dataBuffer["gauges"]["fan1"] = self.speed


# @brief Enable the DS18B20 and update the temperature data
# @param def __init__(self, sensorPin1, sensorPin2, dataBuffer):
#   sensorPin1, sensorPin2, are the GPIO number of the MCU
#   dataBuffer, the global data struct that uses to storage the information of the system
# @return No explicit update. This class updates the temperature and sensor status
class tempSensor():

    def __init__(self, sensorPin1, sensorPin2, dataBuffer):

        self.dataBuffer = dataBuffer
        # This dict is used
        self.tempStruct = {"sensor1": {"sensorStatus": False, "temp": 50},
                           "sensor2": {"sensorStatus": False, "temp": 50}}
        try:
            self.sensor1ow = OneWire(sensorPin1)
            self.sensor1Obj = DS18X20(self.sensor1ow)
            self.sensor1Addr = self.sensor1Obj.scan()[0]
            self.sensor1Obj.resolution(self.sensor1Addr, 12)
            self.tempStruct["sensor1"]["sensorStatus"] = True
        except:
            self.tempStruct["sensor1"]["sensorStatus"] = False

        try:
            self.sensor2ow = OneWire(sensorPin2)
            self.sensor2Obj = DS18X20(self.sensor2ow)
            self.sensor2Addr = self.sensor2Obj.scan()[0]
            self.sensor2Obj.resolution(self.sensor2Addr, 12)
            self.tempStruct["sensor2"]["sensorStatus"] = True
        except:
            self.tempStruct["sensor2"]["sensorStatus"] = False

    def init(self):
        # start the timer to update the temperature info
        self.sensorTimer = Timer()
        self.sensorTimer.init(period=1000, mode=Timer.PERIODIC, callback=self.tempMeasure)

    def tempMeasure(self, a):

        try:
            sensor1Temp = self.sensor1Obj.read_temp(self.sensor1Addr)
            self.sensor1Obj.convert_temp()
            self.tempStruct["sensor1"]["temp"] = sensor1Temp
            self.tempStruct["sensor1"]["sensorStatus"] = True
        except:
            self.tempStruct["sensor1"]["sensorStatus"] = False

        try:
            sensor2Temp = self.sensor2Obj.read_temp(self.sensor2Addr)
            self.sensor2Obj.convert_temp()
            self.tempStruct["sensor2"]["temp"] = sensor2Temp
            self.tempStruct["sensor2"]["sensorStatus"] = True
        except:
            self.tempStruct["sensor2"]["sensorStatus"] = False

        # update the result to global data buffer
        self.dataBuffer["sensors"] = self.tempStruct


def button_action(pin, event, dataBuffer):
    # print(f'Button connected to Pin {pin} has been {event}')

    if event == Button.RELEASED:

        if pin == 4:
            dataBuffer["levels"]['lowLevel'] += 10
            if dataBuffer["levels"]['lowLevel'] > 30:
                dataBuffer["levels"]['lowLevel'] = 0
        elif pin == 6:
            dataBuffer["levels"]['highLevel'] += 10
            if dataBuffer["levels"]['highLevel'] > 70:
                dataBuffer["levels"]['highLevel'] = 40
        # print(pin, event, dataBuffer)


######################################################################
# @brief Change the color of system status LED
# @param def __init__(self, dataBuffer):
#
#   dataBuffer, the global data struct that uses to storage the information of the system
# @return No explicit update. This class is responsible for update the LED color and refresh it twice a second
######################################################################

class statusDot():

    def __init__(self, dataBuffer):
        self.dataBuffer = dataBuffer
        self.red = 0
        self.green = 0
        self.blue = 0

    def init(self):
        self.colorDotTimer = Timer()
        self.colorDotTimer.init(period=500, mode=Timer.PERIODIC, callback=self.changeColor)

    def changeColor(self, a):
        self.dataBuffer["dotColor"] = [random.randint(1, 100), random.randint(1, 100), random.randint(1, 100)]


######################################################################
# @brief Enable the file loader and track the change of the highlevel and low level
# @param def __init__(self, dataBuffer):
#
#   dataBuffer, the global data struct that uses to storage the information of the system
# @return No explicit update. This class is responsible for update the configuration from file and sync the
#   change from the device using.
######################################################################

class fileObj():

    def __init__(self, dataBuffer):
        self.filePath = "config.json"
        self.dataBuffer = dataBuffer

    def init(self):
        try:
            with open('config.json', mode='r') as file_obj:
                self.databuf = json.load(file_obj)
            print("load config file successful!")
            ## Add a configuration validator here
        except Exception as e:
            print(e)
            self.databuf = {"levels": {"highLevel": 40, "lowLevel": 20}}
            with open('config.json', mode='w') as file_obj:
                json.dump(self.databuf, file_obj)
            print("load config file failed and use self-generated config!")
        self.dataCopy = json.loads(json.dumps(self.databuf))

        self.dataBuffer['levels'] = self.databuf['levels']

        self.configCheckTimer = Timer()
        self.configCheckTimer.init(period=5000, mode=Timer.PERIODIC, callback=self.configCheck)

    def configCheck(self, a):
        print("check config", self.databuf["levels"], self.dataBuffer["levels"], self.dataCopy)
        # The below code is used to sync the global dataBuffer to the private databuf. If the dataBuffer is changed, this function will write the config to flash
        # The json loads and dumps are used to generate a new object of the configuration since the microypthon doesn't have the deepcopy function
        self.databuf["levels"] = self.dataBuffer["levels"]
        if self.databuf != self.dataCopy:
            print("config different")
            with open('config.json', mode='w') as file_obj:
                json.dump(self.databuf, file_obj)
            self.dataCopy = json.loads(json.dumps(self.databuf))
            print(id(self.dataCopy), id(self.databuf))
