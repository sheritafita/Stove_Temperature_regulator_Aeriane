#include <Arduino.h>
#include"IODriver.h"
#include <OneWire.h>
#include "DallasTemperature.h"
#include <Wire.h>
#include "display.h"
#include "WebMonitor.h"

#include"profil.h"
#include"stdbool.h"


#define RUNNING 0
#define FINISHED 1
#define ERROR 2

#define MAX_SENSOR_DIFFERENCE 5

#define ONE_WIRE_BUS OneWirePin

#define TEMPERATURE1_AILE 30
#define DURATION1_AILE 8
#define TEMPERATURE2_AILE 40
#define DURATION2_AILE 4
#define TEMPERATURE3_AILE 50
#define DURATION3_AILE 4

#define TEMPERATURE1_PREPREG 50
#define DURATION1_PREPREG 3
#define TEMPERATURE2_PREPREG 90
#define DURATION2_PREPREG 6
#define TEMPERATURE3_PREPREG 0
#define DURATION3_PREPREG 0

#define HYSTERESIS_UP 0
#define HYSTERESIS_DOWN 0
#define TEMPERATURE_DYNAMIC_THRESHOLD 0.3
#define HEATING_TIMEOUT_MS 900000 //15mn

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

String errorLabel = "";
String currentProfilName = "";
bool errorOccured = false;

int actualProfilDuration = 0;


int actualPhase = 0;
int numberOfPhase = 3;

uint64_t duration1Hour;
uint64_t duration2Hour;
uint64_t duration3Hour;

bool readTemperature()
{
 sensors.requestTemperatures();
 float tempSensor1 = sensors.getTempCByIndex(0);
 float tempSensor2 = sensors.getTempCByIndex(1);

 int sensorsOutputDifference = abs(tempSensor1 - tempSensor2);

  if (sensorsOutputDifference > MAX_SENSOR_DIFFERENCE)
  {
    // Serial.println("Sensors temperature difference too high ");

    if(tempSensor1 != DEVICE_DISCONNECTED_C)
    {
        Serial.print("Temperature for the sensor 1 is: ");
        Serial.println(tempSensor1);

    }
    else
    {
        Serial.println("Error: Could not read the sensor 1");
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("SENSOR 1 ERROR");

        lcd.setCursor(0, 1);
        lcd.print("SENS1 COM ERROR");
        errorLabel = "Sensor 1 disconnected";
        return false;
    }

    if(tempSensor2 != DEVICE_DISCONNECTED_C)
    {
        Serial.print("Temperature for the sensor 2 is: ");
        Serial.println(tempSensor1);
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("1 FAULTY SENSOR");
    }
    else
    {
        Serial.println("Error: Could not read the sensor 1");
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("SENSOR 2 ERROR");


    }


    return false;
    }
 else
 {
   measuredTemperature = (tempSensor1 + tempSensor2)/2;
   //measuredTemperature = random(30, 100);
    Serial.print("Temperature: ");
    Serial.println(measuredTemperature);
    return true;
    }
}




void activateResistance(numberOfResistance numberOfResistance)
{
switch (numberOfResistance)
{
    case NONE:
        // Serial.println("Stop heating");
        stopHeating();
        break;
    case ONE:
    //    Serial.println("Active resistance : 1");
        activeOneResistance();
        break;
    case TWO:
        // Serial.println("Active resistance : 2");
        activeTwoResistance();
        break;
    case THREE:
    //  Serial.println("Active resistance : 3");
        activeThreeResistance();
        break;
    default:
    // Serial.println("Stop heating default");
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
uint32_t OneMinute = 60000;
static unsigned long DataSendingTemperatureStamp = 0;
unsigned long DataTimeElapsed = millis()-DataSendingTemperatureStamp;

   if (DataTimeElapsed >= OneMinute || currentState == STATE_PROFIL_SELECT)
   {
    sendData();
     DataSendingTemperatureStamp = millis();
   }
}

void dynamic_error_display()
{
    Serial.println("Error : heating took too long");
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("BAD DYNAMIC ERR");
    errorLabel = "system dynamic too slow, check sensors or heater";
}

uint8_t processCooking(cookingProfil *self, uint8_t temperature, uint64_t duration, numberOfResistance activeResistance)
{
static bool steadyState = false;
static unsigned long PhaseTimeStamp = 0;
static float phaseTemperatureStamp = 0;
unsigned long timeElapsed;
float temperatureEvolution;

uint8_t overheatThreshold = temperature + 10;

Serial.print("Steady state : ");
Serial.println(steadyState);

//sendData();

if (!readTemperature())
   {
    // send email, sensor error

    return ERROR;

   }



if (measuredTemperature > temperature && !steadyState)
    {
    steadyState = true;
    Serial.println("Steady state reached");
    PhaseTimeStamp = millis();
    phaseTemperatureStamp = measuredTemperature;
    }

if (steadyState)
    {
    unsigned long time_elapsed = millis()-PhaseTimeStamp;
    uint8_t temperature_evolution = measuredTemperature - PhaseTemperatureStamp;
    Serial.print("Time elapsed : ");
    Serial.println(timeElapsed);
   if (timeElapsed >= duration)
   {
        steadyState = false;
        return FINISHED;
   }

   /* OVERHEAT CHECK*/
   if (measuredTemperature > overheatThreshold)
   {

    // send email overheating
    Serial.println("Error : overheating, heating stopped");
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("ERROR OVERHEAT");
    return ERROR;
   }
    /* HEATING DYNAMIC CHECK*/
   if (time_elapsed > tenMinutes && temperature_evolution < 2 )
   {
    // send email heating progress problem
    Serial.println("Error : heating took too long");
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("HEATER ERROR");
    return ERROR;
   }

    /* TEMPERATURE REGULATION*/
    if (setPointReached)
    {
        if (measuredTemperature > temperature )
        {
            Serial.println("Max temperature regulation reached, heating off");
            stopHeating();
            delay(100);

        }
        uint8_t DownThreshold = temperature - HYSTERESIS_DOWN;
        if (setPointReached && measuredTemperature < DownThreshold)
        {
            Serial.println("Min temperature regulation reached, heating on");
            activateResistance(activeResistance);
            delay(100);

        }
    }
    else
    {
        if (measuredTemperature > temperature + 2)
        {
        Serial.println("Set point reached, heating off");
        stopHeating();
        delay(100);

        setPointReached = true;
        }
        }

    }
else
{
    unsigned long time_elapsed = millis() - preheatTimeStamp;
    uint8_t temperature_evolution = measuredTemperature - preheatTimeStamp;
    if (time_elapsed > tenMinutes && temperature_evolution < 3 )
        {
            // send email heating progress problem
            return ERROR;
        }
    activateResistance(activeResistance);
    delay(100);
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

void resetCookingParameters()
{
    errorOccured = false;
    currentProfilName = "";
    errorLabel = "";
    actualProfilDuration = 0;
    targetTemperature = 0;
    actualPhase = 0;
    duration1Hour = 0;
    duration2Hour = 0;
    duration3Hour = 0;
    setPointReached = false;
}


void resetCookingParameters()
{
    errorOccured = false;
    currentProfilName = "";
    errorLabel = "";
    actualProfilDuration = 0;
    targetTemperature = 0;
    actualPhase = 0;
    duration1Hour = 0;
    duration2Hour = 0;
    duration3Hour = 0;
    setPointReached = false;
}


void restoreDisplayAfterInterrupt()
{

lcd.clear();
switch (selectedProfilName)
{
    case CUSTOM:
        lcd.setCursor(2, 0);
        lcd.print("CUSTOM");
        break;
    case AILE:

        lcd.setCursor(2, 0); // Set the cursor on the first column, first row
        lcd.print("AILE");
        break;
    case PREPREG:

        lcd.setCursor(2, 0); // Set the cursor on the first column, first row
        lcd.print("PREPREG");
        break;
    default:
        break;
}

switch (selectedProfilName)
{
    case STATE_PHASE1:
        lcd.setCursor(11, 0);
        lcd.print("P:1/3");
        break;
    case STATE_PHASE2:
        lcd.setCursor(11, 0);
        lcd.print("P:2/3");
        break;
    case STATE_PHASE3:
        lcd.setCursor(11, 0);
        lcd.print("P:3/3");
        break;
    default:
        break;
}

}


void IRAM_ATTR restartInterrupt() {
if (currentState == STATE_PHASE1 || currentState == STATE_PHASE2 || currentState == STATE_PHASE3)
{
lcd.clear();
lcd.setCursor(2, 0); // Set the cursor on the first column, first row
detachInterrupt(digitalPinToInterrupt(CancelButtonPin));
lcd.print("Redemarrer ?");
delay(200);
while (!restart)
{
   restart = confirmRestart();
}
attachInterrupt(digitalPinToInterrupt(CancelButtonPin), restartInterrupt, FALLING);
restoreDisplayAfterInterrupt();
}
else
{
  esp_restart();
}

}

void setup()
{


 Serial.begin(115200);
 Serial.println("Starting");
 init_GPIO();
 initRelay();
 initDisplay();
 initWifi();




// init wifi & display
}

void backtoMainMenu()
{
    if (CancelButton) currentState = STATE_PROFIL_SELECT;
}

void loop() {


    static cookingState last_phase = STATE_IDLE;
    static uint64_t time_count = 0;
    static profilName selectedProfilName = PREPREG;
    // static profilName selectedProfilName = AILE;
    static uint8_t cooking_state = RUNNING;



    //  while (1) {
      // stop button (demander confirmation)


        switch (currentState) {
            case STATE_IDLE:
                Serial.println("STATE IDLE");
                resetCookingParameters();
                currentState = STATE_PROFIL_SELECT;
                break;

            case STATE_ERROR:
                // affichage + demande de restart => Idle
                errorOccured = true;
                sendEmail(errorLabel, currentProfilName, errorOccured);
                Serial.println("STATE ERROR");
                stopHeating();
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
                uint64_t duration3;
                numberOfResistance activeResistance3;
                displayMainMenu();
                delay(100);
                while(!OkButton())
                {
                  main_Menu_Browse();
                }

                selectedProfilName = launchProfil();

                if (selectedProfilName == AILE )
                {
                    lcd.clear();
                    lcd.setCursor(2, 0); // Set the cursor on the first column, first row
                    lcd.print("AILE");
                    temperature1 = TEMPERATURE1_AILE;
                    duration1Hour = DURATION1_AILE;
                    activeResistance1 = ONE;
                    temperature2 = TEMPERATURE2_AILE;
                    duration2Hour = DURATION2_AILE;
                    activeResistance2 = ONE;
                    temperature3 = TEMPERATURE3_AILE;
                    duration3 = DURATION3_AILE;
                    activeResistance3 = TWO;

                }
                else if (selectedProfilName == PREPREG )
                {
                    lcd.clear();
                    lcd.setCursor(2, 0); // Set the cursor on the first column, first row
                    lcd.print("PREPREG");
                    temperature1 = TEMPERATURE1_PREPREG;
                    duration1Hour = DURATION1_PREPREG;
                    activeResistance1 = THREE;
                    temperature2 = TEMPERATURE2_PREPREG;
                    duration2Hour = DURATION2_PREPREG;
                    activeResistance2 = THREE;
                    temperature3 = TEMPERATURE3_PREPREG;
                    duration3 = DURATION3_PREPREG;
                    activeResistance3 = NONE;
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
                    lcd.setCursor(0, 0); // Set the cursor on the first column, first row
                    lcd.print("Select T1[");
                    lcd.print((char)223); // ° symbol
                    lcd.print("C]");
                    resetTemperature ();
                    displayTempMenu();
                    delay(200);
                    while(OkButton())
                    {
                        selectTemperature();
                    }
                    temperature1 = setTemperature();
                    lcd.setCursor(0, 0); // Set the cursor on the first column, first row
                    lcd.print("Select T2[");
                    lcd.print((char)223); // ° symbol
                    lcd.print("C]");
                    resetTemperature ();
                    displayTempMenu();
                    delay(100);
                    while(!OkButton())
                    {
                        selectTemperature();
                       // backtoMainMenu();
                    }
                    temperature2 = setTemperature();
                    lcd.setCursor(0, 0); // Set the cursor on the first column, first row
                    lcd.print("Select T3[");
                    lcd.print((char)223); // ° symbol
                    lcd.print("C]");
                    resetTemperature ();
                    displayTempMenu();
                    delay(100);
                    while(!OkButton())
                    {
                        selectTemperature();
                      //  backtoMainMenu();
                    }
                    temperature3 = setTemperature();


                    /* DURATION SELECT */
                    lcd.clear();
                    lcd.setCursor(0, 0); // Set the cursor on the first column, first row
                    lcd.print("Select t1[hour]");
                    lcd.setCursor(0, 0);
                    lcd.print("Select t1[hour]");
                    resetDuration();
                    displayDurationMenu();
                    delay(100);
                    while(!OkButton())
                    {
                        selectDuration();
                    }
                    duration1 = setDuration();
                     lcd.setCursor(0, 0); // Set the cursor on the first column, first row
                    lcd.print("Select t2[hour]");
                       // backtoMainMenu();
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
                        selectDuration();
                    }
                    duration2 = setDuration();
                     lcd.setCursor(0, 0); // Set the cursor on the first column, first row
                    lcd.print("Select t3[hour]");
                    resetDuration();
                    displayDurationMenu();
                    delay(100);
                    while(!OkButton())
                    {
                        selectDuration();
                    }
                    duration3 = setDuration();
                    /* Number of Resistance*/
                    lcd.clear();
                    lcd.setCursor(0, 0); // Set the cursor on the first column, first row
                    lcd.print("Select NbRes1 ");
                    lcd.setCursor(0, 0);
                    lcd.print("Select NbRes1 ");
                    resetNbResistance();
                    displayNbResistanceMenu();
                    delay(100);
                    while(!OkButton())
                    {
                        selectNbResistance();
                    }
                       // backtoMainMenu();
                    }

                    activeResistance1 = setNbResistance();
                     lcd.setCursor(0, 0); // Set the cursor on the first column, first row
                    lcd.print("Select NbRes2 ");
                    resetNbResistance();
                    displayNbResistanceMenu();
                    delay(100);
                    while(!OkButton())
                    {
                        selectNbResistance();
                    }
                    activeResistance2 = setNbResistance();
                     lcd.setCursor(0, 0); // Set the cursor on the first column, first row
                    lcd.print("Select NbRes3 ");
                    resetNbResistance();
                    displayNbResistanceMenu();
                    delay(100);
                    while(!OkButton())
                    {
                        selectNbResistance();
                    }
                    activeResistance3 = setNbResistance();

                    #endif

                    lcd.clear();
                    lcd.setCursor(2, 0);
                    lcd.print("CUSTOM");

                }

                profil_init(&currentProfil, temperature1, duration1,activeResistance1, temperature2, duration2, activeResistance2, temperature3, duration3, activeResistance3);
                // Serial.print("currentProfil.phase1.activeResistance");
                // Serial.println(currentProfil.phase1.activeResistance);
                preheatTemperatureStamp = measuredTemperature;
                preheatTimeStamp = millis ();

                Serial.println("Phase 1");
                currentState = STATE_PHASE1;
                lcd.setCursor(11, 0);
                lcd.print("P:1/3");
                targetTemperature = currentProfil.phase1.temperature;
                break;

            case STATE_PHASE1:
                Serial.println("STATE PHASE 1");

                temperatureProgressDisplay();

                cooking_state = processCooking(&currentProfil,currentProfil.phase1.temperature,currentProfil.phase1.duration, currentProfil.phase1.activeResistance);
                if (cooking_state == FINISHED)
                {
                    preheatTemperatureStamp = measuredTemperature;
                    preheatTimeStamp = millis ();
                    currentState = STATE_PHASE2;
                    lcd.setCursor(11, 0);
                    lcd.print("P:2/3");
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
                    targetTemperature = currentProfil.phase3.temperature;
                    lcd.print("P:3/3");
                }
                if (cookingState == ERROR)
                {
                    currentState = STATE_ERROR;
                }
                break;

            case STATE_PHASE3:
                Serial.println("STATE PHASE 3");
                temperatureProgressDisplay();

                cookingState = processCooking(&currentProfil, currentProfil.phase3.temperature, currentProfil.phase3.duration, currentProfil.phase3.activeResistance);
                if (cookingState==FINISHED)
                {
                    // send email & display
                    stopHeating();
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
                    currentState = STATE_IDLE;
                }
                if (cookingState==ERROR)
                {
                    currentState = STATE_ERROR;
                }
                break;
            default:
                currentState = STATE_IDLE;
        }

        // send data
        // check connection error
    //  }

}



