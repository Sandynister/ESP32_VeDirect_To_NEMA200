
#include <string>
#include <map>

class VeDirectData
{

public:
    int32_t DeepestDischarge;              // H1 - Depth of the deepest discharge [mAh]
    int32_t LastDischarge;                 // H2 - Depth of the last discharge [mAh]
    int32_t AverageDischarge;              // H3 - Depth of the average discharge [mAh]
    int32_t ChargeCyclesCounts;            // H4 - Number of charge cycles
    int32_t FullDischargeCounts;           // H5 - Number of full discharges
    int32_t MainLifetimeAmpHours;          // H6 - Cumulative Amp Hours drawn [mAh]
    int32_t MainMinVoltage;                // H7 - Minimum main (battery) voltage [mV]
    int32_t MainMaxVoltage;                // H8 - Maximum main (battery) voltage [mV]
    int32_t SecondsSinceLastCharge;        // H9 - Number of seconds since last full charge [sec]
    int32_t AutoSyncCounts;                // H10 - Number of automatic synchronizations
    int32_t MainLowVoltageAlarmCounts;     // H11 - Number of low main voltage alarms
    int32_t MainHighVoltageAlarmCounts;    // H12 - Number of high main voltage alarms
    int32_t StarterLowVoltageCounts;       // H13 - Number of low auxiliary voltage alarms
    int32_t StarterHighVoltageAlarmCounts; // H14 - Number of high auxiliary voltage alarms
    int32_t StarterMinVoltage;             // H15 - Minimum auxiliary (battery) voltage [mV]
    int32_t StarterMaxVoltage;             // H16 - Maximum auxiliary (battery) voltage [mV]

    int32_t MainBatteryVoltage;    // V - Main or channel 1 (battery) voltage [mV]
    int32_t StarterBatteryVoltage; // VS - Auxiliary (starter) voltage [mV]
    int32_t MainCurrent;           // I - Main or channel 1 battery current [mA]
    int32_t ConsumedAmpHours;      // CE - Consumed Amp Hours [mAh]
    int32_t StateOfCharge;         // SOC - State-of-charge [promille]
    int32_t TimeToGo;              // TTG - Time-to-go [minutes]

    bool Alarm;      // ALARM - Alarm condition active
    bool RelayState; // RELAY - Relay state

    String AlarmReason;     // AR - Alarm reason
    String BmvModel;        // BMV - Model description (deprecated)
    String FirmwareVersion; // FW - Firmware version (16 bit)

    void mapToField(const String, const String);
};