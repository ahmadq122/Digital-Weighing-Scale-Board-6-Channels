#ifndef __MICRO_SD__
#define __MICRO_SD__

#include "Arduino.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"

class MicroSDCard
{
public:
    bool setup(void);
    void setupAfterRemoved(void);
    void unmount(void);
    void writeFileCSV(void);
    void appendFileCsvWithName(String name, String data);
    void appendFileCsv(String data);
    void writeTableHeader(void);
    String getCSVFileName(void);
    void getFileTitle(char *buffer);
    void updateCsvFileName(void);
    void setCsvFileName(String fileName);

    bool cardMounted = false;

private:
    String title;
    String csvFileName;

    void listDir(fs::FS &fs, const char *dirname, uint8_t levels);
    void createDir(fs::FS &fs, const char *path);
    void removeDir(fs::FS &fs, const char *path);
    void readFile(fs::FS &fs, const char *path);
    void writeFile(fs::FS &fs, const char *path, const char *message);
    void appendFile(fs::FS &fs, const char *path, const char *message);
    void renameFile(fs::FS &fs, const char *path1, const char *path2);
    void deleteFile(fs::FS &fs, const char *path);
    void testFileIO(fs::FS &fs, const char *path);
};

extern MicroSDCard card;
#endif