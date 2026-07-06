#include "taskglobals.h"

SemaphoreHandle_t Mutexes::SPI         = NULL;
SemaphoreHandle_t Mutexes::SerialPrint = NULL;
SemaphoreHandle_t Mutexes::AccelData   = NULL;
SemaphoreHandle_t Mutexes::SDCard      = NULL;

TaskHandle_t Tasks::soundfx        = NULL;
TaskHandle_t Tasks::rgbled         = NULL;
TaskHandle_t Tasks::mouse_out      = NULL;
TaskHandle_t Tasks::keyboard_out   = NULL;
TaskHandle_t Tasks::heartbeat      = NULL;
TaskHandle_t Tasks::display        = NULL;
TaskHandle_t Tasks::buttons        = NULL;
TaskHandle_t Tasks::accel          = NULL;
TaskHandle_t Tasks::wificonnection = NULL;

QueueHandle_t q_sfx_tts = NULL;

SettingsConfig settings;

AccelEvent g_accelEvent;
bool g_wifiReady = false;
