#include "global.h"
#include "led_button.h"
#include "led_blinky.h"
#include "neo_blinky.h"
#include "temp_humi_monitor.h"
#include "tinyml.h"
// include task
#include "task_check_info.h"
#include "task_toogle_boot.h"
#include "task_wifi.h"
#include "task_webserver.h"
#include "task_core_iot.h"

void setup()
{
  Serial.begin(115200);
  check_info_File(0);
  //Delete_info_File();
  setup_monitoring_system();

  startAP();

  //  Semaphore 
  xBinarySemaphoreInternet = xSemaphoreCreateBinary();
  xLedMutex = xSemaphoreCreateMutex();
  xNeoPixelMutex = xSemaphoreCreateMutex();

  xTaskCreate(led_blinky, "Task LED Blink", 2048, NULL, 2, NULL);
  xTaskCreate(neo_blinky, "Task NEO Blink", 2048, NULL, 2, NULL);
  xTaskCreate(temp_humi_monitor, "Task TEMP HUMI Monitor", 4096, NULL, 2, NULL);
  xTaskCreate(task_monitor_button, "Task Monitor Button", 2048, NULL, 2, NULL);
  xTaskCreate(task_monitor_button_fan, "Task Monitor Button", 2048, NULL, 2, NULL);
  xTaskCreate( tiny_ml_task, "Tiny ML Task" ,2048  ,NULL  ,2 , NULL);
  xTaskCreate(coreiot_task, "CoreIOT Task" ,4096  ,NULL  ,2 , NULL);
}

void loop()
{
  if (check_info_File(1))
  {
    if (!Wifi_reconnect())
    {
      Webserver_stop();
    }
    else
    {
      //CORE_IOT_reconnect();
    }
  }
  Webserver_reconnect();
}
