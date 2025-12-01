#ifndef LED_BUTTON_H
#define LED_BUTTON_H
#include <Arduino.h>
#include "global.h"
#include "task_core_iot.h"
#include "tinyml.h"
#define LED_PIN 48
void task_monitor_button(void *pvParameters);
void task_monitor_button_fan(void *pvParameters);
#endif
