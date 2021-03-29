#include "MicroSD.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "FlashMemory/FlashMemory.h"
#include "Time/MyTime.h"
#include "DebugSerial/DebugSerial.h"

/*
   Connect the SD card to the following pins:

   SD Card | ESP32
      D2       -
      D3       SS
      CMD      MOSI
      VSS      GND
      VDD      3.3V
      CLK      SCK
      VSS      GND
      D0       MISO
      D1       -
*/

void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
    if (data.getDebugMode())
        Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if (!root)
    {
        printDebug("Failed to open directory");
        return;
    }
    if (!root.isDirectory())
    {
        printDebug("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            Serial.print("  DIR : ");
            printDebug(file.name());
            if (levels)
            {
                listDir(fs, file.name(), levels - 1);
            }
        }
        else
        {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            printDebug(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char *path)
{
    if (data.getDebugMode())
        Serial.printf("Creating Dir: %s\n", path);
    if (fs.mkdir(path))
    {
        printDebug("Dir created");
    }
    else
    {
        printDebug("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char *path)
{
    if (data.getDebugMode())
        Serial.printf("Removing Dir: %s\n", path);
    if (fs.rmdir(path))
    {
        printDebug("Dir removed");
    }
    else
    {
        printDebug("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char *path)
{
    if (data.getDebugMode())
        Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if (!file)
    {
        printDebug("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while (file.available())
    {
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char *path, const char *message)
{
    if (data.getDebugMode())
        Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if (!file)
    {
        printDebug("Failed to open file for writing");
        return;
    }
    if (file.print(message))
    {
        printDebug("File written");
    }
    else
    {
        printDebug("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char *path, const char *message)
{
    if (data.getDebugMode())
        Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if (!file)
    {
        printDebug("Failed to open file for appending");
        return;
    }
    if (file.print(message))
    {
        printDebug("Message appended");
    }
    else
    {
        printDebug("Append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char *path1, const char *path2)
{
    if (data.getDebugMode())
        Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2))
    {
        printDebug("File renamed");
    }
    else
    {
        printDebug("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char *path)
{
    if (data.getDebugMode())
        Serial.printf("Deleting file: %s\n", path);
    if (fs.remove(path))
    {
        printDebug("File deleted");
    }
    else
    {
        printDebug("Delete failed");
    }
}

void testFileIO(fs::FS &fs, const char *path)
{
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if (file)
    {
        len = file.size();
        size_t flen = len;
        start = millis();
        while (len)
        {
            size_t toRead = len;
            if (toRead > 512)
            {
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        if (data.getDebugMode())
            Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    }
    else
    {
        printDebug("Failed to open file for reading");
    }

    file = fs.open(path, FILE_WRITE);
    if (!file)
    {
        printDebug("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for (i = 0; i < 2048; i++)
    {
        file.write(buf, 512);
    }
    end = millis() - start;
    if (data.getDebugMode())
        Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}

void MicroSDCard::getCSVFileName(char *buffer)
{
    char name[20];
    integerToString(RTC.rtc.date, name, 2);
    integerToString(RTC.rtc.month, &name[2], 2);
    integerToString(RTC.rtc.year + 2000, &name[4], 4);
    strcpy(buffer, name);
}

bool MicroSDCard::setup(void)
{
    if (!SD.begin())
    {
        printDebug("Card Mount Failed");
        cardMounted = false;
        return false;
    }

    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE)
    {
        printDebug("No SD card attached");
        cardMounted = false;
        return false;
    }

    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC)
    {
        printDebug("MMC");
    }
    else if (cardType == CARD_SD)
    {
        printDebug("SDSC");
    }
    else if (cardType == CARD_SDHC)
    {
        printDebug("SDHC");
    }
    else
    {
        printDebug("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    if (data.getDebugMode())
        Serial.printf("SD Card Size: %lluMB\n", cardSize);

    if (data.getDebugMode())
        Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    if (data.getDebugMode())
        Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
 
    cardMounted = true;
    getCSVFileName(csvFileName);
    if (data.getDatalogStatus(LOCAL))
        writeFileCSV(csvFileName);
    return cardMounted;
}

void MicroSDCard::setupAfterRemoved()
{
    delay(1000);
    setup();
    delay(1000);
}

void MicroSDCard::writeFileCSV(String fileName)
{
    String name;
    name = "/";
    name += fileName;
    name += ".csv";

    if (cardMounted)
    {
        writeFile(SD, name.c_str(), "");
        writeTableHeader(fileName);
    }
}

void MicroSDCard::appendFileCSVWithName(const char *name, const char *data)
{
    char fileName[20];
    char dataBuffer[100];

    if (cardMounted)
    {
        strcpy(fileName, "/");
        strcat(fileName, name);
        strcat(fileName, ".csv");
        dataBuffer[0] = '\n';
        strcpy(&dataBuffer[1], data);
        appendFile(SD, fileName, dataBuffer);
    }
}

void MicroSDCard::appendFileCSVWithName(String name, String data)
{
    char fileName[20];
    char dataBuffer[100];
    byte lengthString = name.length() + 1;

    if (cardMounted)
    {
        fileName[0] = '/';
        name.toCharArray(&fileName[1], lengthString);
        fileName[lengthString] = '\0';
        strcat(fileName, ".csv");

        lengthString = data.length() + 1;
        strcpy(dataBuffer, "\n");
        data.toCharArray(&dataBuffer[1], lengthString);
        dataBuffer[lengthString] = '\0';

        appendFile(SD, fileName, dataBuffer);
    }
}

void MicroSDCard::getFileTitle(char *buffer)
{
    char fileTitle[50];

    strcpy(fileTitle, "File is written on ");
    RTC.getDateString(&fileTitle[(String() + fileTitle).length()]);
    strcat(fileTitle, " ");
    RTC.getTimeString(&fileTitle[(String() + fileTitle).length()]);

    strcpy(buffer, fileTitle);
}

String getStringUnit(uint8_t unit)
{
    if (unit == 0)
        return "[g]";
    else if (unit == 1)
        return "[mg]";
    else if (unit == 2)
        return "[lb]";
    else if (unit == 3)
        return "[oz]";
    else if (unit == 4)
        return "[ozt]";
    else if (unit == 5)
        return "[ct]";
    else if (unit == 6)
        return "[kg]";
    else if (unit == 7)
        return "[N]";
    else if (unit == 8)
        return "[d]";
    else if (unit == 9)
        return "[GN]";
    else
        return "[g]";
}

void MicroSDCard::writeTableHeader(String fileName)
{
    String strHeader;
    String strTemp = "";
    char title[50];
    strHeader = "Date;Time";

    for (uint8_t i = 0; i < MAX_CHANNEL; i++)
    {
        strTemp += ";";
        strTemp += String() + "CH" + (i + 1) + " " + getStringUnit(data.getMeasurementUnit());
    }

    strHeader += strTemp;

    getFileTitle(title);
    printDebug(String() + fileName + ".csv");
    printDebug(title);
    printDebug(strHeader);

    appendFileCSVWithName(fileName, title);
    appendFileCSVWithName(fileName, strHeader);
}


MicroSDCard card;