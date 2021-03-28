#ifndef __NEXTION__H__
#define __NEXTION__H__

#include <Arduino.h>

#define NEX_RET_NUMBER_HEAD (0x71)
#define NEX_RET_CMD_FINISHED (0x01)

#define NexSerial Serial2

#define FLAG_BUTTON "btn"
#define FLAG_STRING "str"
#define FLAG_INTEGER "int"
#define FLAG_FLOAT "flo"
#define FLAG_BUZZER "buz"
#define FLAG_WAIT "wet"
#define FLAG_EXIT "ext"
#define FLAG_TOUCH "tch"

//Type of Data get from nextion
enum DataFromNextionFlag
{
    BUTTON,        //"btn-"
    STRING,        //"str-"
    INTEGER,       //"int-"
    FLOAT,         //"flo-"
    BUZZER,        //"buz-"
    WAIT,          //"wet-"
    EXIT,          //"ext-"
    TOUCH,         //"tch-"
    UNKNOWN = 0xFF //error
};

class Nextion
{
public:
    void init(void);
    // void setStringToNextion(const char *variableName, const char *newString);
    void setStringToNextion(String variableName, String newString);
    void setIntegerToNextion(const char *variableName, uint64_t newValue);
    void setIntegerToNextion(String variableName, uint64_t newValue);
    bool checkDataStringFromNextion(const char *stringData);

    uint8_t getDataButton(uint8_t id);
    bool getExitPageFlag(void);
    bool getDataBuzzer(void);
    const char *getDataString(uint8_t id);
    uint64_t getDataInteger(uint8_t id);
    double getDataFloat(uint8_t id);
    void serialEvent_2(void);
    bool getWaitingEndSignal(void);
    void flushAvailableSerial(void);
    bool checkAnyButtonPressed(uint8_t *getId);
    void setVisObjectNextion(String object, bool visValue);
    void showPage(String page);
    uint16_t getValue(const char *variableName, uint32_t *number);
    uint16_t getText(const char *variableName, char *buffer, uint32_t len);
    void flushAvailableButton(void);
    void waitForPageRespon(void);

private:
    void sendCommandToNextion(const char *cmd);
    uint8_t getTypeOfDataFromNextion(const char *stringData);
    bool recvRetNumber(uint32_t *number, uint32_t timeout = 100);
    uint16_t recvRetString(char *buffer, uint16_t len, uint32_t timeout = 100);
    void presetScreenBrightness(void);
    char dataString[10][20];
    uint8_t dataButton[20] = {0};
    uint64_t dataInteger[10] = {0};
    double dataFloat[10] = {0};
    uint8_t dataBuzzer = 0;
    bool waitingEndSignal = false;
    bool exitPageFlag = false;
    uint8_t exitPageFlagCounter = 0;
};

extern Nextion hmi;
#endif