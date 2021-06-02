#include "ADS1232.h"
#include "DebugSerial/DebugSerial.h"
#include "RTOS/RTOS.h"
#include "PinMap.h"
#include "Utility/Utility.h"
#include "FlashMemory/FlashMemory.h"

void ADS1232::begin(void)
{
    printDebugln("ADS1232 Initial Pin");

    PDWN = Pin_PowerDown;
    SCLK[0] = Pin_SCLK_ADS1;
    DOUT[0] = Pin_DOUT_ADS1;
    A0[0] = Pin_A0_ADS1;

    SCLK[1] = Pin_SCLK_ADS2;
    DOUT[1] = Pin_DOUT_ADS2;
    A0[1] = Pin_A0_ADS2;

    SCLK[2] = Pin_SCLK_ADS3;
    DOUT[2] = Pin_DOUT_ADS3;
    A0[2] = Pin_A0_ADS3;

    speed = Pin_Speedrate;

    pinMode(PDWN, OUTPUT);
    pinMode(speed, OUTPUT);
    for (uint8_t i = 0; i < 3; i++)
    {
        pinMode(SCLK[i], OUTPUT);
        pinMode(DOUT[i], INPUT);
        pinMode(A0[i], OUTPUT);
    }
    if (!data.getSpeedRate())
        digitalWrite(speed, LOW);
    else
        digitalWrite(speed, HIGH);
    delay(10);
    powerUp();
    printDebugln("Init Pin done!");
}

bool ADS1232::dataReady(uint8_t board)
{
    return digitalRead(DOUT[board]) == LOW; //return true if meet the condition
}

void ADS1232::powerUp(void)
{
    printDebugln(String() + "Power up ADS boards");
    delayMicroseconds(26);
    digitalWrite(PDWN, HIGH);
    delayMicroseconds(26);
    digitalWrite(PDWN, LOW);
    delayMicroseconds(26);
    digitalWrite(PDWN, HIGH);
    printDebugln(String() + "ADS boards up");
}

void ADS1232::powerDown(void)
{
    digitalWrite(PDWN, LOW);
}
void ADS1232::setChannel(uint8_t board, uint8_t channel)
{
    if (channel)
        digitalWrite(A0[board], HIGH); //channel 2 is selected
    else
        digitalWrite(A0[board], LOW); //channel 1 is selected
}

//This function must be called in setup
bool ADS1232::init(uint8_t board)
{
    uint8_t timeOutCounter = 0;

    printDebugln(String() + "Init ADS board " + (board + 1) + " Channel " + 0);
    index[board][0] = 0;
    adcRead[board][0] = 0;
    while (adcRead[board][0] == 0)
    {
        if (!dataRead(board, 0, 1))
            timeOutCounter++;
        if (timeOutCounter > 5)
        {
            printDebugln(String() + "Init ADS board " + (board + 1) + " Failed!");
            return false;
        }
    }
    timeOutCounter = 0;
    printDebugln(String() + "Init ADS board " + (board + 1) + " Channel " + 1);
    index[board][1] = 0;
    adcRead[board][1] = 0;
    while (adcRead[board][1] == 0)
    {
        if (!dataRead(board, 1, 1))
            timeOutCounter++;
        if (timeOutCounter > 5)
        {
            printDebugln(String() + "Init ADS board " + (board + 1) + " Failed!");
            return false;
        }
    }

    printDebugln(String() + "Init ADS board " + (board + 1) + " Finished");
    return true;
}

bool ADS1232::dataRead(uint8_t board, bool channel, bool calibrating)
{
    long ADC_value_read = 0;
    long to_add = 0;
    unsigned long start;
    unsigned int waitingTime;
    unsigned int SettlingTimeAfterChangeChannel = 0;
    char tempStr[15];
    uint8_t dat[3] = {0, 0, 0};

    if (channel != prevChannel[board])
    {
        setChannel(board, channel);

        ////if setting speedrate used 80SPS
        if (data.getSpeedRate())
            SettlingTimeAfterChangeChannel = 55;
        //// if setting speedrate use 10SPS
        else
            SettlingTimeAfterChangeChannel = 405;
        prevChannel[board] = channel;
    }

    /* A high to low transition on the data pin means that the ADS1231
         * has finished a measurement (see datasheet page 13).
         * This can take up to 100ms (the ADS1231 runs at 10 samples per
         * second!).
         * Note that just testing for the state of the pin is unsafe.
         */

    if (calibrating)
    {
        ////if setting speedrate used 80SPS
        if (data.getSpeedRate())
            waitingTime = 150;
        ////if setting speedrate use 10SPS
        else
            waitingTime = 850;
    }
    else
    {
        ////if setting speedrate used 80SPS
        if (data.getSpeedRate())
            waitingTime = 20;
        ////if setting speedrate use 10SPS
        else
            waitingTime = 150;
    }

    waitingTime += SettlingTimeAfterChangeChannel;
    waitingTime += 600; //[ms] Add some extra time ( sometimes takes longer than what datasheet claims! )
    start = rtos.milliSeconds;

    while (dataReady(board))
    {
        if ((rtos.milliSeconds - start) > waitingTime)
        {
            if (rtos.startProgressBar < 100)
                rtos.updateStartProgressBar(2);
            return false; // Timeout waiting for HIGH}
        }
    }

    start = rtos.milliSeconds;
    while (!dataReady(board))
    {
        if ((rtos.milliSeconds - start) > waitingTime)
        {
            if (rtos.startProgressBar < 100)
                rtos.updateStartProgressBar(2);
            return false; // Timeout waiting for LOW
        }
    }

    /* From datasheet (page 19): The data must be retrieved before
             * new data are updated or else it will be overwritten
             * This means that I need to wait DOUT goes LOW.
             * to LOW (that's the signal I have new data) and get it in 12.5 or 100 ms 100ms (4.9152MHz)
             */
    if (dataReady(board))
    {
        digitalWrite(SCLK[board], 0);
        //pulse the clock pin 24 times to read the data
        dat[2] = shiftIn(DOUT[board], SCLK[board], MSBFIRST);
        dat[1] = shiftIn(DOUT[board], SCLK[board], MSBFIRST);
        dat[0] = shiftIn(DOUT[board], SCLK[board], MSBFIRST);

        /* giving two additional clock cycle to put the DOUT pin to high after shifting the data and
                 * get offset calibration to avoid the inhereted offset error, this method can be used also
                 * for avoiding the DRDY/DOUT remain in the state of the last bit data but will force to high (Page 19. Figure 35)
                 *                       _   _
                 * Additional SCLK ... _| |_| |_
                 */
        if (calibrating)
        {
            digitalWrite(SCLK[board], LOW);
            digitalWrite(SCLK[board], HIGH);
            digitalWrite(SCLK[board], LOW);
            digitalWrite(SCLK[board], HIGH);
            digitalWrite(SCLK[board], LOW);
        }
        else
        {
            digitalWrite(SCLK[board], LOW);
            digitalWrite(SCLK[board], HIGH);
            digitalWrite(SCLK[board], LOW);
        }

        // From datasheet : output 24 bits of data in binary two's complement &
        // The positive full-scale input produces an output code of 7FFFFFh
        // and the negative full-scale input produces an output code of 800000h
        // If we have a negative number, this will be translated between 0 -> 0x7FFFFF
        if (dat[2] & B1000000)
        {
            dat[2] &= B01111111;
            to_add = 0x0UL;
        }
        // If we have a positive number, this will be translated between 0x800000 -> 0x1000000
        else
        {
            to_add = 0x7FFFFFUL; // 2^23
        }
        ADC_value_read = (static_cast<long>(dat[2]) << 16 | static_cast<long>(dat[1]) << 8 | static_cast<long>(dat[0]));
        ADC_value_read += to_add;
        //////data reading is stored in array to be filtered by moving average filter
        if (++index[board][channel] < SAMPLE_MOV_AVERAGE)
        {
            adcBuffer[board][channel][index[board][channel]] = ADC_value_read;
        }
        else
        {
            //push back the buffer value
            for (byte i = 0; i < SAMPLE_MOV_AVERAGE - 1; i++)
                adcBuffer[board][channel][i] = adcBuffer[board][channel][i + 1];
            index[board][channel] = SAMPLE_MOV_AVERAGE - 1;
            adcBuffer[board][channel][index[board][channel]] = ADC_value_read;
        }
        //////get the filter moving average value after all buffer have a value
        if (index[board][channel] == SAMPLE_MOV_AVERAGE - 1)
        {
            adcRead[board][channel] = 0; //Reset the previous value
            for (byte i = 0; i < SAMPLE_MOV_AVERAGE; i++)
                adcRead[board][channel] += static_cast<uint32_t>(adcBuffer[board][channel][i]);
            adcRead[board][channel] /= SAMPLE_MOV_AVERAGE;
        }
        utils.integerToString(adcRead[board][channel], tempStr, 10);
        adcReadString[board][channel] = tempStr;
        if (rtos.startProgressBar < 100)
            rtos.updateStartProgressBar(2);
        return true;
    }
    if (rtos.startProgressBar < 100)
        rtos.updateStartProgressBar(2);
    return false;
}

float ADS1232::getWeightInUnit(byte channel)
{
    uint32_t adcValue = 0;
    uint8_t adcState = 0;
    float weight = 0;

    if (channel == Channel1)
        adcValue = adcRead[ads1][0];
    else if (channel == Channel2)
        adcValue = adcRead[ads1][1];
    else if (channel == Channel3)
        adcValue = adcRead[ads2][0];
    else if (channel == Channel4)
        adcValue = adcRead[ads2][1];
    else if (channel == Channel5)
        adcValue = adcRead[ads3][0];
    else if (channel == Channel6)
        adcValue = adcRead[ads3][1];

    int pointCalibrationStatus = getPointDoneCalibrated(data.getPointCalibrationStatus(channel));
    //  Serial.print(ch ? "CHANNEL2:" : "CHANNEL1:");
    // pointCalibrationStatus = constrain(pointCalibrationStatus - 1, 0, MAX_POINT_CAL);
    adcState = constrain(weightADCstate(adcValue, channel), 0, pointCalibrationStatus);
    // adcState = weightADCstate(adcValue, channel);
    switch (adcState)
    {
    case 0:
        weight = 0;
        break;

    case 1:
        weight = (weightCalculationInGram((data.getAdcCalibrationPoint(channel, (adcState - 1)) + adcTare[channel]), 0.00,
                                          (data.getAdcCalibrationPoint(channel, adcState) + adcTare[channel]), data.getGramCalibrationPoint(channel, (adcState - 1)), adcValue));
        break;

    default:
        weight = (weightCalculationInGram((data.getAdcCalibrationPoint(channel, (adcState - 1)) + adcTare[channel]), data.getGramCalibrationPoint(channel, (adcState - 2)),
                                          (data.getAdcCalibrationPoint(channel, adcState) + adcTare[channel]), data.getGramCalibrationPoint(channel, (adcState - 1)), adcValue));
        break;
    }

    return weight / dividerUnits[data.getMeasurementUnit()];
}

String ADS1232::getStringWeightInUnit(uint8_t channel)
{
    uint8_t unit = data.getMeasurementUnit();
    float weight = getWeightInUnit(channel);

    switch (unit)
    {
    case gram:
        return utils.floatToString(weight, 5, 2);
        // break;
    case milligram:
        return utils.floatToString(weight, 7, 0);
        // break;
    case pound:
        return utils.floatToString(weight, 3, 4);
        // break;
    case ounce:
        return utils.floatToString(weight, 4, 3);
        // break;
    case troy_ounce:
        return utils.floatToString(weight, 4, 3);
        // break;
    case carat:
        return utils.floatToString(weight, 6, 1);
        // break;
    case kilogram:
        return utils.floatToString(weight, 3, 4);
        // break;
    case newton:
        return utils.floatToString(weight, 7, 0);
        // break;
    case dram:
        return utils.floatToString(weight, 6, 1);
        // break;
    case grain:
        return utils.floatToString(weight, 4, 3);
        // break;
    default:
        return utils.floatToString(weight, 5, 2);
        // break;
    }
}

uint8_t ADS1232::getPointDoneCalibrated(uint8_t pointCalibrationStatus)
{
    for (uint8_t i = 0; i < MAX_POINT_CAL; i++)
    {
        if (!bitRead(pointCalibrationStatus, i))
            return i;
    }
    return MAX_POINT_CAL;
}

unsigned char ADS1232::weightADCstate(unsigned long adcValue, uint8_t channel)
{
    for (byte i = 0; i < MAX_POINT_CAL; i++)
    {
        if (adcValue < data.getAdcCalibrationPoint(channel, i) + ADC_OFFSET)
            return i;
    }
    return 0xFF;
}

float ADS1232::weightCalculationInGram(unsigned long x1, float y1, unsigned long x2, float y2, unsigned long adcValue)
{
    float m, b, gram;

    m = ((float)x1 - (float)x2) / (y1 - y2);
    b = (float)x1 - (m * y1);
    gram = ((double)adcValue - b) / m;

    if (gram <= 0)
        gram = 0;
    if (x1 >= x2 || y1 >= y2)
    {
        //    Serial.println(String() + "X1:" + x1 + " X2:" + x2 + " Y1:" + y1 + " Y2:" + y2);
        return ERROR_WEIGHT;
    }
    return gram;
}

ADS1232 ads;