#include"Arduino.h"
#include"IODriver.h"




<<<<<<< HEAD
const int OkButtonPin =  26;

const int UpButtonPin = 9;
const int DownButtonPin = 10;



Multi_Channel_Relay relay1;
Multi_Channel_Relay relay2;
=======
>>>>>>> 8f80f66f4c6342970f765635d5f8c8b36718842c

void init_GPIO ()
{
    pinMode(OkButtonPin, INPUT_PULLUP);
    pinMode(CancelButtonPin, INPUT_PULLUP);
    pinMode(UpButtonPin, INPUT_PULLUP);
    pinMode(DownButtonPin, INPUT_PULLUP);
<<<<<<< HEAD
<<<<<<< HEAD
=======
    pinMode(relay2_1, OUTPUT);
    pinMode(relay2_2, OUTPUT);
    pinMode(relay2_3, OUTPUT);
    pinMode(Led_pin, OUTPUT);
>>>>>>> 3de042f (Enable test mode, toggle 3 resistances each 5s)
=======
    pinMode(relay2_1, OUTPUT);
    pinMode(relay2_2, OUTPUT);
    pinMode(relay2_3, OUTPUT);
>>>>>>> 8f80f66f4c6342970f765635d5f8c8b36718842c
}

void initRelay()
{
    stopHeating();
<<<<<<< HEAD
<<<<<<< HEAD
    ActivateAutomaticMode();
    
=======
>>>>>>> 3de042f (Enable test mode, toggle 3 resistances each 5s)
=======

>>>>>>> 8f80f66f4c6342970f765635d5f8c8b36718842c
}


void turn0nResistance1()
{
<<<<<<< HEAD
<<<<<<< HEAD
    //Serial.println("Resistance 1 on");
    relay2.turn_on_channel(1);
=======
    digitalWrite(relay2_1, LOW);
    delay(100);
>>>>>>> 3de042f (Enable test mode, toggle 3 resistances each 5s)
=======
    digitalWrite(relay2_1, HIGH);
    delay(100);
>>>>>>> 8f80f66f4c6342970f765635d5f8c8b36718842c
}

void turn0nResistance2()
{
<<<<<<< HEAD
<<<<<<< HEAD
    relay2.turn_on_channel(2);
=======
   digitalWrite(relay2_2, LOW);
   delay(100);
>>>>>>> 3de042f (Enable test mode, toggle 3 resistances each 5s)
=======
     digitalWrite(relay2_2, HIGH);
   delay(100);
>>>>>>> 8f80f66f4c6342970f765635d5f8c8b36718842c
}

void turn0nResistance3()
{
<<<<<<< HEAD
<<<<<<< HEAD
    relay2.turn_on_channel(3);
=======
   digitalWrite(relay2_3, LOW);
   delay(100);
>>>>>>> 3de042f (Enable test mode, toggle 3 resistances each 5s)
=======
   digitalWrite(relay2_3, HIGH);
   delay(100);
>>>>>>> 8f80f66f4c6342970f765635d5f8c8b36718842c
}


void turn0ffResistance1()
{
<<<<<<< HEAD
<<<<<<< HEAD
    relay2.turn_off_channel(1);
=======
    digitalWrite(relay2_1, HIGH);
    delay(100);
>>>>>>> 3de042f (Enable test mode, toggle 3 resistances each 5s)
=======
    digitalWrite(relay2_1, LOW);
    delay(100);
>>>>>>> 8f80f66f4c6342970f765635d5f8c8b36718842c
}

void turn0ffResistance2()
{
<<<<<<< HEAD
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
=======
   digitalWrite(relay2_2, LOW);
   delay(100);
}
void turn0ffResistance3()
{
  digitalWrite(relay2_3, LOW);
  delay(100);
>>>>>>> 8f80f66f4c6342970f765635d5f8c8b36718842c
}


void stopHeating()
{
    turn0ffResistance1();
    turn0ffResistance2();
    turn0ffResistance3();
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
=======
static int Ok_lastState = HIGH;
int  currentState = digitalRead(OkButtonPin);
  if (Ok_lastState == HIGH && currentState == LOW)
  {
    Ok_lastState = currentState;
>>>>>>> 8f80f66f4c6342970f765635d5f8c8b36718842c
    return true;
  }

  else
    {
<<<<<<< HEAD
    OkLastState = currentState;
    return false;
>>>>>>> 3de042f (Enable test mode, toggle 3 resistances each 5s)
    }
  else // button released
  {
    ok_lastState = currentState;
    return false;
   }

=======
    Ok_lastState = currentState;
    return false;
    }
>>>>>>> 8f80f66f4c6342970f765635d5f8c8b36718842c
}



bool CancelButton()
{
static int cancelLastState = HIGH;
int  currentState = digitalRead(CancelButtonPin);
  if (cancelLastState == HIGH && currentState == LOW)
  {
    cancelLastState = currentState;
    return true;
  }

  else
    {
    cancelLastState = currentState;
    return false;
    }
}

bool upButton()
{
static int upLastState = HIGH;
int  currentState = digitalRead(UpButtonPin);
  if (upLastState == HIGH && currentState == LOW)
  {
    upLastState = currentState;
    return true;
  }
  else
    {
        upLastState = currentState;
        return false;

    }
}

bool downButton()
{
    static int downLastState = HIGH;
    int  currentState = digitalRead(DownButtonPin);
    if (downLastState == HIGH && currentState == LOW)
    {
        downLastState = currentState;
        return true;
    }
    else
    {    downLastState = currentState;
        return false;
    }

}



