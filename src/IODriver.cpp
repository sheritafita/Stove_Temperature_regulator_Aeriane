#include"Arduino.h"
#include"IODriver.h"
#include "multi_channel_relay.h"




const int OkButtonPin =  26;

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
<<<<<<< HEAD
=======
    pinMode(relay2_1, OUTPUT);
    pinMode(relay2_2, OUTPUT);
    pinMode(relay2_3, OUTPUT);
    pinMode(Led_pin, OUTPUT);
>>>>>>> 3de042f (Enable test mode, toggle 3 resistances each 5s)
}

void initRelay()
{
    relay1.begin(0x11);
    relay2.begin(0x12);
    stopHeating();
<<<<<<< HEAD
    ActivateAutomaticMode();
    
=======
>>>>>>> 3de042f (Enable test mode, toggle 3 resistances each 5s)
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
<<<<<<< HEAD
    //Serial.println("Resistance 1 on");
    relay2.turn_on_channel(1);
=======
    digitalWrite(relay2_1, LOW);
    delay(100);
>>>>>>> 3de042f (Enable test mode, toggle 3 resistances each 5s)
}

void turn0nResistance2()
{
<<<<<<< HEAD
    relay2.turn_on_channel(2);
=======
   digitalWrite(relay2_2, LOW);
   delay(100);
>>>>>>> 3de042f (Enable test mode, toggle 3 resistances each 5s)
}

void turn0nResistance3()
{
<<<<<<< HEAD
    relay2.turn_on_channel(3);
=======
   digitalWrite(relay2_3, LOW);
   delay(100);
>>>>>>> 3de042f (Enable test mode, toggle 3 resistances each 5s)
}


void turn0ffResistance1()
{
<<<<<<< HEAD
    relay2.turn_off_channel(1);
=======
    digitalWrite(relay2_1, HIGH);
    delay(100);
>>>>>>> 3de042f (Enable test mode, toggle 3 resistances each 5s)
}

void turn0ffResistance2()
{
<<<<<<< HEAD
    relay2.turn_off_channel(2);
}
void turn0ffResistance3()
{
    relay2.turn_off_channel(3);
=======
   digitalWrite(relay2_2, HIGH);
   delay(100);
}
void turn0ffResistance3()
{
  digitalWrite(relay2_3, HIGH);
  delay(100);
>>>>>>> 3de042f (Enable test mode, toggle 3 resistances each 5s)
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


bool OkButton()
{
<<<<<<< HEAD
static int ok_lastState = HIGH;  // the previous state from the input pin  
int  currentState = digitalRead(OkButtonPin);
  if (ok_lastState == HIGH && currentState == LOW) // button pressed
   { 
    ok_lastState = currentState;
    return true;
=======
static int OkLastState = HIGH;
int  currentState = digitalRead(OkButtonPin);
  if (OkLastState == HIGH && currentState == LOW)
  {
    OkLastState = currentState;
    return true;
  }

  else
    {
    OkLastState = currentState;
    return false;
>>>>>>> 3de042f (Enable test mode, toggle 3 resistances each 5s)
    }
  else // button released
  {
    ok_lastState = currentState;
    return false;
   }

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



