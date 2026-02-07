//------------------------------------------------------------------------------------------------------------------------------------
const String ver = "v.1.4";
const String id  = "Контроль 2 ";
//контроль состояния
//основной канал автора на ютубе  "НеОбзор58"            https://www.youtube.com/channel/UC6mMZ4GGXMdpSGEy-j1EnxQ
//второй   канал автора на ютубе  "Доступная автоматика" https://www.youtube.com/channel/UCxfwRP66zE4zwn6lgvS6wQg
//сайт и страница проекта                                http://www.simple-automation.ru/publ/proekty/
//
//     желающим поддержать автора: карта Сбера 676280489001157828 получатель Григорий Валерьевич Ц.
//
//------------------------------------------------------------------------------------------------------------------------------------

//#define USE_LOG
//#define USE_DHT
//#define USE_LCD
#define USE_PIPE

//пин на датчики температуры
const byte  pinOneWire PROGMEM = 0;

//пины на управление реле
const byte swt1_pin PROGMEM =  1;  // Реле1

//файлы настроек
const String SensorConfFile = "sensor.cfg";
const String NameSettingsFile = "settings.cfg";

const byte pinBuiltinLed PROGMEM = LED_BUILTIN;
#include <sntp.h>;

#include "Date.h";
/*#include "Mail.h";

  String smtpHost, smtpUser, smtpPass, mailTo, mailSubject;
  uint16_t smtpPort = 465;
  bool useMail, systemStartedMail, useMailSwt1, useMailSwt2, useMailSwt3, useMailSwt4;*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
//#include <ESP8266SSDP.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>
#include <FS.h>
#include <ArduinoJson.h>
//-------------------
#ifdef USE_DHT
#include <DHT.h>
int8_t t_dht, h_dht;
int8_t h_dht_tek, t_dht_tek;
const byte  pinDHT PROGMEM = 3;
DHT dht(pinDHT, DHT11);
uint32_t DHT_control;
#endif

#define ONE_WIRE_BUS pinOneWire
#define TEMPERATURE_PRECISION 9
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

ESP8266WebServer server(80);
WiFiClient espClient;

/*
  //----MQTT----------------------------------------------------------------------------------------------------------------

  https://mqtt.4api.ru/
  Сервер Кол-во подключений не ограничено
  srv1.mqtt.4api.ru
  Протокол|Порт Без (SSL)
  websocket:9123
  mqtt:9124

  Пользователь используется для авторизации на всех устройствах
  user_xxxxxxxx
  Пароль отображается в открытом виде один раз после генерации
  pass_xxxxxxxx

  Как это работает?

  [Как создать топик?]
  Топик создается в личном кабинете и должен иметь уникальное имя (к примеру lamp),
  тогда топик будет иметь полное название ваш_логин/ваше_имя_топика, к примеру user_11223344/lamp
  Топики могут иметь многоуровневые названия, к примеру user_11223344/lamp/status, user_11223344/lamp/switch/1 и т.д.

  [Как обращаться к топику?]
  При обращении к топику, необходимо указывать его полное имя: user_11223344/lamp

  [Как подключить Arduino, ESP8266, ESP12, ESP32 ?]
  Мы проверяли работу с библиотекой PubSubClient от автора Nicholas O'Leary

  [Какой клиент использовать для мобильного телефона?]
  Мы используем клиент IoT MQTT Panel для Android.

  [Моё устройство не подключается, что делать?]
  Проверьте правильно ли Вы указываете логин, пароль, адрес и порт для mqtt сервера, правильно ли указываете топик и
  подписку на него. Используйте веб клиент по адресу для проверки. Зайдите в личный кабинет и убедитесь, что статус
  сервера "в работе", возможно проводятся работы, но об этом мы как правило сообщаем на главной страницы.

  // клиенту нужно разрешить/подписать на swt/# (swt/0, swt/1,swt/3,swt/4) и добавить движки в publish
  // клиенту нужно разрешить/подписать на temp/# (temp/t0, temp/t1....) и добавить значения в subscribe
  // клиенту нужно разрешить/подписать на pipe/# (pipe/l, pipe/lm) и добавить значения в subscribe
  // клиенту нужно разрешить/подписать на dht/# (dht/t, dht/h) и добавить значения в subscribe
  


*/

#include <PubSubClient.h>// MQTT library

PubSubClient mqttClient(espClient);
bool useMQTT, connectMqtt, saveValMQTT, addUserTopic;
String mqttServer, mqttUser, mqttUserPassword, mqttClientId, mqttTopicPref;
uint16_t mqttServerPort;
uint16_t mqttReconnectSec;
uint32_t nextTimeMQTT, mqtt_control, mqtt_delay;

//---------------------------------------------------
bool resumeState;
const String NameResumeStateFile = "resumeState.cfg";
//---------------------------------------------------настройки Wi-Fi------------------------
const char* const ssidAP PROGMEM = "ESP_Rele2";
const char* const passwordAP PROGMEM = "Pa$$w0rd";

bool ApMode, AccessMode;
String AccessLogin, AccessPassword;
String ssid, password, domain;
uint8_t ip1, ip2, ip3, ip4;
uint8_t ms1, ms2, ms3, ms4;
uint8_t gt1, gt2, gt3, gt4;
uint8_t dn1, dn2, dn3, dn4;
byte arduino_mac[] = { 0xAE, 0xEE, 0xAA, 0xEE, 0xFD, 0xDD };

//--------------------------------------------------разные константы и переменные --------
String NameProg;//для с
const byte maxStrParamLength = 32;
const char configSign[4] = { '#', 'G', 'R', 'S' };
char PerevodStr = 0x0D;
char VozvrStr = 0x0A;
String str_info;
uint32_t Other_control, t_control;

bool writeSysLog, writeSenseLog;
bool systemStarted;
const String NameSysLogFile = "system.log";
const String NameMesureLogFile = "mesure.log";
uint32_t logFileSize;//размер файла лога (файлов будет четыре system.log, mesure.log, кода будет превышен размер произойдет
// копирование в system01.log и mesure01.log, а исходные файлы начнутся сначала)
uint16_t log_t;//колич минут через котор пишется в лог температура
uint32_t t_control_log_t;//таймер на лог по температуре

//-------------------------------------------------датчик температуры DS18B20-------------
const uint8_t NSenseMax = 4;
uint8_t NSenseFact;
byte insideThermometer[NSenseMax][8];
float tempC[NSenseMax], tempC_now[NSenseMax], tempC_mqtt[NSenseMax];

int8_t t[NSenseMax];
String tNameSens[NSenseMax];
uint8_t error_read_ds_Max;//количество ошибок (подряд) при опросе
uint16_t t_msec_get_ds;//интервал опроса датчиков, Мсек
uint8_t Zaderj, jamp_t;
uint16_t error_read_ds[NSenseMax], error_read_ds_all, error_read_correct_ds[NSenseMax], error_read_correct_ds_all;
char chGrad;
String stGrad;

int32_t time_pub[NSenseMax];
float temp_warn[NSenseMax];
float temp_warn_delt[NSenseMax];
bool on_warn[NSenseMax];

//-----исполнительные устройства--------------
bool autoMode;
bool swt1, swt2;//состояние движка
bool prev_swt1, prev_swt2;//предыдущее состояние движка
bool relay1Level, relay2Level;// уровень которым управляется реле (т.е. вкл по 0 или по 1)
bool now_relay1, now_relay2; //настоящее состояние реле (т.е. сейчас реле включено или нет)
//-------------------------------------Relay----------------
String str_t_working_Relay1, str_t_working_Relay2;//время работы строка 00:00:00
uint32_t t_Relay1, t_Relay2; //момент срабатывания в миллис()
uint32_t t_work_Relay1, t_work_Relay2;//время работы в миллис

//------------------------------------------------------------работа со временем-------------
String TimeOn;//прошло времени с момента включения контроллера
uint32_t startTime, startMs, nextTime;
String ntpServer1, ntpServer2, ntpServer3;
uint32_t updateInterval;
int8_t timeZone;
const char* const strNever PROGMEM = "NoNTP";
const char* const defNtpServer1 PROGMEM = "pool.ntp.org";
const char* const defNtpServer2 PROGMEM = "ntp1.stratum1.ru";
const char* const defNtpServer3 PROGMEM = "ntp1.vniiftri.ru";

const uint32_t defUpdateInterval = 3600; // 1 hour
const int8_t defTimeZone = 3; // GMT+3 Moscow TZ
String timeStr;
const uint32_t noTime = (uint32_t)0;
const char* const noDef = '\0';

bool ntpUpd, setTimeAccompl;
int8_t now_hour, now_min, now_sec, now_day, now_month;
uint8_t now_wd;
int16_t now_year;
uint32_t now_timeUnx;
uint8_t countFailNtp;
uint8_t countFailMqtt;

//-----------------------------------------------------------------------------------------------------------------------
#ifdef USE_PIPE
const byte Interrupt_Pin PROGMEM = 3;

volatile uint16_t count_imp;
float count_imp_all, count_imp_all_mqtt;
uint16_t liter_hour;
uint16_t liter_min;
uint32_t currentTime, loopTime, loop_blink_time ;
float liter;

uint16_t count_imp_litr, time_calc;
//-----------------------------------------------
ICACHE_RAM_ATTR void getFlow ()
{
  count_imp++;
  // if (digitalRead(Interrupt_Pin) == HIGH) digitalWrite(pinBuiltinLed, LOW);
  // else digitalWrite(pinBuiltinLed, HIGH);
}
#endif
//-----------------------------------------------
void addStringToLog(String logFile, String timeDate, String logMessage, String openMode = "a") {

  if (((writeSysLog) && (logFile == NameSysLogFile)) || ((writeSenseLog) && (logFile == NameMesureLogFile))) {
    if (setTimeAccompl == 1) {
#ifdef USE_LOG
      Serial.print("Try write log to " + logFile);
#endif
      File f = SPIFFS.open("/" + logFile, openMode.c_str());

      if (!f)  {
#ifdef USE_LOG
        Serial.println(F("....failed"));
#endif
      }
      else  {
#ifdef USE_LOG
        Serial.print(F("(")); Serial.print(f.size()); Serial.print(F(")")); Serial.print(F("Open mode ")); Serial.print(openMode);
#endif
        if (openMode == "w") {
#ifdef USE_LOG
          Serial.println(F("....open Ok"));
#endif
        }
        else {
          if (f.size() > logFileSize * 1024) {
            f.close();
            String logFileRez = logFile.substring(0, logFile.length() - 4) + "01.log";

            if (SPIFFS.exists("/" + logFileRez))  {
              SPIFFS.remove("/" + logFileRez);
#ifdef USE_LOG
              Serial.println(F("....old removed"));
#endif
            }
            if (!SPIFFS.rename("/" + logFile, "/" + logFileRez))  {
#ifdef USE_LOG
              Serial.println(F("....rename failed"));
#endif
              return;
            }
            else {
#ifdef USE_LOG
              Serial.println(F("....rename Ok"));
#endif
              addStringToLog(logFile, timeDate, logMessage, "w");
            }
          }
        }
#ifdef USE_LOG
        Serial.println(F("..... Writing log file "));
#endif
        String strM;
        strM = timeDate + " " + logMessage;
#ifdef USE_LOG
        Serial.print(strM);
#endif
        f.println(strM);
        f.close();
#ifdef USE_LOG
        Serial.println(F(" ....done."));
#endif
      }
    }
  }
}
//----------------------------------------------------------------------------------


void sendStringToMail(String timeDate, String logMessage) {
  /*if ((!ApMode) && (WiFi.status() == WL_CONNECTED) && (setTimeAccompl) && (useMail)) {
    if (sendMail(smtpHost, smtpPort, smtpUser, smtpPass, mailTo, mailSubject, timeDate + " " + logMessage)) {
    #ifdef USE_LOG
      Serial.print(F("Mail sended through "));
      Serial.println(smtpHost);
    #endif
    }
    else
    {
    #ifdef USE_LOG
      Serial.print(F("Error sending mail through "));
      Serial.print(smtpHost);
      Serial.println(F("!"));
    #endif
    }
    }*/
}

//-------------------------------------------------------------------------------------------------------------------------------------------
void MydigitalWrite(int pin, bool state, bool RLevel) {

  if (RLevel == 0) state = !state;//если реле управляется низким уровнем - инвертируем
  if (digitalRead(pin) != state) {//если состояние отлично от текущего
    digitalWrite(pin, state);     //то прописываем новое состояние
  }
}
//--------------------------------------------------------------
String quoteEscape(const String& str)
{
  String result = "";
  int start = 0, pos;

  while (start < str.length()) {
    pos = str.indexOf('"', start);
    if (pos != -1) {
      result += str.substring(start, pos) + F("&quot;");
      start = pos + 1;
    } else {
      result += str.substring(start);
      break;
    }
  }
  return result;
}
//---------------------------------------------
bool readConfig()
{
  uint16_t offset = 0;
#ifdef USE_LOG
  Serial.println(F("Reading config from EEPROM"));
#endif
  for (byte i = 0; i < sizeof(configSign); i++)
  {
    if (EEPROM.read(offset + i) != configSign[i])
      return false;
  }
  offset += sizeof(configSign);
  offset = readEEPROMString(offset, ssid);
  offset = readEEPROMString(offset, password);
  offset = readEEPROMString(offset, AccessLogin);
  offset = readEEPROMString(offset, AccessPassword);
  offset = readEEPROMString(offset, NameProg);

  EEPROM.get(offset, ApMode); offset += sizeof(ApMode);
  EEPROM.get(offset, AccessMode); offset += sizeof(AccessMode);

  EEPROM.get(offset, ip1);       offset += sizeof(ip1);
  EEPROM.get(offset, ip2);       offset += sizeof(ip2);
  EEPROM.get(offset, ip3);       offset += sizeof(ip3);
  EEPROM.get(offset, ip4);       offset += sizeof(ip4);

  EEPROM.get(offset, ms1);       offset += sizeof(ms1);
  EEPROM.get(offset, ms2);       offset += sizeof(ms2);
  EEPROM.get(offset, ms3);       offset += sizeof(ms3);
  EEPROM.get(offset, ms4);       offset += sizeof(ms4);

  EEPROM.get(offset, gt1);       offset += sizeof(gt1);
  EEPROM.get(offset, gt2);       offset += sizeof(gt2);
  EEPROM.get(offset, gt3);       offset += sizeof(gt3);
  EEPROM.get(offset, gt4);       offset += sizeof(gt4);

  EEPROM.get(offset, dn1);       offset += sizeof(dn1);
  EEPROM.get(offset, dn2);       offset += sizeof(dn2);
  EEPROM.get(offset, dn3);       offset += sizeof(dn3);
  EEPROM.get(offset, dn4);       offset += sizeof(dn4);

  return true;
}
//-------------------------------------
void writeConfig()
{
  uint16_t offset = 0;
#ifdef USE_LOG
  Serial.println(F("Writing config to EEPROM"));
#endif
  EEPROM.put(offset, configSign);

  offset += sizeof(configSign);
  offset = writeEEPROMString(offset, ssid);
  offset = writeEEPROMString(offset, password);
  offset = writeEEPROMString(offset, AccessLogin);
  offset = writeEEPROMString(offset, AccessPassword);
  offset = writeEEPROMString(offset, NameProg);

  EEPROM.put(offset, ApMode); offset += sizeof(ApMode);
  EEPROM.put(offset, AccessMode); offset += sizeof(AccessMode);

  EEPROM.put(offset, ip1); offset += sizeof(ip1);
  EEPROM.put(offset, ip2); offset += sizeof(ip2);
  EEPROM.put(offset, ip3); offset += sizeof(ip3);
  EEPROM.put(offset, ip4); offset += sizeof(ip4);

  EEPROM.put(offset, ms1);       offset += sizeof(ms1);
  EEPROM.put(offset, ms2);       offset += sizeof(ms2);
  EEPROM.put(offset, ms3);       offset += sizeof(ms3);
  EEPROM.put(offset, ms4);       offset += sizeof(ms4);

  EEPROM.put(offset, gt1);       offset += sizeof(gt1);
  EEPROM.put(offset, gt2);       offset += sizeof(gt2);
  EEPROM.put(offset, gt3);       offset += sizeof(gt3);
  EEPROM.put(offset, gt4);       offset += sizeof(gt4);

  EEPROM.put(offset, dn1);       offset += sizeof(dn1);
  EEPROM.put(offset, dn2);       offset += sizeof(dn2);
  EEPROM.put(offset, dn3);       offset += sizeof(dn3);
  EEPROM.put(offset, dn4);       offset += sizeof(dn4);

  EEPROM.commit();
}
//-----------------------------------------------------------------------
uint16_t readEEPROMString(uint16_t offset, String& str)
{
  char buffer[maxStrParamLength + 1];
  buffer[maxStrParamLength] = 0;
  for (byte i = 0; i < maxStrParamLength; i++) {
    if (! (buffer[i] = EEPROM.read(offset + i)))
      break;
  }
  str = String(buffer);
  return offset + maxStrParamLength;
}
//-----------------------------------------------------------------------
uint16_t writeEEPROMString(uint16_t offset, const String& str) {
  for (byte i = 0; i < maxStrParamLength; i++) {
    if (i < str.length())
      EEPROM.write(offset + i, str[i]);
    else
      EEPROM.write(offset + i, 0);
  }
  return offset + maxStrParamLength;
}

//---------------------------------------------------WiFi setup functions--------------------------------------------------
bool setupWiFiAsStation() {
  const uint32_t timeout = 40000;
  uint32_t maxtime = millis() + timeout;
#ifdef USE_LOG
  Serial.println(F("Connecting to "));
  Serial.println(ssid);
#endif


  WiFi.mode(WIFI_STA);

  if (ssid.length() == 0) return false;

  WiFi.begin(ssid.c_str(), password.c_str());

  if ((ip1 + ip2 + ip3 + ip4) != 0) {
    IPAddress ip(ip1, ip2, ip3, ip4);
    IPAddress subnet(ms1, ms2, ms4, ms4);
    IPAddress dns(dn1, dn2, dn3, dn4);
    IPAddress gateway(gt1, gt2, gt3, gt4);
    WiFi.config(ip, gateway, subnet, dns);
  }
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(pinBuiltinLed, HIGH); delay(500);
    digitalWrite(pinBuiltinLed, LOW); delay(500);

#ifdef USE_LOG
    Serial.print(".");
#endif
    if (millis() >= maxtime) {
#ifdef USE_LOG
      Serial.println();
      Serial.println(ssid);
      Serial.println(password);
      Serial.println(F(" fail!"));
#endif
      return false;
    }
  }
#ifdef USE_LOG
  Serial.println();
  Serial.println(F("WiFi connected"));
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());
#endif

  return true;
}
//----------------------------------------------------------------
void setupWiFiAsAP() {
#ifdef USE_LOG
  Serial.print(F("Configuring access point "));
  Serial.println(ssidAP);
#endif
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssidAP, passwordAP);
  digitalWrite(pinBuiltinLed, HIGH); delay(250);
#ifdef USE_LOG
  Serial.print(F("IP address: "));
  Serial.println(WiFi.softAPIP());
#endif


}
//----------------------------------------------------------------
void setupWiFi() {
  if (ApMode || (! setupWiFiAsStation())) setupWiFiAsAP();

  server.begin();
#ifdef USE_LOG
  Serial.println(F("HTTP server started (use '/update' url to OTA update)"));
#endif
}
//------------------------------------
String AddNull (String StrForAdd)
{
  if (StrForAdd.length() == 1)
    return "0" + StrForAdd;
  if (StrForAdd.length() == 0)
    return "00" + StrForAdd;

  return StrForAdd;
}
//--------------------------------------------------------------------------------------


#include "handles.h"
#include "h_switch.h"
#include "upg_spiffs.h"
#include "Control_other.h"
//#include "SSDP.h"

//---------------------------------------------------------------------------------------
void openSettingsFile () {


  if (!SPIFFS.exists("/" + NameSettingsFile))
  { //файл настроек не существует
  } else {
#ifdef USE_LOG
    Serial.println(F("Открываем ")); Serial.println(NameSettingsFile);
#endif
    File f = SPIFFS.open("/" + NameSettingsFile, "r");
    if (!f) {
#ifdef USE_LOG
      Serial.println(F("..не получилось открыть файл"));
#endif
    } else {
#ifdef USE_LOG
      Serial.println(F("..читаем файл"));
#endif
      String s;
      uint16_t st_start, st_end, st_len;
      for (uint8_t i = 0; i < 20; i++)  {
        s = f.readStringUntil(PerevodStr); s.replace(F("\n"), "");
        //s.remove(s.length() - 1);
        st_start = s.indexOf("{"); st_end = s.indexOf("}") + 1;

        if (s.indexOf(F("END")) == -1) {
          if ((st_start != -1) && (st_end != -1)) {
            s = s.substring(st_start, st_end); st_len = s.length();
#ifdef USE_LOG
            Serial.println(s);
#endif
            DynamicJsonBuffer jsonBuffer;   //Memory pool
            //DynamicJsonDocument doc(1024);
            JsonObject& root = jsonBuffer.parseObject(s);
            //deserializeJson(doc, s);
            if (root.success()) {
              int8_t hh, mm, ss;
              uint8_t wd;
              int8_t d, m;
              int16_t y;


              if (root["Vol"] == 3) {
#ifdef USE_LOG
                Serial.println("Vol3");
#endif
                resumeState = root["resumeState"];
                relay1Level = root["relayLevel"][0];
                relay2Level = root["relayLevel"][1];

                const char* ntpServer = root["ntpServer"][0]; if (ntpServer != noDef) ntpServer1 = String(ntpServer);
                ntpServer = root["ntpServer"][1]; if (ntpServer != noDef) ntpServer2 = String(ntpServer);
                ntpServer = root["ntpServer"][2]; if (ntpServer != noDef) ntpServer3 = String(ntpServer);
                updateInterval = root["updateInterval"];
                timeZone = root["timeZone"];
                ntpUpd = root["ntpUpd"];

                Zaderj = root["Zaderj"];
                jamp_t = root["jamp_t"];

                writeSysLog = root["writeSysLog"];
                writeSenseLog = root["writeSenseLog"];
                log_t = root["log_t"];
                logFileSize = root["logFileSize"];

                error_read_ds_Max = root["error_read_ds_Max"];
                t_msec_get_ds = root["t_msec_get_ds"];

#ifdef USE_PIPE
                count_imp_litr = root["count_imp_litr"];
                time_calc = root["time_calc"];
#endif

                useMQTT = root["useMQTT"];
                addUserTopic = root["addUserTopic"];
                const char* mqttS = root["mqttServer"]; if (mqttS != noDef) mqttServer = String(mqttS);
                mqttServerPort = root["mqttServerPort"];
                mqttReconnectSec = root["mqttReconnectSec"];

                const char* mqttTopicP = root["mqttTopicPref"]; if (mqttTopicP != noDef) mqttTopicPref = String(mqttTopicP);
                const char* mqttU = root["mqttUser"]; if (mqttU != noDef) mqttUser = String(mqttU);
                const char* mqttUP = root["mqttUserPassword"]; if (mqttUP != noDef) mqttUserPassword = String(mqttUP);
                const char* mqttC = root["mqttClientId"]; if (mqttC != noDef) mqttClientId = String(mqttC);

                /*                useMail = root["useMail"];
                                const char* smtpH = root["smtpHost"]; if (smtpH != noDef) smtpHost = String(smtpH);
                                smtpPort = root["smtpPort"];
                                const char* smtpU = root["smtpUser"]; if (smtpU != noDef) smtpUser = String(smtpU);
                                const char* smtpPs = root["smtpPass"]; if (smtpPs != noDef) smtpPass = String(smtpPs);
                                const char* mailT = root["mailTo"]; if (mailT != noDef) mailTo = String(mailT);
                                const char* mailS = root["mailSubject"]; if (mailS != noDef) mailSubject = String(mailS);
                                useMailSwt1 = root["useMailSwt"][0];
                                useMailSwt2 = root["useMailSwt"][1];*/
              }
            } else {
#ifdef USE_LOG
              Serial.println(F("..Проблемы с парсингом файла"));
#endif
            }
          } else {
#ifdef USE_LOG
            Serial.println(F("..Не форматная строка в файле"));
#endif
          }
        } else { //нашли слово END в конце файла
#ifdef USE_LOG
          Serial.println(F("..дошли до конца файла"));
#endif
          break;
        }
      }
      f.close();
    }
  }
}
//------------------------------------------------------
void openSensorConfFile() {

  if (!SPIFFS.exists("/" + SensorConfFile))
  { //файл настроек не существует
  }
  else {
#ifdef USE_LOG
    Serial.println(F("Открываем ")); Serial.println(SensorConfFile);
#endif
    File f = SPIFFS.open("/" + SensorConfFile, "r");
    if (!f) {
#ifdef USE_LOG
      Serial.println(F(" не получилось открыть файл адресов датчиков"));
#endif
    }
    else {
#ifdef USE_LOG
      Serial.println(F(" читаем файл адресов датчиков "));
#endif
      String s;
      uint8_t st_start, st_end, st_len;
      for (uint8_t i = 0; i <= NSenseMax; i++)  {
        s = f.readStringUntil(PerevodStr); s.replace(F("\n"), "");
        st_start = s.indexOf(F("{")); st_end = s.indexOf(F("}")) + 1;
#ifdef USE_LOG
        Serial.println(s);
#endif
        if (s.indexOf(F("END")) == -1) {
#ifdef USE_LOG
          Serial.println("есть");
#endif
          if ((st_start != -1) && (st_end != -1)) {
            s = s.substring(st_start, st_end); st_len = s.length();
            DynamicJsonBuffer jsonBuffer;   //Memory pool
            //DynamicJsonDocument doc(1024);
            JsonObject& root = jsonBuffer.parseObject(s);
            //deserializeJson(doc, s);
            if (root.success()) {
              t[i] = root["t"];
              for (uint8_t k = 0; k < 8; k++) {
                insideThermometer[i][k] = root["frame"][k];
              }
              const char* tNameS = root["tNameSens"]; if (tNameS != noDef) tNameSens[i] = String(tNameS);
              temp_warn[i] = root["temp_warn"];
              temp_warn_delt[i] = root["temp_warn_delt"];
            }
            else {
#ifdef USE_LOG
              Serial.println(F("Проблемы с парсингом (адресов датчиков)"));
#endif
            }
          }
          else {
#ifdef USE_LOG
            Serial.println(F("Не форматная строка (адресов датчиков)"));
#endif
          }
        }
        else { //нашли слово END в конце файла
          NSenseFact = i;
#ifdef USE_LOG
          Serial.println(F("Дошли до конца файла адресов датчиков"));
          Serial.print(F("Всего датчиков в файле адресов = ")); Serial.println(NSenseFact);
#endif
#ifdef USE_LCD
          lcd.setCursor(6, 0);
          lcd.print(F("ok"));
          delay(1500);
          lcd.clear();
#endif
          break;
        }
      }
      f.close();
    }
  }
}
//------------------------------------------------------
void openResumeStateFile() {

  if (resumeState == 1) {
    if (!SPIFFS.exists("/" + NameResumeStateFile))
    { //файл настроек не существует
    }
    else {
#ifdef USE_LOG
      Serial.println(F("Открываем ")); Serial.println(NameResumeStateFile);
#endif
      File f = SPIFFS.open("/" + NameResumeStateFile, "r");
      if (!f) {
#ifdef USE_LOG
        Serial.println(F("..не получилось открыть файл"));
#endif
      }
      else {
#ifdef USE_LOG
        Serial.println(F("..читаем файл"));
#endif
        String s;
        uint8_t st_start, st_end, st_len;
        for (uint8_t i = 0; i < 20; i++)  {
          s = f.readStringUntil(PerevodStr); s.replace(F("\n"), "");
          st_start = s.indexOf(F("{")); st_end = s.indexOf(F("}")) + 1;
#ifdef USE_LOG
          Serial.println(s);
#endif
          if (s.indexOf(F("END")) == -1) {
            if ((st_start != -1) && (st_end != -1)) {
              s = s.substring(st_start, st_end); st_len = s.length();
              DynamicJsonBuffer jsonBuffer;   //Memory pool
              //DynamicJsonDocument doc(1024);
              JsonObject& root = jsonBuffer.parseObject(s);
              //deserializeJson(doc, s);
              if (root.success()) {
                autoMode = root["autoMode"];
                swt1 = root["rele1"];
                swt2 = root["rele2"];

              }
              else {
#ifdef USE_LOG
                Serial.println(F("..проблемы с парсингом"));
#endif
              }
            }
            else {
#ifdef USE_LOG
              Serial.println(F("..не форматная строка"));
#endif
            }
          }
          else { //нашли слово END в конце файла
#ifdef USE_LOG
            Serial.println(F("..дошли до конца файла"));
            Serial.println(F("..состояние реле восстановлено"));
#endif
#ifdef USE_LCD
            lcd.setCursor(6, 0);
            lcd.print(F("ok"));
            delay(1500);
            lcd.clear();
#endif
            break;
          }
        }
        f.close();

      }
    }
  }
}
//-----------------------------------------------------------------------------------------
String mqtt_topic(String topic) {
  String ret_topic;
  if (addUserTopic) {
    ret_topic = mqttUser;
    if (strlen(mqttTopicPref.c_str()) != 0) {
      ret_topic += F("/");
      ret_topic += mqttTopicPref;
    }
    ret_topic += F("/");
    ret_topic += topic;
    return ret_topic;
  }
  else {
    if (strlen(mqttTopicPref.c_str()) != 0) {
      ret_topic = mqttTopicPref;
      ret_topic += F("/");
    }
    ret_topic += topic;
    return ret_topic;
  }
}
//---------------------------------------
void mqttConnect() {
  //if (countFailMqtt < 20) {
  if (useMQTT && !mqttClient.connected()) {
#ifdef USE_LOG
    Serial.println();
    Serial.print(F("Trying to connect MQTT... "));
    Serial.print(mqttServer);
    Serial.print(F(":"));
    Serial.println(String(mqttServerPort));
#endif
    mqttClient.setServer(mqttServer.c_str(), mqttServerPort);
    mqttClient.setCallback(mqttCallback);

    //for (uint8_t i = 0; i < 3; i++) {
#ifdef USE_LOG
    Serial.println(F("..Connecting to MQTT..."));
#endif
    if (mqttClient.connect(mqttClientId.c_str(), mqttUser.c_str(), mqttUserPassword.c_str())) {

      countFailMqtt = 0;

      //что бы при переподключении к Mqtt прописать на сервере текущее состояние движков,
      //иначе при подключении движки установятся как на сервере
      delay(20); mqttClient.publish(mqtt_topic(F("swt/1")).c_str(), String(swt1).c_str(), saveValMQTT);
      delay(20); mqttClient.publish(mqtt_topic(F("swt/2")).c_str(), String(swt2).c_str(), saveValMQTT);

      delay(20); mqttClient.publish(mqtt_topic(F("time/on")).c_str(), String(uint32_t( millis() / 1000 / 60)).c_str(), saveValMQTT);

      mqttClient.subscribe(mqtt_topic(F("swt/1")).c_str());
      mqttClient.subscribe(mqtt_topic(F("swt/2")).c_str());

#ifdef USE_LOG
      Serial.println(F("connected. "));
#endif
      //break;
    } else {
#ifdef USE_LOG
      Serial.print(F("failed with state = ")); Serial.println(mqttClient.state());
#endif
      countFailMqtt++;
    }
    //}
  }
  //}
}

//-----------------------------------------------------------------------------------------
void setup() {


  pinMode(swt1_pin, OUTPUT);//реле 1


  //-------- для себя чтоб не ждать установки --------------
  digitalWrite(swt1_pin, LOW);

#ifdef USE_LOG
  Serial.begin(115200);
  Serial.println();
#endif
#ifdef USE_DHT
  dht.begin();
#endif

  chGrad = (char)49840; stGrad = chGrad; stGrad = stGrad + F("C");

  pinMode(pinBuiltinLed, OUTPUT);


  prev_swt1 = LOW;

  for (byte i = 0; i < NSenseMax; i++) {
    tempC[i] = -127;
    tempC_now[i] = -127;
  }

  error_read_ds_all = 0;
  error_read_correct_ds_all = 0;

  EEPROM.begin(1024);
  if (! readConfig()) {
#ifdef USE_LOG
    Serial.println(F("EEPROM is empty!"));
#endif
    digitalWrite(swt1_pin, !LOW);
  }



  if (! SPIFFS.begin()) {
#ifdef USE_LOG
    Serial.println(F("Unable to mount SPIFFS!"));
#endif
    return;
  }
  else {
    FSInfo fs_info;
    SPIFFS.info(fs_info);
#ifdef USE_LOG
    Serial.print(F("SPIFFS    totalBytes ")); Serial.println(fs_info.totalBytes);
    Serial.print(F("SPIFFS     usedBytes ")); Serial.println(fs_info.usedBytes);
    Serial.print(F("SPIFFS     blockSize ")); Serial.println(fs_info.blockSize);
    Serial.print(F("SPIFFS      pageSize ")); Serial.println(fs_info.pageSize);
    Serial.print(F("SPIFFS  maxOpenFiles ")); Serial.println(fs_info.maxOpenFiles);
    Serial.print(F("SPIFFS maxPathLength ")); Serial.println(fs_info.maxPathLength);
    /*      struct FSInfo {
          size_t totalBytes;
          size_t usedBytes;
          size_t blockSize;
          size_t pageSize;
          size_t maxOpenFiles;
          size_t maxPathLength;
      };*/
#endif

    openSettingsFile();
    openSensorConfFile();
    openResumeStateFile();
  }

  setupWiFi();
  server.begin();
#ifdef USE_LOG
  Serial.println(F("HTTP server started"));
#endif
  server.on("/", handleRoot);
  server.on("/index.html", handleRoot);
  server.on("/switch", h_switch);
  server.on("/select", h_select);
  server.on("/selectProm", h_selectProm);
  //server.on("/Relay2", h_Relay2);
  //server.on("/Relay1", h_Relay1);
  server.on("/save", h_save);

  server.on("/reboot", h_reboot);
  server.on("/wifi", h_wifi);
  server.on("/data_mainPage", HTTP_GET, h_data_mainPage);

  server.onNotFound(handleNotFound);
  server.on("/spiffs", HTTP_GET, handleSPIFFS);
  server.on("/spiffs", HTTP_POST, handleFileUploaded, handleFileUpload);
  server.on("/spiffs", HTTP_DELETE, handleFileDelete);
  server.on("/update", HTTP_GET, handleUpdate);
  server.on("/update", HTTP_POST, handleSketchUpdated, handleSketchUpdate);
  server.on("/timeConf", handleTimeConfig);
  server.on("/settime", handleSetTime);
  server.on("/gettimer", handleGetTimer);
  server.on("/data_settimer", handleSetTimer);
  server.on("/mqtt", h_mqtt);
  //server.on("/mail", h_mail);
  if (ntpServer1.length() < 2) ntpServer1 = FPSTR(defNtpServer1);
  if (ntpServer2.length() < 2) ntpServer2 = FPSTR(defNtpServer2);
  if (ntpServer3.length() < 2) ntpServer3 = FPSTR(defNtpServer3);

  if (updateInterval < 60) updateInterval = defUpdateInterval;

  sntp_set_timezone(timeZone);
  if (ntpServer1.length()) sntp_setservername(0, (char*)ntpServer1.c_str());
  if (ntpServer2.length()) sntp_setservername(1, (char*)ntpServer2.c_str());
  if (ntpServer3.length()) sntp_setservername(2, (char*)ntpServer3.c_str());
  sntp_init();
  startTime = noTime;
  startMs = millis();
  now_timeUnx = 0;
  setTimeAccompl = 0;
  systemStarted = 0;
  t_control_log_t = 0;
  nextTimeMQTT = 0;
  mqtt_control = 0;
  if (logFileSize == 0) logFileSize = 50;
  countFailNtp = 0;
  countFailMqtt = 0;

  connectMqtt = 0;
  mqtt_delay = 0;
  saveValMQTT = 1;
  //systemStartedMail = 0;

  //SSDP_init();
#ifdef USE_PIPE
  //для датчика потока
  pinMode(Interrupt_Pin, INPUT);
  attachInterrupt(Interrupt_Pin, getFlow, FALLING);

  currentTime = millis();
  loopTime = currentTime;
  if (count_imp_litr == 0) count_imp_litr = 450;
  if (time_calc == 0) time_calc = 1000;
#endif
}
//----------------------------------------------------------------------------
void loop() {
  const uint32_t timeout = 100000; // 30сек
  static uint32_t nextTime = timeout;
  uint32_t t_sec, vol_hour, vol_min, vol_sec;

  if ((!ApMode) && (WiFi.status() != WL_CONNECTED) && ((WiFi.getMode() == WIFI_STA) || ((int32_t)(millis() - nextTime) >= 0)))  {
    setupWiFi();
    nextTime = millis() + timeout;
  }
  server.handleClient();

  t_sec = millis() / 1000;
  vol_hour = t_sec / 60 / 60; vol_min = t_sec / 60 - vol_hour * 60; vol_sec = t_sec - vol_hour * 60 * 60 - vol_min * 60;
  TimeOn = AddNull(String(vol_hour)) + ":" + AddNull(String(vol_min)) + ":" + AddNull(String(vol_sec));
  //----------------
  vol_hour = t_work_Relay1 / 60 / 60; vol_min = t_work_Relay1 / 60 - vol_hour * 60; vol_sec = t_work_Relay1 - vol_hour * 60 * 60 - vol_min * 60;
  str_t_working_Relay1  = AddNull(String(vol_hour)) + ":" + AddNull(String(vol_min)) + ":" + AddNull(String(vol_sec));
  //----------------
  vol_hour = t_work_Relay2 / 60 / 60; vol_min = t_work_Relay2 / 60 - vol_hour * 60; vol_sec = t_work_Relay2 - vol_hour * 60 * 60 - vol_min * 60;
  str_t_working_Relay2  = AddNull(String(vol_hour)) + ":" + AddNull(String(vol_min)) + ":" + AddNull(String(vol_sec));
  //----------------

  working_other();

  delay (10);

  //синхронизация времени
  //if (countFailNtp < 20) {
  if ((WiFi.getMode() == WIFI_STA) && (WiFi.status() == WL_CONNECTED) && (ntpUpd)) {
    static uint32_t nextTime = millis();
    if ((nextTime != noTime) && (millis() >= nextTime)) {
#ifdef USE_LOG
      Serial.print(F("Getting time from NTP server(s)... "));
#endif
      uint32_t now = sntp_get_current_timestamp();
      if ((now == noTime) || (now < 1000000000)) {
        nextTime = millis() + 30 * 1000; //при наличии интернета, но неудаче обновления повтор через 90 сек
#ifdef USE_LOG
        Serial.println(F("FAIL!"));
#endif
        countFailNtp++;
      }
      else {
        countFailNtp = 0;
#ifdef USE_LOG
        Serial.println(now);
        Serial.println(F("success"));
#endif
        setTimeAccompl = 1;
        startTime = now;
        startMs = millis();
        if (updateInterval)
          nextTime = startMs + updateInterval * 1000;
        else
          nextTime = noTime;
      }
    }
  }
  else {

  }
  //}
  now_timeUnx = startTime + (millis() - startMs) / 1000;
  parseUnixTime(now_timeUnx, now_hour, now_min, now_sec, now_wd, now_day, now_month, now_year);
  timeStr = dateWdTimeToStr(now_timeUnx);

}
//--------------------------------------------------------------------------------------
