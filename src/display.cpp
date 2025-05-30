
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

int currentMenu = 0;            // Tracks current menu item

uint8_t maxMainMenuItems = 2;       // Number of menu options
bool inSubMenu = false;              // Tracks whether user is in a submenu
LiquidCrystal_I2C lcd(0x27, 16, 2);

uint8_t currentTemperature = 30;
const uint8_t maxTemperature = 90;
const uint8_t minTemperature = 30;
const uint8_t temperatureStep = 5;

int currentDuration = 0;
const uint8_t maxDuration= 20;
const uint8_t minDuration = 0;
const uint8_t DurationStep = 1;

int currentNbResistance = 0;
const uint8_t maxNbResistance= 3;
const uint8_t minNbResistance = 1;
const uint8_t NbResistanceStep = 1;


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
lcd.setCursor(5, 1);
lcd.print("CUSTOM");  // Left arrow
lcd.setCursor(15, 1); // First row, last column
lcd.write(byte(1));   // Right arrow
}


void  initDisplay()
{
    Wire.setPins(I2C_SDA, I2C_SCL);
    lcd.init();
    Wire.setClock(100000);
    lcd.backlight();  // Turn on the LCD backlight
    lcd.createChar(0, leftArrow);
    lcd.createChar(1, rightArrow);
}

void main_Menu_Browse()
{
  if (upButton()) {

    currentMenu++;
    if (currentMenu > maxMainMenuItems)
        {
        currentMenu = 0;  // Loop back to first menu item
        }
    displayMainMenu();
    delay(200);
  }
  if (downButton()) {

    currentMenu--;
    if (currentMenu < 0 )
        {
        currentMenu = maxMainMenuItems;  // Loop back to first menu item
        }
    displayMainMenu();
    delay(200);
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
    default:
      return CUSTOM;
  }
  return CUSTOM;
}


void resetTemperature ()
{
  currentTemperature = minTemperature;
}

void selectTemperature()
{
if (downButton()) {
    currentTemperature +=temperatureStep;
    if (currentTemperature > maxTemperature)
        {
        currentTemperature = minTemperature;  // Loop back to first menu item
        }
    displayTempMenu();
    delay(200);
  }
  if (upButton()) {

    currentTemperature -= temperatureStep ;
    if (currentTemperature < minTemperature )
        {
        currentTemperature = maxTemperature;  // Loop back to first menu item
        }
    displayTempMenu();
    delay(200);
  }
}


uint8_t setTemperature()
{
return currentTemperature;
}

void displayTempMenu()
{

lcd.setCursor(5, 1);
lcd.write(byte(0));  // Left arrow
lcd.setCursor(7, 1);
lcd.print(currentTemperature);
lcd.setCursor(10, 1); // First row, last column
lcd.write(byte(1));   // Right arrow

}

void displayDurationMenu()
{
lcd.setCursor(5, 1);
lcd.write(byte(0));  // Left arrow
lcd.setCursor(7, 1);
lcd.print(currentDuration);
lcd.setCursor(9, 1); // First row, last column
lcd.write(byte(1));   // Right arrow
}

void resetDuration()
{
  currentDuration = minDuration;
}

void selectDuration()
{

if (downButton()) {
    currentDuration +=DurationStep ;
    if (currentDuration > maxDuration)
        {
        currentDuration = minDuration;  // Loop back to first menu item
        }
    displayDurationMenu();
    delay(200);
  }
  if (upButton()) {

    currentDuration -=DurationStep ;
    if (currentDuration < minDuration )
        {
        currentDuration = maxDuration;  // Loop back to first menu item
        }
    displayDurationMenu();
    delay(200);
  }
}


uint8_t setDuration()
{
return currentDuration;
}


void displayNbResistanceMenu()
{

lcd.setCursor(5, 1);
lcd.write(byte(0));  // Left arrow
lcd.setCursor(7, 1);
lcd.print(currentNbResistance);
lcd.setCursor(9, 1); // First row, last column
lcd.write(byte(1));   // Right arrow

}

void resetNbResistance()
{
  currentNbResistance = minNbResistance;
}

void selectNbResistance()
{

if (downButton()) {
    currentNbResistance +=NbResistanceStep ;
    if (currentNbResistance > maxNbResistance)
        {
        currentNbResistance = minNbResistance;  // Loop back to first menu item
        }

    displayNbResistanceMenu();
    delay(200);

  }
  if (upButton()) {

    currentNbResistance -=NbResistanceStep ;
    if (currentNbResistance < minNbResistance )
        {
        currentNbResistance = maxNbResistance;  // Loop back to first menu item
        }
    displayNbResistanceMenu();
    delay(200);
  }
}


numberOfResistance  setNbResistance()
{
    switch (currentNbResistance)
    {
        case 0:
            return NONE;
        case 1:
            return ONE;
        case 2:
            return TWO;
        case 3:
            return THREE;
        default:
            return NONE; // Handle unexpected cases
    }

    return NONE;
}


<<<<<<< HEAD
bool confirmRestart()
{
  
    // Restart the ESP32
  if (OkButton())
  {
  esp_restart();
  }
  if (CancelButton)
  {
  return true;
  }
}
=======
>>>>>>> 8f80f66f4c6342970f765635d5f8c8b36718842c
