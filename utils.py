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
# }
# output
# {
#     "temp": 22.22,
#     "sensorActive": 1,
#     "highLevel": 50,
#     "lowLevel": 20,
#     "speed": 2321,
#     "alarmBit": True
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
        if globalDataBuffer["sensors"]["sensor1"]["sensorStatus"] and globalDataBuffer["sensors"]["sensor2"][
            "sensorStatus"]:
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

    return displayStruct


# This function is used to receive the data generated and collected thought other components
#

#
def displayEngine(dataBuffer, MAX7219display, speedLine, buttonLine, statPoint, t1OK, t2OK):
    stringBuffer = f"{dataBuffer['temp']:.2f}{dataBuffer['speed']:04d}"
    MAX7219display.write_to_buffer_with_dots(stringBuffer)
    MAX7219display.display()
    # Add a suppression function to stop unnecessary update
    #
    #
    #

    ### display the LED string following the temperature range
    speedLevel = int(rangeMapping(dataBuffer["lowLevel"], dataBuffer["highLevel"], dataBuffer["temp"]) * 10)

    ### The red flag. This function is used to show all pots are RED to warn the user to check the speed and temperature sensor
    # if dataBuffer['speed'] == 0 or dataBuffer['alarmBit'] == True:
    if dataBuffer['alarmBit'] == True or (speedLevel != 0 and dataBuffer['speed'] == 0):
        redFlag = True
    else:
        redFlag = False

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
