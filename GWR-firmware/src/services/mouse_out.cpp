#include "mouse_out.h"

#include <Arduino.h>
#include <USB.h>
#include <USBHIDMouse.h>

#include "config/app_conf.h"
#include "config/hardware_conf.h"

#include "structs/AccelEvent.h"
#include "structs/MouseClickMessage.h"

#include "taskglobals.h" // constains Settings object, Mouse object

void task_mouse(void *pv) {
    TickType_t taskTimer = xTaskGetTickCount();

    while (1) {
        // new clicker
        MouseclickMessage mc;
        if (xQueueReceive(q_mouseclicks, (void *) &mc, (TickType_t) 0)) {
            if (mc.state == MouseButtonState::PRESSED) {
                Mouse.press(mc.button);
            } else if (mc.state == MouseButtonState::RELEASED) {
                Mouse.release(mc.button);
            }
        }

        // plain old accelerometer:
        // doubles x, y, z represent accelerationvector in a gravitational field
        // hold still when pointing in a circle around Y direction
        // as Z value gets more positive -> mouse up
        // as Z value gets more negative -> mouse down
        // as X value gets more positive -> mouse left
        // as X value gets more negative -> mouse right

        // gravity
        static const double g                  = 9.81;
        static const double TILT_DP_THRESH     = 0.97; // if we fall below this, start using accel data to move the mouse
        static const uint32_t REPORT_FREQUENCY = 100;  // HZ
        // static const uint32_t DPI              = 800;  // for inches per second calcuations
        // static const double INCH_PER_SEC_MAX   = 4.0;  // under 1g
        static const int DOTS_PER_TICK_MAX = (settings.hid.mouseSense) / REPORT_FREQUENCY;

        double x, y, z;
        x = g_accelEvent.x * g;
        y = g_accelEvent.y * g;
        z = g_accelEvent.z * g;

        // debugf("X: %f, Y: %f, Z: %f (m/s^2)\n", x, y, z);

        // turn into a unit vector, since we only care about direction
        // todo some vector helper functions
        // todo average of numerous samples for smooth operation
        // todo take DERIVATIVE or something, for mouse acceleration enabling FLICKS!
        double acc_mag = sqrt(x * x + y * y + z * z);
        if (acc_mag != 0.0) {
            // dont divide by zero;
            // we cant hit a 'continue' because we need the xTaskDelay
            
            double vec3_direction[3] = {x / acc_mag, y / acc_mag, z / acc_mag};
            // debugf("unit vector is: %f %f %f\n", vec3_direction[0], vec3_direction[1], vec3_direction[2]);

            double vec3_stationary[3]      = {0.0, g, 0.0};
            double vec3_stationary_unit[3] = {0.0, 1.0, 0.0};

            // dp is zero when totes perpendicular (definitely an input) and is one when pointed in same direction
            double scalar_dotproduct_Y = 1.0 * vec3_direction[1]; // other components go to zero in our coordinate system

            // debugf("dot product with downward: %f\n", scalar_dotproduct_Y);

            double scalar_dotproduct_Zn = -1.0 * vec3_direction[2]; // "downness" (-1.0 to 1.0)
            double scalar_dotproduct_Xn = -1.0 * vec3_direction[0]; // "rightness" (-1.0 to 1.0)

            if (abs(scalar_dotproduct_Y) < TILT_DP_THRESH) { // abs in case its like, upside down or something idk
                int8_t dotsX = scalar_dotproduct_Xn * DOTS_PER_TICK_MAX;
                int8_t dotsY = scalar_dotproduct_Zn * DOTS_PER_TICK_MAX;
                // debugf("dX: %d, dY: %d\n", dotsX, dotsY);

                Mouse.move(dotsX, dotsY);
            }
        }

        xTaskDelayUntil(&taskTimer, (1000 / REPORT_FREQUENCY) / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}