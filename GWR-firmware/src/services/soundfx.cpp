#include "soundfx.h"

#include <Arduino.h>
#include <Audio.h>

#include "config/app_conf.h"
#include "config/hardware_conf.h"
#include "structs/AudioMessage.h"
#include "taskglobals.h"

Audio audio;

uint8_t volumeToVolume(double v) {
    // convert from volume (as double) to volumesteps (0..21 usually)
    return (uint8_t) (v * audio.getVolumeSteps()); // could floor or ceil but who cares
}

void audioLoopToCompletion() {
    while (audio.isRunning()) {
        audio.loop();
        vTaskDelay(1); // dont touch
    }
}

void task_soundfx(void *pv) {
    // Audio::audio_info_callback = my_audio_info;

    bool success = audio.setPinout(Pins::I2S::BCLK, Pins::I2S::LRC, Pins::I2S::DOUT); 
    if (!success) {
        debuglnF("I2s IO matrix config failed, probably");
        // todo: handle failure
        vTaskDelete(NULL);
    }

    AudioMessage thisAudioEvent;

    while (1) {
        if (xQueueReceive(q_sfx_tts, (void *) &thisAudioEvent, portMAX_DELAY) == pdTRUE) {
            debugf("audio queue consumer received data: %s\n", thisAudioEvent.data);
            audio.setVolume(volumeToVolume(settings.sfx.volume));
            debugf("set volume to %d\n", volumeToVolume(settings.sfx.volume));
            bool connectionSuccess = false;
            switch (thisAudioEvent.type) {
                case AudioEventEnum::SFX: {
                    SD.begin(Pins::SD::CS); // todo semamphore
                    connectionSuccess = audio.connecttoFS(SD, thisAudioEvent.data);
                    audioLoopToCompletion();
                    SD.end();
                    break;
                }
                case AudioEventEnum::TTS: {
                    debugf("using dialect %s\n", settings.sfx.lang.c_str());
                    if (g_wifiReady) {
                        connectionSuccess = audio.connecttospeech(thisAudioEvent.data, settings.sfx.lang.c_str());
                        audioLoopToCompletion();
                    } else {
                        debugln("No WiFi, skipping TTS");
                    }
                    break;
                }
            }
            if (!connectionSuccess) {
                debugln("audio goof >_<");
            } else {
                debugln("ok done :3");
            }
        }
    }
    vTaskDelete(NULL);
}
