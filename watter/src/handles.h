void LoginContr()
{
  if (AccessMode == true)
  {
    char _AccessLogin[sizeof(AccessLogin)], _AccessPassword[sizeof(AccessPassword)];
    AccessLogin.toCharArray(_AccessLogin, sizeof(_AccessLogin));
    AccessPassword.toCharArray(_AccessPassword, sizeof(_AccessPassword));
    if (!server.authenticate(_AccessLogin, _AccessPassword)) return server.requestAuthentication();
  }
}
//---------------------------------------------------------------------------
String refreshTempMessage(String url)
{
  String message;
  message = F("function refreshTempMessage(){\n");
  message +=   F("var request = new XMLHttpRequest();\n");
  message +=   F("request.open('GET', '/data_tempMess', true);\n");
  message +=   F("request.onreadystatechange = function(){\n");
  message +=       F("if (request.readyState == 4) {\n");
  message +=         F("var data = JSON.parse(request.responseText);\n ");
  message +=         F("if (data.nom_mess != 255) {location.href ='/tempMess?nom_mess='+data.nom_mess+'&url="); message += url; message += F("'}}};\n");
  message +=   F("request.send(null);\n");
  message +=   F("}\n");
  message += F("setInterval(refreshTempMessage, 1000);\n");

  return message;
}
//---------------------------------------------------------------------------
String textStyle()
{
  String message;

  message = F(".textStyle1 {");
  message += F("width: 25px; height: 20px; text-align: center;");
  message += F("text-align: center;");
  message += F("}");
  message += F(".textStyle2 {");
  message += F("width: 45px; height: 20px; text-align: center;");
  message += F("text-align: center;");
  message += F("}");
  message += F(".textStyle3 {");
  message += F("width: 100px; height: 20px; text-align: center;");
  message += F("text-align: center;");
  message += F("}");

  return message;
}
//--------------------------------------------
String fillTimer()
{
  String message;


  return message;
}
//---------------------------------------------------------------------------
void handleRoot()
{
  LoginContr();

  String message;
  message = F("<!DOCTYPE html>");
  message += F("<html>");
  message += F("<head>");
  message += F("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  message += F("<meta charset=\"utf-8\">");
  message += F("<title>");
  message += id;//Управление скважиной на ESP
  message += F("</title>");
  message += F("<style type=\"text/css\">");
  message += F(".menu_but {width: 200px; text-align: center; padding: 5px;}");

  message += F(".checkboxRed {vertical-align:top; margin:0 3px 0 0; width:17px; height:17px;}");
  message += F(".checkboxRed + label {cursor:pointer;}");
  message += F(".checkboxRed:not(checked) {position:absolute; opacity:0; }");
  message += F(".checkboxRed:not(checked) + label {position:relative; padding:0 0 0 50px;}");
  message += F(".checkboxRed:not(checked) + label:before {content:''; position:absolute; top:-4px; left:0; width:50px; height:26px; border-radius:13px; background:#CDD1DA;box-shadow:inset 0 2px 3px rgba(0,0,0,.2);}");
  message += F(".checkboxRed:not(checked) + label:after {content:''; position:absolute; top:-2px; left:2px; width:22px; height:22px; border-radius:10px; background:#FFF; box-shadow:0 2px 5px rgba(0,0,0,.3); transition:all .1s;}");
  message += F(".checkboxRed:checked + label:before {background:#ec1212;}");
  message += F(".checkboxRed:checked + label:after {left:26px;}");

  message += F(".checkboxGreen {vertical-align:top; margin:0 3px 0 0; width:17px; height:17px;}");
  message += F(".checkboxGreen + label {cursor:pointer;}");
  message += F(".checkboxGreen:not(checked) {position:absolute; opacity:0; }");
  message += F(".checkboxGreen:not(checked) + label {position:relative; padding:0 0 0 50px;}");
  message += F(".checkboxGreen:not(checked) + label:before {content:''; position:absolute; top:-4px; left:0; width:50px; height:26px; border-radius:13px; background:#CDD1DA;box-shadow:inset 0 2px 3px rgba(0,0,0,.2);}");
  message += F(".checkboxGreen:not(checked) + label:after {content:''; position:absolute; top:-2px; left:2px; width:22px; height:22px; border-radius:10px; background:#FFF; box-shadow:0 2px 5px rgba(0,0,0,.3); transition:all .1s;}");
  message += F(".checkboxGreen:checked + label:before {background:#41d233;}");
  message += F(".checkboxGreen:checked + label:after {left:26px;}");

  message += F(".LevelPinGreen {vertical-align:top; margin:0 3px 0 0; width:17px; height:17px;}");
  message += F(".LevelPinGreen:not(checked) {position:absolute; opacity:0; }");
  message += F(".LevelPinGreen:not(checked) + label {position:relative; padding:0 0 0 26px;}");
  message += F(".LevelPinGreen:not(checked) + label:before {content:''; position:absolute; top:-4px; left:0; width:26px; height:26px; border-radius:13px; background:#CDD1DA;}");
  message += F(".LevelPinGreen:checked + label:before {background:#41d233;}");
  message += F(".LevelPinGreen:checked + label:after {left:26px;}");

  message += F(".LevelPinRed {vertical-align:top; margin:0 3px 0 0; width:17px; height:17px;}");
  message += F(".LevelPinRed:not(checked) {position:absolute; opacity:0; }");
  message += F(".LevelPinRed:not(checked) + label {position:relative; padding:0 0 0 26px;}");
  message += F(".LevelPinRed:not(checked) + label:before {content:''; position:absolute; top:-4px; left:0; width:26px; height:26px; border-radius:13px; background:#CDD1DA;}");
  message += F(".LevelPinRed:checked + label:before {background:#ec1212;}");
  message += F(".LevelPinRed:checked + label:after {left:26px;}");

  message += F(".column-1 {height: 26px; text-align: left; vertical-align: middle;background-color: lightyellow;}");
  message += F(".column-2 {text-align: center; vertical-align: middle;}");

  message += F("</style>");
  message += F("<script type=\"text/javascript\">\n");
  message += F("function openUrl(url) {\n");
  message += F("var request = new XMLHttpRequest();\n");
  message += F("request.open('GET', url, true);\n");
  message += F("request.send(null);\n ");
  message += F("};\n ");
  message += F("function refreshData(){\n");
  message +=   F("var request = new XMLHttpRequest();\n");
  message +=   F("request.open('GET', '/data_mainPage', true);\n");
  message +=   F("request.onreadystatechange = function(){\n");
  message +=       F("if (request.readyState == 4) {\n");
  message +=         F("var data = JSON.parse(request.responseText);\n ");
  //message +=         F("document.getElementById('autoMode').checked = data.autoMode;\n");

  message +=         F("document.getElementById('relay1').checked = data.relay1;\n");
  message +=         F("document.getElementById('now_relay1').checked = data.now_relay1;\n");
  //message +=         F("document.getElementById('now_relay2').checked = data.now_relay2;\n");
  message +=         F("document.getElementById('str_t_working_Relay1').innerHTML = data.str_t_working_Relay1;\n");

  for (byte i = 0; i < NSenseFact; i++) {
    if (t[i] == 1) {
      message += F("document.getElementById('tempC"); message += String(i); message += F("').innerHTML = data.tempC"); message += String(i); message += F(";\n");
    }
  }
  message +=         F("document.getElementById('TimeOn').innerHTML = data.TimeOn;\n");
  message +=         F("document.getElementById('timeStr').innerHTML = data.timeStr;\n");
  message +=         F("document.getElementById('MQTT').innerHTML = data.MQTT;\n");
  message +=         F("document.getElementById('info').innerHTML = data.info;\n");
  message +=         F("}};\n");
  //message +=         F("if (data.beepeep > 0) {document.location.href = \"/alert\"}}};\n");
  message +=   F("request.send(null);\n");
  message +=   F("}\n");
  message += F("setInterval(refreshData, 1000);\n");

  message += F("</script>\n");
  message += F("</head>\n");
  message += F("<body>\n");
  message += F("<form>\n");

  message += F("<h2>");
  message += id;
  message += F("<a href=\"http://www.simple-automation.ru/publ/proekty/kontroller_dlja_skvazhiny_na_esp8266_nodemcu_lua/kontroller-dlja-skvazhiny-na-esp8266-nodemcu-v3-lua/19-1-0-15\" target=\"_blank\">");
  message += ver + F("</a></h2><p>\n");

  /*  message +=  F("<input type=\"checkbox\" class=\"checkboxGreen\" id=\"autoMode\" onchange=\"openUrl('/switch?autoMode=' + this.checked);\" ");
    if (autoMode == HIGH) message += F("checked");
    message += F("/><label for=\"autoMode\">Автоматический режим работы</label></p>");
  */
  message += F("<span id=\"timeStr\">.</span><br>\n");
  message += F("Время работы: <span id=\"TimeOn\">.</span><br>\n");
  message += F("MQTT: <span id=\"MQTT\">.</span></p>\n");

  message += F("<table cellspacing=\"1\" border=\"0\" style=\"vertical-align:middle; align:left\" >");
  message += F("<tr>");
  message +=  F("<th></th>");
  message +=  F("<th></th>");
  message +=  F("<th>Pin</th>");
  message +=  F("<th>В работе</th>");
  message +=  F("<th>След вкл</th>");
  message += F("</tr>");

  message += F("<tr>");
  message +=  F("<th class=\"column-1\">");
  message += F("Реле 1");
  message +=  F("</th>");

  message +=  F("<td class=\"column-2\">");
  message +=  F("<input type=\"checkbox\" class=\"checkboxGreen\" id=\"relay1\" onchange=\"openUrl('/switch?swt1=' + this.checked);\" ");
  if (swt1 == HIGH) message += F("checked");
  message += F("/><label for=\"relay1\"></label>");
  message +=  F("</td>");

  message +=  F("<td class=\"column-2\">");
  message +=  F("<input type=\"checkbox\" class=\"LevelPinGreen\" id=\"now_relay1\" ");
  if (now_relay1 == HIGH) message += F("checked");
  message += F("/><label for=\"now_relay1\"></label>");
  message +=  F("</td>");
  message +=  F("<td class=\"column-2\"><span id=\"str_t_working_Relay1\">.</span></td>");
  message += F("</tr>");


  message += F("<tr align=\"left\">");
  message +=  F("<td colspan=\"5\">");
  for (byte i = 0; i < NSenseFact; i++) {
    if (t[i] == 1) {
      message += F("<span id=\"tempC"); message += String(i); message += F("\">.</span> &#176С ");
      if (tNameSens[i].length() > 0) {
        message += F(" /"); message += tNameSens[i] + F("/");
      }
      message += F("<br />\n");
    }
  }

  message += F("<p>\n");
  message += F("Инфо : <span id=\"info\">.</span></p>\n");

  message += F("<input type = \"button\" class=\"menu_but\" id=\"butt_adjContr\" value=\"Реле и датчики\" onclick=\"location.href='/select';\" /><br>\n");

  message += F("<p>");
  message += F("<input type = \"button\" class=\"menu_but\" id=\"butt_adjWifi\" value=\"Настройка Wi-fi\" onclick=\"location.href='/wifi';\" /><br>\n");
  message += F("<input type = \"button\" class=\"menu_but\" id=\"butt_adjMQTT\" value=\"Настройка MQTT\" onclick=\"location.href='/mqtt';\" /><br>\n");
  //message += F("<input type = \"button\" class=\"menu_but\" id=\"butt_adjMail\" value=\"Настройка почты\" onclick=\"location.href='/mail';\" /><br>\n");
  message += F("<input type = \"button\" class=\"menu_but\" id=\"butt_adjTime\" value=\"Настройка времени\" onclick=\"location.href='/timeConf';\" /><br>\n");
  message += F("<input type = \"button\" class=\"menu_but\" id=\"butt_adjDevPrg\" value=\"Обновление прошивки\" onclick=\"location.href='/update';\" /><br>\n");
  message += F("<input type = \"button\" class=\"menu_but\" value=\"Менеджер файлов\" onclick=\"location.href='/spiffs';\" /><br>\n");
  message += F("<p>");
  message += F("<input type = \"button\" class=\"menu_but\" value=\"Перезагрузка\" id=\"butt_reboot\" onclick=\"if (confirm('Перезагрузить модуль?') == true) location.href='/reboot'\" /><br>\n");



  message +=  F("</td>");
  message += F("</tr>");
  message += F("</table>");


  message += F("</form>\n");
  message += F(" </body>\n");
  message += F(" </html>\n");
  server.send(200, F("text/html"), message);
}

//---------------------------------------------------------------------------------
void save_ResumeStateFile()
{
#ifdef USE_LOG
  Serial.print("Try save " + NameResumeStateFile);
#endif
  File f = SPIFFS.open("/" + NameResumeStateFile, "w");
  if (!f)
  {
#ifdef USE_LOG
    Serial.println(F("....failed"));
#endif
  }
  else
  {
#ifdef USE_LOG
    Serial.println(F("..... Writing SPIFFS file "));
#endif

    String strM;
    strM = F("{");
    strM += F("\"autoMode\":"); strM += String(autoMode) + F(",");
    strM += F("\"rele1\":"); strM += String(swt1) + F(",");
    strM += F("\"rele2\":"); strM += String(swt2);
    strM += F("}");
#ifdef USE_LOG
    Serial.println(strM);
#endif

    f.println(strM);
    f.print(F("END"));
    f.close();
#ifdef USE_LOG
    Serial.println(F(" done."));
#endif
  }
}
//-----------------------------------------------------------------------------------------------------------------------------

void save_SensorConfFile()
{
#ifdef USE_LOG
  Serial.println("Try open " + SensorConfFile);
#endif
  File f = SPIFFS.open("/" + SensorConfFile, "w");
  if (!f)
  {
#ifdef USE_LOG
    Serial.println(F("file open failed"));
#endif
  }
  else
  {
#ifdef USE_LOG
    Serial.println(F(" Writing SPIFFS file "));
#endif
    String strM;

    for (int i = 0; i < NSenseFact; i++) {
      strM = F("{\"t\":"); strM += String(t[i]) + ",";
      strM += F("\"frame\":[");
      for (uint8_t k = 0; k < 8; k++) {
        strM += String(insideThermometer[i][k]);
        if (k != 7) strM += F(",");
      }
      strM += F("],");
      strM += F("\"tNameSens\":\""); strM += String(tNameSens[i]) + F("\",");
      strM += F("\"temp_warn\":"); strM += String(temp_warn[i]) + F(",");
      strM += F("\"temp_warn_delt\":"); strM += String(temp_warn_delt[i]) ;
      strM += F("}");
#ifdef USE_LOG
      Serial.println(strM);
#endif
      f.println(strM);
    }

    f.print(F("END"));
    f.close();
#ifdef USE_LOG
    Serial.println(F(" done."));
#endif
  }
  for (int i = 0; i < NSenseFact; i++) {
    //стираем инфу о достижении/превышении температуры
    on_warn[i] = false;
  }
}
//-----------------------------------------------------------------------------------------

void save_CfgFile()
{
#ifdef USE_LOG
  Serial.print("Try save " + NameSettingsFile);
#endif
  File f = SPIFFS.open("/" + NameSettingsFile, "w");
  if (!f) {
#ifdef USE_LOG
    Serial.println(F("....failed"));
#endif
  }
  else {
#ifdef USE_LOG
    Serial.println(F("..... Writing SPIFFS file "));
#endif
    String strM;
    /*strM = F("{\"Vol\":"); strM += F("0,");
      strM += F("\"Relay1OnTime\":["); strM += String(Relay1OnHour) + F(","); strM += String(Relay1OnMin) + F(","); strM += String(Relay1OnSec) + F("],");
      strM += F("\"Relay1OffTime\":["); strM += String(Relay1OffHour) + F(","); strM += String(Relay1OffMin) + F(","); strM += String(Relay1OffSec) + F("],");
      strM += F("\"Relay1Time\":["); strM += String(Relay1Hour) + F(","); strM += String(Relay1Min) + F(","); strM += String(Relay1Sec) + F("],");
      strM += F("\"TRelay1Contr\":"); strM += String(TRelay1Contr) + F(",");
      strM += F("\"NsenseRelay1Contr\":"); strM += String(NsenseRelay1Contr) + F(",");;

      for (byte i = 0; i < NomPrerRelay1All; i++) {
      PrerRelay1[i].PrerReleyStUnx = combineUnixTime(PrerRelay1[i].PrerReleyHourSt, PrerRelay1[i].PrerReleyMinSt, 0, 1, 1, 1970); // Собрать время в формат UNIX-time
      PrerRelay1[i].PrerReleyEndUnx = combineUnixTime(PrerRelay1[i].PrerReleyHourEnd, PrerRelay1[i].PrerReleyMinEnd, 0, 1, 1, 1970); // Собрать время в формат UNIX-time

      strM += F("\"Avail"); strM +=   String(i) + F("\":"); strM += String(PrerRelay1[i].Avail) + F(",");
      strM += F("\"PrerRelay1StUnx"); strM +=  String(i) + F("\":"); strM += String(PrerRelay1[i].PrerReleyStUnx) + F(",");
      strM += F("\"PrerRelay1EndUnx"); strM +=  String(i) + F("\":"); strM += String(PrerRelay1[i].PrerReleyEndUnx) + F(",");
      }

      TimerRelay1StUnx = combineUnixTime(TimerRelay1HourSt, TimerRelay1MinSt, TimerRelay1SecSt, TimerRelay1DaySt, TimerRelay1MonthSt, TimerRelay1YearSt); // Собрать время в формат UNIX-time
      TimerRelay1EndUnx = combineUnixTime(TimerRelay1HourEnd, TimerRelay1MinEnd, TimerRelay1SecEnd, TimerRelay1DayEnd, TimerRelay1MonthEnd, TimerRelay1YearEnd); // Собрать время в формат UNIX-time

      strM += F("\"TimerRelay1AvailSt\":"); strM += String(TimerRelay1AvailSt) + F(",");
      strM += F("\"TimerRelay1StUnx\":"); strM += String(TimerRelay1StUnx) + F(",");
      strM += F("\"TimerRelay1AvailEnd\":"); strM += String(TimerRelay1AvailEnd) + F(",");
      strM += F("\"TimerRelay1EndUnx\":"); strM += String(TimerRelay1EndUnx) + F(",");

      strM += F("\"over_t_lev1_on\":"); strM += String(over_t_lev1_on) + F(",");
      strM += F("\"over_t_lev2_on\":"); strM += String(over_t_lev2_on) + F(",");
      strM += F("\"over_t_lev1_off\":"); strM += String(over_t_lev1_off) + F(",");
      strM += F("\"over_t_lev2_off\":"); strM += String(over_t_lev2_off);

      strM += F("}");
      #ifdef USE_LOG
      Serial.println(strM);
      #endif
      f.println(strM);
      //-------------------------
      strM = F("{\"Vol\":"); strM += F("1,");
      strM += F("\"Relay2OnTime\":["); strM += String(Relay2OnHour) + F(","); strM += String(Relay2OnMin) + F(","); strM += String(Relay2OnSec) + F("],");
      strM += F("\"Relay2OffTime\":["); strM += String(Relay2OffHour) + F(","); strM += String(Relay2OffMin) + F(","); strM += String(Relay2OffSec) + F("],");
      strM += F("\"Relay2Time\":["); strM += String(Relay2Hour) + F(","); strM += String(Relay2Min) + F(","); strM += String(Relay2Sec) + F("],");
      strM += F("\"TRelay2Contr\":"); strM += String(TRelay2Contr) + F(",");
      strM += F("\"NsenseRelay2Contr\":"); strM += String(NsenseRelay2Contr) + F(",");;

      for (byte i = 0; i < NomPrerRelay2All; i++) {
      PrerRelay2[i].PrerReleyStUnx = combineUnixTime(PrerRelay2[i].PrerReleyHourSt, PrerRelay2[i].PrerReleyMinSt, 0, 1, 1, 1970); // Собрать время в формат UNIX-time
      PrerRelay2[i].PrerReleyEndUnx = combineUnixTime(PrerRelay2[i].PrerReleyHourEnd, PrerRelay2[i].PrerReleyMinEnd, 0, 1, 1, 1970); // Собрать время в формат UNIX-time

      strM += F("\"Avail"); strM +=   String(i) + F("\":"); strM += String(PrerRelay2[i].Avail) + F(",");
      strM += F("\"PrerRelay2StUnx"); strM +=  String(i) + F("\":"); strM += String(PrerRelay2[i].PrerReleyStUnx) + F(",");
      strM += F("\"PrerRelay2EndUnx"); strM +=  String(i) + F("\":"); strM += String(PrerRelay2[i].PrerReleyEndUnx) + F(",");
      }

      TimerRelay2StUnx = combineUnixTime(TimerRelay2HourSt, TimerRelay2MinSt, TimerRelay2SecSt, TimerRelay2DaySt, TimerRelay2MonthSt, TimerRelay2YearSt); // Собрать время в формат UNIX-time
      TimerRelay2EndUnx = combineUnixTime(TimerRelay2HourEnd, TimerRelay2MinEnd, TimerRelay2SecEnd, TimerRelay2DayEnd, TimerRelay2MonthEnd, TimerRelay2YearEnd); // Собрать время в формат UNIX-time

      strM += F("\"TimerRelay2AvailSt\":"); strM += String(TimerRelay2AvailSt) + F(",");
      strM += F("\"TimerRelay2StUnx\":"); strM += String(TimerRelay2StUnx) + F(",");
      strM += F("\"TimerRelay2AvailEnd\":"); strM += String(TimerRelay2AvailEnd) + F(",");
      strM += F("\"TimerRelay2EndUnx\":"); strM += String(TimerRelay2EndUnx);


      strM += F("}");
      #ifdef USE_LOG
      Serial.println(strM);
      #endif
      f.println(strM);*/
    //----------------------------------
    strM = F("{\"Vol\":"); strM += F("3,");

    strM += F("\"resumeState\":"); strM += String(resumeState) + F(",");
    strM += F("\"autoMode\":"); strM += String(autoMode) + F(",");
    strM += F("\"relayLevel\":["); strM += String(relay1Level) + F(","); strM += String(relay2Level) + F("],");


    strM += F("\"Zaderj\":"); strM += String(Zaderj) + F(",");
    strM += F("\"jamp_t\":"); strM += String(jamp_t) + F(",");
    strM += F("\"error_read_ds_Max\":"); strM += String(error_read_ds_Max) + F(",");
    strM += F("\"t_msec_get_ds\":"); strM += String(t_msec_get_ds) + F(",");

#ifdef USE_PIPE
    strM += F("\"count_imp_litr\":"); strM += String(count_imp_litr) + F(",");
    strM += F("\"time_calc\":"); strM += String(time_calc) + F(",");
#endif

    strM += F("\"ntpServer\":[\""); strM += ntpServer1 + F("\",\""); strM += ntpServer2 + F("\",\""); strM += ntpServer3 + F("\"],");
    strM += F("\"updateInterval\":"); strM += String(updateInterval) + F(",");
    strM += F("\"timeZone\":"); strM += String(timeZone) + F(",");
    strM += F("\"ntpUpd\":"); strM += String(ntpUpd) + F(",");

    strM += F("\"useMQTT\":"); strM += String(useMQTT) + F(",");
    strM += F("\"mqttTopicPref\":"); strM += String(mqttTopicPref) + F(",");
    strM += F("\"addUserTopic\":"); strM += String(addUserTopic) + F(",");
    strM += F("\"mqttServer\":\""); strM += mqttServer + F("\",");
    strM += F("\"mqttServerPort\":"); strM += String(mqttServerPort) + F(",");
    strM += F("\"mqttUser\":\""); strM += mqttUser + F("\",");
    strM += F("\"mqttUserPassword\":\""); strM += mqttUserPassword + F("\",");
    strM += F("\"mqttClientId\":\""); strM += mqttClientId + F("\",");
    strM += F("\"mqttReconnectSec\":"); strM += String(mqttReconnectSec) + F(",");

    /*    strM += F("\"useMail\":"); strM += String(useMail) + F(",");
        strM += F("\"smtpHost\":\""); strM += smtpHost + F("\",");
        strM += F("\"smtpPort\":"); strM += String(smtpPort) + F(",");
        strM += F("\"smtpUser\":\""); strM += smtpUser + F("\",");
        strM += F("\"smtpPass\":\""); strM += smtpPass + F("\",");
        strM += F("\"mailTo\":\""); strM += mailTo + F("\",");
        strM += F("\"mailSubject\":\""); strM += mailSubject + F("\",");
        strM += F("\"useMailSwt\":["); strM += String(useMailSwt1) + F(","); strM += String(useMailSwt2) + F(","); strM += String(useMailSwt3) + F(","); strM += String(useMailSwt4) + F("],");*/

    strM += F("\"log_t\":"); strM += String(log_t) + F(",");
    strM += F("\"logFileSize\":"); strM += String(logFileSize) + F(",");
    strM += F("\"writeSysLog\":"); strM += String(writeSysLog) + F(",");
    strM += F("\"writeSenseLog\":"); strM += String(writeSenseLog);
    strM += F("}");


#ifdef USE_LOG
    Serial.println(strM);
#endif
    f.println(strM);
    f.print(F("END"));
    f.close();
#ifdef USE_LOG
    Serial.println(F(" done."));
#endif
  }
}
//------------------------------------------------------------

//---------------------------------------------------------------------------------------------
void h_save() //save
{
  LoginContr();
  String argName, argValue;
#ifdef USE_LOG
  Serial.println("module save");
  Serial.print("args()=");
  Serial.println(server.args());
#endif
  for (byte i = 0; i < server.args(); i++)
  {
    argName = server.argName(i);
#ifdef USE_LOG
    Serial.print(argName);
    Serial.print("=\"");
#endif
    argValue = server.arg(i);
#ifdef USE_LOG
    Serial.print(argValue);
    Serial.println("\"");
#endif
    if (argName == F("ssid"))                  ssid = argValue;
    else if (argName == F("password"))     password = argValue;
    else if (argName == F("AccessLogin"))     AccessLogin = argValue;
    else if (argName == F("AccessPassword"))     AccessPassword = argValue;

    else if (argName == F("ip1"))               ip1 = constrain(argValue.toInt(), 0, 255);
    else if (argName == F("ip2"))               ip2 = constrain(argValue.toInt(), 0, 255);
    else if (argName == F("ip3"))               ip3 = constrain(argValue.toInt(), 0, 255);
    else if (argName == F("ip4"))               ip4 = constrain(argValue.toInt(), 0, 255);

    else if (argName == F("ms1"))               ms1 = constrain(argValue.toInt(), 0, 255);
    else if (argName == F("ms2"))               ms2 = constrain(argValue.toInt(), 0, 255);
    else if (argName == F("ms3"))               ms3 = constrain(argValue.toInt(), 0, 255);
    else if (argName == F("ms4"))               ms4 = constrain(argValue.toInt(), 0, 255);

    else if (argName == F("gt1"))               gt1 = constrain(argValue.toInt(), 0, 255);
    else if (argName == F("gt2"))               gt2 = constrain(argValue.toInt(), 0, 255);
    else if (argName == F("gt3"))               gt3 = constrain(argValue.toInt(), 0, 255);
    else if (argName == F("gt4"))               gt4 = constrain(argValue.toInt(), 0, 255);

    else if (argName == F("dn1"))               dn1 = constrain(argValue.toInt(), 0, 255);
    else if (argName == F("dn2"))               dn2 = constrain(argValue.toInt(), 0, 255);
    else if (argName == F("dn3"))               dn3 = constrain(argValue.toInt(), 0, 255);
    else if (argName == F("dn4"))               dn4 = constrain(argValue.toInt(), 0, 255);

    /* else if (argName == F("Relay2OnHour"))   Relay2OnHour = constrain(argValue.toInt(), 0, 23);
      else if (argName == F("Relay2OnMin"))     Relay2OnMin = constrain(argValue.toInt(), 0, 59);
      else if (argName == F("Relay2OnSec"))     Relay2OnSec = constrain(argValue.toInt(), 0, 59);
      else if (argName == F("Relay2OffHour")) Relay2OffHour = constrain(argValue.toInt(), 0, 23);
      else if (argName == F("Relay2OffMin"))   Relay2OffMin = constrain(argValue.toInt(), 0, 59);
      else if (argName == F("Relay2OffSec"))   Relay2OffSec = constrain(argValue.toInt(), 0, 59);

      else if (argName == F("Relay2Hour"))     Relay2Hour = constrain(argValue.toInt(), 0, 23);
      else if (argName == F("Relay2Min"))       Relay2Min = constrain(argValue.toInt(), 0, 59);
      else if (argName == F("Relay2Sec"))       Relay2Sec = constrain(argValue.toInt(), 0, 59);
      else if (argName == F("TRelay2Contr")) TRelay2Contr = argValue.toInt();
      else if (argName == F("NsenseRelay2Contr")) NsenseRelay2Contr = argValue.toInt();


      else if (argName == F("Relay1OnHour"))   Relay1OnHour = constrain(argValue.toInt(), 0, 23);
      else if (argName == F("Relay1OnMin"))     Relay1OnMin = constrain(argValue.toInt(), 0, 59);
      else if (argName == F("Relay1OnSec"))     Relay1OnSec = constrain(argValue.toInt(), 0, 59);
      else if (argName == F("Relay1OffHour")) Relay1OffHour = constrain(argValue.toInt(), 0, 23);
      else if (argName == F("Relay1OffMin"))   Relay1OffMin = constrain(argValue.toInt(), 0, 59);
      else if (argName == F("Relay1OffSec"))   Relay1OffSec = constrain(argValue.toInt(), 0, 59);

      else if (argName == F("Relay1Hour"))     Relay1Hour = constrain(argValue.toInt(), 0, 23);
      else if (argName == F("Relay1Min"))       Relay1Min = constrain(argValue.toInt(), 0, 59);
      else if (argName == F("Relay1Sec"))       Relay1Sec = constrain(argValue.toInt(), 0, 59);
      else if (argName == F("TRelay1Contr")) TRelay1Contr = argValue.toInt();
      else if (argName == F("NsenseRelay1Contr")) NsenseRelay1Contr = argValue.toInt();
    */
    else if (argName == F("mqttServer"))             mqttServer = argValue;
    else if (argName == F("mqttTopicPref"))             mqttTopicPref = argValue;
    else if (argName == F("mqttServerPort"))     mqttServerPort = constrain(argValue.toInt(), 1024, 65535);
    else if (argName == F("mqttUser"))                 mqttUser = argValue;
    else if (argName == F("mqttUserPassword")) mqttUserPassword = argValue;
    else if (argName == F("mqttClientId"))         mqttClientId = argValue;
    else if (argName == F("mqttReconnectSec")) mqttReconnectSec = constrain(argValue.toInt(), 60, 65535);

    /*    else if (argName == F("smtpHost"))             smtpHost = argValue;
        else if (argName == F("smtpPort"))             smtpPort = argValue.toInt();
        else if (argName == F("smtpUser"))             smtpUser = argValue;
        else if (argName == F("smtpPass"))             smtpPass = argValue;
        else if (argName == F("mailTo"))                 mailTo = argValue;
        else if (argName == F("mailSubject"))       mailSubject = argValue;*/


    else if (argName == F("Zaderj"))                        Zaderj = constrain(argValue.toInt(), 0, 1000);
    else if (argName == F("jamp_t"))                        jamp_t = constrain(argValue.toInt(), 0, 99);
    else if (argName == F("error_read_ds_Max"))  error_read_ds_Max = constrain(argValue.toInt(), 2, 99);
    else if (argName == F("t_msec_get_ds"))          t_msec_get_ds = constrain(argValue.toInt(), 100, 40000);
#ifdef USE_PIPE
    else if (argName == F("count_imp_litr"))                        count_imp_litr = constrain(argValue.toInt(), 100, 10000);
    else if (argName == F("time_calc"))                        time_calc = constrain(argValue.toInt(), 1000, 10000);
#endif

    else if (argName == F("log_t"))                          log_t = constrain(argValue.toInt(), 1, 1440);
    else if (argName == F("logFileSize"))             logFileSize = constrain(argValue.toInt(), 20, 250);



    else if (argName == F("ntpServer1"))     ntpServer1 = argValue;
    else if (argName == F("ntpServer2"))     ntpServer2 = argValue;
    else if (argName == F("ntpServer3"))     ntpServer3 = argValue;
    else if (argName == F("updateInterval"))     updateInterval = constrain(argValue.toInt(), 3600, 72 * 3600);
    else if (argName == F("timeZone"))     timeZone = argValue.toInt();


    //---------------
    /*else if (argName == F("TimerRelay2DaySt"))  TimerRelay2DaySt = constrain(argValue.toInt(), 1, 31);
      else if (argName == F("TimerRelay2MonthSt"))  TimerRelay2MonthSt = constrain(argValue.toInt(), 1, 12);
      else if (argName == F("TimerRelay2YearSt"))  TimerRelay2YearSt = constrain(argValue.toInt(), 2018, 2040);
      else if (argName == F("TimerRelay2HourSt"))  TimerRelay2HourSt = constrain(argValue.toInt(), 0, 23);
      else if (argName == F("TimerRelay2MinSt"))  TimerRelay2MinSt = constrain(argValue.toInt(), 0, 59);
      else if (argName == F("TimerRelay2SecSt"))  TimerRelay2SecSt = constrain(argValue.toInt(), 0, 59);


      else if (argName == F("TimerRelay2DayEnd"))  TimerRelay2DayEnd = constrain(argValue.toInt(), 1, 31);
      else if (argName == F("TimerRelay2MonthEnd"))  TimerRelay2MonthEnd = constrain(argValue.toInt(), 1, 12);
      else if (argName == F("TimerRelay2YearEnd"))  TimerRelay2YearEnd = constrain(argValue.toInt(), 2018, 2040);
      else if (argName == F("TimerRelay2HourEnd"))  TimerRelay2HourEnd = constrain(argValue.toInt(), 0, 23);
      else if (argName == F("TimerRelay2MinEnd"))  TimerRelay2MinEnd = constrain(argValue.toInt(), 0, 59);
      else if (argName == F("TimerRelay2SecEnd"))  TimerRelay2SecEnd = constrain(argValue.toInt(), 0, 59);

      //---------------
      else if (argName == F("TimerRelay1DaySt"))  TimerRelay1DaySt = constrain(argValue.toInt(), 1, 31);
      else if (argName == F("TimerRelay1MonthSt"))  TimerRelay1MonthSt = constrain(argValue.toInt(), 1, 12);
      else if (argName == F("TimerRelay1YearSt"))  TimerRelay1YearSt = constrain(argValue.toInt(), 2018, 2040);
      else if (argName == F("TimerRelay1HourSt"))  TimerRelay1HourSt = constrain(argValue.toInt(), 0, 23);
      else if (argName == F("TimerRelay1MinSt"))  TimerRelay1MinSt = constrain(argValue.toInt(), 0, 59);
      else if (argName == F("TimerRelay1SecSt"))  TimerRelay1SecSt = constrain(argValue.toInt(), 0, 59);


      else if (argName == F("TimerRelay1DayEnd"))  TimerRelay1DayEnd = constrain(argValue.toInt(), 1, 31);
      else if (argName == F("TimerRelay1MonthEnd"))  TimerRelay1MonthEnd = constrain(argValue.toInt(), 1, 12);
      else if (argName == F("TimerRelay1YearEnd"))  TimerRelay1YearEnd = constrain(argValue.toInt(), 2018, 2040);
      else if (argName == F("TimerRelay1HourEnd"))  TimerRelay1HourEnd = constrain(argValue.toInt(), 0, 23);
      else if (argName == F("TimerRelay1MinEnd"))  TimerRelay1MinEnd = constrain(argValue.toInt(), 0, 59);
      else if (argName == F("TimerRelay1SecEnd"))  TimerRelay1SecEnd = constrain(argValue.toInt(), 0, 59);

      else if (argName == F("U_lev"))  U_lev = constrain(argValue.toInt(), 0, 1023);
      else if (argName == F("U_delta"))  U_delta = constrain(argValue.toInt(), 0, 100);
    */

    String _argName;


    for (uint8_t k = 0; k < NSenseMax; k++)  {
      String argN;
      argN = F("tNameSens"); argN += String(k);
      if (argName == argN) tNameSens[k] = argValue;

      argN = F("temp_warn"); argN += String(k);
      if (argName == argN) temp_warn[k] = atof(argValue.c_str());

      argN = F("temp_warn_delt"); argN += String(k);
      if (argName == argN) temp_warn_delt[k] = atof(argValue.c_str());

    }
  }
  writeConfig();
  save_SensorConfFile();
  save_CfgFile();
  String message = F("<!DOCTYPE html>");
  message += F("<html>");
  message += F("<head>");
  message += F("<title>Store Setup</title>");
  message += F("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  message += F("<meta charset=\"utf-8\" http-equiv=\"refresh\" content=\"0; /index.html\">");
  message += F("</head>");
  message += F("<body>");
  message += F("Настройи сохранены успешно.");
  message += F("<p>Ждите 1 сек. или нажмите <a href=\"/index.html\">сюда</a> для перехода в главное меню.");
  message += F("</body>");
  message += F("</html>");

  server.send(200, F("text/html"), message);

}
//-------------------------------------------------------------------------

void h_select()//select
{
  LoginContr();
  String message;
  message = F("<!DOCTYPE html>");
  message += F("<html>");
  message += F("<head>");
  message += F("<title>Управление контролем</title>");
  message += F("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  message += F("<meta charset=\"utf-8\">");
  message += F("<style type=\"text/css\">");
  message += F(".checkbox {vertical-align:top; margin:0 3px 0 0; width:17px; height:17px;}");
  message += F(".checkbox + label {cursor:pointer;}");
  message += F(".checkbox:not(checked) {position:absolute; opacity:0; }");
  message += F(".checkbox:not(checked) + label {position:relative; padding:0 0 0 60px;}");
  message += F(".checkbox:not(checked) + label:before {content:''; position:absolute; top:-4px; left:0; width:50px; height:26px; border-radius:13px; background:#CDD1DA;box-shadow:inset 0 2px 3px rgba(0,0,0,.2);}");
  message += F(".checkbox:not(checked) + label:after { content:''; position:absolute; top:-2px; left:2px; width:22px; height:22px; border-radius:10px; background:#FFF; box-shadow:0 2px 5px rgba(0,0,0,.3); transition:all .2s;}");
  message += F(".checkbox:checked + label:before {background:#9FD468;}");
  message += F(".checkbox:checked + label:after {left:26px;}");
  message += textStyle();
  message += F("</style>");
  message += F("<script type=\"text/javascript\">");
  message += F("function openUrl(url) {");
  message += F("var request = new XMLHttpRequest();");
  message += F("request.open('GET', url, true);");
  message += F("request.send(null);");
  message += F("}");
  message += F("function PowChange(Powval,k){");
  message += F("var ret;");
  message += F("if (isNaN(Powval)==true) return 0;");
  message += F("ret=(parseFloat(Powval,10)+k);");
  message += F("if (ret>100) ret=100;");
  message += F("if (ret<0) ret=0;");
  message += F("return ret;}");
  message += F("function CheckVal(obj){");
  message += F("var ret;");
  message += F("ret=(parseFloat(document.getElementById(obj).value,10));");
  message += F("if (isNaN(ret)==true) document.getElementById(obj).value=0;");
  message += F("if (isNaN(ret)==false) document.getElementById(obj).value=ret;}");
  message += F("function refreshSens(){\n");
  message +=   F("var request = new XMLHttpRequest();\n");
  message +=   F("request.open('GET', '/data_mainPage', true);\n");
  message +=   F("request.onreadystatechange = function(){\n");
  message +=       F("if (request.readyState == 4) {\n");
  message +=         F("var data = JSON.parse(request.responseText);\n ");
  message +=         F("document.getElementById('relay1Level').checked = data.relay1Level;\n");
  message +=         F("document.getElementById('relay2Level').checked = data.relay2Level;\n");
  message +=         F("if (data.beepeep > 0) {document.location.href = \"/alert\"}}};\n");
  message +=   F("request.send(null);\n");
  message +=   F("}\n");
  message += F("setInterval(refreshSens, 1000);\n");
  message += F("</script>");
  message += F("</head>");
  message += F("<body>");
  message += F("<form method=\"get\" action=\"/save\">");
  message += F("<h3>Управление контролем</h3>");
  //message += "<br>";

  message += F("&nbsp;<fieldset style=\"width: 304px\">");
  message += F("<legend><b>Настройки реле</b></legend>");

  message += F("<input type=\"checkbox\" name=\"resumeState\" class=\"checkbox\" id=\"resumeState\" onchange=\"openUrl('/switch?resumeState=' + this.checked);\" ");
  if (resumeState == 1) message += F("checked ");
  message += F("/><label for=\"resumeState\">Восстанавливать сотояния реле</label>");

  message += F("<br>");
  message += F("<br>");
  message += F("<b>Активация реле высоким уровнем</b>");
  message += F("<br>");
  message += F("<br>");

  message += F("<input type=\"checkbox\" name=\"relay1Level\" class=\"checkbox\" id=\"relay1Level\" onchange=\"openUrl('/switch?relay1Level=' + this.checked);\" ");
  if (relay1Level == 1) message += F("checked ");
  message += F("/><label for=\"relay1Level\">Реле 1</label><p>");

  message += F("<input type=\"checkbox\" name=\"relay2Level\" class=\"checkbox\" id=\"relay2Level\" onchange=\"openUrl('/switch?relay2Level=' + this.checked);\" ");
  if (relay2Level == 1) message += F("checked ");
  message += F("/><label for=\"relay2Level\">Реле 2</label><p>");

  message += F("</fieldset>");

  message += F("&nbsp;<fieldset style=\"width: 304px\">");
  message += F("<legend><b>Датчики температуры</b></legend>");
  message += F("<br>");

  for (byte i = 0; i < NSenseFact; i++) {
    if (t[i] != 0) {
      message += F("<input type=\"checkbox\" name=\"t"); message += String(i); message += F("\" class=\"checkbox\" id=\"t"); message += String(i);
      message += F("\" onchange=\"openUrl('/switch?SenseStatus"); message += String(i); message += F("=' + this.checked);\" ");
      if (t[i] == 1) message += F("checked ");
      message += F("/><label for=\"t"); message += String(i); message += F("\">Д "); message += String(i); message += F(" </label>");
      message += F("<input type=\"text\" name=\"tNameSens"); message += String(i); message += F("\" id=\"tNameSens"); message += String(i) + ("\" class=\"textStyle3\" title=\"наименование датчика без запятых\" maxlength=\"30\" value=\"");
      message += tNameSens[i]; message += F("\" />");

      message += F("<input type=\"text\" name=\"temp_warn"); message += String(i); message += F("\" id=\"temp_warn"); message += String(i) + ("\" class=\"textStyle2\" title=\"температура тревоги в градусах\" maxlength=\"6\" value=\"");
      message += temp_warn[i]; message += F("\" />");

      message += F("<input type=\"text\" name=\"temp_warn_delt"); message += String(i); message += F("\" id=\"temp_warn_delt"); message += String(i) + ("\" class=\"textStyle2\" title=\"дельта в градусах\" maxlength=\"6\" value=\"");
      message += temp_warn_delt[i]; message += F("\" />");

      message += F("<p>");
    }
  }

  message += F("<b>Настройка чтения</b><br>");

  message += F("<input name = \"error_read_ds_Max\" id=\"error_read_ds_Max\" type=\"text\" value=\"");
  message += String(error_read_ds_Max) + F("\" class=\"textStyle2\" title=\"Разрешенное количество ошибок (подряд) при опросе датчиков температуры\" maxlength=\"2\"");
  message += F("/> Кол.ошибок (подряд) при опросе<br />");

  message += F("<input name=\"t_msec_get_ds\" id=\"t_msec_get_ds\" type=\"text\" value=\"");
  message += String(t_msec_get_ds) + F("\" class=\"textStyle2\" title=\"Интервал опроса датчиков,мСек\" maxlength=\"5\""); message += F("/> Интервал опроса,мСек<br />");

  message += F("<input name=\"Zaderj\" id=\"Zaderj\" type=\"text\" value=\"");
  message += String(Zaderj) + F("\" class=\"textStyle2\" title=\"задержка опроса датчиков,мСек\" maxlength=\"5\""); message += F("/> Задержка опроса,мСек<br />");

  message += F("<input name=\"jamp_t\" id=\"jamp_t\" type=\"text\" value=\"");
  message += String(jamp_t) + F("\" class=\"textStyle2\" title=\"Максимальная значение изменния температуры за опрос\" maxlength=\"2\""); message += F("/> Макс.скор.измен темп.,С/опрос<br />");

  message += F("<input name=\"log_t\" id=\"log_t\" type=\"text\" value=\"");
  message += String(log_t) + F("\" class=\"textStyle2\" title=\"Записывать температуру в лог файл mesure.log через заданное количество минут\" maxlength=\"4\""); message += F("/> Фиксир.температ через, мин<br />");

  message += F("<input type=\"button\" id=\"butt_search_sense\" value=\"Поиск\" onclick=\"openUrl('/switch?Srch=' + '1'); location.href='/selectProm'; \" >\n");
  message += F("<br />");

  if ((error_read_ds > 0) || (error_read_ds_all > 0)) {
    message += F("Ошибок при чтении t (всего): "); message += String(error_read_ds_all) + F("<br />");
  }
  if (error_read_correct_ds_all > 0) {
    message += F("Корректность чтения t (всего): "); message += String(error_read_correct_ds_all) + F("<br />");
  }

  message += F("</fieldset>");
  message += F("<br>");
  //message += F("<p>");


#ifdef USE_PIPE
  message += F("&nbsp;<fieldset style=\"width: 304px\">");
  message += F("<legend><b>Датчик потока</b></legend>");
  message += F("<br>");

  message += F("<b>Настройка чтения</b><br>");

  message += F("<input name=\"count_imp_litr\" id=\"count_imp_litr\" type=\"text\" value=\"");
  message += String(count_imp_litr) + F("\" class=\"textStyle2\" title=\"Импульсов на литр,шт\" maxlength=\"5\""); message += F("/> Импульсов на литр,шт<br />");

  message += F("<input name=\"time_calc\" id=\"time_calc\" type=\"text\" value=\"");
  message += String(time_calc) + F("\" class=\"textStyle2\" title=\"Интервал для расчета,мСек\" maxlength=\"5\""); message += F("/> Интервал для расчета,мСек<br />");

  message += F("</fieldset>");
#endif

  message += F("<br />");

  message += F("<fieldset style=\"width: 304px\">");
  message += F("<legend><b>Логи</b></legend>");

  message += F("<input type=\"checkbox\" name=\"writeSysLog\" class=\"checkbox\" id=\"writeSysLog\" onchange=\"openUrl('/switch?writeSysLog=' + this.checked);\" ");
  if (writeSysLog == 1) message += F("checked ");
  message += F("/><label for=\"writeSysLog\">Записывать системные логи</label><p>");
  message += F("<input type=\"checkbox\" name=\"writeSenseLog\" class=\"checkbox\" id=\"writeSenseLog\" onchange=\"openUrl('/switch?writeSenseLog=' + this.checked);\" ");
  if (writeSenseLog == 1) message += F("checked ");
  message += F("/><label for=\"writeSenseLog\">Записывать темпер. логи</label><p>");

  message += F("<input name=\"logFileSize\" id=\"logFileSize\" type=\"text\" value=\""); message += String(logFileSize) + F("\" class=\"textStyle2\" title=\"Размер файла лога в kBytes (файлов будет четыре system.log, mesure.log, кода будет превышен размер произойдет копирование в system01.log и mesure01.log, а исходные файлы начнутся сначала) от 20 до 250\" maxlength=\"3\"/> Размер файла,kBytes (20-250)<br>");

  message += F("</fieldset>");
  message += F("<br>");
  message += F("<input type=\"button\" value=\"Назад\" onclick=\"location.href='/'\"><input type=\"submit\" value=\"Сохранить\" />");

  message += F("</form>");
  message += F("</body>");
  message += F("</html>");
  server.send(200, F("text/html"), message);
}
//-------------------------------------------------------------------------------
void h_selectProm()//selectProm
{
  LoginContr();

  String message = F("<!DOCTYPE html>");
  message += F("<html>");
  message += F("<head>");
  message += F("<title>Store Setup</title>");
  message += F("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  message += F("<meta charset=\"utf-8\" http-equiv=\"refresh\" content=\"0; /select\">");
  message += F("</head>");
  message += F("<body>");
  message += F("Поиск датчиков выполнен.");
  message += F("<p>Ждите 1 сек. или нажмите <a href=\"/select\">сюда</a> для перехода в настройку сообщений.");
  message += F("</body>");
  message += F("</html>");
  server.send(200, F("text/html"), message);
}
//---------------------------------------------------------------------------------
void h_data_mainPage()//data_mainPage
{
  LoginContr();
  String message;
  message += F("{");

  message += F("\"autoMode\":");
  if (autoMode) message += 1;
  else message += 0;
  message += F(",");

  message += F("\"relay1\":");
  if (swt1) message += 1;
  else message += 0;
  message += F(",");

  message += F("\"relay2\":");
  if (swt2) message += 1;
  else message += 0;
  message += F(",");

  message += F("\"relay1Level\":");
  if (relay1Level) message += 1;
  else message += 0;
  message += F(",");

  message += F("\"relay2Level\":");
  if (relay2Level) message += 1;
  else message += 0;
  message += F(",");


  message += F("\"now_relay1\":");
  if (now_relay1) message += 1;
  else message += 0;
  message += F(",");

  message += F("\"now_relay2\":");
  if (now_relay2) message += 1;
  else message += 0;
  message += F(",");

  message += F("\"str_t_working_Relay1\":\""); message += String(str_t_working_Relay1) + F("\",");
  message += F("\"str_t_working_Relay2\":\""); message += String(str_t_working_Relay2) + F("\",");

  for (byte i = 0; i <= NSenseFact; i++) {
    if (t[i] == 1) {//датчик активен (0-не использ,1-активен,2-используется но не активен)
      message += F("\"tempC"); message += String(i); message += F("\":"); message += tempC[i]; message += F(",");
    }
  }
  message += F("\"timeStr\":\""); message += String(timeStr) + F("\"");
  message += F(",");
  message += F("\"TimeOn\":\""); message += String(TimeOn) + F("\"");
  message += F(",");
  message += F("\"MQTT\":\"");
  if (useMQTT) {
    if (mqttClient.connected()) {
      message += F("Соединено");
      message += F("\"");
    }
    else {
      message += F("Разорвано");
      message += F("\"");
    }
  }
  else {
    message += F("Выключено");
    message += F("\"");
  }
  message += F(",");
  message += F("\"info\":\""); message += String(str_info) + F("\"");
  message += F("}");

  server.send(200, F("text/html"), message);
}
//---------------------------------------------------------------------------------------------------
void handleGetTimer() {

  LoginContr();
  int8_t R_hour, R_min, R_sec, R_day, R_month;
  uint8_t R_wd;
  int16_t R_year;

  parseUnixTime(startTime, R_hour, R_min, R_sec, R_wd, R_day, R_month, R_year);
  server.send(200, F("text/html"), "ok");
}
//-------------------------------------
void handleSetTimer() {
  LoginContr();

  int8_t R_hour, R_min, R_sec, R_day, R_month;
  uint8_t R_wd;
  int16_t R_year;
  String message;

  parseUnixTime(now_timeUnx, R_hour, R_min, R_sec, R_wd, R_day, R_month, R_year);

  message += F("{");
  message += F("\"R_hour\":"); message += String(R_hour) + F(",");
  message += F("\"R_min\":"); message += String(R_min) + F(",");
  message += F("\"R_sec\":"); message += String(R_sec) + F(",");

  message += F("\"R_day\":"); message += String(R_day) + F(",");
  message += F("\"R_month\":"); message += String(R_month) + F(",");
  message += F("\"R_year\":"); message += String(R_year);
  message += F("}");

  server.send(200, F("text/html"), message);
}
