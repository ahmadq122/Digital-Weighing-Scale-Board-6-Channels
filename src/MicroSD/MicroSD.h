#ifndef __MICRO_SD__
#define __MICRO_SD__

#include "Arduino.h"

class MicroSDCard
{
public:


    bool setup(void);
    void setupAfterRemoved(void);
    void writeFileCSV(void);
    void appendFileCSVWithName(String name, String data);
    void appendFileCSVWithName(String data);
    void writeTableHeader(void);
    String getCSVFileName(void);
    void getFileTitle(char *buffer);
    void setCsvFileName(String fileName);

private:
    String title;
        String csvFileName;
    bool cardMounted = false;
};

extern MicroSDCard card;
#endif