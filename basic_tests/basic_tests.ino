#include <RadioLib.h>
#include <Adafruit_GPS.h>

Adafruit_GPS GPS(&Wire);

RF69 radio = new Module(9, 2, 3);
int transmissionState = RADIOLIB_ERR_NONE;
volatile bool transmittedFlag = false;
volatile bool enableInterrupt = false;
void setFlag(void) {
  if(!enableInterrupt) {
    return;
  }
  transmittedFlag = true;
  radio.standby();
}

struct GPSdata {
  uint8_t fixquality;
  nmea_float_t latitudeDegrees;
  nmea_float_t longitudeDegrees;
  nmea_float_t altitude;
  uint8_t satellites;
};
struct SensorData {
  GPSdata gps;
  String temp;
};
SensorData sensordata;

GPSdata updateGPS() {
  char c = GPS.read(); // keep reading from the i2c buffer
  GPSdata data = {0, 0., 0., 0.};
  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
    
      data.fixquality = GPS.fixquality;
      data.latitudeDegrees = GPS.latitudeDegrees;
      data.longitudeDegrees = GPS.longitudeDegrees;
      data.altitude = GPS.altitude;
      data.satellites = GPS.satellites;
  }
  if (data.fixquality) {
    return data
  }
}

void setup() {
  Serial.begin(9600);

  GPS.begin(0x10); //I2C address
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  GPS.sendCommand(PGCMD_ANTENNA);

  radio.begin();
  radio.setDio0Action(setFlag);
  radio.packetMode();
  radio.setOutputPower(20, true);
  radio.setFrequency(469.20);
  radio.setBitRate(1.2);
  uint8_t syncWord[] = {0x93,0x0b,0x51,0xde};
  radio.setSyncWord(syncWord, sizeof(syncWord));
  radio.setEncoding(RADIOLIB_ENCODING_NRZ);
  //radio.setDataShaping(RADIOLIB_SHAPING_0_3);
  //radio.setRxBandwidth(400);
  radio.setFrequencyDeviation(10);
  radio.setPreambleLength(64); 
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
    Serial.print(F("[RF69] Sending packet ... "));
    transmissionState = radio.startTransmit("header" + GPS_data);
    enableInterrupt = true;
  }

  //byte byteArr[] = {0x01, 0x23, 0x45, 0x67,
    //                0x89, 0xAB, 0xCD, 0xEF,0x01, 0x23, 0x45, 0x67,
      //              0x89, 0xAB, 0xCD, 0xEF,0x01, 0x23, 0x45, 0x67,
        //            0x89, 0xAB, 0xCD, 0xEF,0x01, 0x23, 0x45, 0x67,
          //          0x89, 0xAB, 0xCD, 0xEF};
  //transmissionState = radio.startTransmit(byteArr, 32);
    delay(3000);
}