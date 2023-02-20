#include <Arduino.h>
#include "VeDirectData.hpp"

/*
VeDirectData::VeDirectData()
{
}
*/
void VeDirectData::mapToField(String name, String value)
{
    if (0 == name.compareTo("H1"))
        DeepestDischarge = value.toInt();
    else if (0 == name.compareTo("H2"))
        LastDischarge = value.toInt();
    else if (0 == name.compareTo("H3"))
        AverageDischarge = value.toInt();
    else if (0 == name.compareTo("H4"))
        ChargeCyclesCounts = value.toInt();
    else if (0 == name.compareTo("H5"))
        FullDischargeCounts = value.toInt();
    else if (0 == name.compareTo("H6"))
        MainLifetimeAmpHours = value.toInt();
    else if (0 == name.compareTo("H7"))
        MainMinVoltage = value.toInt();
    else if (0 == name.compareTo("H8"))
        MainMaxVoltage = value.toInt();
    else if (0 == name.compareTo("H9"))
        SecondsSinceLastCharge = value.toInt();
    else if (0 == name.compareTo("H10"))
        AutoSyncCounts = value.toInt();
    else if (0 == name.compareTo("H11"))
        MainLowVoltageAlarmCounts = value.toInt();
    else if (0 == name.compareTo("H12"))
        MainHighVoltageAlarmCounts = value.toInt();
    else if (0 == name.compareTo("H13"))
        StarterLowVoltageCounts = value.toInt();
    else if (0 == name.compareTo("H14"))
        StarterHighVoltageAlarmCounts = value.toInt();
    else if (0 == name.compareTo("H15"))
        StarterMinVoltage = value.toInt();
    else if (0 == name.compareTo("H16"))
        StarterMaxVoltage = value.toInt();

    else if (0 == name.compareTo("V"))
        MainBatteryVoltage = value.toInt();
    else if (0 == name.compareTo("VS"))
        StarterBatteryVoltage = value.toInt();

    else if (0 == name.compareTo("I"))
        MainCurrent = value.toInt();
    else if (0 == name.compareTo("CE"))
        ConsumedAmpHours = value.toInt();
    else if (0 == name.compareTo("SOC"))
        StateOfCharge = value.toInt();
    else if (0 == name.compareTo("TIG"))
        TimeToGo = value.toInt();

    else if (0 == name.compareTo("ALARM")) // is't in "ON" ??
        Alarm = value == "OFF" ? false : true;
    else if (0 == name.compareTo("RelayState")) // Whats "ON" ?? 
        Alarm = value == "OFF" ? false : true;

    else if (0 == name.compareTo("AR"))
        AlarmReason = value;
    else if (0 == name.compareTo("BMV"))
        BmvModel = value;
    else if (0 == name.compareTo("FW"))
        FirmwareVersion = value;
}