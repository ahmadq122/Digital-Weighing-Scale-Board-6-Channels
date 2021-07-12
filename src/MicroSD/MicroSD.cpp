#include "MicroSD.h"
#include "FlashMemory/FlashMemory.h"
#include "Time/MyTime.h"
#include "DebugSerial/DebugSerial.h"
#include "RTOS/RTOS.h"
#include "Utility/Utility.h"

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

void MicroSDCard::listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
    if (data.getDebugMode())
        Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if (!root)
    {
        printDebugln("Failed to open directory");
        return;
    }
    if (!root.isDirectory())
    {
        printDebugln("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            Serial.print("  DIR : ");
            printDebugln(file.name());
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
            printDebugln(file.size());
        }
        file = root.openNextFile();
    }
}

void MicroSDCard::createDir(fs::FS &fs, const char *path)
{
    if (data.getDebugMode())
        Serial.printf("Creating Dir: %s\n", path);
    if (fs.mkdir(path))
    {
        printDebugln("Dir created");
    }
    else
    {
        printDebugln("mkdir failed");
    }
}

void MicroSDCard::removeDir(fs::FS &fs, const char *path)
{
    if (data.getDebugMode())
        Serial.printf("Removing Dir: %s\n", path);
    if (fs.rmdir(path))
    {
        printDebugln("Dir removed");
    }
    else
    {
        printDebugln("rmdir failed");
    }
}

void MicroSDCard::readFile(fs::FS &fs, const char *path)
{
    if (data.getDebugMode())
        Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if (!file)
    {
        printDebugln("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while (file.available())
    {
        Serial.write(file.read());
    }
    file.close();
}

void MicroSDCard::writeFile(fs::FS &fs, const char *path, const char *message)
{
    if (data.getDebugMode())
        Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if (!file)
    {
        printDebugln("Failed to open file for writing");
        return;
    }
    if (file.print(message))
    {
        printDebugln("File written");
    }
    else
    {
        printDebugln("Write failed");
    }
    file.close();
}

void MicroSDCard::appendFile(fs::FS &fs, const char *path, const char *message)
{
    if (data.getDebugMode())
        Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if (!file)
    {
        printDebugln("Failed to open file for appending");
        return;
    }
    if (file.print(message))
    {
        printDebugln("Message appended");
    }
    else
    {
        printDebugln("Append failed");
    }
    file.close();
}

void MicroSDCard::renameFile(fs::FS &fs, const char *path1, const char *path2)
{
    if (data.getDebugMode())
        Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2))
    {
        printDebugln("File renamed");
    }
    else
    {
        printDebugln("Rename failed");
    }
}

void MicroSDCard::deleteFile(fs::FS &fs, const char *path)
{
    if (data.getDebugMode())
        Serial.printf("Deleting file: %s\n", path);
    if (fs.remove(path))
    {
        printDebugln("File deleted");
    }
    else
    {
        printDebugln("Delete failed");
    }
}

void MicroSDCard::testFileIO(fs::FS &fs, const char *path)
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
        printDebugln("Failed to open file for reading");
    }

    file = fs.open(path, FILE_WRITE);
    if (!file)
    {
        printDebugln("Failed to open file for writing");
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

String MicroSDCard::getCSVFileName(void)
{
    return csvFileName;
}

bool MicroSDCard::setup(void)
{
    if (!SD.begin())
    {
        printDebugln("Card Mount Failed");
        cardMounted = false;
        return false;
    }

    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE)
    {
        printDebugln("No SD card attached");
        cardMounted = false;
        return false;
    }

    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC)
    {
        printDebugln("MMC");
    }
    else if (cardType == CARD_SD)
    {
        printDebugln("SDSC");
    }
    else if (cardType == CARD_SDHC)
    {
        printDebugln("SDHC");
    }
    else
    {
        printDebugln("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    if (data.getDebugMode())
        Serial.printf("SD Card Size: %lluMB\n", cardSize);

    if (data.getDebugMode())
        Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    if (data.getDebugMode())
        Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));

    cardMounted = true;
    if (data.getDatalogStatus(local))
    {
        updateCsvFileName();
    }
    return cardMounted;
}

void MicroSDCard::setupAfterRemoved(void)
{
    delay(1000);
    setup();
    delay(1000);
}

void MicroSDCard::unmount(void)
{
    SD.end();
    cardMounted = false;
}

void MicroSDCard::writeFileCSV(void)
{
    String path;
    String msg;
    uint8_t countOut = 0;
    bool fileExists = false;
    path = "/";
    path += csvFileName;

    if (cardMounted)
    {
        if (SD.exists(path))
        {
            printDebugln("File in \"" + path + "\" path already exist!");
            writeTableHeader();
        }
        else
        {
            printDebugln("Write new file at path \"" + path + "\"!");
            msg = String() + "This file has created on " + mtime.getDayStr() + ", " + mtime.getDateStr() + " at " + mtime.getTimeStr();
            printDebugln(msg);
            while (!fileExists && countOut < 3)
            {
                writeFile(SD, path.c_str(), msg.c_str());
                delay(250);
                fileExists = SD.exists(path);
                countOut++;
            }
            if (fileExists)
                writeTableHeader();
            else
                printDebugln("Time out in writing file " + path);
        }
    }
}

void MicroSDCard::appendFileCsvWithName(String name, String data)
{
    char path[20];
    char dataBuffer[100];
    byte lengthString = name.length() + 1;

    if (cardMounted)
    {
        path[0] = '/';
        name.toCharArray(&path[1], lengthString);
        path[lengthString] = '\0';

        lengthString = data.length() + 1;
        strcpy(dataBuffer, "\n");
        data.toCharArray(&dataBuffer[1], lengthString);
        dataBuffer[lengthString] = '\0';

        appendFile(SD, path, dataBuffer);
    }
}

void MicroSDCard::appendFileCsv(String data)
{
    char path[20];
    char dataBuffer[100];
    byte lengthString = csvFileName.length() + 1;

    path[0] = '/';
    csvFileName.toCharArray(&path[1], lengthString);
    path[lengthString] = '\0';

    lengthString = data.length() + 1;
    strcpy(dataBuffer, "\n");
    data.toCharArray(&dataBuffer[1], lengthString);
    dataBuffer[lengthString] = '\0';

    if (SD.exists(path))
    {
        appendFile(SD, path, dataBuffer);
    }
    else
    {
        // SD.end();
        printDebug(path);
        printDebugln(" file doesn't exist!");
        // setupAfterRemoved();
    }
}

void MicroSDCard::getFileTitle(char *buffer)
{
    char fileTitle[80];

    strcpy(fileTitle, "The following table has written on ");
    mtime.getDateStr(&fileTitle[(String() + fileTitle).length()]);
    strcat(fileTitle, " at ");
    mtime.getTimeStr(&fileTitle[(String() + fileTitle).length()]);

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

void MicroSDCard::writeTableHeader(void)
{
    String strHeader;
    String strTemp = "";
    char title[80];
    strHeader = "Time";

    for (uint8_t i = 0; i < MAX_CHANNEL; i++)
    {
        strTemp += ";";
        strTemp += String() + "Ch" + (i + 1) + " " + getStringUnit(data.getMeasurementUnit());
    }

    strHeader += strTemp;

    getFileTitle(title);
    // printDebugln(csvFileName);
    // printDebugln(title);
    // printDebugln(strHeader);

    appendFileCsvWithName(csvFileName, title);
    appendFileCsvWithName(csvFileName, strHeader);
}

void MicroSDCard::updateCsvFileName(void)
{
    uint8_t date, year, month;
    mtime.getActualDate(&date, &month, &year);
    String fileName = String() + (2000 + year) + "-" + utils.integerToString(month, 2) + "-" + utils.integerToString(date, 2) + ".csv";
    // printDebugln(csvFileName);
    setCsvFileName(fileName);
}

void MicroSDCard::setCsvFileName(String fileName)
{
    if (csvFileName != fileName)
    {
        csvFileName = fileName;
        printDebugln(String() + "Set new *.csv filename : " + fileName);
        writeFileCSV();
    }
}
MicroSDCard card;