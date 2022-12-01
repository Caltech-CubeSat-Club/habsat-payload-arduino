
#include <RadioLib.h>
#include <Adafruit_GPS.h>

// Connect to the GPS on the hardware I2C port
Adafruit_GPS GPS(&Wire);

// RF69 has the following connections:
// CS pin:    9
// DIO0 pin:  2
// RESET pin: 3

RF69 radio = new Module(9, 2, 3);


int transmissionState = RADIOLIB_ERR_NONE;

void setup() {
  Serial.begin(9600);

  GPS.begin(0x10); //I2C address
  // initialize RF69 with default settings

  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);

  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate

  GPS.sendCommand(PGCMD_ANTENNA);
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

  transmissionState = radio.startTransmit("Hello World!");
    //altitude
    //gps
    //acceleration
    //

    // byte byteArr[] = {0x01, 0x23, 0x45, 0x67,
    //                   0x89, 0xAB, 0xCD, 0xEF,0x01, 0x23, 0x45, 0x67,
    //                   0x89, 0xAB, 0xCD, 0xEF,0x01, 0x23, 0x45, 0x67,
    //                   0x89, 0xAB, 0xCD, 0xEF,0x01, 0x23, 0x45, 0x67,
    //                   0x89, 0xAB, 0xCD, 0xEF};
    // transmissionState = radio.startTransmit(byteArr, 32);
  
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
  
 String GPS_data = "";
    char c = GPS.read();
    if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trying to print out data
    
    Serial.println("nmea recieved");
    String GPS_data = GPS.lastNMEA(); // this also sets the newNMEAreceived() flag to false
    Serial.println(GPS_data);
   
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another

    
    if (GPS.fix) {
      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("Angle: "); Serial.println(GPS.angle);
      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
    }
    else {
      Serial.print("No GPS fix ):");
    }
    
  if(transmittedFlag) {
    enableInterrupt = false;
    transmittedFlag = false;
    if (transmissionState == RADIOLIB_ERR_NONE) {
      Serial.println(F("transmission finished!"));
    } else {
      Serial.print(F("failed, code "));
      Serial.println(transmissionState);
    }

    


    }
   
    


    Serial.print(F("[RF69] Sending another packet ... "));
    transmissionState = radio.startTransmit("header" + GPS_data);
    delay(1000);
    //byte byteArr[] = {0x01, 0x23, 0x45, 0x67,
      //                0x89, 0xAB, 0xCD, 0xEF,0x01, 0x23, 0x45, 0x67,
        //              0x89, 0xAB, 0xCD, 0xEF,0x01, 0x23, 0x45, 0x67,
          //            0x89, 0xAB, 0xCD, 0xEF,0x01, 0x23, 0x45, 0x67,
            //          0x89, 0xAB, 0xCD, 0xEF};
    //transmissionState = radio.startTransmit(byteArr, 32);
  
  
  
  }
}
