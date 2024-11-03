#include <Arduino.h>
#include"IODriver.h"
#include <OneWire.h>
#include "DallasTemperature.h"
#include <Wire.h>
#include "display.h"


#include"profil.h"
#include"stdbool.h"


#define RUNNING 0
#define FINISHED 1
#define ERROR 2

#define MAX_SENSOR_DIFFERENCE 5

#define ONE_WIRE_BUS OneWirePin

#define TEMPERATURE1_AILE 30;
#define DURATION1_AILE 8;
#define TEMPERATURE2_AILE 40;
#define DURATION2_AILE 4;
#define TEMPERATURE3_AILE 50;
#define DURATION3_AILE 4;

#define TEMPERATURE1_PREPREG 50;
#define DURATION1_PREPREG 3;
#define TEMPERATURE2_PREPREG 90;
#define DURATION2_PREPREG 6;
#define TEMPERATURE3_PREPREG 0;
#define DURATION3_PREPREG 0;

// ONE WIRE
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// COOKING
cookingProfil currentProfil;
cookingState currentState = STATE_IDLE;
profilName selectedProfilName = CUSTOM;
float measuredTemperature = 0;
int preheatTemperatureStamp = 0;
unsigned long preheatTimeStamp = 0;
uint8_t lastTemperature = 0;
uint8_t targetTemperature = 0;

bool restart = false;



bool readTemperature()
{
 sensors.requestTemperatures();
 float tempSensor1 = sensors.getTempCByIndex(0);
 float tempSensor2 = sensors.getTempCByIndex(1);

  // Check if reading was successful
// Serial.println(tempSensor1);
// Serial.println(tempSensor2);

 int sensorsOutputDifference = abs(tempSensor1 - tempSensor2);

  if (sensorsOutputDifference > MAX_SENSOR_DIFFERENCE)
  {
    // Serial.println("Sensors temperature difference too high ");
    if(tempSensor1 != DEVICE_DISCONNECTED_C)
    {
        Serial.print("Temperature for the sensor 1 is: ");
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
        lcd.print("SENSOR 1 ERROR");

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
        Serial.println("Error: Could not read the sensor 2");
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("SENSOR 2 ERROR");


    }
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("ONE FAULTY SENSOR");
    while(OkButton())
    {

    }
    lcd.clear();
    return false;
    }
 else
 {
    measuredTemperature = (tempSensor1 + tempSensor1)/2;
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


uint8_t processCooking(cookingProfil *self, uint8_t temperature, uint64_t duration, numberOfResistance activeResistance)
{
static bool steady_state = false;
static unsigned long PhaseTimeStamp = 0;
static int PhaseTemperatureStamp = 0;
uint32_t tenMinutes = 600000;
static bool regulationStared = false;
uint8_t overheatThreshold = temperature + 10;

Serial.print("Steady state : ");
Serial.println(steady_state);

if (!readTemperature())
   {
    // send email, sensor error

    return ERROR;

   }



if (measuredTemperature > temperature && !steady_state)
    {
    steady_state = true;
    Serial.println("Steady state reached");
    PhaseTimeStamp = millis();
    PhaseTemperatureStamp = measuredTemperature;
    }

if (steady_state)
    {
    unsigned long time_elapsed = millis()-PhaseTimeStamp;
    uint8_t temperature_evolution = measuredTemperature - PhaseTemperatureStamp;
    Serial.print("Time elapsed : ");
    Serial.println(time_elapsed);
   if (time_elapsed >= duration)
   {
        steady_state = false;
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

    /* TEPERATURE REGULATION*/
    if (regulationStared)
    {
        if (measuredTemperature > temperature + 2)
        {
            Serial.println("Max temperature regulation reached, heating off");
            stopHeating();
            delay(50);
        }

        if (regulationStared && measuredTemperature < temperature - 2)
        {
            Serial.println("Min temperature regulation reached, heating on");
            activateResistance(activeResistance);
            delay(50);
        }
    }
    else
    {
        if (measuredTemperature > temperature + 2)
        {
        Serial.println("Max temperature regulation reached, heating off");
        stopHeating();
        delay(50);
        regulationStared = true;
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
    delay(50);
    regulationStared = false;
}
    // Serial.print("Active resistance : ");
    // Serial.println(activeResistance);
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

 Wire.setClock(50000); // Set I2C speed to 100kH
 Serial.begin(115200);
 Serial.println("Starting");
 initDisplay();
 initRelay();
 init_GPIO();

// init wifi & display
}


void loop() {


    static cookingState last_phase = STATE_IDLE;
    static uint64_t time_count = 0;
    // static profilName selectedProfilName = AILE;
    static uint8_t cooking_state = RUNNING;



    //  while (1) {
      // stop button (demander confirmation)


        switch (currentState) {
            case STATE_IDLE:
                Serial.println("STATE IDLE");
                currentState = STATE_PROFIL_SELECT;
                break;

            case STATE_ERROR:
                // affichage + demande de restart => Idle
                Serial.println("STATE ERROR");
                stopHeating();
                while(OkButton())
                {

                }
                currentState = STATE_IDLE;
                break;

            case STATE_PROFIL_SELECT:
                Serial.println("STATE PROFIL SELECT");
                uint8_t temperature1;
                uint64_t duration1;
                numberOfResistance activeResistance1;
                uint8_t temperature2;
                uint64_t duration2;
                numberOfResistance activeResistance2;
                uint8_t temperature3;
                uint64_t duration3;
                numberOfResistance activeResistance3;
                displayMainMenu();
                while(!OkButton())
                {
                    //Serial.print("we're here");
                  main_Menu_Browse();
                }

                selectedProfilName = launchProfil();

                if (selectedProfilName == AILE )
                {
                    lcd.clear();
                    lcd.setCursor(2, 0); // Set the cursor on the first column, first row
                    lcd.print("AILE");
                    temperature1 = TEMPERATURE1_AILE;
                    duration1 = DURATION1_AILE;
                    activeResistance1 = ONE;
                    temperature2 = TEMPERATURE2_AILE;
                    duration2 = DURATION2_AILE;
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
                    duration1 = DURATION1_PREPREG;
                    activeResistance1 = THREE;
                    temperature2 = TEMPERATURE2_PREPREG;
                    duration2 = DURATION2_PREPREG;
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
                    while(!OkButton())
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
                    delay(200);
                    while(!OkButton())
                    {
                        selectTemperature();
                    }
                    temperature2 = setTemperature();
                    lcd.setCursor(0, 0); // Set the cursor on the first column, first row
                    lcd.print("Select T3[");
                    lcd.print((char)223); // ° symbol
                    lcd.print("C]");
                    resetTemperature ();
                    displayTempMenu();
                    delay(200);
                    while(!OkButton())
                    {
                        selectTemperature();
                    }
                    temperature3 = setTemperature();


                    /* DURATION SELECT */
                    lcd.clear();
                    lcd.setCursor(0, 0); // Set the cursor on the first column, first row
                    lcd.print("Select t1[hour]");
                    resetDuration();
                    displayDurationMenu();
                    delay(200);
                    while(!OkButton())
                    {
                        selectDuration();
                    }
                    duration1 = setDuration();
                     lcd.setCursor(0, 0); // Set the cursor on the first column, first row
                    lcd.print("Select t2[hour]");
                    resetDuration();
                    displayDurationMenu();
                    delay(200);
                    while(!OkButton())
                    {
                        selectDuration();
                    }
                    duration2 = setDuration();
                    lcd.setCursor(0, 0); // Set the cursor on the first column, first row
                    lcd.print("Select t3[hour]");
                    resetDuration();
                    displayDurationMenu();
                    delay(200);
                    while(!OkButton())
                    {
                        selectDuration();
                    }
                    duration3 = setDuration();
                    /* Number of Resistance*/
                    lcd.clear();
                    lcd.setCursor(0, 0); // Set the cursor on the first column, first row
                    lcd.print("Select NbRes1 ");
                    resetNbResistance();
                    displayNbResistanceMenu();
                    delay(200);
                    while(!OkButton())
                    {
                        selectNbResistance();
                    }
                    activeResistance1 = setNbResistance();
                    lcd.setCursor(0, 0); // Set the cursor on the first column, first row
                    lcd.print("Select NbRes2 ");
                    resetNbResistance();
                    displayNbResistanceMenu();
                    delay(200);
                    while(!OkButton())
                    {
                        selectNbResistance();
                    }
                    activeResistance2 = setNbResistance();
                    lcd.setCursor(0, 0); // Set the cursor on the first column, first row
                    lcd.print("Select NbRes3 ");
                    resetNbResistance();
                    displayNbResistanceMenu();
                    delay(200);
                    while(!OkButton())
                    {
                        selectNbResistance();
                    }
                    activeResistance3 = setNbResistance();

                    #endif

                    lcd.clear();
                    lcd.setCursor(2, 0); // Set the cursor on the first column, first row
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
                if (cooking_state==ERROR)
                {
                    currentState = STATE_ERROR;
                }

                break;

            case STATE_PHASE2:
                Serial.println("STATE PHASE 2");
                temperatureProgressDisplay();
                cooking_state = processCooking(&currentProfil,currentProfil.phase2.temperature,currentProfil.phase2.duration, currentProfil.phase2.activeResistance);

                if (cooking_state == FINISHED)
                {
                    preheatTemperatureStamp = measuredTemperature;
                    preheatTimeStamp = millis ();

                    currentState = STATE_PHASE3;
                    lcd.setCursor(11, 0);
                    targetTemperature = currentProfil.phase3.temperature;
                    lcd.print("P:3/3");
                }
                if (cooking_state == ERROR)
                {
                    currentState = STATE_ERROR;
                }
                break;

            case STATE_PHASE3:
                Serial.println("STATE PHASE 3");
                temperatureProgressDisplay();
                cooking_state = processCooking(&currentProfil, currentProfil.phase3.temperature, currentProfil.phase3.duration, currentProfil.phase3.activeResistance);
                if (cooking_state==FINISHED)
                {
                    // send email & display
                    stopHeating();
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("COOKING DONE");
                    lcd.setCursor(3, 1);
                    lcd.print("Press OK");
                    while(!OkButton())
                    {

                    }
                    currentState = STATE_IDLE;
                }
                if (cooking_state==ERROR)
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



