#ifndef _SMS_h
#define _SMS_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

//#include <SoftwareSerial.h>
#include <AltSoftSerial.h>
#include "Settings.h"

#define BUFFER_LENGTH 147

class SmsClass
{
private:
    AltSoftSerial sms;
    char BUFFER[BUFFER_LENGTH];
    int BUFFER_INDEX = 0;
    bool _parsingData = false;
    unsigned int _initStart = 0;
    bool _isReady = false;
    bool _isRegistered = false;
    long _lastCREG = 0;
    unsigned int _lastCPIN=0;
    int _simStatus = 0;
    bool waitOk();
    bool readLine();
    int csq = 0;
    void processCSQ(char command[]);
    void parseData(char * data);
    void parseNumber(const char * data, char* number);
    bool startsWith(const char *pre, const char *str);
    void parseSMS(char* command);
    //bool isAdmin(char * number);
    bool _modemDetected = false;
    uint8_t errorCode = 0;
    bool _smsSendStarted = false;
    void(*_onSignalChanged)(int) = nullptr;
    void(*_onSimNumberChanged)(void) = nullptr;
    void(*_onSettingsReceived)(void) = nullptr;
    void(*_onAlarm)()=nullptr;
    void(*_onFeeder)()=nullptr;
    void(*_onWater)()=nullptr;
    void(*_onCleaner)()=nullptr;
    unsigned long _lastCSQ=0;
    unsigned long _lastCNUM=0;
    void parseCNUM(char * data);
    void ProcessSettings(char *);
    void ProcessSensors(char * message);
    void checkSIM();
public:
    SmsClass();
    uint8_t getError() { return errorCode; }
    bool init();
    void onAlarm(void(*)(void));
    void onSettingsReceived(void(*)(void));
    void onSignalChanged(void(*)(int));
    void onNumberChanged(void(*)(void));
    void attachOnFeeder(void(*)(void));
    void attachOnCleaner(void(*)(void));
    void attachOnWater(void(*)(void));
    bool modemDetected(){return _modemDetected;}
    bool isReady() { return _isReady; }
    int getSimStatus() {return _simStatus;};
    bool isRegistered() { return _isRegistered; }
    int getRSSI() { return csq; }
    int getSignal();
    void update();
    void send(char * number,char * message);
    
    void getIMEI(char *);
    bool startSend(char * number);
    bool write(char * message);
    bool write(char text);
    bool commitSend();
    void cancelSend();
    void restart();
    void getNumber(char *);
    //void readUnread();
    //void sendWarning(uint8_t);
};

#endif