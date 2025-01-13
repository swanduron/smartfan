import random


# @brief This function is used to calc the percentage of the temperature in certain range
# @param rangeMapping(lowLevel, highLevel, temp):
#   lowLevel, highLevel, these options are the new range of the percentage calculation
#   temp, the temperature
# @return return a float number, between 0 ~ 1.

def rangeMapping(lowLevel, highLevel, temp):
    # return the percentage to provide the pwm change
    if lowLevel <= temp <= highLevel:
        return (temp - lowLevel) / (highLevel - lowLevel)
    elif lowLevel > temp:
        return 0
    else:
        return 1


# Data sample
# input
# {
#     "sensors": {"sensor1": {"sensorStatus": True, "temp": 50},
#                 "sensor2": {"sensorStatus": True, "temp": 50}},  # updated by sensor test
#     "levels": {"highLevel": 50, "lowLevel": 10},  # updated by button event
#     "gauges": {"fan1": 0}  # updated by gauge test
#     "dotColor": [15, 15, 15],
#     "opMode": 1, # opmode = 1 : flexible mode, opmode = 0 : fix mode
#     # The boardMode is use to identify the different models of board.
#     # 0 : basic board, with WS2132B, 2 temp sensors, 2 buttons, 1 fan, 1 display
#     # 1 : easy board, has simple LED indicator
#     # 2 : tiny board, has no display, for 5v fan
#     "boardMode": 0
# }
# output
# {
#     "temp": 22.22,
#     "sensorActive": 1,
#     "highLevel": 50,
#     "lowLevel": 20,
#     "speed": 2321,
#     "alarmBit": True,
#     "dotColor": [15, 15, 15],
#     "opMode": 1,
#     "boardMode": 0
#     "manualLevel": 0
# }

def dataComposer(globalDataBuffer):
    displayStruct = {
        "temp": 22.22,
        "sensorActive": 1,
        "highLevel": 50,
        "lowLevel": 20,
        "speed": 2321,
        "alarmBit": False,
        "dotColor": [15, 15, 15]
    }
    if globalDataBuffer["sensors"]["sensor1"]["sensorStatus"] or globalDataBuffer["sensors"]["sensor2"]["sensorStatus"]:
        # compare temp here
        sensor1Temp = globalDataBuffer["sensors"]["sensor1"]["temp"]
        sensor2Temp = globalDataBuffer["sensors"]["sensor2"]["temp"]
        sensor1Status = globalDataBuffer["sensors"]["sensor1"]["sensorStatus"]
        if globalDataBuffer["sensors"]["sensor1"]["sensorStatus"] and globalDataBuffer["sensors"]["sensor2"]["sensorStatus"]:
            displayStruct["temp"] = sensor1Temp if sensor1Temp > sensor2Temp else sensor2Temp
            displayStruct["sensorActive"] = 1 if sensor1Temp > sensor2Temp else 2
        else:
            displayStruct["temp"] = sensor1Temp if sensor1Status else sensor2Temp
            displayStruct["sensorActive"] = 1 if sensor1Status else 2
        displayStruct["alarmBit"] = False
    else:
        displayStruct["alarmBit"] = True
    displayStruct["speed"] = globalDataBuffer["gauges"]["fan1"]

    displayStruct['highLevel'] = globalDataBuffer['levels']['highLevel']
    displayStruct['lowLevel'] = globalDataBuffer['levels']['lowLevel']

    displayStruct["dotColor"] = globalDataBuffer["dotColor"]
    displayStruct["opMode"] = globalDataBuffer["opMode"]
    displayStruct["boardMode"] = globalDataBuffer["boardMode"]
    displayStruct["manualLevel"] = globalDataBuffer["manualLevel"]

    return displayStruct


# This function is used to receive the data generated and collected thought other components
#
# Data sample
# input
# {
#     "sensors": {"sensor1": {"sensorStatus": True, "temp": 50},
#                 "sensor2": {"sensorStatus": True, "temp": 50}},  # updated by sensor test
#     "levels": {"highLevel": 50, "lowLevel": 10},  # updated by button event
#     "gauges": {"fan1": 0}  # updated by gauge test
#     "dotColor": [15, 15, 15],
#     "opMode": 1, # opmode = 1 : flexible mode, opmode = 0 : fix mode
#     # The boardMode is use to identify the different models of board.
#     # 0 : basic board, with WS2132B, 2 temp sensors, 2 buttons, 1 fan, 1 display
#     # 1 : easy board, has simple LED indicator
#     # 2 : tiny board, has no display, for 5v fan
#     "boardMode": 0
# }
## Add below data from datacomposer()
# {
#     "temp": 22.22,
#     "sensorActive": 1,
#     "highLevel": 50,
#     "lowLevel": 20,
#     "speed": 2321,
#     "alarmBit": True,
#     "dotColor": [15, 15, 15],
#     "opMode": 1,
#     "boardMode": 0
#     "manualLevel": 0
# }
def displayEngine(dataBuffer, MAX7219display, speedLine, buttonLine, statPoint, t1OK, t2OK, modeLED, beep):

    ### display the LED string following the temperature and speed
    # show the modeLED
    if dataBuffer["opMode"] == 1:
        # 1 is auto mode
        modeLED.value(1)
    else:
        # 0 is fix mode
        modeLED.value(0)

    # Build the string to display
    stringBuffer = f"{dataBuffer['temp']:.2f}{dataBuffer['speed']:04d}"
    MAX7219display.write_to_buffer_with_dots(stringBuffer)
    MAX7219display.display()
    # Add a suppression function to stop unnecessary update
    #
    #
    #
    if dataBuffer["opMode"] == 1: # in auto mode
        ### display the LED string following the temperature range
        speedLevel = int(rangeMapping(dataBuffer["lowLevel"], dataBuffer["highLevel"], dataBuffer["temp"]) * 10)

        ### The red flag. This function is used to show all pots are RED to warn the user to check the speed and temperature sensor
        # if dataBuffer['speed'] == 0 or dataBuffer['alarmBit'] == True:
        if dataBuffer['alarmBit'] == True or (speedLevel != 0 and dataBuffer['speed'] == 0):
            redFlag = True
            beep.value(1)
        else:
            redFlag = False
            beep.value(0)

        if speedLevel == 0:
            for i in range(10):
                speedLine[i] = (0, 0, 0)
        else:
            if speedLevel < 4:
                if not redFlag:
                    lineColor = (18, 50, 18)
                else:
                    lineColor = (50, 0, 0)
            elif 7 > speedLevel >= 4:
                if not redFlag:
                    lineColor = (35, 35, 10)
                else:
                    lineColor = (50, 0, 0)
            else:
                if not redFlag:
                    lineColor = (50, 18, 18)
                else:
                    lineColor = (50, 0, 0)
            for i in range(10):
                if i < speedLevel:
                    speedLine[i] = lineColor
                else:
                    speedLine[i] = (0, 0, 0)
        speedLine.write()
        ### Process the configuration of speed
        preDefineData = [0, 10, 20, 30, 40, 50, 60, 70]
        btnBuffer = [0, 0, 0, 0, 0, 0, 0, 0]
        btnBuffer[preDefineData.index(dataBuffer["lowLevel"])] = 1
        btnBuffer[preDefineData.index(dataBuffer["highLevel"])] = 1
        for i in range(8):
            if btnBuffer[i] == 1:
                if not redFlag:
                    buttonLine[i] = (18, 80, 18)
                else:
                    buttonLine[i] = (50, 0, 0)
            else:
                buttonLine[i] = (0, 0, 0)
        buttonLine.write()
        statPoint[0] = dataBuffer["dotColor"]
        statPoint.write()
    elif dataBuffer["opMode"] == 0: # in fix mode
        if dataBuffer["manualLevel"] != 0 and dataBuffer['speed'] == 0:
            redFlag = True # Add code to keep the consistant of the redFlag
            lineColor = (50, 0, 0)
            beep.value(1)
        else:
            redFlag = False
            lineColor = (35, 35, 10)
            beep.value(0)
        for i in range(10):
            if i < dataBuffer["manualLevel"]:
                speedLine[i] = lineColor
            else:
                speedLine[i] = (0, 0, 0)
        speedLine.write()
        for i in range(8):
            buttonLine[i] = (0, 0, 0)
        buttonLine.write()
        statPoint[0] = dataBuffer["dotColor"]
        statPoint.write()
    ### set the sensor indicator
    if dataBuffer["sensorActive"] == 1:
        t2OK.value(1)
        t1OK.value(0)
    else:
        t2OK.value(0)
        t1OK.value(1)


if "__main__" == __name__:
    fakaData = {
        "sensors": {"sensor1": {"sensorStatus": False, "temp": 13},
                    "sensor2": {"sensorStatus": False, "temp": 42}},  # updated by sensor test
        "levels": {"highLevel": 50, "lowLevel": 10},  # updated by button event
        "gauges": {"fan1": 0}  # updated by gauge test
    }
    m = dataComposer(fakaData)
    print(m)

