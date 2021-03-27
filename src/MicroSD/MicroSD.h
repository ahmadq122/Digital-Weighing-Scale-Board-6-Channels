#ifndef __MICRO_SD__
#define __MICRO_SD__

#include "Arduino.h"

class MicroSDCard
{
public:
    char csvFileName[20];

    bool setup(void);
    void setupAfterRemoved(void);
    void writeFileCSV(String fileName);
    void appendFileCSVWithName(const char *name, const char *data);
    void appendFileCSVWithName(String name, String data);
    void writeTableHeader(String fileName);
    void getCSVFileName(char *buffer);
    void getFileTitle(char *buffer);

private:
    String title;
    bool cardMounted = false;
};

extern MicroSDCard card;
#endif