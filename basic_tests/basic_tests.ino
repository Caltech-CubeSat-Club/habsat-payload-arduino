
#include <RadioLib.h>

// RF69 has the following connections:
// CS pin:    9
// DIO0 pin:  2
// RESET pin: 3
RF69 radio = new Module(9, 2, 3);


int transmissionState = RADIOLIB_ERR_NONE;

void setup() {
  Serial.begin(9600);

  // initialize RF69 with default settings
  Serial.print(F("[RF69] Initializing ... "));
  int state = radio.begin();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }


  radio.setDio0Action(setFlag);
  radio.packetMode(); 
    Serial.print(F("[RF69] Setting high power module ... "));
    state = radio.setOutputPower(20, true);
    if (state == RADIOLIB_ERR_NONE) {
      Serial.println(F("success!"));
    } else {
      Serial.print(F("failed, code "));
      Serial.println(state);
      while (true);
    }

   if (radio.setFrequency(469.20) == RADIOLIB_ERR_INVALID_FREQUENCY) {
    Serial.println(F("[RF69] Selected frequency is invalid for this module!"));
    while (true);
  }

  state = radio.setBitRate(1.2);
  if (state == RADIOLIB_ERR_INVALID_BIT_RATE) {
    Serial.println(F("[RF69] Selected bit rate is invalid for this module!"));
    while (true);
  } else if (state == RADIOLIB_ERR_INVALID_BIT_RATE_BW_RATIO) {
    Serial.println(F("[RF69] Selected bit rate to bandwidth ratio is invalid!"));
    Serial.println(F("[RF69] Increase receiver bandwidth to set this bit rate."));
    while (true);
  }

  uint8_t syncWord[] = {0x93,0x0b,0x51,0xde};
  radio.setSyncWord(syncWord, sizeof(syncWord));

  radio.setEncoding(RADIOLIB_ENCODING_NRZ);

  //radio.setDataShaping(RADIOLIB_SHAPING_0_3);

  //radio.setRxBandwidth(400);

  radio.setFrequencyDeviation(10);

   radio.setPreambleLength(64);





  Serial.print(F("[RF69] Sending first packet ... "));

//  transmissionState = radio.startTransmit("Hello World!");

    byte byteArr[] = {0x01, 0x23, 0x45, 0x67,
                      0x89, 0xAB, 0xCD, 0xEF,0x01, 0x23, 0x45, 0x67,
                      0x89, 0xAB, 0xCD, 0xEF,0x01, 0x23, 0x45, 0x67,
                      0x89, 0xAB, 0xCD, 0xEF,0x01, 0x23, 0x45, 0x67,
                      0x89, 0xAB, 0xCD, 0xEF};
    transmissionState = radio.startTransmit(byteArr, 32);
  
}

volatile bool transmittedFlag = false;
volatile bool enableInterrupt = true;

void setFlag(void) {
  if(!enableInterrupt) {
    return;
  }
  transmittedFlag = true;
  radio.standby();
}

void loop() {
  
 
  if(transmittedFlag) {
    enableInterrupt = false;
    transmittedFlag = false;
    if (transmissionState == RADIOLIB_ERR_NONE) {
      Serial.println(F("transmission finished!"));
    } else {
      Serial.print(F("failed, code "));
      Serial.println(transmissionState);
    }

    
    delay(3000);

    
    Serial.print(F("[RF69] Sending another packet ... "));

    //transmissionState = radio.startTransmit("Hello World!");


    byte byteArr[] = {0x01, 0x23, 0x45, 0x67,
                      0x89, 0xAB, 0xCD, 0xEF,0x01, 0x23, 0x45, 0x67,
                      0x89, 0xAB, 0xCD, 0xEF,0x01, 0x23, 0x45, 0x67,
                      0x89, 0xAB, 0xCD, 0xEF,0x01, 0x23, 0x45, 0x67,
                      0x89, 0xAB, 0xCD, 0xEF};
    transmissionState = radio.startTransmit(byteArr, 32);
  
  
    enableInterrupt = true;
  }
}
