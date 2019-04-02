#include "Sms.h"

//const char AT[]     PROGMEM = "AT\r";
//const char CMGD[]   PROGMEM = "AT+CMGD=1,1\r";
const char CPIN[]   PROGMEM = "AT+CPIN?\r";
const char CREG1[]  PROGMEM = "AT+CREG=1\r";            // Network Registration
//const char CSMS[]   PROGMEM = "AT+CSMS=1\r";            // Select Message Service
//const char CNMI[]   PROGMEM = "AT+CNMI=2,2,0,0,0\r";    // New Message Indication
//const char CMGF[]   PROGMEM = "AT+CMGF=1\r";            // Select Message Format (0:PDU; 1:TEXT)
//const char CSCS[]   PROGMEM = "AT+CSCS=\"GSM\"\r";      // Select Character Set
const char OK[]     PROGMEM = "OK";
const char ERROR[]  PROGMEM = "ERROR";
const char CallReady[]  PROGMEM = "Call Ready";
const char CLIP[]  PROGMEM = "+CLIP:";
const char NO_SIM[] PROGMEM = "+CME ERROR: SIM not inserted";

//const char* const COMMANDS[] PROGMEM = { AT,CPIN, CREG1,CSMS,CNMI,CMGF,CSCS };

char simNumber[15];

void SmsClass::parseCNUM(char* data)
{
    bool comma=false;
    bool start = false;
    int index = 0;
    int len=strlen(data);
    for(auto i=7;i<len,i++;)
    {
        if(comma && start && data[i]!='"')
        {
            simNumber[index]=data[i];
            index++;
        }
        else
        {
            if(data[i]==',') comma = true;
            if(data[i]=='"')
            {
                if(start){
                    simNumber[index]=0;
                    if(index>0 && _onSimNumberChanged)
                    {
                        _onSimNumberChanged();
                    }
                    return;
                }
                else if(comma) start = true;
                
            }
            
        }
    }
}

SmsClass::SmsClass()
{
    //sms = new SoftwareSerial(rx, tx);
}

bool SmsClass::init()
{
    if (_isReady) return true;
    
    sms.begin(9600);
    
    /*unsigned int start = millis();
    while (!sms)
    {
        if(millis()-start>7777)
        {
            _modemDetected = false;
            return false;
        }
    }
*/

    sms.println(F("AT"));
    
    _initStart = millis();
    _modemDetected = waitOk();
    return _modemDetected;
}

void SmsClass::onAlarm(void(* callback)())
{
    _onAlarm = callback;
}

void SmsClass::onSettingsReceived(void(* callback)())
{
    _onSettingsReceived = callback;
}

void SmsClass::onSignalChanged(void(* callback)(int))
{
    _onSignalChanged=callback;
}

void SmsClass::onNumberChanged(void(* callback)())
{
    _onSimNumberChanged=callback;
}

void SmsClass::attachOnCleaner(void(* callback)())
{
    _onCleaner=callback;
}
void SmsClass::attachOnWater(void(* callback)())
{
    _onWater=callback;
}
void SmsClass::attachOnFeeder(void(* callback)())
{
    _onFeeder=callback;
}

bool SmsClass::readLine()
{
    //while(true){
        while(sms.available()>0){
            byte b = sms.read();
            if(b==0) continue;
            _modemDetected = true;
            Serial.write(b);

            if (b == '\n' || b == '\r') {
                BUFFER[BUFFER_INDEX] = '\0';
                for(auto i=BUFFER_INDEX;i<BUFFER_LENGTH;i++)
                    BUFFER[i]=0;
                if(BUFFER_INDEX==0) return false;
                BUFFER_INDEX = 0;
                return true;
            }
            BUFFER[BUFFER_INDEX] = b;
            BUFFER_INDEX++;
        }
    //}
    return false;
}

/// Returns the signal strength (0-4)
int SmsClass::getSignal()
{
    if(!_isRegistered) return 0;
    if (csq == 99 || csq==0) return -1;
    if (csq < 7) return 0;
    if (csq < 10) return 1;
    if (csq < 15) return 2;
    if (csq < 20) return 3;
    return 4;
}

void SmsClass::update()
{
    if(_parsingData) return;
    
    checkSIM();

    if(sms.available())
        if(readLine()) parseData(BUFFER);

    while (Serial.available())
    {
        sms.write(Serial.read());
    }
    
    auto timeout = 44777;
    if(_isReady && millis()-_lastCSQ>timeout)
    {
        _lastCSQ = millis();
        sms.println(F("AT+CSQ"));
    }
}

void SmsClass::send(char* number, char* text)
{
    if (!number || strlen(number) == 0 || !text || strlen(text) == 0) return;
    if (!number || strlen(number) < 7) return;
    if(!(number[0]=='0' || number[0]=='+')) return;

    startSend(number);
    write(text);
    commitSend();
}

void SmsClass::getIMEI(char * imei)
{
    if(_parsingData || _smsSendStarted) return;
    sms.println(F("AT+GSN"));
    while(!readLine()){}
    strcpy(imei,BUFFER);
}

bool SmsClass::startSend(char* number)
{
    if(!_isReady) return false;
    if (!number || strlen(number) ==0) return false;
    if (_smsSendStarted) return false;
    _smsSendStarted = true;
    sms.print(F("AT+CMGS=\""));
    sms.print(number);
    sms.print(F("\"\r"));
    delay(1111);
    return true;
}

bool SmsClass::write(char* message)
{
    if (!_smsSendStarted) return false;
    sms.write(message);
    return true;
}

bool SmsClass::write(char text)
{
    if (!_smsSendStarted) return false;
    sms.write(text);
    return true;
}

bool SmsClass::commitSend()
{
    if (!_smsSendStarted) return false;
    sms.println();
    sms.println(F("https://goo.gl/RBy5eb"));
    sms.write(26);
    waitOk();
    _smsSendStarted = false;
    return true;
}

void SmsClass::cancelSend()
{
    if(!_smsSendStarted) return;
    sms.println(F("777"));
    sms.println(F("777"));
    sms.write(3);
}

void SmsClass::restart()
{

}

void SmsClass::getNumber(char *number)
{
    strcpy(number, simNumber);
}


//void SmsClass::readUnread()
//{
    /*if(_parsingData || _smsSendStarted) return;
    sms.println(F("AT+CMGL=\"REC UNREAD\""));*/
//}

//void SmsClass::sendWarning(uint8_t level)
//{
    /*for (auto number : Settings.Current.NotifyNumbers){
        if(!startSend(number)) return;
        sms.print(F("WARNING! Water level at "));
        sms.print(Settings.Current.SensorName);
        sms.print(F(" has reached to LEVEL "));
        sms.print(level);
        sms.print(F("."));
        commitSend();
        delay(777);
    }*/
//}

unsigned long waitStart = 0;
bool SmsClass::waitOk()
{
    waitStart = millis();
    while (millis() - waitStart < 4444)
    {
        
        if(!readLine())continue;

        if (BUFFER && strlen(BUFFER) > 0) {
            if (strcasecmp_P(BUFFER, OK) == 0)
                return true;

            auto res = strstr_P(ERROR, BUFFER);
            if (res)
                return false;
        }
    }
    return false;
}

void SmsClass::processCSQ(char command[])
{
    char c[20];
    for (auto x = 5; x < strlen(command); x++)
    {
        if (command[x] == ',')
        {
            auto newCsq = atoi(c);
            if(csq!=newCsq)
            {
                csq = newCsq;
                if(_onSignalChanged) _onSignalChanged(getSignal());
            }
            return;
        }
        c[x - 5] = command[x];
    }
}

bool SmsClass::startsWith(const char* pre, const char* str)
{
    //if (sizeof(str) < sizeof(pre) || sizeof(str) == 0 || sizeof(pre) == 0) return false;
    return strncmp(pre, str, strlen(pre)) == 0;
}

void SmsClass::parseNumber(const char* str, char * number)
{
    int len = strlen(str);
    
    int index = 0;
    for (int i = 7; i < len; i++)
    {
        if (str[i] == '"')
        {
            number[index] = '\0';
            return;
        }
        number[index] = str[i];

        index++;
    }
}

//#ifndef UNRESTRICTED
//bool SmsClass::isAdmin(char* number)
//{
//    char * _number = "00000000000";
//
//    if (number[0] == '0') strcpy(_number, number);
//    else if (number[0] == '+')
//        for (auto i = 3; i < strlen(number); i++)
//            _number[i - 2] = number[i];
//    else return false;
//
//    for (auto i = 0; i < 4; i++)
//        if (strcmp(_number, Settings.Current.Monitor[i]) == 0) return true;
//
//    return false;
//}
//#endif



void SmsClass::checkSIM()
{
    if(_simStatus == -1)
    {
        csq = 0;
        return;
    }

    if(millis()-_initStart<7777) return;

    if(millis()-_lastCPIN<7777) return;
    if(_parsingData && _smsSendStarted) return;
    _lastCPIN = millis();
    if(_simStatus==0)
    {
        sms.println(F("AT+CPIN?"));
    } else if(strlen(simNumber)==0)
    {
        sms.println(F("AT+CNUM"));
    } else if(getSignal()==-1)
    {
        sms.println(F("AT+CSQ"));
    }
}

void SmsClass::ProcessSettings(char * message)
{
    auto ci = 0;
    auto vi = 0;
    char value[147];
    for (auto i = 1; i < strlen(message); i++)
    {
        if (message[i] == ';')
        {
            if(vi>0){
                value[vi] = 0;
                vi = 0;
                if (ci == 0)
                {
                    message[47 - 1] = 0;
                    //strcpy(Settings.Current.SensorName, value);
                }
                else if (ci == 1)
                {
                    //Settings.Current.SirenLevel[0] = atoi(value);
                }
                else if (ci == 2)
                {
                    //Settings.Current.SirenLevel[1] = atoi(value);
                }
                else if (ci == 3)
                {
                    //Settings.Current.SirenLevel[2] = atoi(value);
                }
                else if (ci == 4)
                {
                    //Settings.Current.WarningLevel = atoi(value);
                }
            }
            ci++;
        } else
        {
            value[vi] = message[i];
            vi++;    
        }

    }

    //Settings.SaveConfig();
}

void SmsClass::parseSMS(char* command)
{
    _parsingData = true;
    char number[15];
    parseNumber(command,number);

    BUFFER[0] = 0;
    while(strlen(BUFFER)==0){
        while(!readLine()){}
    }
        
    //char * code = "777";//"https://goo.gl/RBy5eb";
    //if(strcmp(code,BUFFER)==0)
    //{
       // strcpy(Settings.Current.Monitor,number);
       // Settings.SaveConfig();
       // startSend(number);
       // sms.println(F("444"));
       // commitSend();
       // return;
    //}
    
    switch (BUFFER[0]) {
    case '1':
    case 'f':
    case 'F':
        if(_onFeeder) _onFeeder();
        return;
    case '2':
    case 'w':
    case 'W':
        if(_onWater) _onWater();
        return;
    case '3':
    case 'c':
    case 'C':
        if(_onCleaner) _onCleaner();
        return;
    
    default:;
    }

    sms.println(F("AT"));
    while(!readLine()){}
    sms.println(F("AT+CMGD=1,1"));

    _parsingData = false;
}

void SmsClass::parseData(char* command)
{
    if (strlen(command) == 0) return;

    if(strcmp_P(command,CallReady)==0)
    {
        _isReady = true;
    } else

    if(strcmp_P(command,NO_SIM)==0)
    {
        _simStatus = -1;
    } else

    if(startsWith("+CPIN:",command))
    {
        _isReady = true;
        _simStatus = 1;
        if(strlen(simNumber)==0)
            sms.println(F("AT+CNUM"));
    } else

    if (startsWith("+CSQ:", command))
    {
        processCSQ(command);
    } else

    if(startsWith("+CNUM:",command))
    {
        parseCNUM(command);
    } else

    if (startsWith("+CREG: ", command))
    {
        _isRegistered = startsWith("+CREG 1",command);
        _isReady = true;
        
        if(!(_parsingData || _smsSendStarted))
            sms.println(F("AT+CSQ"));
    } else if (strstr_P(command,CLIP) || startsWith("RING",command)) //Hangup call
    {
        sms.println(F("ATH"));
        if(_onAlarm) _onAlarm();
    }
    else if (startsWith("+CMT:", command)) //New message
        parseSMS(command);
}