#ifndef _ADS1232_H_
#define _ADS1232_H_

#include "Arduino.h"

#define SAMPLE_MOV_AVERAGE 80

enum Channel
{
    Channel1,
    Channel2,
    Channel3,
    Channel4,
    Channel5,
    Channel6
};
enum MeasurementUnits
{
    gram,       //1g
    milligram,  //1mg   = (1/0.001)g
    pound,      //1lb   = (1/453.59237)g
    ounce,      //1oz   = (1/28.34952)g
    troy_ounce, //1ozt  = (1/31.1034768)g
    carat,      //1ct   = (1/0.2)g
    kilogram,   //1kg   = (1/1000)g
    newton,     //1N    = (1/0.009807)g
    dram,       //1d    = (1/0.564383)g
    grain       //1GN   = (1/15.43236)g
};

enum BoardADS
{
    ads1,
    ads2,
    ads3
};

class ADS1232
{
public:
    uint32_t adcRead[3][2];
    String adcReadString[3][2];

    void begin();
    void init();
    float getWeightInUnit(float gram, uint8_t unit);

private:
    uint8_t PDWN[3];
    uint8_t SCLK[3];
    uint8_t DOUT[3];
    uint8_t A0[3];
    uint32_t adcBuffer[3][2][SAMPLE_MOV_AVERAGE];
    uint8_t index[3][2];
    float dividerUnits[11] = {
        1,
        0.001,
        453.59237,
        28.34952,
        31.1034768,
        0.2,
        1000,
        0.009807,
        0.564383,
        15.43236};
    bool prevChannel[3];

    bool dataReady(uint8_t board);
    bool dataRead(uint8_t board, bool channel, bool calibrating);
    void setChannel(uint8_t board, uint8_t channel);
    void powerUp(uint8_t board);
    void powerDown(uint8_t board);
};

extern ADS1232 ads;

#endif