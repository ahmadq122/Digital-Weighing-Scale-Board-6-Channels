#include "RTOS.h"

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

//Define the tasks
void Task_01(void *pvParameters);
// void Task_02(void *pvParameters);
// void Task_03(void *pvParameters);
// void Task_04(void *pvParameters);

// the setup function runs once when you press reset or power the board
void RealTimeOS::setup(void)
{
    // Now set up two tasks to run independently.
    xTaskCreatePinnedToCore(
        Task_01,
        "Task_01", // A name just for humans
        512,       // This stack size can be checked & adjusted by reading the Stack Highwater
        NULL,
        configMAX_PRIORITIES - 1, // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        NULL,
        ARDUINO_RUNNING_CORE);

    // xTaskCreatePinnedToCore(
    //     Task_02,
    //     "Task_02",
    //     512, // Stack size
    //     NULL,
    //     1, // Priority
    //     NULL,
    //     ARDUINO_RUNNING_CORE);
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void Task_01(void *pvParameters) // This is a task.
{
    (void)pvParameters;

    for (;;) // A Task shall never return or exit.
    {
        if (++rtos.counterUpSeconds == 0xFFFFFFFFFF)
            rtos.counterUpSeconds = 0;
        if (rtos.counterDownSeconds)
            rtos.counterDownSeconds--;
        if (++rtos.interruptSeconds > 59)
            rtos.interruptSeconds = 0;

        if (rtos.interruptSeconds % 5 == 0 && !rtos.wifiConnectionTriggered)
            rtos.wifiConnectionTriggered = true;

        for (uint8_t i = 0; i < 10; i++)
        {
            rtos.secondTriggered[i] = 1;
        }
        rtos.secondBlink = !rtos.secondBlink;
        vTaskDelay(1000); // Delay for 1 second
    }
}

RealTimeOS rtos;