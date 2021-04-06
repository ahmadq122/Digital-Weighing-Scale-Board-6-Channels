#include "Datalogger.h"
#include "FlashMemory/FlashMemory.h"
#include "Nextion/Nextion.h"
#include "DebugSerial/DebugSerial.h"
#include "RTC/RTCDS1307.h"
#include "RTOS/RTOS.h"
#include "Time/MyTime.h"

void integerToString(uint32_t number, char *buffer, uint8_t len);

void Datalogger::showObjDatalogPage(uint8_t loggerType, bool show)
{
    if (loggerType == serial)
    {
        hmi.setVisObjectNextion("q3", show);
        hmi.setVisObjectNextion("q4", show);
    }
    if (loggerType != local)
        hmi.setVisObjectNextion("b1", show);
    hmi.setVisObjectNextion("b2", show);
    hmi.setVisObjectNextion("b3", show);
    hmi.setVisObjectNextion("b5", show);
    hmi.setVisObjectNextion("b7", show);
    hmi.setVisObjectNextion("t0", show);
    hmi.setVisObjectNextion("q0", show);
    hmi.setVisObjectNextion("t1", show);
    hmi.setVisObjectNextion("q1", show);
    hmi.setVisObjectNextion("t2", show);
    hmi.setVisObjectNextion("b4", show);
    hmi.setVisObjectNextion("b6", show);
    hmi.setVisObjectNextion("b8", show);
    hmi.setVisObjectNextion("b9", show);
}

void Datalogger::updateStrPeriod(uint8_t hourPeriod, uint8_t minutePeriod, uint8_t secondPeriod)
{
    char strHour[3];
    char strMinute[3];
    char strSecond[3];

    integerToString(hourPeriod, strHour, 2);
    integerToString(minutePeriod, strMinute, 2);
    integerToString(secondPeriod, strSecond, 2);

    hmi.setStringToNextion("t0.txt", strHour);
    hmi.setStringToNextion("t1.txt", strMinute);
    hmi.setStringToNextion("t2.txt", strSecond);
}

void Datalogger::setting(uint8_t loggerType)
{
    uint32_t period = 0;
    bool button[20];
    uint8_t btn = 0;
    bool baudrateSetState = false;
    bool enDis = false;
    int8_t hourPeriod = 0;
    int8_t minutePeriod = 0;
    int8_t secondPeriod = 0;
    uint8_t savingProgress = 0;

    enDis = data.getDatalogStatus(loggerType);
    period = data.getPeriodDatalog(loggerType);
    hourPeriod = period / 3600;
    minutePeriod = (period - (hourPeriod * 3600)) / 60;
    secondPeriod = period - ((hourPeriod * 3600) + (minutePeriod * 60));

__start:
    if (loggerType == serial)
        hmi.showPage("serial");
    else if (loggerType == local)
        hmi.showPage("local");
    else if (loggerType == remote)
        hmi.showPage("remote");
    hmi.waitForPageRespon();

    if (loggerType == serial)
        hmi.setStringToNextion("b1.txt", String() + data.getBaudrateSerial(logging));
    hmi.setIntegerToNextion("b0.val", enDis);
    hmi.setIntegerToNextion("q2.picc", enDis ? 67 : 66);
    showObjDatalogPage(loggerType, enDis);
    updateStrPeriod(hourPeriod, minutePeriod, secondPeriod);

    if (loggerType == serial)
        printDebugln("Datalog Serial page opened");
    else if (loggerType == local)
        printDebugln("Datalog Local page opened");
    else if (loggerType == remote)
        printDebugln("Datalog Remote page opened");

    while (true)
    {
        for (uint8_t i = 0; i < 10; i++)
        {
            button[i] = hmi.getDataButton(i);
            if (button[i])
            {
                switch (i)
                {
                case 0:
                    enDis = !enDis;
                    data.setDatalogStatus(loggerType, enDis);
                    hmi.setIntegerToNextion("b0.val", enDis);
                    if (enDis)
                        hmi.setIntegerToNextion("q2.picc", 67);
                    else
                        hmi.setIntegerToNextion("q2.picc", 66);
                    showObjDatalogPage(loggerType, enDis);
                    break;
                case 1:
                    if (loggerType == serial)
                    {
                        showBaudrateOption(logging, true);
                        updateSelectedBaudrateToNextion(logging, data.getBaudrateSerialIndex(logging));
                        baudrateSetState = true;
                        while (baudrateSetState)
                        {
                            for (uint8_t i = 10; i < 20; i++)
                            {
                                button[i] = hmi.getDataButton(i);
                                if (button[i])
                                {
                                    data.setBaudrateSerial(logging, i - 10);
                                    baudrateSetState = false;
                                }
                                if (!baudrateSetState)
                                    continue;
                            }
                        }
                        hmi.setStringToNextion("b1.txt", String() + data.getBaudrateSerial(logging));
                        updateSelectedBaudrateToNextion(logging, data.getBaudrateSerialIndex(logging));
                        delay(2000);
                        showBaudrateOption(logging, false);
                        hmi.flushAvailableButton();
                    }
                    else if (loggerType == remote)
                    {
                        hmi.showPage("keyboard");
                        hmi.waitForPageRespon();
                        hmi.setStringToNextion("kb_string.txt", data.getKeyAPI());
                        btn = 0;
                        while (!hmi.checkAnyButtonPressed(&btn))
                        {
                        }
                        if (hmi.getDataButton(1))
                            data.setKeyAPI(hmi.getDataString(0));
                        goto __start;
                    }
                    break;
                case 2:
                    hmi.showPage("scheduler");
                    hmi.waitForPageRespon();
                    printDebugln("Scheduler page opened");
                    scheduler(loggerType);
                    goto __start;
                    break;
                case 3:
                    if (++hourPeriod > 23)
                        hourPeriod = 0;
                    break;
                case 4:
                    if (--hourPeriod < 0)
                        hourPeriod = 23;
                    break;
                case 5:
                    if (++minutePeriod > 59)
                        minutePeriod = 0;
                    break;
                case 6:
                    if (--minutePeriod < 0)
                        minutePeriod = 59;
                    break;
                case 7:
                    if (++secondPeriod > 59)
                        secondPeriod = 0;
                    break;
                case 8:
                    if (--secondPeriod < 0)
                        secondPeriod = 59;
                    break;
                case 9:
                    if (data.setPeriodDatalog(loggerType, ((hourPeriod * 3600) + (minutePeriod * 60) + secondPeriod)))
                    {
                        hmi.setVisObjectNextion("saving", true);
                        savingProgress = 0;
                        while (savingProgress <= 100)
                        {
                            hmi.setIntegerToNextion("saving.val", savingProgress);
                            savingProgress += 10;
                            delay(100);
                        }
                        delay(100);
                        hmi.setVisObjectNextion("saving", false);
                    }
                    hmi.flushAvailableButton();
                    break;
                default:
                    break;
                }
                if (i >= 3 && i <= 8)
                {
                    updateStrPeriod(hourPeriod, minutePeriod, secondPeriod);
                }
            }
        }
        if (hmi.getExitPageFlag())
        {
            printDebugln("Exit Datalog page");
            break;
        }
    }
}

void updateSchedulerSettingState(uint8_t settingState)
{
    for (uint8_t j = 0; j < 6; j++)
    {
        if (j == settingState)
            hmi.setIntegerToNextion(String() + "b" + j + ".pco", 2047);
        else
            hmi.setIntegerToNextion(String() + "b" + j + ".pco", 65535);
    }
}

void updateStrOfAllSchedule(uint8_t loggerType)
{
    uint8_t hourSchedule = 0;
    uint8_t minuteSchedule = 0;
    char strHour[3];
    char strMinute[3];

    for (uint8_t i = 0; i < 6; i++)
    {
        if (i < 3)
        {
            hourSchedule = data.getScheduleDatalog(_on_, loggerType, i) / 60;
            minuteSchedule = data.getScheduleDatalog(_on_, loggerType, i) % 60;
        }
        else
        {
            hourSchedule = data.getScheduleDatalog(_off_, loggerType, i - 3) / 60;
            minuteSchedule = data.getScheduleDatalog(_off_, loggerType, i - 3) % 60;
        }
        integerToString(hourSchedule, strHour, 2);
        integerToString(minuteSchedule, strMinute, 2);
        hmi.setStringToNextion(String() + "b" + i + ".txt", String() + strHour + ":" + strMinute);
    }
}
void updateStatusOfAllSchedule(uint8_t loggerType)
{
    for (uint8_t i = 6; i < 12; i++)
    {
        if (i < 9)
            hmi.setIntegerToNextion(String() + "b" + i + ".val", data.getEnableSchedule(_on_, loggerType, i - 6));
        else
            hmi.setIntegerToNextion(String() + "b" + i + ".val", data.getEnableSchedule(_off_, loggerType, i - 9));
    }
}

void updateStrOfSetSchedule(uint8_t hourSchedule, uint8_t minuteSchedule)
{
    char strHour[3];
    char strMinute[3];

    integerToString(hourSchedule, strHour, 2);
    integerToString(minuteSchedule, strMinute, 2);

    hmi.setStringToNextion("t0.txt", strHour);
    hmi.setStringToNextion("t1.txt", strMinute);
}

void Datalogger::scheduler(uint8_t loggerType)
{
    bool button[17];
    bool enDis[6] = {false};
    int8_t hourSchedule;
    int8_t minuteSchedule;
    uint8_t savingProgress = 0;
    uint8_t settingState = 0;
    bool set = false;

    hmi.showPage("scheduler");
    hmi.waitForPageRespon();

    for (uint8_t i = 0; i < 6; i++)
    {
        if (i < 3)
            enDis[i] = data.getEnableSchedule(_off_, loggerType, i);
        else
            enDis[i] = data.getEnableSchedule(_on_, loggerType, (i - 3));
    }

    updateStrOfAllSchedule(loggerType);
    updateStatusOfAllSchedule(loggerType);
    updateSchedulerSettingState(settingState);

    hourSchedule = data.getScheduleDatalog(_on_, loggerType, 0) / 60;
    minuteSchedule = data.getScheduleDatalog(_on_, loggerType, 0) % 60;
    updateStrOfSetSchedule(hourSchedule, minuteSchedule);

    while (true)
    {
        for (uint8_t i = 0; i < 17; i++)
        {
            button[i] = hmi.getDataButton(i);
            if (button[i])
            {
                switch (i)
                {
                case 12:
                    if (++hourSchedule > 23)
                        hourSchedule = 0;
                    break;
                case 13:
                    if (--hourSchedule < 0)
                        hourSchedule = 23;
                    break;
                case 14:
                    if (++minuteSchedule > 59)
                        minuteSchedule = 0;
                    break;
                case 15:
                    if (--minuteSchedule < 0)
                        minuteSchedule = 59;
                    break;
                case 16:
                    if (settingState < 3)
                        set = data.setScheduleDatalog(_on_, loggerType, settingState, ((hourSchedule * 60) + (minuteSchedule)));
                    else
                        set = data.setScheduleDatalog(_off_, loggerType, settingState - 3, ((hourSchedule * 60) + (minuteSchedule)));
                    if (set)
                    {
                        hmi.setVisObjectNextion("saving", true);
                        savingProgress = 0;
                        while (savingProgress <= 100)
                        {
                            hmi.setIntegerToNextion("saving.val", savingProgress);
                            savingProgress += 10;
                            delay(100);
                        }
                        delay(100);
                        hmi.setVisObjectNextion("saving", false);
                        updateStrOfAllSchedule(loggerType);
                    }
                    hmi.flushAvailableButton();
                    break;
                default:
                    break;
                }
                if (i >= 0 && i <= 5)
                {
                    settingState = i;
                    updateSchedulerSettingState(settingState);
                    if (settingState < 3)
                    {
                        hourSchedule = (data.getScheduleDatalog(_on_, loggerType, settingState) / 60);
                        minuteSchedule = (data.getScheduleDatalog(_on_, loggerType, settingState) % 60);
                    }
                    else
                    {
                        hourSchedule = (data.getScheduleDatalog(_off_, loggerType, settingState - 3) / 60);
                        minuteSchedule = (data.getScheduleDatalog(_off_, loggerType, settingState - 3) % 60);
                    }
                    updateStrOfSetSchedule(hourSchedule, minuteSchedule);
                }
                else if (i >= 6 && i <= 11)
                {
                    if (i < 9)
                    {
                        enDis[i - 3] = !enDis[i - 3];
                        data.setEnableSchedule(_on_, loggerType, (i - 6), enDis[i - 3]);
                    }
                    else
                    {
                        enDis[i - 9] = !enDis[i - 9];
                        data.setEnableSchedule(_off_, loggerType, (i - 9), enDis[i - 9]);
                    }
                    updateStatusOfAllSchedule(loggerType);
                }
                else if (i >= 12 && i <= 15)
                {
                    updateStrOfSetSchedule(hourSchedule, minuteSchedule);
                }
            }
        }

        if (hmi.getExitPageFlag())
        {
            printDebugln("Exit Scheduler page");
            break;
        }
    }
}
void Datalogger::showBaudrateOption(bool type, bool show)
{
    hmi.setVisObjectNextion("b0", !show);
    hmi.setVisObjectNextion("q2", !show);
    hmi.setVisObjectNextion("b2", !show);

    hmi.setVisObjectNextion("q5", show);
    if (type == logging)
        hmi.setVisObjectNextion("q6", show);
    for (uint i = 10; i <= 19; i++)
        hmi.setVisObjectNextion(String() + "b" + i, show);
}
void Datalogger::updateSelectedBaudrateToNextion(bool type, uint8_t selected)
{
    data.setBaudrateSerial(type, selected);
    printDebugln(String() + "Baudrate 1 set: " + data.getBaudrateSerial(type));

    for (uint8_t i = 10; i <= 19; i++)
    {
        if ((i - 10) == selected)
            hmi.setIntegerToNextion(String() + "b" + i + ".picc", 59);
        else
            hmi.setIntegerToNextion(String() + "b" + i + ".picc", 57);
    }
}

bool Datalogger::checkSchedule(bool scheduleType, uint8_t loggerType)
{
    uint16_t timeMinute = 0;

    mtime.getActualTimeInMinute(&timeMinute);

    printDebug(String() + (timeMinute / 60) + ":" + (timeMinute % 60) + " && " + (data.getScheduleDatalog(scheduleType, loggerType, 0) / 60) + ":" + (data.getScheduleDatalog(scheduleType, loggerType, 0) % 60) + " || ");
    printDebug(String() + (timeMinute / 60) + ":" + (timeMinute % 60) + " && " + (data.getScheduleDatalog(scheduleType, loggerType, 1) / 60) + ":" + (data.getScheduleDatalog(scheduleType, loggerType, 1) % 60) + " || ");
    printDebug(String() + (timeMinute / 60) + ":" + (timeMinute % 60) + " && " + (data.getScheduleDatalog(scheduleType, loggerType, 2) / 60) + ":" + (data.getScheduleDatalog(scheduleType, loggerType, 2) % 60) + " -> ");
    printDebug(loggerType == 0 ? "Serial " : loggerType == 1 ? "Local "
                                                             : "Remote ");
    printDebug(scheduleType ? "On " : "Off ");

    if ((timeMinute == data.getScheduleDatalog(scheduleType, loggerType, 0) && data.getEnableSchedule(scheduleType, loggerType, 0)) ||
        (timeMinute == data.getScheduleDatalog(scheduleType, loggerType, 1) && data.getEnableSchedule(scheduleType, loggerType, 1)) ||
        (timeMinute == data.getScheduleDatalog(scheduleType, loggerType, 2) && data.getEnableSchedule(scheduleType, loggerType, 2)))
    {
        printDebugln("true");
        return true;
    }
    printDebugln("false");
    return false;
}

void Datalogger::logData(uint8_t loggerType)
{
    if (data.getDatalogStatus(loggerType))
    {
        if (!rtos.counterDownSecondsLog[loggerType])
        {
            if (loggerType == serial)
            {
                printDebugln("Serial data logging triggered!");
            }
            else if (loggerType == local)
            {
                printDebugln("Local data logging triggered!");
            }
            else if (loggerType == remote)
            {
                printDebugln("Remote data logging triggered!");
            }
            rtos.counterDownSecondsLog[loggerType] = data.getPeriodDatalog(loggerType);
        }
    }
}

Datalogger logger;