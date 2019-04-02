#define LCD_RX 5
#define LCD_TX 4
#define WASHER 8
#define WATER 9
#define MENU_PAGE 1
#define HOME_PAGE 0
#define SYSTEM_PAGE 2
#define FEEDS_MENU 0
#define WATER_MENU 1
#define CLEANER_MENU 2

#define ACKNOWLEDGE_PIN 6
#define FEEDER_PIN 3
#define WATER_PIN 4
#define CLEANER_PIN 5
#define SIGNAL_1 7
#define SIGNAL_2 11
#define SIGNAL_3 12
#define SIGNAL_4 13

#define FEEDER_ON 77
#define FEEDER_OFF 10

#include "Settings.h"
#include <Nextion.h>
#include <NextionPage.h>
#include <NextionButton.h>
#include <NextionText.h>
#include <INextionNumericalValued.h>
#include <NextionVariableNumeric.h>
#include <NextionDualStateButton.h>

#include <PWMServo.h>
#define nexSerial Serial

Nextion nex(nexSerial);
NextionPage menuPage(nex, 0, 0, "menu");
NextionPage homePage(nex, 0, 0, "home");

NextionButton testButton(nex, MENU_PAGE, 12, "testButton");
NextionButton appButton(nex,HOME_PAGE,2,"b0");
NextionButton backButton(nex,MENU_PAGE,2,"b4");

NextionDualStateButton feedsButton(nex, MENU_PAGE,8,"b0");
NextionDualStateButton waterButton(nex, MENU_PAGE,9,"b1");
NextionDualStateButton cleanerButton(nex, MENU_PAGE,10,"b2");

NextionDualStateButton feedsButton2(nex, SYSTEM_PAGE,2,"b0");
NextionDualStateButton waterButton2(nex, SYSTEM_PAGE,3,"b1");
NextionDualStateButton cleanerButton2(nex, SYSTEM_PAGE,4,"b2");

NextionButton plusButton(nex, MENU_PAGE,3,"b5");
NextionButton minusButton(nex, MENU_PAGE,4,"b6");
NextionButton tenButton(nex, MENU_PAGE,5,"b7");
NextionButton thirtyButton(nex, MENU_PAGE,6,"b8");
NextionButton sixtyButton(nex, MENU_PAGE,7,"b9");

NextionText currentValueText(nex,MENU_PAGE,12,"currentvalue");

NextionVariableNumeric feedsValue(nex,MENU_PAGE,14,"feeds");
NextionVariableNumeric waterValue(nex,MENU_PAGE,15,"water");
NextionVariableNumeric cleanerValue(nex,MENU_PAGE,16,"cleaner");

PWMServo feeder;

int prevMinuteValue = 0;

int currentMenu = FEEDS_MENU;

void SendCurrentValue()
{
    unsigned int v = 0;
    switch (currentMenu)
    {
    case FEEDS_MENU:
        v = Settings.Current.FeederTimeout;
        break;
        case WATER_MENU:
        v = Settings.Current.WaterTimeout;
        break;
        case CLEANER_MENU:
        v = Settings.Current.WasherTimeout;
        break;
    }
    currentValueText.setTextAsNumber(v);
}

unsigned long lastSignalCheck = 0;
int currentSignal = 0;
void SendCurrentSignal()
{
    if(millis()-lastSignalCheck<1111)
        return;
    lastSignalCheck = millis();
    int signal = 0;

    if(digitalRead(SIGNAL_4)==LOW)
        signal=4;
    else if(digitalRead(SIGNAL_3)==LOW)
        signal = 3;
    else if(digitalRead(SIGNAL_2)==LOW)
        signal = 2;
    else if(digitalRead(SIGNAL_1)==LOW)
        signal = 1;

    if(signal==currentSignal) return;
    currentSignal = signal;

    char commandBuffer[15];
    snprintf(commandBuffer, 15, "signal.pic=%d", signal+1);
    nex.sendCommand(commandBuffer);
  //return nex.checkCommandComplete();
}

void testCallback(NextionEventType type, INextionTouchable *widget)
{
   
    if(type == NEX_EVENT_PUSH)
    {
        if(currentMenu == FEEDS_MENU)
        {
            feeder.write(FEEDER_ON);
        } else if(currentMenu == WATER_MENU)
        {
            digitalWrite(WATER,LOW);
        } else if(currentMenu == CLEANER_MENU)
        {
            digitalWrite(WASHER,LOW);
        }
    } else if(type == NEX_EVENT_POP)
    {
        if(currentMenu == FEEDS_MENU)
        {
            feeder.write(FEEDER_OFF);
        } else if(currentMenu == WATER_MENU)
        {
            digitalWrite(WATER,HIGH);
        } else if(currentMenu == CLEANER_MENU)
        {
            digitalWrite(WASHER,HIGH);
        }
    }
}

void appButtonCallback(NextionEventType type, INextionTouchable *widget)
{
    menuPage.show();
    feedsValue.setValue(Settings.Current.FeederTimeout);
    waterValue.setValue(Settings.Current.WaterTimeout);
    cleanerValue.setValue(Settings.Current.WasherTimeout);
    
    feedsButton.setNumberProperty("val",1);
    waterButton.setNumberProperty("val",0);
    cleanerButton.setNumberProperty("val",0);
    currentMenu = FEEDS_MENU;
    SendCurrentValue();
}



void feedsButtonCallback(NextionEventType type, INextionTouchable *widget)
{
    currentMenu = FEEDS_MENU;
}

void waterButtonCallback(NextionEventType type, INextionTouchable *widget)
{
    currentMenu = WATER_MENU;
}

void cleanerButtonCallback(NextionEventType type, INextionTouchable *widget)
{
    currentMenu = CLEANER_MENU;
}

void sixtyButtonCallback(NextionEventType type, INextionTouchable *widget)
{
    int cValue = 0;
    switch (currentMenu)
    {
    case FEEDS_MENU:
        Settings.Current.FeederTimeout=60;
        cValue = Settings.Current.FeederTimeout;
        feedsValue.setValue(cValue);
        break;
    case WATER_MENU:
        Settings.Current.WaterTimeout=60;
        cValue = Settings.Current.WaterTimeout;
        waterValue.setValue(cValue);
        break;
    case CLEANER_MENU:
        Settings.Current.WasherTimeout=60;
        cValue = Settings.Current.WasherTimeout;
        cleanerValue.setValue(cValue);
        break;
    }
    Settings.SaveConfig();
    currentValueText.setTextAsNumber(cValue);
}
void thirtyButtonCallback(NextionEventType type, INextionTouchable *widget)
{
    int cValue = 0;
    switch (currentMenu)
    {
    case FEEDS_MENU:
        Settings.Current.FeederTimeout=30;
        cValue = Settings.Current.FeederTimeout;
        feedsValue.setValue(cValue);
        break;
    case WATER_MENU:
        Settings.Current.WaterTimeout=30;
        cValue = Settings.Current.WaterTimeout;
        waterValue.setValue(cValue);
        break;
    case CLEANER_MENU:
        Settings.Current.WasherTimeout=30;
        cValue = Settings.Current.WasherTimeout;
        cleanerValue.setValue(cValue);
        break;
    }
    Settings.SaveConfig();
    currentValueText.setTextAsNumber(cValue);
}
void tenButtonCallback(NextionEventType type, INextionTouchable *widget)
{
    int cValue = 0;
    switch (currentMenu)
    {
    case FEEDS_MENU:
        Settings.Current.FeederTimeout=10;
        cValue = Settings.Current.FeederTimeout;
        feedsValue.setValue(cValue);
        break;
    case WATER_MENU:
        Settings.Current.WaterTimeout=10;
        cValue = Settings.Current.WaterTimeout;
        waterValue.setValue(cValue);
        break;
    case CLEANER_MENU:
        Settings.Current.WasherTimeout=10;
        cValue = Settings.Current.WasherTimeout;
        cleanerValue.setValue(cValue);
        break;
    }
    Settings.SaveConfig();
    currentValueText.setTextAsNumber(cValue);
}
void minusButtonCallback(NextionEventType type, INextionTouchable *widget)
{
    int cValue = 0;
    switch (currentMenu)
    {
    case FEEDS_MENU:
        Settings.Current.FeederTimeout--;
        cValue = Settings.Current.FeederTimeout;
        feedsValue.setValue(cValue);
        break;
    case WATER_MENU:
        Settings.Current.WaterTimeout--;
        cValue = Settings.Current.WaterTimeout;
        waterValue.setValue(cValue);
        break;
    case CLEANER_MENU:
        Settings.Current.WasherTimeout--;
        cValue = Settings.Current.WasherTimeout;
        cleanerValue.setValue(cValue);
        break;
    }
    Settings.SaveConfig();
    currentValueText.setTextAsNumber(cValue);
}
void plusButtonCallback(NextionEventType type, INextionTouchable *widget)
{
    int cValue = 0;
    switch (currentMenu)
    {
    case FEEDS_MENU:
        Settings.Current.FeederTimeout++;
        cValue = Settings.Current.FeederTimeout;
        feedsValue.setValue(cValue);
        break;
    case WATER_MENU:
        Settings.Current.WaterTimeout++;
        cValue = Settings.Current.WaterTimeout;
        waterValue.setValue(cValue);
        break;
    case CLEANER_MENU:
        Settings.Current.WasherTimeout++;
        cValue = Settings.Current.WasherTimeout;
        cleanerValue.setValue(cValue);
        break;
    }
    Settings.SaveConfig();
    currentValueText.setTextAsNumber(cValue);
}

bool IsCleanerOn = false;
unsigned long CleanerStarted = 0;
int FeederState = FEEDER_OFF;
unsigned long FeederStarted = 0;
bool IsWaterOn = false;
unsigned long WaterStarted = 0;

void onFeeder()
{
    if(FeederState == FEEDER_ON) return;
    FeederState = FEEDER_ON;
    FeederStarted =  millis();
    feeder.write(FEEDER_ON);
}

void onWater()
{
    if(IsWaterOn) return;
    WaterStarted = millis();
    IsWaterOn = true;
    digitalWrite(WATER,LOW);
}

void onCleaner()
{
    if(IsCleanerOn) return;
    CleanerStarted = millis();
    IsCleanerOn = true;
    digitalWrite(WASHER,LOW);
}

// the setup function runs once when you press reset or power the board
void setup() {
    pinMode(WASHER, OUTPUT);
    pinMode(WATER, OUTPUT);
    digitalWrite(WASHER,HIGH);
    digitalWrite(WATER, HIGH);

    pinMode(SIGNAL_1,INPUT_PULLUP);
    pinMode(SIGNAL_2,INPUT_PULLUP);
    pinMode(SIGNAL_3,INPUT_PULLUP);
    pinMode(SIGNAL_4,INPUT_PULLUP);

    pinMode(WATER_PIN,INPUT_PULLUP);
    pinMode(CLEANER_PIN,INPUT_PULLUP);
    pinMode(FEEDER_PIN,INPUT_PULLUP);

    pinMode(13,OUTPUT);

    nexSerial.begin(9600);
    nex.init();
    homePage.show();
    Settings.LoadConfig();

    testButton.attachCallback(&testCallback);
    feedsButton.attachCallback(&feedsButtonCallback);
    waterButton.attachCallback(&waterButtonCallback);
    cleanerButton.attachCallback(&cleanerButtonCallback);

    appButton.attachCallback(&appButtonCallback);

    plusButton.attachCallback(&plusButtonCallback);
    minusButton.attachCallback(&minusButtonCallback);
    tenButton.attachCallback(&tenButtonCallback);
    thirtyButton.attachCallback(&thirtyButtonCallback);
    sixtyButton.attachCallback(&sixtyButtonCallback);

    feeder.attach(SERVO_PIN_B);
    feeder.write(FEEDER_OFF);
    delay(1000);
    feeder.write(FEEDER_OFF);

    //Serial.println("Done!");
}

// the loop function runs over and over again until power down or reset
void loop() {
  if(FeederState == FEEDER_ON && millis()-FeederStarted>=Settings.Current.FeederTimeout*1000)
    {
        feeder.write(FEEDER_OFF);
        FeederState = FEEDER_OFF;
    }
    if(IsWaterOn && millis()-WaterStarted>=Settings.Current.WaterTimeout*1000)
    {
        digitalWrite(WATER,HIGH);
        IsWaterOn = false;
    }
    if(IsCleanerOn && millis()-CleanerStarted>=Settings.Current.WasherTimeout*1000)
    {
        digitalWrite(WASHER,HIGH);
        IsCleanerOn = false;
    }

    nex.poll();

    SendCurrentSignal();

    if(digitalRead(FEEDER_PIN)==LOW) onFeeder();
    if(digitalRead(WATER_PIN)==LOW) onWater();
    if(digitalRead(CLEANER_PIN)==LOW) onCleaner();
}
