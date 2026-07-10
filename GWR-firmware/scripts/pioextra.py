Import("env")
if env.IsIntegrationDump():
   Return()


import serial, time
from serial.tools import list_ports_windows

def doubleTap1200bpsTouch() -> bool:
    USB_COMPOSITE_ENUMERATION_DELAY = 3.0 # seconds, because windows is slow
    def touch1200bps() -> bool:
        try:
            lpwc = list_ports_windows.comports()
            if len(lpwc) == 1:
                thePort = lpwc[0].name
                s = serial.Serial(thePort, 1200)
                s.dtr = False
                time.sleep(0.4) # pulse time
                s.close()
                return True
            return False
        except:
            return False

    if touch1200bps():
        time.sleep(USB_COMPOSITE_ENUMERATION_DELAY)
        return True
    else:
        # try again after longer delay
        time.sleep(USB_COMPOSITE_ENUMERATION_DELAY)
        if touch1200bps():
            time.sleep(USB_COMPOSITE_ENUMERATION_DELAY)
            return True
        else:
            return False

pre_success: bool = False
def pre(*_):
    print("pre()")
    global pre_success
    if doubleTap1200bpsTouch():
        pre_success = True

def post(source, target, env):
    print("post()")
    if not pre_success:
      return

pre()
env.AddPostAction("${BUILD_DIR}/firmware.bin", post)