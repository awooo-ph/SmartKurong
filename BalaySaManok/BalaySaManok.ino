#include "Sms.h"

#define ACKNOWLEDGE_PIN A1
#define FEEDER_PIN 3
#define WATER_PIN 4
#define CLEANER_PIN 5

#define SIGNAL_1 7
#define SIGNAL_2 11
#define SIGNAL_3 12
#define SIGNAL_4 13




SmsClass sms;

int currentSignal=0;

void SignalChanged(int signal)
{
    digitalWrite(SIGNAL_1,HIGH);
    digitalWrite(SIGNAL_2,HIGH);
    digitalWrite(SIGNAL_3,HIGH);
    digitalWrite(SIGNAL_4,HIGH);
    currentSignal = signal;
    if(signal==4)
        digitalWrite(SIGNAL_4,LOW);
    else if(signal==3)
        digitalWrite(SIGNAL_3,LOW);
    else if(signal==2)
        digitalWrite(SIGNAL_2,LOW);
    else if(signal==1)
        digitalWrite(SIGNAL_1,LOW);

    Serial.print("SIGNAL: ");
    Serial.println(signal);
}

void onFeeder()
{
    digitalWrite(FEEDER_PIN,LOW);
    delay(777);
    digitalWrite(FEEDER_PIN,HIGH);
    Serial.println(F("FEEDER"));
}

void onWater()
{
    digitalWrite(WATER_PIN,LOW);
    delay(777);
    digitalWrite(WATER_PIN,HIGH);
    Serial.println(F("WATER"));
}

void onCleaner()
{
    digitalWrite(CLEANER_PIN,LOW);
    delay(777);
    digitalWrite(CLEANER_PIN,HIGH);
    Serial.println(F("CLEANER"));
}

void onAlarm()
{
    Serial.println(F("ALARM"));
    onFeeder();
    onCleaner();
    onWater();
}

void NumberChanged()
{
    char buffer[20];
    sms.getNumber(buffer);
    Serial.print(F("NUMBER: "));
    Serial.println(buffer);
}

void setup()
{
    Serial.begin(9600);
    Serial.println("LOADING");

    pinMode(SIGNAL_1,OUTPUT);
    pinMode(SIGNAL_2,OUTPUT);
    pinMode(SIGNAL_3,OUTPUT);
    pinMode(SIGNAL_4,OUTPUT);

    digitalWrite(SIGNAL_1,HIGH);
    digitalWrite(SIGNAL_2,HIGH);
    digitalWrite(SIGNAL_3,HIGH);
    digitalWrite(SIGNAL_4,HIGH);

    pinMode(CLEANER_PIN,OUTPUT);
    pinMode(WATER_PIN,OUTPUT);
    pinMode(FEEDER_PIN,OUTPUT);

    digitalWrite(CLEANER_PIN,HIGH);
    digitalWrite(FEEDER_PIN,HIGH);
    digitalWrite(WATER_PIN,HIGH);

    sms.onNumberChanged(NumberChanged);
    sms.onSignalChanged(SignalChanged);
    sms.attachOnFeeder(onFeeder);
    sms.attachOnWater(onWater);
    sms.attachOnCleaner(onCleaner);
    sms.onAlarm(onAlarm);
    sms.init();
 
    sms.update();
    delay(1111);
    Serial.println("DONE");
}

void loop()
{
    sms.update();
}