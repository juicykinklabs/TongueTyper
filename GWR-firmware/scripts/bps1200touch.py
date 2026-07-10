# Run this script to put an ESP on a given comport into bootloader mode
# (without pressing the tiny buttons on it)

import serial, time
from serial.tools import list_ports_windows
lpwc = list_ports_windows.comports()

if len(lpwc) == 1:
    thePort = lpwc[0].name
    print(thePort)
    try:
        s = serial.Serial(thePort, 1200)
        s.dtr = False
        time.sleep(0.4)
        s.close()
        time.sleep(2)
    except:
        # try again like a sicko, it might've changed port real quick
        time.sleep(3)
        lpwc = list_ports_windows.comports()
        thePort = lpwc[0].name
        print(thePort)
        s = serial.Serial(thePort, 1200)
        s.dtr = False
        time.sleep(0.4)
        s.close()
        time.sleep(2)
else:
    print("you need exactly one COM port device connected for this to work, buddy")
