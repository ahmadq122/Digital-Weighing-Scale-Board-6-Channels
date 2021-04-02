#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#include "RTOS.h"
#include "Nextion/Nextion.h"
#include "FlashMemory/FlashMemory.h"
#include "ADC/ADS1232.h"

void integerToString(uint32_t number, char *buffer, uint8_t len);

//Define the tasks
void Task_01(void *pvParameters);
void Task_02(void *pvParameters);
void Task_03(void *pvParameters); //millis()
void Task_04(void *pvParameters); //ADS Board 1
void Task_05(void *pvParameters); //ADS Board 2
void Task_06(void *pvParameters); //ADS Board 3

// the setup function runs once when you press reset or power the board
void RealTimeOS::setup(void)
{
    // Now set up two tasks to run independently.
    xTaskCreatePinnedToCore(
        Task_01,
        "Task_01", // A name just for humans
        1024,      // This stack size can be checked & adjusted by reading the Stack Highwater
        NULL,
        configMAX_PRIORITIES - 1, // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        NULL,
        ARDUINO_RUNNING_CORE);

    xTaskCreatePinnedToCore(
        Task_02,
        "Task_02",
        1024, // Stack size
        NULL,
        configMAX_PRIORITIES - 2, // Priority
        NULL,
        ARDUINO_RUNNING_CORE);

    xTaskCreatePinnedToCore(
        Task_03,
        "Task_03",
        1024, // Stack size
        NULL,
        configMAX_PRIORITIES - 3, // Priority
        NULL,
        ARDUINO_RUNNING_CORE);

    xTaskCreatePinnedToCore(
        Task_04,
        "Task_04",
        1024 * 4, // Stack size
        NULL,
        configMAX_PRIORITIES - 4, // Priority
        NULL,
        ARDUINO_RUNNING_CORE);

    xTaskCreatePinnedToCore(
        Task_05,
        "Task_05",
        1024 * 4, // Stack size
        NULL,
        configMAX_PRIORITIES - 5, // Priority
        NULL,
        ARDUINO_RUNNING_CORE);

    xTaskCreatePinnedToCore(
        Task_06,
        "Task_06",
        1024 * 4, // Stack size
        NULL,
        configMAX_PRIORITIES - 6, // Priority
        NULL,
        ARDUINO_RUNNING_CORE);
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

        if (data.getDimScreenTimer() != 0)
        {
            if (rtos.dimmCounterDownSecond)
                rtos.dimmCounterDownSecond--;
            else
            {
                if (rtos.currentBrightness != 10)
                {
                    rtos.currentBrightness = 10;
                    hmi.setIntegerToNextion("dim", rtos.currentBrightness);
                }
            }
        }

        rtos.secondBlink = !rtos.secondBlink;
        vTaskDelay(1000); // Delay for 1 second
    }
}

void Task_02(void *pvParameters) // This is a task.
{
    (void)pvParameters;

    for (;;) // A Task shall never return or exit.
    {
        hmi.serialEvent_2();
        vTaskDelay(30); // Delay for 1 second
    }
}

void Task_03(void *pvParameters) // This is a task.
{
    (void)pvParameters;

    for (;;) // A Task shall never return or exit.
    {
        if (++rtos.milliSeconds >= 0xFFFFFFFFFFFFFFFF)
            rtos.milliSeconds = 0;
        vTaskDelay(1); // Delay for 1 milli second
        // if (rtos.milliSeconds % 100 == 0)
        //     Serial.println(rtos.milliSeconds);
    }
}

void Task_04(void *pvParameters) // This is a task.
{
    (void)pvParameters;

    bool channel = false;
    uint8_t counter = 0;
    uint8_t board = ads1;

    if (ads.init(board))
    {
        ads.isAvailable[board] = true;
        for (;;) // A Task shall never return or exit.
        {
            if (ads.dataRead(board, channel, 1))
            {
                if (++counter > SAMPLE_MOV_AVERAGE / 5)
                {
                    counter = 0;
                    channel = !channel;
                }
            }
            vTaskDelay(10);
        }
    }
    else
    {
        for (;;)
        {
            // Serial.println("Do Nothing1!");
            vTaskDelay(10000);
            //do noting
        }
    }
}

void Task_05(void *pvParameters) // This is a task.
{
    (void)pvParameters;

    bool channel = false;
    uint8_t counter = 0;
    uint8_t board = ads2;

    if (ads.init(board))
    {
        ads.isAvailable[board] = true;
        for (;;) // A Task shall never return or exit.
        {
            if (ads.dataRead(board, channel, 1))
            {
                if (++counter > SAMPLE_MOV_AVERAGE / 5)
                {
                    counter = 0;
                    channel = !channel;
                }
            }
            vTaskDelay(10);
        }
    }
    else
    {
        for (;;)
        {
            // Serial.println("Do Nothing2!");
            vTaskDelay(10000);
            //do noting
        }
    }
}
void Task_06(void *pvParameters) // This is a task.
{
    (void)pvParameters;

    bool channel = false;
    uint8_t counter = 0;
    uint8_t board = ads3;

    if (ads.init(board))
    {
        ads.isAvailable[board] = true;
        for (;;)
        // A Task shall never return or exit.
        {
            if (ads.dataRead(board, channel, 1))
            {
                if (++counter > SAMPLE_MOV_AVERAGE / 5)
                {
                    counter = 0;
                    channel = !channel;
                }
            }
            vTaskDelay(10);
        }
    }
    else
    {
        for (;;)
        {
            // Serial.println("Do Nothing3!");
            vTaskDelay(10000);
            //do noting
        }
    }
}

void RealTimeOS::updateStartProgressBar(uint8_t add)
{
    startProgressBar += add;
    startProgressBar = constrain(startProgressBar, 0, 100);
    hmi.setIntegerToNextion("init.val", startProgressBar);

}

RealTimeOS rtos;