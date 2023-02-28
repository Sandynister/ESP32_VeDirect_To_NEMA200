#include <Arduino.h>
#include <WiFi.h>
#include <HardwareSerial.h>
#include "VeDirectFrameHandler.hpp"
#include <NMEA2000_CAN.h> // This will automatically choose right CAN library and create suitable NMEA2000 object
#include "N2kMessages.h"

//#define ESP32_CAN_TX_PIN GPIO_NUM_5 //GPIO_NUM_16 // If you use ESP32 and do not have TX on default IO 16, uncomment this and and modify definition to match your CAN TX pin.
//#define ESP32_CAN_RX_PIN GPIO_NUM_4 //GPIO_NUM_17 // If you use ESP32 and do not have RX on default IO 4, uncomment this and and modify definition to match your CAN TX pin.

const unsigned long TransmitMessages[] PROGMEM = {127506L, 127508L, 127513L, 0};

// -- -Example of using PROGMEM to hold Product ID.However, doing this will prevent any updating of
//      these details outside of recompiling the program.
const tNMEA2000::tProductInformation BatteryMonitorProductInformation PROGMEM = {
    2100,                              // N2kVersion
    100,                               // Manufacturer's product code
    "Victron BMV 6xx battery monitor", // Manufacturer's Model ID
    "0.1.0.0 (2023-02-01)",            // Manufacturer's Software version code
    "0.1.0.0 (2023-02-01)",            // Manufacturer's Model version
    "00000001",                        // Manufacturer's Model serial code
    0,                                 // SertificationLevel
    1                                  // LoadEquivalency
};

// ---  Example of using PROGMEM to hold Configuration information.  However, doing this will prevent any updating of
//      these details outside of recompiling the program.
const char BatteryMonitorManufacturerInformation[] PROGMEM = "sandynister";
const char BatteryMonitorInstallationDescription1[] PROGMEM = "ESP32_VeDirect_To_NEMA200";
const char BatteryMonitorInstallationDescription2[] PROGMEM = "Sending Ve.Direct-Informations to bus";

// Define schedulers for messages. Define schedulers here disabled. Schedulers will be enabled
// on OnN2kOpen so they will be synchronized with system.
// We use own scheduler for each message so that each can have different offset and period.
// Setup periods according PGN definition (see comments on IsDefaultSingleFrameMessage and
// IsDefaultFastPacketMessage) and message first start offsets. Use a bit different offset for
// each message so they will not be sent at same time.
tN2kSyncScheduler DCBatStatusScheduler(false, 1000, 500);
tN2kSyncScheduler DCStatusScheduler(false, 1000, 510);
// tN2kSyncScheduler BatConfScheduler(false, 5000, 520); // Non periodic

HardwareSerial SerialPort1(1);
VeDirectFrameHandler VeDirect;
ulong _dcBatFrameCount = 0;
ulong _dcStatusFrameCount = 0;

void onReceiveBMVData(void)
{
  while (0 < SerialPort1.available())
  {
    int byte = SerialPort1.read();
    if (byte >= 0)
    {
      VeDirect.RxData(byte);
    }
  }
}
// *****************************************************************************
// Call back for NMEA2000 open. This will be called, when library starts bus communication.
// See NMEA2000.SetOnOpen(OnN2kOpen); on setup()
void OnN2kOpen()
{
  // Start schedulers now.
  DCBatStatusScheduler.UpdateNextTime();
  DCStatusScheduler.UpdateNextTime();
  // BatConfScheduler.UpdateNextTime();
}

void setupNMEA2000()
{
  uint8_t chipid[6];
  uint32_t id = 0;
  int i = 0;

  esp_efuse_mac_get_default(chipid);
  for (i = 0; i < 6; i++)
    id += (chipid[i] << (7 * i));

  // Set Product information
  NMEA2000.SetProductInformation(&BatteryMonitorProductInformation);
  // Set Configuration information
  NMEA2000.SetProgmemConfigurationInformation(BatteryMonitorManufacturerInformation, BatteryMonitorInstallationDescription1, BatteryMonitorInstallationDescription2);
  // Set device information
  NMEA2000.SetDeviceInformation(id,   // Unique number. Use e.g. Serial number.
                                170, // Device function=Battery. See codes on https://web.archive.org/web/20190531120557/https://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                35,  // Device class=Electrical Generation. See codes on  https://web.archive.org/web/20190531120557/https://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                2046 // Just choosen free from code list on https://web.archive.org/web/20190529161431/http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
  );

  //NMEA2000.SetForwardStream(&Serial);
  // NMEA2000.SetForwardType(tNMEA2000::fwdt_Text);     // Show in clear text. Leave uncommented for default Actisense format.

  // If you also want to see all traffic on the bus use N2km_ListenAndNode instead of N2km_NodeOnly below
  NMEA2000.SetMode(tNMEA2000::N2km_SendOnly, 25);
  // NMEA2000.SetDebugMode(tNMEA2000::dm_ClearText);     // Uncomment this, so you can test code without CAN bus chips on Arduino Mega
  // NMEA2000.EnableForward(false);                      // Disable all msg forwarding to USB (=Serial)

  //  NMEA2000.SetN2kCANMsgBufSize(2);                    // For this simple example, limit buffer size to 2, since we are only sending data
  // Define OnOpen call back. This will be called, when CAN is open and system starts address claiming.
  NMEA2000.SetOnOpen(OnN2kOpen);
  NMEA2000.Open();

  //Serial.println("nmea200-id: %d", id);
}
int _counter = 0;
void SendN2kBattery() 
{
  tN2kMsg N2kMsg;
  long currentFrame = VeDirect.GetFrameCount();
  char msg[100]; 
  
  sprintf(msg, "\r%d current Frame: %d\t", _counter++, currentFrame);
  
  if ( DCBatStatusScheduler.IsTime() && _dcBatFrameCount < currentFrame) 
  {
    sprintf(msg, "%s%s", msg, "-> DCBatStatusScheduler"); 
    VeDirectData data = VeDirect.GetLastVeDirectData();
    DCBatStatusScheduler.UpdateNextTime();
    SetN2kDCBatStatus(N2kMsg, 1, data.MainBatteryVoltage / 1000.0, data.MainCurrent / 1000.0, N2kDoubleNA, currentFrame);
    NMEA2000.SendMsg(N2kMsg);
    _dcBatFrameCount = currentFrame;
  }
  
  if ( DCStatusScheduler.IsTime() && _dcStatusFrameCount < currentFrame) 
  {
    sprintf(msg, "%s%s", msg, "-> DCStatusScheduler");
    DCStatusScheduler.UpdateNextTime();
    SetN2kDCStatus(N2kMsg,1,1,N2kDCt_Battery,56,92,38500,0.012);
    NMEA2000.SendMsg(N2kMsg);
    _dcStatusFrameCount = currentFrame;
  }
  
  sprintf(msg, "%s%s", msg, "                                                                                  ");
  Serial.printf(msg);

/*if ( BatConfScheduler.IsTime() ) {
    BatConfScheduler.UpdateNextTime();
    SetN2kBatConf(N2kMsg,1,N2kDCbt_Gel,N2kDCES_Yes,N2kDCbnv_12v,N2kDCbc_LeadAcid,AhToCoulomb(420),53,1.251,75);
    NMEA2000.SendMsg(N2kMsg);
  }
*/
}

void setup()
{
  Serial.begin(115200);
  SerialPort1.begin(19200, SERIAL_8N1, GPIO_NUM_4, GPIO_NUM_5);
  SerialPort1.onReceive(onReceiveBMVData);

  WiFi.mode(WIFI_OFF);
  btStop();
  setupNMEA2000();
  delay(100);

  Serial.println("");
  Serial.println("++++ Victron BMV - NMEA200 IO ++++");
  Serial.println("starting....");
}

void loop()
{
  SendN2kBattery();
  /*
  long currentFrame = VeDirect.GetFrameCount();

  if (_dcBatFrameCount < currentFrame)
  {
    Serial.printf("\n++++++Frame: %4i +++++++++++++\n", currentFrame);

    for (int i = 0; i < VeDirect.veEnd; i++)
    {
      Serial.printf("%s\t%s\n", VeDirect.veName[i], VeDirect.veValue[i]);
    }
    printf("-----------------------------\n");
    VeDirectData data = VeDirect.GetLastVeDirectData();
    Serial.printf("H1   : %d\n", data.DeepestDischarge);
    Serial.printf("H2   : %d\n", data.LastDischarge);
    Serial.printf("V    : %.2f\n", data.MainBatteryVoltage / 1000.0);
    Serial.printf("VS   : %.2f\n", data.MainBatteryVoltage / 1000.0);
    Serial.printf("ALARM: %d\n", data.Alarm);
    Serial.printf("++++++ endFrame +++++++++++++\n\n");
    _dcBatFrameCount = currentFrame;
  } */

}