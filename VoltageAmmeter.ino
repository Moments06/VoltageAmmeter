#include <Arduino.h>
#include <Wire.h>
#include "INA226.h"
#include "Ticker.h"
#include "Adafruit_ssd1306syp.h"

#define MaxCurrent 8
#define SamplingResistor 0.05
#define SDA_PIN PA_1
#define SCL_PIN PA_0

void getINA226Data();
void updateOled();
void ChargeIntegration();

double dt = 0.003;
double electricChargeSum = 0;

Ticker getDataTicker(getINA226Data, 250, 0);
Ticker updateOledTicker(updateOled, 60, 0);
Ticker integrationTicker(ChargeIntegration,3,0);
Adafruit_ssd1306syp display(SDA_PIN, SCL_PIN);
INA226 INA(0x40);

float INA_BusVoltage;
float INA_Current_mA;
float INA_Current_A;
float INA_Power;
float INA_Power_mW;

void setup()
{
    Serial.begin(115200);
    display.initialize();
    display.clear();
    display.update();
    Wire.begin();
    if (!INA.begin())
    {
        Serial.println("ERROR:could not connect");
    }
    else
    {
        Serial.println("INA226 is ready!");
    }
    INA.setMaxCurrentShunt(MaxCurrent, SamplingResistor);
    getDataTicker.start();
    updateOledTicker.start();
    integrationTicker.start();
}

void loop()
{
    getDataTicker.update();
    updateOledTicker.update();
    integrationTicker.update();
}

void getINA226Data()
{
    Serial.println("update INA226 data");
    INA_BusVoltage = INA.getBusVoltage() * 2;
    INA_Current_mA = INA.getShuntVoltage_mV() / SamplingResistor;
    INA_Current_A = INA_Current_mA / 1000;
    INA_Power = INA_Current_A * INA_BusVoltage;
    INA_Power_mW = INA_Current_mA * INA_BusVoltage;
}

void updateOled()
{
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.drawRect(0, 0, 128, 64, WHITE);
    display.drawRoundRect(8, 5, 112, 10, 3, WHITE);
    display.setCursor(23, 6);
    display.println("VoltageAmmeter");
    display.setCursor(8, 17);
    display.print("Voltage:");
    display.print(INA_BusVoltage);
    display.print(" V");
    display.setCursor(8, 26);
    display.print("Current:");
    if (INA_Current_mA < 1000)
    {
        display.print(INA_Current_mA);
        display.print(" mA");
    }
    else
    {
        display.print(INA_Current_A);
        display.print(" A");
    }
    display.setCursor(8, 34);
    display.print("Power:  ");
    if (INA_Power_mW < 1000)
    {
        display.print(INA_Power_mW);
        display.print(" mW");
    }
    else
    {
        display.print(INA_Power);
        display.print(" W");
    }
    display.setCursor(8, 41);
    display.print("Charge: ");
    display.print(electricChargeSum);
    display.print("C");
    display.update();
    display.clear();
}

void ChargeIntegration(){
    electricChargeSum += INA_Current_A * dt;
}