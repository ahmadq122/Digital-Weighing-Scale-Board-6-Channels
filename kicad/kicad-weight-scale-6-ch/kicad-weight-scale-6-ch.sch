EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Device:Buzzer BZ1
U 1 1 606D3770
P 1425 3600
F 0 "BZ1" H 1577 3629 50  0000 L CNN
F 1 "Buzzer" H 1577 3538 50  0000 L CNN
F 2 "" V 1400 3700 50  0001 C CNN
F 3 "~" V 1400 3700 50  0001 C CNN
	1    1425 3600
	1    0    0    -1  
$EndComp
$Comp
L DataLoggingBoard:DataLoggingBoard U?
U 1 1 606DBE53
P 3050 3725
F 0 "U?" H 3025 3150 39  0000 C CNN
F 1 "DataLoggingBoard" H 3025 3225 39  0000 C CNN
F 2 "" H 3050 4225 50  0001 C CNN
F 3 "" H 3050 4225 50  0001 C CNN
	1    3050 3725
	1    0    0    -1  
$EndComp
$Comp
L Transistor_FET:Si4542DY Q?
U 1 1 606F125E
P 3075 2450
F 0 "Q?" H 3280 2496 39  0000 L CNN
F 1 "Si4542DY" H 3280 2405 24  0000 L CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 3275 2375 50  0001 L CNN
F 3 "https://www.onsemi.com/pub/Collateral/SI4542DY-D.PDF" H 3175 2450 50  0001 L CNN
	1    3075 2450
	-1   0    0    -1  
$EndComp
$Comp
L Transistor_FET:AO3401A Q?
U 1 1 606F53CC
P 1225 3975
F 0 "Q?" H 1430 4021 50  0000 L CNN
F 1 "AO3401A" H 1430 3930 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 1425 3900 50  0001 L CIN
F 3 "http://www.aosmd.com/pdfs/datasheet/AO3401A.pdf" H 1225 3975 50  0001 L CNN
	1    1225 3975
	1    0    0    -1  
$EndComp
$Comp
L Device:Battery_Cell BT1
U 1 1 60706077
P 1000 1050
F 0 "BT1" H 1025 1025 39  0000 L CNN
F 1 "18650 Li-ion" H 1025 975 39  0000 L CNN
F 2 "" V 1000 1110 50  0001 C CNN
F 3 "~" V 1000 1110 50  0001 C CNN
	1    1000 1050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 60707086
P 725 1000
F 0 "#PWR?" H 725 750 50  0001 C CNN
F 1 "GND" H 730 827 50  0000 C CNN
F 2 "" H 725 1000 50  0001 C CNN
F 3 "" H 725 1000 50  0001 C CNN
	1    725  1000
	1    0    0    -1  
$EndComp
Text GLabel 1000 800  1    39   BiDi ~ 0
VCC_B
Wire Wire Line
	1000 800  1000 850 
Text GLabel 725  850  1    39   Input ~ 0
GND
Wire Wire Line
	725  1000 725  850 
Text GLabel 1000 1200 3    39   Input ~ 0
GND
Text GLabel 2375 1300 3    39   Input ~ 0
GND
Text GLabel 1675 1025 0    39   Output ~ 0
~STDBY
Text GLabel 1675 1225 0    39   Output ~ 0
5V
$Comp
L Switch:SW_Push SW?
U 1 1 60720789
P 2875 1600
F 0 "SW?" H 2875 1500 39  0000 C CNN
F 1 "SW_Push" H 2875 1550 24  0000 C CNN
F 2 "" H 2875 1800 50  0001 C CNN
F 3 "~" H 2875 1800 50  0001 C CNN
	1    2875 1600
	1    0    0    -1  
$EndComp
Text GLabel 2625 1600 0    39   Input ~ 0
VCC_1
$Comp
L Transistor_FET:Si4542DY Q?
U 2 1 606F2ACF
P 2975 1950
F 0 "Q?" V 2975 2100 39  0000 L CNN
F 1 "Si4542DY" V 2925 2050 24  0000 L CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 3175 1875 50  0001 L CNN
F 3 "https://www.onsemi.com/pub/Collateral/SI4542DY-D.PDF" H 3075 1950 50  0001 L CNN
	2    2975 1950
	0    1    -1   0   
$EndComp
Wire Wire Line
	2675 1600 2625 1600
Wire Wire Line
	2975 2150 2975 2250
Wire Wire Line
	2975 2250 2675 2250
$Comp
L Device:R R?
U 1 1 607362F6
P 2675 2050
F 0 "R?" H 2525 2050 39  0000 L CNN
F 1 "10k" H 2525 2000 24  0000 L CNN
F 2 "" V 2605 2050 50  0001 C CNN
F 3 "~" H 2675 2050 50  0001 C CNN
	1    2675 2050
	1    0    0    -1  
$EndComp
Wire Wire Line
	2675 1600 2675 1850
Wire Wire Line
	2775 1850 2675 1850
Connection ~ 2675 1850
Wire Wire Line
	2675 1850 2675 1900
Wire Wire Line
	2675 2200 2675 2250
Connection ~ 2975 2250
Text GLabel 2975 2800 3    39   Input ~ 0
GND
Wire Wire Line
	2975 2650 2975 2750
$Comp
L Device:R R?
U 1 1 60743EF1
P 3275 2600
F 0 "R?" H 3125 2600 39  0000 L CNN
F 1 "1M" H 3125 2550 24  0000 L CNN
F 2 "" V 3205 2600 50  0001 C CNN
F 3 "~" H 3275 2600 50  0001 C CNN
	1    3275 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	3275 2750 2975 2750
Connection ~ 2975 2750
Wire Wire Line
	2975 2750 2975 2800
Wire Wire Line
	3400 2450 3275 2450
Connection ~ 3275 2450
Text GLabel 3400 2450 2    39   Output ~ 0
VCC_CNTRL
Text GLabel 850  2125 0    39   Output ~ 0
VCC_B1
Wire Wire Line
	900  2125 850  2125
Text GLabel 800  2425 0    39   Output ~ 0
5V
Wire Wire Line
	1100 2425 1100 2375
$Comp
L Device:R R?
U 1 1 6071CDB3
P 1100 2625
F 0 "R?" H 950 2625 39  0000 L CNN
F 1 "1M" H 950 2575 24  0000 L CNN
F 2 "" V 1030 2625 50  0001 C CNN
F 3 "~" H 1100 2625 50  0001 C CNN
	1    1100 2625
	-1   0    0    1   
$EndComp
Text GLabel 1100 2825 3    39   Input ~ 0
GND
$Comp
L Transistor_FET:Si4542DY Q?
U 2 1 60717610
P 1100 2225
F 0 "Q?" V 1100 2375 39  0000 L CNN
F 1 "Si4542DY" V 1050 2325 24  0000 L CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 1300 2150 50  0001 L CNN
F 3 "https://www.onsemi.com/pub/Collateral/SI4542DY-D.PDF" H 1200 2225 50  0001 L CNN
	2    1100 2225
	0    1    -1   0   
$EndComp
Wire Wire Line
	800  2425 1100 2425
Connection ~ 1100 2425
Wire Wire Line
	1100 2475 1100 2425
Text GLabel 3725 1600 2    39   Output ~ 0
VCC_2
Wire Wire Line
	3675 1850 3675 1600
Wire Wire Line
	3725 1600 3675 1600
$Comp
L Device:D_Schottky D?
U 1 1 60756907
P 1650 2125
F 0 "D?" H 1650 1975 39  0000 C CNN
F 1 "D_Schottky" H 1650 2025 24  0000 C CNN
F 2 "" H 1650 2125 50  0001 C CNN
F 3 "~" H 1650 2125 50  0001 C CNN
	1    1650 2125
	-1   0    0    1   
$EndComp
$Comp
L Device:D_Schottky D?
U 1 1 60757499
P 1650 2425
F 0 "D?" H 1650 2275 39  0000 C CNN
F 1 "D_Schottky" H 1650 2325 24  0000 C CNN
F 2 "" H 1650 2425 50  0001 C CNN
F 3 "~" H 1650 2425 50  0001 C CNN
	1    1650 2425
	-1   0    0    1   
$EndComp
$Comp
L Device:D_Schottky D?
U 1 1 60757886
P 3525 1600
F 0 "D?" H 3525 1450 39  0000 C CNN
F 1 "D_Schottky" H 3525 1500 24  0000 C CNN
F 2 "" H 3525 1600 50  0001 C CNN
F 3 "~" H 3525 1600 50  0001 C CNN
	1    3525 1600
	-1   0    0    1   
$EndComp
Connection ~ 3675 1600
$Comp
L Device:R R?
U 1 1 607627EF
P 1400 2625
F 0 "R?" H 1250 2625 39  0000 L CNN
F 1 "1M" H 1250 2575 24  0000 L CNN
F 2 "" V 1330 2625 50  0001 C CNN
F 3 "~" H 1400 2625 50  0001 C CNN
	1    1400 2625
	-1   0    0    1   
$EndComp
Text GLabel 1400 2825 3    39   Input ~ 0
GND
Connection ~ 2675 1600
Wire Wire Line
	3175 1850 3675 1850
$Comp
L Device:CP C?
U 1 1 6076BDF2
P 1850 2625
F 0 "C?" H 1968 2650 39  0000 L CNN
F 1 "220u" H 1968 2587 24  0000 L CNN
F 2 "" H 1888 2475 50  0001 C CNN
F 3 "~" H 1850 2625 50  0001 C CNN
	1    1850 2625
	1    0    0    -1  
$EndComp
Wire Wire Line
	1800 2125 1850 2125
Wire Wire Line
	1800 2425 1850 2425
Text GLabel 1975 2250 2    39   Output ~ 0
VCC_1
$Comp
L Step~Up~DC~to~5V~USB:StepUpDC5V U?
U 1 1 606FC509
P 3150 950
F 0 "U?" H 3100 725 39  0000 L CNN
F 1 "StepUpDC5V" H 3025 775 24  0000 L CNN
F 2 "" H 3110 970 50  0001 C CNN
F 3 "" H 3110 970 50  0001 C CNN
	1    3150 950 
	1    0    0    1   
$EndComp
Wire Wire Line
	1850 2475 1850 2425
Connection ~ 1850 2425
Wire Wire Line
	1400 2825 1400 2775
Wire Wire Line
	1100 2825 1100 2775
Text GLabel 1850 2825 3    39   Input ~ 0
GND
Wire Wire Line
	1850 2825 1850 2775
Wire Wire Line
	1300 2125 1400 2125
Wire Wire Line
	1400 2125 1400 2475
Wire Wire Line
	1400 2125 1500 2125
Wire Wire Line
	1500 2425 1100 2425
$Comp
L Device:R R?
U 1 1 60787FE0
P 3325 2100
F 0 "R?" H 3175 2100 39  0000 L CNN
F 1 "1k" H 3175 2050 24  0000 L CNN
F 2 "" V 3255 2100 50  0001 C CNN
F 3 "~" H 3325 2100 50  0001 C CNN
	1    3325 2100
	-1   0    0    1   
$EndComp
Wire Wire Line
	3325 1950 3325 1600
$Comp
L Device:R R?
U 1 1 6078AB48
P 3525 2100
F 0 "R?" H 3375 2100 39  0000 L CNN
F 1 "1M" H 3375 2050 24  0000 L CNN
F 2 "" V 3455 2100 50  0001 C CNN
F 3 "~" H 3525 2100 50  0001 C CNN
	1    3525 2100
	-1   0    0    1   
$EndComp
Wire Wire Line
	3525 2250 3325 2250
Text GLabel 3725 1950 2    39   Input ~ 0
GND
Wire Wire Line
	3075 1600 3325 1600
Connection ~ 3325 1600
Wire Wire Line
	3325 1600 3375 1600
Wire Wire Line
	3725 1950 3525 1950
Text GLabel 3725 2250 2    39   Output ~ 0
VCC_SENSE
Wire Wire Line
	3725 2250 3525 2250
Connection ~ 3525 2250
Wire Wire Line
	1000 1150 1000 1200
Wire Wire Line
	1850 2125 1850 2250
Wire Wire Line
	1975 2250 1850 2250
Connection ~ 1850 2250
Wire Wire Line
	1850 2250 1850 2425
Text GLabel 3825 775  1    39   Output ~ 0
VCC
Text GLabel 2725 850  0    39   Output ~ 0
VCC_2
Wire Wire Line
	2725 850  2800 850 
Wire Wire Line
	3825 775  3825 850 
Wire Wire Line
	3825 850  3500 850 
Text GLabel 2700 1075 3    39   Input ~ 0
GND
Wire Wire Line
	2700 1075 2700 1050
Wire Wire Line
	2700 1050 2800 1050
$Comp
L ESP~WROOM32~DEV-BOARD:ESPWROOM32 U?
U 1 1 6081619D
P 7400 4275
F 0 "U?" H 7400 5574 50  0000 C CNN
F 1 "ESPWROOM32" H 7400 5491 39  0000 C CNN
F 2 "" H 7400 3525 50  0001 C CNN
F 3 "" H 7400 3525 50  0001 C CNN
	1    7400 4275
	1    0    0    -1  
$EndComp
$Comp
L WCMCU-ADS1232:WCMCU-ADS1232 U?
U 1 1 6082A8C3
P 5250 1400
F 0 "U?" H 5250 2117 50  0000 C CNN
F 1 "WCMCU-ADS1232" H 5250 2026 50  0000 C CNN
F 2 "" H 5250 1000 50  0001 C CIN
F 3 "http://www.ti.com/lit/ds/symlink/ads1232.pdf" H 5250 1000 50  0001 C CNN
	1    5250 1400
	1    0    0    -1  
$EndComp
Text GLabel 4550 1000 0    39   Input ~ 0
VCC
Text GLabel 4550 1100 0    39   Input ~ 0
VCC
Text GLabel 4550 1200 0    39   Input ~ 0
GND
Text GLabel 4550 1300 0    39   Input ~ 0
~PWDN
Text GLabel 4550 1400 0    39   Input ~ 0
DATA_1
Text GLabel 4550 1500 0    39   Input ~ 0
SCLK_1
Text GLabel 4550 1600 0    39   Input ~ 0
VCC
Text GLabel 4550 1700 0    39   Input ~ 0
GND
Text GLabel 4550 1800 0    39   Input ~ 0
VCC
Text GLabel 6750 3325 0    39   Output ~ 0
3V3
Text GLabel 1675 925  0    39   Output ~ 0
~CHRG
Text GLabel 1675 800  0    39   Output ~ 0
VCC_B1
Text GLabel 1675 700  0    39   BiDi ~ 0
VCC_B
Connection ~ 1400 2125
Text GLabel 5950 1800 2    39   Input ~ 0
3V3
Text GLabel 5950 1700 2    39   Input ~ 0
GND
Text GLabel 5950 1600 2    39   Input ~ 0
GND
Text GLabel 5950 1500 2    39   Input ~ 0
A0_1
Text GLabel 5950 1300 2    39   Input ~ 0
AINN1_1
Text GLabel 5950 1200 2    39   Input ~ 0
AINP1_1
Text GLabel 5950 1100 2    39   Input ~ 0
AINN2_1
Text GLabel 5950 1000 2    39   Input ~ 0
AINP2_1
Text GLabel 5950 1400 2    39   Input ~ 0
CLKIN_1
$Comp
L WCMCU-ADS1232:WCMCU-ADS1232 U?
U 1 1 60853325
P 7400 1400
F 0 "U?" H 7400 2117 50  0000 C CNN
F 1 "WCMCU-ADS1232" H 7400 2026 50  0000 C CNN
F 2 "" H 7400 1000 50  0001 C CIN
F 3 "http://www.ti.com/lit/ds/symlink/ads1232.pdf" H 7400 1000 50  0001 C CNN
	1    7400 1400
	1    0    0    -1  
$EndComp
Text GLabel 6700 1000 0    39   Input ~ 0
VCC
Text GLabel 6700 1100 0    39   Input ~ 0
VCC
Text GLabel 6700 1200 0    39   Input ~ 0
GND
Text GLabel 6700 1300 0    39   Input ~ 0
~PWDN
Text GLabel 6700 1400 0    39   Input ~ 0
DATA_2
Text GLabel 6700 1500 0    39   Input ~ 0
SCLK_1
Text GLabel 6700 1600 0    39   Input ~ 0
VCC
Text GLabel 6700 1700 0    39   Input ~ 0
GND
Text GLabel 6700 1800 0    39   Input ~ 0
VCC
Text GLabel 8100 1800 2    39   Input ~ 0
3V3
Text GLabel 8100 1700 2    39   Input ~ 0
GND
Text GLabel 8100 1600 2    39   Input ~ 0
GND
Text GLabel 8100 1500 2    39   Input ~ 0
A0_2
Text GLabel 8100 1300 2    39   Input ~ 0
AINN1_2
Text GLabel 8100 1200 2    39   Input ~ 0
AINP1_2
Text GLabel 8100 1100 2    39   Input ~ 0
AINN2_2
Text GLabel 8100 1000 2    39   Input ~ 0
AINP2_2
Text GLabel 8100 1400 2    39   Input ~ 0
CLKIN_2
$Comp
L WCMCU-ADS1232:WCMCU-ADS1232 U?
U 1 1 6085691D
P 9550 1400
F 0 "U?" H 9550 2117 50  0000 C CNN
F 1 "WCMCU-ADS1232" H 9550 2026 50  0000 C CNN
F 2 "" H 9550 1000 50  0001 C CIN
F 3 "http://www.ti.com/lit/ds/symlink/ads1232.pdf" H 9550 1000 50  0001 C CNN
	1    9550 1400
	1    0    0    -1  
$EndComp
Text GLabel 8850 1000 0    39   Input ~ 0
VCC
Text GLabel 8850 1100 0    39   Input ~ 0
VCC
Text GLabel 8850 1200 0    39   Input ~ 0
GND
Text GLabel 8850 1300 0    39   Input ~ 0
~PWDN
Text GLabel 8850 1400 0    39   Input ~ 0
DATA_3
Text GLabel 8850 1500 0    39   Input ~ 0
SCLK_3
Text GLabel 8850 1600 0    39   Input ~ 0
VCC
Text GLabel 8850 1700 0    39   Input ~ 0
GND
Text GLabel 8850 1800 0    39   Input ~ 0
VCC
Text GLabel 10250 1800 2    39   Input ~ 0
3V3
Text GLabel 10250 1700 2    39   Input ~ 0
GND
Text GLabel 10250 1600 2    39   Input ~ 0
GND
Text GLabel 10250 1500 2    39   Input ~ 0
A0_3
Text GLabel 10250 1300 2    39   Input ~ 0
AINN1_3
Text GLabel 10250 1200 2    39   Input ~ 0
AINP1_3
Text GLabel 10250 1100 2    39   Input ~ 0
AINN2_3
Text GLabel 10250 1000 2    39   Input ~ 0
AINP2_3
Text GLabel 10250 1400 2    39   Input ~ 0
CLKIN_3
$Comp
L Connector_Generic:Conn_01x02 J?
U 1 1 6085DA90
P 4425 2175
F 0 "J?" V 4425 1925 50  0000 L CNN
F 1 "Conn_01x02" V 4525 1875 50  0000 L CNN
F 2 "" H 4425 2175 50  0001 C CNN
F 3 "~" H 4425 2175 50  0001 C CNN
	1    4425 2175
	0    -1   -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J?
U 1 1 60860DB7
P 6550 2150
F 0 "J?" V 6550 1900 50  0000 L CNN
F 1 "Conn_01x02" V 6650 1875 50  0000 L CNN
F 2 "" H 6550 2150 50  0001 C CNN
F 3 "~" H 6550 2150 50  0001 C CNN
	1    6550 2150
	0    -1   -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J?
U 1 1 608627E9
P 8700 2150
F 0 "J?" V 8700 1900 50  0000 L CNN
F 1 "Conn_01x02" V 8800 1875 50  0000 L CNN
F 2 "" H 8700 2150 50  0001 C CNN
F 3 "~" H 8700 2150 50  0001 C CNN
	1    8700 2150
	0    -1   -1   0   
$EndComp
Text GLabel 4525 2375 3    39   Input ~ 0
GND
Text GLabel 4425 2375 3    39   Input ~ 0
CLKIN_1
Text GLabel 6650 2350 3    39   Input ~ 0
GND
Text GLabel 6550 2350 3    39   Input ~ 0
CLKIN_2
Text GLabel 8800 2350 3    39   Input ~ 0
GND
Text GLabel 8700 2350 3    39   Input ~ 0
CLKIN_3
Text GLabel 6750 4625 0    39   Input ~ 0
GND
Text GLabel 8050 3325 2    39   Input ~ 0
GND
Text GLabel 8050 3925 2    39   Input ~ 0
GND
$Comp
L Device:CP C?
U 1 1 608694FA
P 3825 1000
F 0 "C?" H 3943 1025 39  0000 L CNN
F 1 "100U" H 3943 962 24  0000 L CNN
F 2 "" H 3863 850 50  0001 C CNN
F 3 "~" H 3825 1000 50  0001 C CNN
	1    3825 1000
	1    0    0    -1  
$EndComp
Text GLabel 3825 1200 3    39   Input ~ 0
GND
Wire Wire Line
	3825 1200 3825 1150
Connection ~ 3825 850 
Text GLabel 6750 5125 0    39   Input ~ 0
VCC
$Comp
L Connector_Generic:Conn_01x02 J?
U 1 1 6086DED2
P 6125 3425
F 0 "J?" H 6205 3417 50  0000 L CNN
F 1 "Conn_01x02" H 6205 3326 50  0000 L CNN
F 2 "" H 6125 3425 50  0001 C CNN
F 3 "~" H 6125 3425 50  0001 C CNN
	1    6125 3425
	-1   0    0    1   
$EndComp
Text GLabel 6325 3325 2    39   Input ~ 0
GND
Text GLabel 6325 3425 2    39   Input ~ 0
EN
Text GLabel 6750 3425 0    39   Input ~ 0
EN
$Comp
L TP4056:TP4056 U?
U 1 1 6072938B
P 2025 1225
F 0 "U?" H 2025 1090 39  0000 C CNN
F 1 "TP4056" H 2025 1031 20  0000 C CNN
F 2 "" H 2025 1225 50  0001 C CNN
F 3 "" H 2025 1225 50  0001 C CNN
	1    2025 1225
	1    0    0    -1  
$EndComp
Wire Wire Line
	2375 1300 2375 1225
Connection ~ 2375 1225
Wire Wire Line
	1875 625  1750 625 
Wire Wire Line
	1750 625  1750 800 
Wire Wire Line
	1750 800  1675 800 
Wire Wire Line
	1975 625  1975 600 
Wire Wire Line
	1975 600  1725 600 
Wire Wire Line
	1725 600  1725 700 
Wire Wire Line
	1725 700  1675 700 
Wire Wire Line
	2075 625  2075 600 
Wire Wire Line
	2075 600  2175 600 
Wire Wire Line
	2375 600  2375 1225
Wire Wire Line
	2175 625  2175 600 
Connection ~ 2175 600 
Wire Wire Line
	2175 600  2375 600 
Text GLabel 875  1700 0    39   BiDi ~ 0
VCC_B
$Comp
L Device:R R?
U 1 1 60741981
P 1025 1700
F 0 "R?" V 925 1675 39  0000 L CNN
F 1 "27k" V 1025 1700 24  0000 L CNN
F 2 "" V 955 1700 50  0001 C CNN
F 3 "~" H 1025 1700 50  0001 C CNN
	1    1025 1700
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R?
U 1 1 60742133
P 1425 1700
F 0 "R?" V 1325 1675 39  0000 L CNN
F 1 "100k" V 1425 1675 24  0000 L CNN
F 2 "" V 1355 1700 50  0001 C CNN
F 3 "~" H 1425 1700 50  0001 C CNN
	1    1425 1700
	0    -1   -1   0   
$EndComp
Text GLabel 1575 1700 2    39   Input ~ 0
GND
Wire Wire Line
	1225 1700 1225 1550
Text GLabel 1325 1550 2    39   Output ~ 0
VCC_B_SENSE
Wire Wire Line
	1175 1700 1225 1700
Connection ~ 1225 1700
Wire Wire Line
	1225 1700 1275 1700
Wire Wire Line
	1225 1550 1325 1550
Text GLabel 1325 4325 3    39   Input ~ 0
GND
Text GLabel 850  3975 0    39   Input ~ 0
BUZZ
$Comp
L Device:R R?
U 1 1 60754142
P 950 4125
F 0 "R?" H 800 4125 39  0000 L CNN
F 1 "1M" H 800 4075 24  0000 L CNN
F 2 "" V 880 4125 50  0001 C CNN
F 3 "~" H 950 4125 50  0001 C CNN
	1    950  4125
	-1   0    0    1   
$EndComp
Wire Wire Line
	850  3975 950  3975
Wire Wire Line
	950  3975 1025 3975
Connection ~ 950  3975
Wire Wire Line
	1325 4175 1325 4275
Wire Wire Line
	950  4275 1325 4275
Connection ~ 1325 4275
Wire Wire Line
	1325 4275 1325 4325
Wire Wire Line
	1325 3700 1325 3775
Text GLabel 1325 3400 1    39   Input ~ 0
VCC
Wire Wire Line
	1325 3400 1325 3500
Text GLabel 3450 3425 2    39   Input ~ 0
VCC
Text GLabel 3450 3525 2    39   Input ~ 0
GND
Text GLabel 3450 3625 2    39   BiDi ~ 0
SDA
Text GLabel 3450 3725 2    39   Input ~ 0
SCL
Text GLabel 3450 3825 2    39   Input ~ 0
SCK
Text GLabel 3450 3925 2    39   Output ~ 0
MISO
Text GLabel 3450 4025 2    39   Input ~ 0
MOSI
Text GLabel 3450 4125 2    39   Input ~ 0
GND
Text GLabel 8050 3825 2    39   BiDi ~ 0
SDA
Text GLabel 8050 3525 2    39   Output ~ 0
SCL
Text GLabel 8050 4125 2    39   Output ~ 0
SCK
Text GLabel 8050 4025 2    39   Input ~ 0
MISO
Text GLabel 8050 3425 2    39   Output ~ 0
MOSI
$Comp
L Connector_Generic:Conn_01x04 J?
U 1 1 60774B28
P 4950 2175
F 0 "J?" V 4950 1925 50  0000 R CNN
F 1 "Conn_01x04" V 5050 2325 50  0000 R CNN
F 2 "" H 4950 2175 50  0001 C CNN
F 3 "~" H 4950 2175 50  0001 C CNN
	1    4950 2175
	0    -1   -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x04 J?
U 1 1 60786D12
P 5575 2175
F 0 "J?" V 5575 1925 50  0000 R CNN
F 1 "Conn_01x04" V 5675 2325 50  0000 R CNN
F 2 "" H 5575 2175 50  0001 C CNN
F 3 "~" H 5575 2175 50  0001 C CNN
	1    5575 2175
	0    -1   -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x04 J?
U 1 1 6079006F
P 7100 2150
F 0 "J?" V 7100 1900 50  0000 R CNN
F 1 "Conn_01x04" V 7200 2300 50  0000 R CNN
F 2 "" H 7100 2150 50  0001 C CNN
F 3 "~" H 7100 2150 50  0001 C CNN
	1    7100 2150
	0    -1   -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x04 J?
U 1 1 60790075
P 7725 2150
F 0 "J?" V 7725 1900 50  0000 R CNN
F 1 "Conn_01x04" V 7825 2300 50  0000 R CNN
F 2 "" H 7725 2150 50  0001 C CNN
F 3 "~" H 7725 2150 50  0001 C CNN
	1    7725 2150
	0    -1   -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x04 J?
U 1 1 6079363F
P 9250 2150
F 0 "J?" V 9250 1900 50  0000 R CNN
F 1 "Conn_01x04" V 9350 2300 50  0000 R CNN
F 2 "" H 9250 2150 50  0001 C CNN
F 3 "~" H 9250 2150 50  0001 C CNN
	1    9250 2150
	0    -1   -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x04 J?
U 1 1 60793645
P 9875 2150
F 0 "J?" V 9875 1900 50  0000 R CNN
F 1 "Conn_01x04" V 9975 2300 50  0000 R CNN
F 2 "" H 9875 2150 50  0001 C CNN
F 3 "~" H 9875 2150 50  0001 C CNN
	1    9875 2150
	0    -1   -1   0   
$EndComp
Text GLabel 4850 2375 3    39   Input ~ 0
GND
Text GLabel 4950 2375 3    39   Input ~ 0
VCC
Text GLabel 5050 2375 3    39   Input ~ 0
AINN1_1
Text GLabel 5150 2375 3    39   Input ~ 0
AINP1_1
Text GLabel 5475 2375 3    39   Input ~ 0
GND
Text GLabel 7000 2350 3    39   Input ~ 0
GND
Text GLabel 7625 2350 3    39   Input ~ 0
GND
Text GLabel 9150 2350 3    39   Input ~ 0
GND
Text GLabel 9775 2350 3    39   Input ~ 0
GND
Text GLabel 5575 2375 3    39   Input ~ 0
VCC
Text GLabel 7100 2350 3    39   Input ~ 0
VCC
Text GLabel 7725 2350 3    39   Input ~ 0
VCC
Text GLabel 9250 2350 3    39   Input ~ 0
VCC
Text GLabel 9875 2350 3    39   Input ~ 0
VCC
Text GLabel 5675 2375 3    39   Input ~ 0
AINN2_1
Text GLabel 5775 2375 3    39   Input ~ 0
AINP2_1
Text GLabel 7200 2350 3    39   Input ~ 0
AINN1_2
Text GLabel 7300 2350 3    39   Input ~ 0
AINP1_2
Text GLabel 7825 2350 3    39   Input ~ 0
AINN2_2
Text GLabel 7925 2350 3    39   Input ~ 0
AINP2_2
Text GLabel 9350 2350 3    39   Input ~ 0
AINN1_3
Text GLabel 9450 2350 3    39   Input ~ 0
AINP1_3
Text GLabel 9975 2350 3    39   Input ~ 0
AINN2_3
Text GLabel 10075 2350 3    39   Input ~ 0
AINP2_3
$EndSCHEMATC