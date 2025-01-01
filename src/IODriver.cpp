#include"Arduino.h"
#include"IODriver.h"





void init_GPIO ()
{
    pinMode(OkButtonPin, INPUT_PULLUP);
    pinMode(CancelButtonPin, INPUT_PULLUP);
    pinMode(UpButtonPin, INPUT_PULLUP);
    pinMode(DownButtonPin, INPUT_PULLUP);
    pinMode(relay2_1, OUTPUT);
    pinMode(relay2_2, OUTPUT);
    pinMode(relay2_3, OUTPUT);
}

void initRelay()
{
    stopHeating();

}


void turn0nResistance1()
{
    digitalWrite(relay2_1, HIGH);
    delay(100);
}

void turn0nResistance2()
{
     digitalWrite(relay2_2, HIGH);
   delay(100);
}

void turn0nResistance3()
{
   digitalWrite(relay2_3, HIGH);
   delay(100);
}


void turn0ffResistance1()
{
    digitalWrite(relay2_1, LOW);
    delay(100);
}

void turn0ffResistance2()
{
   digitalWrite(relay2_2, LOW);
   delay(100);
}
void turn0ffResistance3()
{
  digitalWrite(relay2_3, LOW);
  delay(100);
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
static int Ok_lastState = HIGH;
int  currentState = digitalRead(OkButtonPin);
  if (Ok_lastState == HIGH && currentState == LOW)
  {
    Ok_lastState = currentState;
    return true;
  }

  else
    {
    Ok_lastState = currentState;
    return false;
    }
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



