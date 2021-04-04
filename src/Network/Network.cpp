#include "Network.h"
#include <WiFi.h>
#include "../DebugSerial/DebugSerial.h"
#include "../FlashMemory/FlashMemory.h"
#include "../Nextion/Nextion.h"
#include "RTOS/RTOS.h"

void Network::setup(void)
{
    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    // data.readAll();
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    if (data.getNetworkEnable())
    {
        WiFi.begin(data.getSSID(), data.getPassword());
        // WiFi.setAutoReconnect(true);
    }
    printDebug("Wifi Setup done");
}

const char *getStringEncrypt(byte parameter)
{
    if (parameter == 0)
        return "OPEN";
    else if (parameter == 1)
        return "WEP";
    else if (parameter == 2)
        return "WPA_PSK";
    else if (parameter == 3)
        return "WPA2_PSK";
    else if (parameter == 4)
        return "WPA_WPA2_PSK";
    else if (parameter == 5)
        return "WPA2_ENTERPRISE";
    else if (parameter == 6)
        return "MAX";
    else
        return "UNKNOWN";
}

String getIPAdderss(void)
{
    String ip;
    ip = String() + WiFi.localIP()[0] + "." + WiFi.localIP()[1] + "." + WiFi.localIP()[2] + "." + WiFi.localIP()[3];
    return ip;
}

String getStringPasswordMask(const char *string)
{
    String str = string;
    uint8_t length = str.length();

    char mask[length + 1];
    for (byte i = 0; i < length; i++)
        mask[i] = '*';
    mask[length] = '\0';
    return mask;
}

void Network::showNetworkProperty(bool flag)
{
    if (flag)
    {
        for (uint8_t i = 1; i <= 6; i++)
        {
            hmi.setVisObjectNextion(String() + "q" + i, true);
            hmi.setVisObjectNextion(String() + "t" + i, true);
            delay(20);
            if (i == 1)
                hmi.setStringToNextion(String() + "t" + i + ".txt", WiFi.isConnected() ? "Connected" : "Unconnected");
            else if (i == 2)
                hmi.setStringToNextion(String() + "t" + i + ".txt", data.getSSID());
            else if (i == 3)
                hmi.setStringToNextion(String() + "t" + i + ".txt", String() + WiFi.RSSI());
            else if (i == 4)
                hmi.setStringToNextion(String() + "t" + i + ".txt", getIPAdderss());
            else if (i == 5)
                hmi.setStringToNextion(String() + "t" + i + ".txt", getStringEncrypt(data.getEncryptType()));
            else if (i == 6)
                hmi.setStringToNextion(String() + "t" + i + ".txt", getStringPasswordMask(data.getPassword()));
        }
    }
    else
    {
        for (uint8_t i = 1; i <= 6; i++)
        {
            hmi.setVisObjectNextion(String() + "q" + i, false);
            hmi.setVisObjectNextion(String() + "t" + i, false);
        }
    }
}

bool Network::networkConfig(void)
{
    uint8_t button;
    bool netEnDis = false;
    bool needToRefreshStatus = false;

    hmi.showPage("netconf");
    hmi.waitForPageRespon();
    printDebug("Network Config page opened");
    netEnDis = data.getNetworkEnable();

    hmi.setIntegerToNextion("b1.val", netEnDis);

    while (!hmi.getExitPageFlag())
    {
        if (!WiFi.isConnected() && netEnDis)
        {
            needToRefreshStatus = true;
            WiFi.begin(data.getSSID(), data.getPassword());
            printDebug(String() + "Connecting to " + data.getSSID());
        }

        if (data.getNetworkEnable())
        {
            hmi.setVisObjectNextion("b2", true);
            showNetworkProperty(true);
        }
        else
        {
            hmi.setVisObjectNextion("b2", false);
            showNetworkProperty(false);
        }
        //Wait for button press
        while (!hmi.checkAnyButtonPressed(&button))
        {
            // hmi.serialEvent_2();
            // delay(10);
            if (needToRefreshStatus && (WiFi.isConnected()))
            {
                showNetworkProperty(true);
                needToRefreshStatus = false;
            }
        }
        if (hmi.getDataButton(1))
        {
            netEnDis = !netEnDis;
            data.setNetworkEnable(netEnDis);
            hmi.setIntegerToNextion("b1.val", netEnDis);
            if (!netEnDis)
            {
                if (WiFi.isConnected())
                {
                    WiFi.disconnect();
                    printDebug("Network disconnected!");
                }
            }
        }
        else if (hmi.getDataButton(2))
        {
            return true;
        }
    }
    return false; //entering the scanning page;
}

void Network::networkScanning(void)
{
    uint8_t counterScan = 0;
    String ssidScanned[3][10];
    int rssiScanned[3][10];
    uint8_t encryptType[3][10];
    uint8_t numberOfNetworkFound[3];
    uint8_t selectedScan = 0, prevNoScan = 0;
    uint8_t progress = 0;
    String str;
    uint8_t button;

    hmi.showPage("netscan");
    hmi.waitForPageRespon();
    printDebug("Network Scan page opened");
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

__scan:
    hmi.setIntegerToNextion("scan.val", progress);
    hmi.setVisObjectNextion("scan", true);
    counterScan = 0;
    while (counterScan < 3)
    {
        progress += 15;
        hmi.setIntegerToNextion("scan.val", progress);
        numberOfNetworkFound[counterScan] = WiFi.scanNetworks();
        for (uint8_t i = 0; i < numberOfNetworkFound[counterScan]; i++)
        {
            ssidScanned[counterScan][i] = WiFi.SSID(i);
            rssiScanned[counterScan][i] = WiFi.RSSI(i);
            encryptType[counterScan][i] = WiFi.encryptionType(i);
        }
        delay(2000);
        progress += 15;
        hmi.setIntegerToNextion("scan.val", progress);
        selectedScan = prevNoScan >= numberOfNetworkFound[counterScan] ? selectedScan : counterScan;
        prevNoScan = numberOfNetworkFound[counterScan];
        counterScan++;
        delay(1000);
    }

    delay(250);
    progress = 100;
    hmi.setIntegerToNextion("scan.val", progress);
    delay(250);
    hmi.setVisObjectNextion("scan", false);

    hmi.flushAvailableSerial();

    if (numberOfNetworkFound[selectedScan] == 0)
    {
        hmi.setVisObjectNextion("nonet", true);
        printDebug("No networks found!");
        while (!hmi.getExitPageFlag())
        {
            while (!hmi.checkAnyButtonPressed(&button))
            {
            }
            if (hmi.getDataButton(0))
            {
                hmi.setVisObjectNextion("nonet", false);
                progress = 0;
                goto __scan;
            }
        }
    }
    else
    {
        showNetworkList(numberOfNetworkFound[selectedScan]);
        printDebug(String() + numberOfNetworkFound[selectedScan] + " Networks found");
        while (!hmi.getExitPageFlag())
        {
            for (int i = 0; i < numberOfNetworkFound[selectedScan]; ++i)
            {
                if (isStoredNetwork(ssidScanned[selectedScan][i]))
                    showSelectedNetworkIndicator(i, numberOfNetworkFound[selectedScan]);
                str = ssidScanned[selectedScan][i];
                str += ":";
                str += rssiScanned[selectedScan][i];
                printDebug(String() + (i + 1) + ". " + str);
                hmi.setStringToNextion(String() + "b" + i + ".txt", str);
            }
            while (!hmi.checkAnyButtonPressed(&button))
            {
                // hmi.serialEvent_2();
                // Serial.println("wait1");
            }

            if (hmi.getDataButton(button))
            {
                if (data.setSSID(ssidScanned[selectedScan][button].c_str()))
                {
                    showSelectedNetworkIndicator(button, numberOfNetworkFound[selectedScan]);
                    data.setEncryptType(encryptType[selectedScan][button]);
                    WiFi.mode(WIFI_STA);
                    WiFi.disconnect();
                    delay(100);
                }
                rtos.wifiConnected = checkConnection();
                if (!rtos.wifiConnected)
                {
                    hmi.showPage("keyboard");
                    hmi.waitForPageRespon();
                    button = 0;
                    while (!hmi.checkAnyButtonPressed(&button))
                    {
                    }
                    if (hmi.getDataButton(1))
                    {
                        data.setPassword(hmi.getDataString(0));
                    }
                    hmi.showPage("netscan");
                    hmi.waitForPageRespon();
                    showNetworkList(numberOfNetworkFound[selectedScan]);
                }
            }
        }
    }
}

void Network::showNetworkList(uint8_t noOfList)
{
    if (noOfList > 10)
        return;
    for (uint8_t i = 0; i < noOfList; i++)
    {
        hmi.setVisObjectNextion(String() + "b" + i, true);
    }
    delay(500);
}

void Network::showSelectedNetworkIndicator(uint8_t selected, uint8_t noOfList)
{
    if (noOfList > 10)
        return;
    hmi.setIntegerToNextion(String() + "b" + selected + ".picc", 12);
    for (uint8_t i = 0; i < noOfList; i++)
    {
        if (i == selected)
            continue;
        else
            hmi.setIntegerToNextion(String() + "b" + i + ".picc", 10);
    }
}

bool Network::isStoredNetwork(String networkName)
{
    if (networkName == String() + data.getSSID())
        return true;
    else
        return false;
}

bool Network::checkConnection(void)
{
    String ssid = data.getSSID();
    String password = data.getPassword();
    uint8_t counter = 0;
    if (ssid == "" || password == "")
    {
        return false;
    }

    if (!data.getNetworkEnable())
    {
        return false;
    }

    if (WiFi.isConnected())
    {
        printDebug("Network connected!");
    }
    else
    {
        printDebug("Network connection failed!");
        WiFi.begin(data.getSSID(), data.getPassword());
        while (!WiFi.isConnected() && counter <= 10)
        {
            delay(100);
            counter++;
        }
        if (WiFi.isConnected())
        {
            printDebug("Network connected!");
        }
    }
    return WiFi.isConnected();
}

bool Network::checkConnection(uint8_t *wifiSignal, bool *wifiConnectionTriggered)
{
    String ssid = data.getSSID();
    String password = data.getPassword();
    if (ssid == "" || password == "")
    {
        return false;
        // data.readAll();
    }

    bool connection = false;

    if (!data.getNetworkEnable())
    {
        *wifiSignal = 0;
        *wifiConnectionTriggered = false;
        return false;
    }

    if (WiFi.isConnected())
    {
        connection = true;
        *wifiSignal = calculateRSSILevel(WiFi.RSSI());
        if (*wifiConnectionTriggered && previousConnectionState != true)
        {
            previousConnectionState = true;
            printDebug("Network connected!");
        }
    }
    else
    {
        *wifiSignal = 0;
        connection = false;
        if (*wifiConnectionTriggered && previousConnectionState != false)
        {
            previousConnectionState = false;
            printDebug("Network connection failed!");
        }
    }
    if (*wifiConnectionTriggered)
    {
        if (!connection)
        {
            WiFi.begin(data.getSSID(), data.getPassword());
        }
        *wifiConnectionTriggered = false;
    }
    return connection;
}
uint8_t Network::calculateRSSILevel(int value)
{
    if (value >= -50)
        return 100;
    else if (value >= -60)
        return 85;
    else if (value >= -70)
        return 65;
    else if (value >= -80)
        return 45;
    else if (value >= -95)
        return 25;
    else
        return 0;
}

Network net;