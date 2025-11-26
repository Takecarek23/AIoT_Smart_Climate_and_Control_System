#ifndef LED_BUTTON_H
#define LED_BUTTON_H
#include <Arduino.h>
#include "global.h"
#include "task_core_iot.h"
#define LED_PIN 48

#define motorPin 5

void task_monitor_button(void *pvParameters);
#endif
