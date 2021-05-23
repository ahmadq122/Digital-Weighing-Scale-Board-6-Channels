#include "StateMachine/StateMachine.h"
// #include "Nextion/Nextion.h"
#include "FlashMemory/FlashMemory.h"

StateMachine state;

void setup()
{
  // put your setup code here, to run once:
  state.setup();
}

void loop()
{
  // put your main code here, to run repeatedly:

  uint8_t nextState = HOME;

  // MemoryFlash d;
  
  // Serial.println(String() + "uint8_t:" + sizeof(uint8_t));
  // Serial.println(String() + "uint16_t:" + sizeof(uint16_t));
  // Serial.println(String() + "uShort:" + sizeof(unsigned short));
  // Serial.println(String() + "uint32_t:" + sizeof(uint32_t));
  // Serial.println(String() + "uint:" + sizeof(unsigned int));
  // Serial.println(String() + "uLong:" + sizeof(unsigned long));
  // Serial.println(String() + "float:" + sizeof(float));
  // Serial.println(String() + "double:" + sizeof(double));
  // Serial.println(String() + "uint64_t:" + sizeof(uint64_t));
  // Serial.end();
  // Serial.begin(115200);
  // delay(1000);
  // uint8_t sizeLeft = MEMORY_SIZE - sizeof(d.flash);
  // uint16_t size = sizeof(d.flash);
  // Serial.println(size);
  // Serial.println(String() + "Free memory: " + sizeLeft + " bytes");

  while (true)
  {
    nextState = state.homeScreen();

    switch (nextState)
    {
    case NETWORK:
      state.networkSettings();
      break;
    case SETTINGS:
      state.settings();
      break;
    case UNITS:
      state.measurementUnits();
      break;
    case DATALOG:
      state.datalogSettings();
      break;
    default:
      break;
    }
  }
}
