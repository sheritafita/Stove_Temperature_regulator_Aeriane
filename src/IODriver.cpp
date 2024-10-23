#include"Arduino.h"
#include"IODriver.h"
#include "multi_channel_relay.h"




const int OkButtonPin =  26;
const int CancelButtonPin = 27;
const int UpButtonPin = 9;
const int DownButtonPin = 10;



Multi_Channel_Relay relay1;
Multi_Channel_Relay relay2;

void init_GPIO ()
{
    pinMode(OkButtonPin, INPUT_PULLUP);
    pinMode(CancelButtonPin, INPUT_PULLUP);
    pinMode(UpButtonPin, INPUT_PULLUP);
    pinMode(DownButtonPin, INPUT_PULLUP);
}

void initRelay()
{
    relay1.begin(0x11);
    relay2.begin(0x12);
    stopHeating();
    ActivateAutomaticMode();
    
}

void ActivateAutomaticMode()
{
    relay1.turn_on_channel(1);
    relay1.turn_on_channel(2);
    relay1.turn_on_channel(3);
}

void DeactivateAutomaticMode()
{
  relay1.channelCtrl(0);
}

void turn0nResistance1()
{
    //Serial.println("Resistance 1 on");
    relay2.turn_on_channel(1);
}

void turn0nResistance2()
{
    relay2.turn_on_channel(2);
}

void turn0nResistance3()
{
    relay2.turn_on_channel(3);
}


void turn0ffResistance1()
{
    relay2.turn_off_channel(1);
}

void turn0ffResistance2()
{
    relay2.turn_off_channel(2);
}
void turn0ffResistance3()
{
    relay2.turn_off_channel(3);
}


void stopHeating()
{
    relay2.channelCtrl(0);   
}

void activeOneResistance()
{
    turn0nResistance1();
    turn0ffResistance2();
    turn0ffResistance3();
}
void activeTwoResistance()
{
    turn0nResistance1();
    turn0nResistance2();
    turn0ffResistance3();
    
}
void activeThreeResistance()
{
    turn0nResistance1();
    turn0nResistance2();
    turn0nResistance3();
}


// bool OkButton()
// {
// static int ok_lastState = HIGH;  // the previous state from the input pin  
// int  currentState = digitalRead(OkButtonPin);
//   if (ok_lastState == HIGH && currentState == LOW) // button pressed
//    { 
//     ok_lastState = currentState;
//     return true;
//     }
//   else // button released
//   {
//     ok_lastState = currentState;
//     return false;
//    }

// }

bool OkButton()
{

  return digitalRead(OkButtonPin);

}

bool CancelButton()
{
static int cancel_lastState = HIGH;  // the previous state from the input pin  
int  currentState = digitalRead(CancelButtonPin);
  if (cancel_lastState == HIGH && currentState == LOW) // button pressed
  {
    cancel_lastState = currentState;
    return true;
  }
    
  else // button released
    {
    cancel_lastState = currentState;
    return false;
    }
}

bool upButton()
{
static int up_lastState = HIGH;  // the previous state from the input pin  
int  currentState = digitalRead(UpButtonPin);
  if (up_lastState == HIGH && currentState == LOW) // button pressed
  {
    up_lastState = currentState;
    return true;
  }
  else // button released
    {
        up_lastState = currentState;
        return false;
        
    }
}

bool downButton()
{
    static int down_lastState = HIGH;  // the previous state from the input pin  
    int  currentState = digitalRead(DownButtonPin);
    if (down_lastState == HIGH && currentState == LOW) // button pressed
    {
        down_lastState = currentState; 
        return true;
    }
    else 
    {    down_lastState = currentState;
        return false;
    }

}



