#ifndef DISPLAY_H
#define DISPLAY_H


#include <LiquidCrystal_I2C.h>
#include "Arduino.h"
#include "profil.h"

extern LiquidCrystal_I2C lcd;
void initDisplay();
void main_Menu_Browse();
void displayAileProfile();
void displayPregProfile();
void displayCustumProfile();
void displayMainMenu();
profilName launchProfil();
void resetTemperature ();


void selectTemperature();
uint8_t setTemperature();
void displayTempMenu();

void selectDuration();
uint8_t setDuration();
void displayDuration();
void resetDuration();
void displayDurationMenu();

void selectNbResistance();
numberOfResistance setNbResistance();
void displayNbResistance();
void resetNbResistance();
void displayNbResistanceMenu();

#endif