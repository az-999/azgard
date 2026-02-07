void h_switch()//switch
{
  LoginContr();
  String message = F("ok");
  uint16_t resp = 200;
  String argName, argValue;
  for (byte i = 0; i < server.args(); i++)
  {
    argName = server.argName(i);
#ifdef USE_LOG
    Serial.print(argName);
    Serial.print(F("="));
#endif
    argValue = server.arg(i);
#ifdef USE_LOG
    Serial.println(argValue);
#endif

    if (argName == F("autoMode")) {
      if (argValue == F("true")) autoMode = HIGH;
      else autoMode = LOW;
      if (resumeState == 1) {
        save_ResumeStateFile();
      }
    }
    else if (argName == F("swt1")) {
      if (argValue == F("true"))
      {
        swt1 = HIGH;
      }
      else swt1 = LOW;

      if (useMQTT && mqttClient.connected()) {
#ifdef USE_LOG
        Serial.print(F("  swt1 = ")); Serial.println(String(swt1).c_str());
#endif
        mqttClient.publish(mqtt_topic(F("swt/1")).c_str(), String(swt1).c_str(), saveValMQTT);

      }
    }
    
    /*else if (argName == F("useMailSwt1")) {
      if (argValue == F("true")) useMailSwt1 = HIGH;
      else useMailSwt1 = LOW;
    }

    else if (argName == F("useMailSwt2")) {
      if (argValue == F("true")) useMailSwt2 = HIGH;
      else useMailSwt2 = LOW;
    }

    else if (argName == F("useMailSwt3")) {
      if (argValue == F("true")) useMailSwt3 = HIGH;
      else useMailSwt3 = LOW;
    }

    else if (argName == F("useMailSwt4")) {
      if (argValue == F("true")) useMailSwt4 = HIGH;
      else useMailSwt4 = LOW;
    }*/

    else if (argName == F("resumeState")) {
      if (argValue == F("true")) {
        resumeState = HIGH; save_ResumeStateFile();
      }
      else resumeState = LOW;

    }

    else if (argName == F("relay1Level")) {
      if (argValue == F("true")) relay1Level = 1;
      else relay1Level = 0;
    }
    else if (argName == F("relay2Level")) {
      if (argValue == F("true")) relay2Level = 1;
      else relay2Level = 0;
    }
    
    else if (argName == F("ApMode")) {
      if (argValue == F("true")) ApMode  = 1;
      else ApMode = 0;
    }
    else if (argName == F("AccessMode")) {
      if (argValue == F("true")) AccessMode  = 1;
      else AccessMode = 0;
    }

    else if (argName == F("Srch"))//поиск датчиков
    {
      //------------------------

      //byte insideThermometer[6][8];
      bool ex_sense;


      NSenseFact = 0;

      for (int i = 0; i < NSenseMax; i++) {
        if (!sensors.getAddress(insideThermometer[i], i))  {
#ifdef USE_LOG
          Serial.print(F("DeviceAddress insideThermometer")); Serial.print(i); Serial.println(";");
#endif
          ex_sense = 0;
          t[i] = 0;
        }
        else
        {
          t[i] = 1;
          NSenseFact++;
        }
      }
    }

    else if (argName == F("SaveSense")) {
      save_SensorConfFile();
    }

    else if (argName == F("ntpUpd")) {
      if (argValue == F("true")) {
        ntpUpd = 1;
        countFailNtp = 0;
      }
      else ntpUpd = 0;
    }
    else if (argName == F("writeSysLog")) {
      if (argValue == F("true")) writeSysLog = 1;
      else writeSysLog = 0;
    }
    else if (argName == F("writeSenseLog")) {
      if (argValue == F("true")) writeSenseLog = 1;
      else writeSenseLog = 0;
    }

    else if (argName == F("useMQTT")) {
      if (argValue == F("true")) {
        useMQTT  = 1;
        nextTimeMQTT = millis();
      }
      else {
        if (mqttClient.connected()) mqttClient.disconnect();
        useMQTT = 0;
      }
    }
    
    else if (argName == F("addUserTopic")) {
      if (argValue == F("true")) {
        addUserTopic = 1;
      }
      else {
        addUserTopic = 0;
      }
    }


    /*else if (argName == F("useMail")) {
      if (argValue == F("true")) {
        useMail  = 1;
      }
      else useMail = 0;
    }*/

    else if (argName == F("mqtt_conn")) {
      countFailMqtt = 0;
      mqttConnect();
    }

    else if (argName == F("mail_test")) {
      sendStringToMail(timeDateToStr(now_timeUnx), F("Mail TEST"));
    }
    String _argName;
    
    _argName = F("SenseStatus");
    for (byte i = 0; i < NSenseFact; i++) {
      _argName = F("SenseStatus"); _argName += String(i);
#ifdef USE_LOG
      Serial.println(_argName);
#endif
      if (argName == _argName)
      {
        if (argValue == F("true")) t[i] = 1;
        else t[i] = 2;
#ifdef USE_LOG
        Serial.print(argValue); Serial.print(i);
#endif
      }
    }

  }
  server.send(resp, F("text/html"), message);
}
//----------------------------------------------------------------------------------------------
