#include <Arduino.h>
#include"IODriver.h"
#include <OneWire.h>
#include "DallasTemperature.h"
#include <Wire.h>
#include "display.h"
#include "WebMonitor.h"

#include"profil.h"
#include"stdbool.h"

#define TEST_MODE
#define RUNNING 0
#define FINISHED 1
#define ERROR 2

#define MAX_SENSOR_DIFFERENCE 5

#define ONE_WIRE_BUS OneWirePin
// AILE
#define TEMPERATURE1_AILE 30
#define DURATION1_AILE 8
#define TEMPERATURE2_AILE 40
#define DURATION2_AILE 4
#define TEMPERATURE3_AILE 50
#define DURATION3_AILE 4
#define ACTIVE_RESISTANCE_1_AILE ONE
#define ACTIVE_RESISTANCE_2_AILE ONE
#define ACTIVE_RESISTANCE_3_AILE TWO

//PREPREG
#define TEMPERATURE1_PREPREG 50
#define DURATION1_PREPREG 3
#define TEMPERATURE2_PREPREG 90
#define DURATION2_PREPREG 6
#define TEMPERATURE3_PREPREG 0
#define DURATION3_PREPREG 0
#define ACTIVE_RESISTANCE_1_PREPREG THREE
#define ACTIVE_RESISTANCE_2_PREPREG THREE
#define ACTIVE_RESISTANCE_3_PREPREG NONE


#define HYSTERESIS_UP 0
#define HYSTERESIS_DOWN 1



// ONE WIRE
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// COOKING
cookingProfil currentProfil;
cookingState currentState = STATE_IDLE;
profilName selectedProfilName = CUSTOM;
float measuredTemperature = 0;
float preheatTemperatureStamp = 0;
unsigned long preheatTimeStamp = 0;
uint8_t lastTemperature = 0;
uint8_t targetTemperature = 0;
bool setPointReached = false;

bool SensorErrorOccured = false;

String errorLabel = "";
String currentProfilName = "";
bool errorOccured = false;

int actualProfilDuration = 0;


int actualPhase = 0;
int numberOfPhase = 3;


uint64_t duration1Hour;
uint64_t duration2Hour;
uint64_t duration3Hour;


void resetOneWire()
{
    oneWire.reset();
    Serial.println("Onewire restarted");
    delay(100);
    SensorErrorOccured = true;

}
bool readTemperature()
{
    sensors.requestTemperatures();

    float tempSensor1 = sensors.getTempCByIndex(0);
    float tempSensor2 = sensors.getTempCByIndex(1);
    int sensorsOutputDifference = abs(tempSensor1 - tempSensor2);

        // Serial.println("Sensors temperature difference too high ");
    if(tempSensor1 != DEVICE_DISCONNECTED_C)
    {
        Serial.print("Temperature for the sensor 1 is: ");
        Serial.println(tempSensor1);

    }
    else
    {
        if (!SensorErrorOccured)
            {
                resetOneWire();

                int retries = 3;
                while (retries--)
                 {
                    sensors.requestTemperatures();
                    tempSensor1 = sensors.getTempCByIndex(0);
                    delay(1600);
                    if (tempSensor1 != DEVICE_DISCONNECTED_C) break;
                }
                readTemperature();
            }
        else
            {
                Serial.println("Error: Could not read the sensor 1");
                lcd.clear();
                lcd.setCursor(0, 1);
                lcd.print("SENS1 COM ERROR");
                errorLabel = "Sensor 1 disconnected";
                return false;
            }
    }

    if(tempSensor2 != DEVICE_DISCONNECTED_C)
    {
        Serial.print("Temperature for the sensor 1 is: ");
        Serial.println(tempSensor2);

    }
    else
    {
        if (!SensorErrorOccured)
            {
                resetOneWire();

                int retries = 3;
                while (retries--)
                 {
                    sensors.requestTemperatures();
                    tempSensor1 = sensors.getTempCByIndex(1);
                    delay(1600);
                    if (tempSensor2 != DEVICE_DISCONNECTED_C) break;
                }
                readTemperature();
            }
        else
            {
                Serial.println("Error: Could not read the sensor 1");
                lcd.clear();
                lcd.setCursor(0, 1);
                lcd.print("SENS1 COM ERROR");
                errorLabel = "Sensor 1 disconnected";
                return false;
            }
    }

    if (sensorsOutputDifference > MAX_SENSOR_DIFFERENCE)
        {
            Serial.println("Error: Sensors return different values");
            lcd.clear();
            lcd.setCursor(0, 1);
            lcd.print("SENS DIFF ERROR");
            errorLabel = "Difference between sensors value too important, 1: "
             + String(tempSensor1, 2) + "°C, 2: "
             + String(tempSensor2, 2) + "°C";

            const char* errorLabelChar = errorLabel.c_str();  // Convert to char*
            return false;
        }

    measuredTemperature = (tempSensor1 + tempSensor2)/2;
    Serial.print("Temperature: ");
    Serial.println(measuredTemperature);
    return true;
}




void activateResistance(numberOfResistance numberOfResistance)
{
switch (numberOfResistance)
{
    case NONE:
        stopHeating();
        break;
    case ONE:
        activeOneResistance();
        break;
    case TWO:
        activeTwoResistance();
        break;
    case THREE:
        activeThreeResistance();
        break;
    default:
        stopHeating();
}
}

void sendData()
{
 sendProfilData(currentProfilName,
                targetTemperature,
                measuredTemperature,
                actualPhase,
                numberOfPhase,
                actualProfilDuration,
                currentProfil.phase1.temperature,duration1Hour, currentProfil.phase1.activeResistance,
                currentProfil.phase2.temperature,duration2Hour, currentProfil.phase2.activeResistance,
                currentProfil.phase3.temperature,duration3Hour, currentProfil.phase3.activeResistance,
                setPointReached);
}
void updateWebData()
{
uint32_t two_seconds = 2000;
static unsigned long DataSendingTemperatureStamp = 0;
unsigned long DataTimeElapsed = millis()-DataSendingTemperatureStamp;

   if (DataTimeElapsed >= two_seconds || currentState == STATE_PROFIL_SELECT)
   {
    ReconnectIfWifiDisconnected();
    sendData();
    DataSendingTemperatureStamp = millis();
   }

   if (currentState != STATE_PROFIL_SELECT)
   {
    RssiDisplay();
   }

}



uint8_t processCooking(cookingProfil *self, uint8_t temperature, uint64_t duration, numberOfResistance activeResistance)
{
static bool steadyState = false;
static unsigned long PhaseTimeStamp = 0;
static float phaseTemperatureStamp = 0;
unsigned long timeElapsed;
float temperatureEvolution;

uint8_t DownThreshold = temperature - HYSTERESIS_DOWN;
uint8_t UpThreshold = temperature + HYSTERESIS_UP;

Serial.print("Steady state : ");
Serial.println(steadyState);


if (!readTemperature())
   {
    // send email, sensor error

    return ERROR;

   }



if (measuredTemperature > UpThreshold && !steadyState)
    {
    steadyState = true;
    setPointReached = true;
    stopHeating();
    Serial.println("Steady state reached");
    PhaseTimeStamp = millis();
    phaseTemperatureStamp = measuredTemperature;
    }

if (steadyState)
    {
    unsigned long time_elapsed = millis()-PhaseTimeStamp;
    uint8_t temperature_evolution = measuredTemperature - phaseTemperatureStamp;
    Serial.print("Time elapsed : ");
    Serial.println(timeElapsed);
    if (timeElapsed >= duration)
    {
            steadyState = false;
            return FINISHED;
    }



    if (SensorErrorOccured) // if sensor error occured and mitigated by onewire.rest
   {
    activateResistance(activeResistance);
    SensorErrorOccured = false;
   }
    /* TEMPERATURE REGULATION*/
    if (setPointReached)
    {

        if (measuredTemperature < DownThreshold)
        {
            Serial.println("Min temperature regulation reached, heating on");
            setPointReached = false;
            activateResistance(activeResistance);
            delay(100);

        }
    }
    else
    {
        if (measuredTemperature > UpThreshold )
            {
                Serial.println("Max temperature regulation reached, heating off");
                setPointReached = true;
                stopHeating();
                delay(100);

            }

        }
    }

else
{
    activateResistance(activeResistance);
    setPointReached = false;

}
    Serial.print("Target temperature : ");
    Serial.println(temperature);
    Serial.print("Duration : ");
    Serial.println(duration);

    return RUNNING;
}


void temperatureProgressDisplay()
{
    if (lastTemperature != measuredTemperature)
    {

        lcd.setCursor(0, 1);
        lcd.print("T:");
        lcd.print(measuredTemperature);
        lcd.print("/");
        lcd.print(targetTemperature);
        lcd.print((char)223); // ° symbol
        lcd.print("C");

    }
}



void initTemperatureSensors()
{
    DeviceAddress sensor1, sensor2;
    sensors.getAddress(sensor1, 0);
    sensors.getAddress(sensor2, 1);
     // Set different resolutions at 11bit
    delay(100);
    sensors.setResolution(sensor1, 11);
    sensors.setResolution(sensor2, 11);
    Serial.print("Sensor 1 Resolution: ");
    Serial.println(sensors.getResolution(sensor1));
    Serial.print("Sensor 2 Resolution: ");
    Serial.println(sensors.getResolution(sensor2));
    delay(100);
}

void setup()
{

    init_GPIO();
    initRelay();
    initTemperatureSensors();
    initDisplay();
    #ifndef TEST_MODE
    initWifi();
    #endif
    digitalWrite(Led_pin, LOW);
}


void loop()
{
#ifndef TEST_MODE
    static cookingState last_phase = STATE_IDLE;
    static uint64_t time_count = 0;
    static profilName selectedProfilName = PREPREG;
    static uint8_t cookingState = RUNNING;

    switch (currentState) {
        case STATE_IDLE:
            Serial.println("STATE IDLE");
            resetCookingParameters();
            currentState = STATE_PROFIL_SELECT;
            break;

        case STATE_ERROR:
            stopHeating();
            errorOccured = true;
            sendEmail(errorLabel, currentProfilName, errorOccured);
            Serial.println("STATE ERROR");

            delay(50);
            while(!OkButton())
            {

            }
            currentState = STATE_IDLE;
            break;

        case STATE_PROFIL_SELECT:
            Serial.println("STATE PROFIL SELECT");

            uint8_t temperature1;
            numberOfResistance activeResistance1;
            uint8_t temperature2;
            numberOfResistance activeResistance2;
            uint8_t temperature3;
            numberOfResistance activeResistance3;
            displayMainMenu();
            delay(100);
            while(!OkButton())
            {
                main_Menu_Browse();
            }

            selectedProfilName = launchProfil();
            delay(100);

            if (selectedProfilName == AILE )
            {
                lcd.clear();
                lcd.setCursor(2, 0);
                lcd.print("AILE");
                currentProfilName = "AILE";
                temperature1 = TEMPERATURE1_AILE;
                duration1Hour = DURATION1_AILE;
                activeResistance1 = ACTIVE_RESISTANCE_1_AILE;
                temperature2 = TEMPERATURE2_AILE;
                duration2Hour = DURATION2_AILE;
                activeResistance2 = ACTIVE_RESISTANCE_2_AILE;
                temperature3 = TEMPERATURE3_AILE;
                duration3Hour = DURATION3_AILE;
                activeResistance3 = ACTIVE_RESISTANCE_3_AILE;

            }
            else if (selectedProfilName == PREPREG )
            {
                lcd.clear();
                lcd.setCursor(2, 0);
                lcd.print("PREPREG");
                currentProfilName = "PREPREG";
                temperature1 = TEMPERATURE1_PREPREG;
                duration1Hour = DURATION1_PREPREG;
                activeResistance1 = ACTIVE_RESISTANCE_1_PREPREG;
                temperature2 = TEMPERATURE2_PREPREG;
                duration2Hour = DURATION2_PREPREG;
                activeResistance2 = ACTIVE_RESISTANCE_2_PREPREG;
                temperature3 = TEMPERATURE3_PREPREG;
                duration3Hour = DURATION3_PREPREG;
                activeResistance3 = ACTIVE_RESISTANCE_3_PREPREG;
            }
            else if (selectedProfilName == CUSTOM)
            {
                #if 0 // test
                temperature1 = 30;
                duration1 = 10000;
                activeResistance1 = ONE;
                temperature2 = 40;
                duration2 = 1000000;
                activeResistance2 = THREE;
                temperature3 = 50;
                duration3 = 10000;
                activeResistance3 = TWO;

                #else
                /* TEMPERATURE SELECT */
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Select T1[");
                lcd.print((char)223); // ° symbol
                lcd.print("C]");
                currentProfilName = "CUSTOM";
                resetTemperature ();
                displayTempMenu();
                delay(100);
                while(!OkButton())
                {
                    selectTemperature();
                    if (CancelButton())
                    {
                        currentState = STATE_IDLE;
                        return;
                    }
                    selectTemperature();
                }

                temperature1 = setTemperature();
                lcd.setCursor(0, 0);
                lcd.print("Select T2[");
                lcd.print((char)223); // ° symbol
                lcd.print("C]");
                resetTemperature ();
                displayTempMenu();
                delay(100);
                while(!OkButton())
                {
                    selectTemperature();
                }
                temperature2 = setTemperature();
                lcd.setCursor(0, 0);
                lcd.print("Select T3[");
                lcd.print((char)223); // ° symbol
                lcd.print("C]");
                resetTemperature ();
                displayTempMenu();
                delay(100);
                while(!OkButton())
                {
                    selectTemperature();
                }
                temperature3 = setTemperature();


                /* DURATION SELECT */
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Select t1[hour]");
                resetDuration();
                displayDurationMenu();
                delay(100);
                while(!OkButton())
                {
                    if (CancelButton())
                    {
                        currentState = STATE_IDLE;
                        return;
                    }
                    selectDuration();
                }
                lcd.clear();
                duration1Hour = setDuration();
                lcd.setCursor(0, 0);
                lcd.print("Select t2[hour]");
                resetDuration();
                displayDurationMenu();
                delay(100);

                while(!OkButton())
                {
                    if (CancelButton())
                    {
                        currentState = STATE_IDLE;
                        return;
                    }
                    selectDuration();
                }
                lcd.clear();
                duration2Hour = setDuration();
                lcd.setCursor(0, 0);
                lcd.print("Select t3[hour]");
                resetDuration();
                displayDurationMenu();
                delay(100);
                while(!OkButton())
                {
                    if (CancelButton())
                    {
                        currentState = STATE_IDLE;
                        return;
                    }
                    selectDuration();
                }

                duration3Hour = setDuration();
                /* Number of Resistance*/
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Select NbRes1 ");
                resetNbResistance();
                displayNbResistanceMenu();
                delay(100);
                while(!OkButton())
                {
                    if (CancelButton())
                    {
                        currentState = STATE_IDLE;
                        return;
                    }
                    selectNbResistance();
                }

                activeResistance1 = setNbResistance();
                lcd.setCursor(0, 0);
                lcd.print("Select NbRes2 ");
                resetNbResistance();
                displayNbResistanceMenu();
                delay(100);
                while(!OkButton())
                {
                    if (CancelButton())
                    {
                        currentState = STATE_IDLE;
                        return;
                    }
                    selectNbResistance();
                }

                activeResistance2 = setNbResistance();
                lcd.setCursor(0, 0);
                lcd.print("Select NbRes3 ");
                resetNbResistance();
                displayNbResistanceMenu();
                delay(100);
                while(!OkButton())
                {
                    if (CancelButton())
                    {
                        currentState = STATE_IDLE;
                        return;
                    }
                    selectNbResistance();
                }

                activeResistance3 = setNbResistance();

                #endif

                lcd.clear();
                lcd.setCursor(2, 0);
                lcd.print("CUSTOM");

            }
            profil_init(&currentProfil, temperature1, duration1Hour,activeResistance1, temperature2, duration2Hour, activeResistance2, temperature3, duration3Hour, activeResistance3);

            preheatTemperatureStamp = measuredTemperature;
            preheatTimeStamp = millis ();

            Serial.println("Phase 1");
            lcd.setCursor(11, 0);
            lcd.print("P:1/3");
            RssiDisplay();
            actualProfilDuration = duration1Hour + duration2Hour + duration3Hour;
            targetTemperature = currentProfil.phase1.temperature;
            actualPhase = 1;
            startWebMonitoring();
            sendData();
            currentState = STATE_PHASE1;

            break;

        case STATE_PHASE1:
            Serial.println("STATE PHASE 1");

            temperatureProgressDisplay();

            cookingState = processCooking(&currentProfil,currentProfil.phase1.temperature,currentProfil.phase1.duration, currentProfil.phase1.activeResistance);
            if (cookingState == FINISHED)
            {
                preheatTemperatureStamp = measuredTemperature;
                preheatTimeStamp = millis ();
                currentState = STATE_PHASE2;
                lcd.setCursor(11, 0);
                lcd.print("P:2/3");
                actualPhase = 2;
                targetTemperature = currentProfil.phase2.temperature;
            }
            if (cookingState==ERROR)
            {
                currentState = STATE_ERROR;
            }

            break;

        case STATE_PHASE2:
            Serial.println("STATE PHASE 2");
            temperatureProgressDisplay();
            cookingState = processCooking(&currentProfil,currentProfil.phase2.temperature,currentProfil.phase2.duration, currentProfil.phase2.activeResistance);

            if (cookingState == FINISHED)
            {
                preheatTemperatureStamp = measuredTemperature;
                preheatTimeStamp = millis ();

                currentState = STATE_PHASE3;
                actualPhase = 3;
                targetTemperature = currentProfil.phase3.temperature;
                lcd.setCursor(11, 0);
                lcd.print("P:3/3");
            }
            if (cookingState == ERROR)
            {
                currentState = STATE_ERROR;
            }
            break;

        case STATE_PHASE3:
            ////Serial.println("STATE PHASE 3");
            temperatureProgressDisplay();

            cookingState = processCooking(&currentProfil, currentProfil.phase3.temperature, currentProfil.phase3.duration, currentProfil.phase3.activeResistance);
            if (cookingState==FINISHED)
            {
                stopHeating();
                lcd.clear();
                lcd.setCursor(2, 0);
                lcd.print("COOLING ...");
                targetTemperature = COOLING_TEMPERATURE;
                actualPhase = 4;
                currentState = COOLING;

            }
            if (cookingState==ERROR)
            {
                currentState = STATE_ERROR;
            }
            break;

        case COOLING :
            if (measuredTemperature > COOLING_TEMPERATURE)
                {
                    ////Serial.println("Cooling");
                    temperatureProgressDisplay();
                }
            else
            {
                errorLabel = "SUCCES";
                errorOccured = false;
                delay(50);
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("COOKING DONE");
                lcd.setCursor(3, 1);
                lcd.print("Press OK");
                sendEmail(errorLabel,currentProfilName, errorOccured);
                delay(50);
                while(!OkButton())
                {

                }
                lcd.clear();
                currentState = STATE_IDLE;
            }
            if (!readTemperature())  currentState == ERROR;
            break;

        default:
            currentState = STATE_IDLE;
    }


updateWebData();

delay(2000); // sensors coversion time & data refresh period

#else

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Relay OFF");
        stopHeating();
        delay(5000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Relay ON");
        activeThreeResistance();
        delay(5000);

#endif

}