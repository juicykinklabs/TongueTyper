#pragma once

#include <Arduino.h>
#ifdef USEBUTTONS
xTaskCreate(inputHandler, "Button Input Handler", 4096, NULL, 5, NULL);
  xTaskCreate(buttonSequenceConsumer, "Thing that uses buttons", 4096, NULL, 4, NULL);


void inputHandler(void *pv)
{
  // static uint32_t t_start = millis();
  // static uint32_t t_completed = 0;
  static uint32_t t_lastinteraction = 0;
  static int32_t internalState = -1;
  static const uint32_t BUTTON_TIMEOUT = 5000; // ms
  static const uint32_t BUTTON_DEBOUNCE = 100; // ms
  static const uint32_t INTRA_CMD_DELAY = 500;
  static boolean button_released = true;

  // -1: waiting for first button press or for button sequence to clear by consumer
  // 1-6: a button was pressed recently enough that a second press should update global buttonSequence

  while (1)
  {
    if (digitalRead(BUTTON1) && digitalRead(BUTTON2) && digitalRead(BUTTON3))
      {
        button_released = true; // all buttons up, allow next valid pass to update state
      }

    if (button_released && (internalState == -1) && (buttonSequence == 0))
    {
      // in the actual code this would be handled by an interrupt from the gpio expander
      // need debouncing too
      if (!digitalRead(BUTTON1))
      {
        internalState = 1;
        button_released = false;
        t_lastinteraction = millis();
      }
      if (!digitalRead(BUTTON2))
      {
        internalState = 2;
        button_released = false;
        t_lastinteraction = millis();
      }
      if (!digitalRead(BUTTON3))
      {
        internalState = 3;
        button_released = false;
        t_lastinteraction = millis();
      }
    }
    else
    {
      // button was pressed, but how recently?
      if (millis() - t_lastinteraction > BUTTON_TIMEOUT)
      {
        internalState = -1;
        Serial.println("took too long, internalstate=-1");
      }
      else if (millis() - t_lastinteraction > BUTTON_DEBOUNCE)
      {
        if (digitalRead(BUTTON1) && digitalRead(BUTTON2) && digitalRead(BUTTON3))
        {
          button_released = true; // all buttons up, allow next valid pass to update state
          Serial.println("awaiting secondary button press...");
        }
        if (button_released)
        {
          // todo: use an array of buttons and loop over them
          if (!digitalRead(BUTTON1))
          {
            buttonSequence = 10 * internalState + 1;
            t_lastinteraction = millis();
            internalState = -1;
            button_released = false;
          }
          if (!digitalRead(BUTTON2))
          {
            buttonSequence = 10 * internalState + 2;
            t_lastinteraction = millis();
            internalState = -1;
            button_released = false;
          }
          if (!digitalRead(BUTTON3))
          {
            buttonSequence = 10 * internalState + 3;
            t_lastinteraction = millis();
            internalState = -1;
            button_released = false;
          }

          delay(100);
        }
      }
    }

    vTaskDelay((1000 / 20) / portTICK_PERIOD_MS); // run at 20 Hz
  }

  vTaskDelete(NULL);
}

void buttonSequenceConsumer(void *pv)
{

  while (1)
  {
    if (buttonSequence > 0)
    {
      Serial.print("oh hey: ");
      Serial.println(buttonSequence);
      buttonSequence = 0;
    }
    vTaskDelay((1000 / 20) / portTICK_PERIOD_MS); // run at 20 Hz
  }
  vTaskDelete(NULL);
}

#endif