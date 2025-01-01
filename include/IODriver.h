


#ifndef __IODRIVER_H__
#define __IODRIVER_H__


// Button
const int OkButtonPin =  26;
const int CancelButtonPin = 27;
const int UpButtonPin = 9;
const int DownButtonPin = 10;

// Relay module

const int relay2_1 = 13;
const int relay2_2 = 5;
const int relay2_3 = 2;



const int I2C_SDA = 21;
const int I2C_SCL = 22;
const int OneWirePin = 25;
const int CancelButtonPin = 27;

void initRelay();

void turn0nResistance1();
void turn0nResistance2();
void turn0nResistance3();



void turn0ffResistance1();
void turn0ffResistance2();
void turn0ffResistance3();


void stopHeating();
void activeOneResistance();
void activeTwoResistance();
void activeThreeResistance();
void ActivateAutomaticMode();
void DeactivateAutomaticMode();

bool OkButton();
bool CancelButton();
bool upButton();
bool downButton();

void init_GPIO ();


#ifdef	__cplusplus
extern "C" {
#endif


#ifdef	__cplusplus
}
#endif

#endif	