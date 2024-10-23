
#include "display.h"
#include "IODriver.h"


byte leftArrow[8] = {
  0b00000,
  0b00100,
  0b01100,
  0b11111,
  0b01100,
  0b00100,
  0b00000,
  0b00000
};

byte rightArrow[8] = {
  0b00000,
  0b00100,
  0b00110,
  0b11111,
  0b00110,
  0b00100,
  0b00000,
  0b00000
};

uint8_t currentMenu = 0;            // Tracks current menu item

uint8_t maxMainMenuItems = 2;       // Number of menu options
bool inSubMenu = false;              // Tracks whether user is in a submenu
LiquidCrystal_I2C lcd(0x27, 16, 2);

uint8_t currentTemperature = 30; 
uint8_t maxTemperature = 60; 
uint8_t minTemperature = 30;
uint8_t temperatureStep = 10;

uint8_t currentDuration = 0; 
uint8_t maxDuration= 10; 
uint8_t minDuration = 0;
uint8_t DurationStep = 1;


void displayAileProfile()
{
lcd.setCursor(2, 0); // Set the cursor on the first column, first row
lcd.print("Choix profil"); 
lcd.setCursor(0, 1);
lcd.write(byte(0));  // Left arrow
lcd.setCursor(6, 1);
lcd.print("AILE");  // Left arrow
lcd.setCursor(15, 1); // First row, last column
lcd.write(byte(1));   // Right arrow
}

void displayPregProfile()
{
lcd.setCursor(2, 0); // Set the cursor on the first column, first row
lcd.print("Choix profil"); 
lcd.setCursor(0, 1);
lcd.write(byte(0));  // Left arrow
lcd.setCursor(4, 1);
lcd.print("PREPREG");  // Left arrow
lcd.setCursor(15, 1); // First row, last column
lcd.write(byte(1));   // Right arrow
}

void displayCustumProfile()
{
lcd.setCursor(2, 0); // Set the cursor on the first column, first row
lcd.print("Choix profil"); 
lcd.setCursor(0, 1);
lcd.write(byte(0));  // Left arrow
lcd.setCursor(4, 1);
lcd.print("CUSTOM");  // Left arrow
lcd.setCursor(15, 1); // First row, last column
lcd.write(byte(1));   // Right arrow
}


void  initDisplay()
{
    lcd.init();
    lcd.backlight();  // Turn on the LCD backlight
    lcd.createChar(0, leftArrow);
    lcd.createChar(1, rightArrow);
    // lcd.setCursor(3, 0); // Set the cursor on the first column, first row
    // lcd.print("AERIANE SA");    
    // delay(2000);
}

void main_Menu_Browse()
{
  if (upButton) {
    
    currentMenu++;
    if (currentMenu >= maxMainMenuItems) 
        {
        currentMenu = 0;  // Loop back to first menu item
        }
    displayMainMenu();
  }
  if (downButton) {
    
    currentMenu--;
    if (currentMenu <=minTemperature ) 
        {
        currentMenu = maxMainMenuItems;  // Loop back to first menu item
        }
    displayMainMenu();
  }
  
  }

void displayMainMenu() {
  lcd.clear();
  switch (currentMenu) {
    case 0:
      displayAileProfile();
      break;
    case 1:
      displayPregProfile();
      break;
    case 2:
      displayCustumProfile();
    break;
  }
}


profilName launchProfil()
{
  switch (currentMenu) {
    case 0:
      return AILE;
    case 1:
      return PREPREG;
    case 2:
      return CUSTOM;
  }
}



void selectTemperature()
{
currentTemperature = minTemperature; 
if (upButton) {
    currentTemperature +=temperatureStep;
    if (currentTemperature >= maxTemperature) 
        {
        currentTemperature = minTemperature;  // Loop back to first menu item
        }
    displayTempMenu();
  }
  if (downButton) {
    
    currentTemperature -=temperatureStep;;
    if (currentTemperature <=minTemperature ) 
        {
        currentTemperature = maxTemperature;  // Loop back to first menu item
        }
    displayTempMenu();
  }
}


uint8_t setTemperature()
{
return currentTemperature;
}

void displayTempMenu()
{
   lcd.setCursor(0, 1); 
    lcd.print("T: ");
    lcd.print(currentTemperature); 
    lcd.print((char)223); // ° symbol
    lcd.print("C"); 
    
}

void displayDurationMenu()
{
    lcd.setCursor(0, 1); 
    lcd.print("t: ");
    lcd.print(currentDuration); 
    lcd.print("h"); 
}



void selectDuration()
{
currentDuration = minDuration; 
if (upButton) {
    currentDuration +=DurationStep;
    if (currentDuration >= maxDuration) 
        {
        currentDuration = minDuration;  // Loop back to first menu item
        }
    displayDurationMenu();
  }
  if (downButton) {
    
    currentDuration -=DurationStep;;
    if (currentDuration <=minDuration ) 
        {
        currentDuration = maxDuration;  // Loop back to first menu item
        }
    displayDurationMenu();
  }
}


uint8_t setDuration()
{
return currentDuration;
}

