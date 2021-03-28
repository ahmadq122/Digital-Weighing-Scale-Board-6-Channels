#include "ADS1232.h"

void integerToString(uint32_t number, char *buffer, uint8_t len);

void ADS1232::begin()
{
    PDWN[0] = 32;
    SCLK[0] = 33;
    DOUT[0] = 25;
    A0[0] = 26;

    PDWN[1] = 27;
    SCLK[1] = 14;
    DOUT[1] = 12;
    A0[1] = 13;

    PDWN[2] = 15;
    SCLK[2] = 2;
    DOUT[2] = 0;
    A0[2] = 4;

    for (uint8_t i = 0; i < 3; i++)
    {
        pinMode(PDWN[i], OUTPUT);
        pinMode(SCLK[i], OUTPUT);
        pinMode(DOUT[i], INPUT);
        pinMode(A0[i], OUTPUT);
    }
}

bool ADS1232::dataReady(uint8_t board)
{
    return digitalRead(DOUT[board]) == LOW; //return true if meet the condition
}

void ADS1232::powerUp(uint8_t board)
{
    digitalWrite(PDWN[board], HIGH);
    while (!dataReady(board))
        ;
}

void ADS1232::powerDown(uint8_t board)
{
    digitalWrite(PDWN[board], LOW);
}
void ADS1232::setChannel(uint8_t board, uint8_t channel)
{
    if (channel)
        digitalWrite(A0[board], HIGH); //channel 2 is selected
    else
        digitalWrite(A0[board], LOW); //channel 1 is selected
}

//This function must be called in setup
void ADS1232::init(void)
{
    powerUp(ads1);
    powerUp(ads2);
    powerUp(ads3);

    index[0][0] = 0;
    adcRead[0][0] = 0;

    while (adcRead[0] == 0)
        dataRead(0, 0, 0);
}


bool ADS1232::dataRead(uint8_t board, bool channel, bool calibrating)
{
    long ADC_value_read = 0;
    long to_add = 0;
    unsigned long start;
    unsigned int waitingTime;
    unsigned int SettlingTimeAfterChangeChannel = 0;
    uint8_t samples_mov_average = SAMPLE_MOV_AVERAGE;

    if (channel != prevChannel[board])
    {
        setChannel(board, channel);

        ////if setting speedrate used 80SPS
        SettlingTimeAfterChangeChannel = 55;
        ////if setting speedrate use 10SPS
        // SettlingTimeAfterChangeChannel = 405;
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
        waitingTime = 150;
        ////if setting speedrate use 10SPS
        // waitingTime = 850;
    }
    else
    {
        ////if setting speedrate used 80SPS
        waitingTime = 20;
        ////if setting speedrate use 10SPS
        // waitingTime = 150;
    }

    waitingTime += SettlingTimeAfterChangeChannel;
    waitingTime += 600; //[ms] Add some extra time ( sometimes takes longer than what datasheet claims! )
    start = millis();

    while (dataReady(board))
    {
        if (millis() - start > waitingTime)
            return false; // Timeout waiting for HIGH
    }

    start = millis();
    while (!dataReady(board))
    {
        if (millis() - start > waitingTime)
            return false; // Timeout waiting for LOW
    }

    byte data[3] = {0, 0, 0};

    /* From datasheet (page 19): The data must be retrieved before
             * new data are updated or else it will be overwritten
             * This means that I need to wait DOUT goes LOW.
             * to LOW (that's the signal I have new data) and get it in 12.5 or 100 ms 100ms (4.9152MHz)
             */
    if (dataReady(board))
    {
        digitalWrite(SCLK[board], 0);
        //pulse the clock pin 24 times to read the data
        data[2] = shiftIn(DOUT[board], SCLK[board], MSBFIRST);
        data[1] = shiftIn(DOUT[board], SCLK[board], MSBFIRST);
        data[0] = shiftIn(DOUT[board], SCLK[board], MSBFIRST);

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
        if (data[2] & B1000000)
        {
            data[2] &= B01111111;
            to_add = 0x0UL;
        }
        // If we have a positive number, this will be translated between 0x800000 -> 0x1000000
        else
        {
            to_add = 0x7FFFFFUL; // 2^23
        }
        ADC_value_read = (static_cast<long>(data[2]) << 16 | static_cast<long>(data[1]) << 8 | static_cast<long>(data[0]));
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
            index[board][channel] = samples_mov_average - 1;
            adcBuffer[board][channel][index[board][channel]] = ADC_value_read;
        }
        //////get the filter moving average value after all buffer have a value
        if (index[board][channel] == samples_mov_average - 1)
        {
            adcRead[board][channel] = 0; //Reset the previous value
            for (byte i = 0; i < SAMPLE_MOV_AVERAGE; i++)
                adcRead[board][channel] += static_cast<uint32_t>(adcBuffer[board][channel][i]);
            adcRead[board][channel] /= samples_mov_average;
        }
        char tempStr[15];
        integerToString(adcRead[board][channel], tempStr, 10);
        adcReadString[board][channel] = tempStr;
        return true;
    }
    return false;
}

float ADS1232::getWeightInUnit(float gram, uint8_t unit)
{
    return gram / dividerUnits[unit];
}

ADS1232 ads;