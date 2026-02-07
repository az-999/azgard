//------------------------------------------------------------------------------------------------
double read_correct(double tmpC, double tmpC_now, int number_sens) {
  if (abs(tmpC - tmpC_now) > jamp_t) {
    if ((tmpC_now != -127) && (tmpC != -127)) {
      error_read_correct_ds[number_sens]++;
      error_read_correct_ds_all++;
      if (error_read_correct_ds[number_sens] > error_read_ds_Max)  {
        //на error_read_ds_Max раз принимаем как есть))
        error_read_correct_ds[number_sens] = 0;
        //Serial.println(" clear error_read_correct_ds ["+String(number_sens)+"]");
        return tmpC_now;
      }
    }
    else {
      return tmpC_now;
    }
  }
  else  {
    error_read_correct_ds[number_sens] = 0;
    return tmpC_now;
  }
  return tmpC;
}

//-------------------------------------
void working_other() {
  uint32_t Other_control_now = millis();
  bool StatusDP;
  uint32_t t_prom;
  String message_to_mail;
#ifdef USE_LCD
  if (((Other_control_now - LCD_control ) > 1000) || (Other_control_now < 1000))
  { //выполняем
    lcd.setCursor(0, 1);
    lcd.print(timeToStr(now_timeUnx) + " " + dateToStr(now_timeUnx).substring(0, 5) + "  ");
    LCD_control = Other_control_now;

  }
#endif

  t_prom = millis(); //текущее время

  if (t_msec_get_ds == 0) t_msec_get_ds = 2000;
  if (error_read_ds_Max == 0) error_read_ds_Max = 5;
  if (jamp_t == 0) jamp_t = 10;
  if (Zaderj == 0) Zaderj = 10;

  if (((Other_control_now - t_control ) >= t_msec_get_ds) || (t_control == 0))
  { //выполняем замер
    if (Zaderj > 100) Zaderj = 1;
    if (Zaderj < 0) Zaderj = 0;
    delay(Zaderj);
    sensors.requestTemperatures();

    for (byte i = 0; i <= NSenseFact; i++) {
      delay(Zaderj);
      if (t[i] == 1) {//датчик активен (0-не использ,1-активен,2-используется но не активен)
        tempC_now[i] = sensors.getTempC(insideThermometer[i]);

        //проверяем показания
        if (tempC_now[i] == -127) { //датчик не зашарился
          error_read_ds[i]++; error_read_ds_all++;
          if (error_read_ds[i] == error_read_ds_Max) {
            //пишем что есть
            tempC[i] = tempC_now[i];
            error_read_ds[i] = 0;
          }
        }
        else {//датчик зашарился, проверяем показания
          error_read_ds[i] = 0;//обнуляем ошибки
          tempC[i] = read_correct(tempC[i], tempC_now[i], i);
        }
      }
    }
    if (useMQTT && mqttClient.connected()) {
      for (uint8_t i = 0; i < NSenseMax; i++)  {
        if ((t[i] == 1) && ((tempC[i] != tempC_mqtt[i]) || (millis() - time_pub[i] > 30000))) {
          tempC_mqtt[i] = tempC[i];
          mqttClient.publish((mqtt_topic(F("temp/")) + String(i)).c_str(), String(tempC_mqtt[i]).c_str(), saveValMQTT);
          time_pub[i] = millis();
        }
      }
    }

#ifdef USE_LCD
    lcd.setCursor(0, 0);
    lcd.print(F("t,C ")); lcd.print(tempC[0]); lcd.print(F("/")); lcd.print(F("G")); lcd.print(input_Lev); lcd.print(F("   "));
#endif
    t_control = Other_control_now;



#ifdef USE_DHT
    str_info = "";
    h_dht_tek = dht.readHumidity();   //Считываем влажность
    t_dht_tek = dht.readTemperature();// Считываем температуру

    if (isnan(h_dht_tek) || isnan(t_dht_tek))  {
#ifdef USE_LOG
      Serial.println(F("Засада с DHT"));
#endif
    }
    else {

#ifdef USE_LOG
      Serial.print("Влажность: "); Serial.print(h_dht_tek); Serial.println(" %\t");
      Serial.print("Температура: "); Serial.print(t_dht_tek); Serial.println(" C");
#endif
    }
    str_info += F(" В: "); str_info += String(h_dht_tek) + F("%,");;
    str_info += F(" Т: ");  str_info += String(t_dht_tek) + F("&#176С");

    if (useMQTT && mqttClient.connected()) {
      if ((h_dht != h_dht_tek) || (t_dht != t_dht_tek))  {

        if (isnan(h_dht_tek) || isnan(t_dht_tek))  {
          delay(25); mqttClient.publish(mqtt_topic(F("dht/t")).c_str(), "-", saveValMQTT);
          delay(25); mqttClient.publish(mqtt_topic(F("dht/h")).c_str(), "-", saveValMQTT);
        }
        else {
          delay(25); mqttClient.publish(mqtt_topic(F("dht/t")).c_str(), String(t_dht_tek).c_str(), saveValMQTT);
          delay(25); mqttClient.publish(mqtt_topic(F("dht/h")).c_str(), String(h_dht_tek).c_str(), saveValMQTT);
        }

        h_dht = h_dht_tek; t_dht = t_dht_tek;
      }
    }
#endif

  }
  //----------------логи по температуре и влажности---------------------------
  if ((setTimeAccompl == 1) && (writeSenseLog == 1)) {
    if (log_t == 0) log_t = 60;
    if (((Other_control_now - t_control_log_t ) >= log_t * 60 * 1000) || (t_control_log_t == 0)) {
      String message;
      for (byte i = 0; i <= NSenseFact; i++) {
        if (t[i] == 1) {//датчик активен (0-не использ,1-активен,2-используется но не активен)
          message += tNameSens[i] + " " + String(tempC[i]); message += F(":");
        }
      }

#ifdef USE_DHT
      message += "H " + String(h_dht_tek); message += F(":");
      message += "t " + String(t_dht_tek);
#endif

      if (message.length() > 0) addStringToLog(NameMesureLogFile, timeDateToStr(now_timeUnx), message);

      t_control_log_t = Other_control_now;
    }
  }



  if (((Other_control_now - Other_control ) > 1000) || (Other_control_now < 1000))
  { //выполняем



    //читаем состояния реле
    //что бы знать в каком оно состояниии сейчас
    //инвертируем полученный ответ если реле управляется низким уровнем
    now_relay1 = digitalRead(swt1_pin); if (!relay1Level) now_relay1 = !now_relay1;
    //now_relay2 = digitalRead(swt2_pin); if (!relay2Level) now_relay2 = !now_relay2;

    Other_control  = Other_control_now;
  }

  if ((setTimeAccompl == 1) && (writeSysLog == 1)) {
    if (systemStarted == 0) {
      addStringToLog(NameSysLogFile, timeDateToStr(now_timeUnx), F("System start"));
      systemStarted = 1;
    }
  }

  //---------------------------------------------------------------------------------------

  //---------------------- swt1_pin -------------------
  bool StatusRelay1;
  if (swt1 == true) {
    StatusRelay1 = HIGH;
    if (prev_swt1 != swt1) {
      t_Relay1 = millis() / 1000;
      if (resumeState == 1) {
        save_ResumeStateFile();
      }
      prev_swt1 = swt1;

      addStringToLog(NameSysLogFile, timeDateToStr(now_timeUnx), F("ON Relay1"));
    }
    t_work_Relay1 = millis() / 1000 - t_Relay1;
  }
  else {
    StatusRelay1 = LOW;
    if (prev_swt1 != swt1)   {
      if (resumeState == 1) {
        save_ResumeStateFile();
      }
      prev_swt1 = swt1;

      addStringToLog(NameSysLogFile, timeDateToStr(now_timeUnx), F("OFF Relay1"));
    }
  }
  MydigitalWrite(swt1_pin, StatusRelay1, relay1Level);

  //-------------------------------------------------------------
  if ((!ApMode) && (WiFi.status() == WL_CONNECTED)) {
    if (useMQTT && !mqttClient.connected()) {
      if (mqttReconnectSec == 0) mqttReconnectSec = 60;
      if ((Other_control_now - nextTimeMQTT) > mqttReconnectSec * 1000)   {
        mqttConnect();
        nextTimeMQTT = Other_control_now;
      }
    }
  }
  //-------------------------------------------------------------
  if (((Other_control_now - mqtt_control ) > 60000) || (mqtt_control == 0)) { //выполняем
    if (useMQTT && mqttClient.connected()) {
      mqttClient.publish(mqtt_topic(F("time/on")).c_str(), String(uint32_t(Other_control_now / 1000 / 60)).c_str(), saveValMQTT);
      mqtt_control = Other_control_now;

#ifdef USE_DHT
      if (isnan(h_dht) || isnan(t_dht))  {
        delay(25); mqttClient.publish(mqtt_topic(F("dht/t")).c_str(), "-", saveValMQTT);
        delay(25); mqttClient.publish(mqtt_topic(F("dht/h")).c_str(), "-", saveValMQTT);
      }
      else {
        delay(25); mqttClient.publish(mqtt_topic(F("dht/t")).c_str(), String(t_dht).c_str(), saveValMQTT);
        delay(25); mqttClient.publish(mqtt_topic(F("dht/h")).c_str(), String(h_dht).c_str(), saveValMQTT);
      }
#endif
    }
  }
  if (useMQTT && mqttClient.connected()) {
    mqttClient.loop();
  }
  //-------------------------------------------------------------
#ifdef USE_PIPE
  //для датчика потока

  currentTime = millis();
  if (currentTime >= (loopTime + time_calc))
  {
    str_info = F("");
    loopTime = currentTime;
    count_imp_all = count_imp_all + count_imp;

    liter_hour = (count_imp * 60 * 60 / count_imp_litr * time_calc / 1000);
    liter_min = (count_imp * 60 / count_imp_litr * time_calc / 1000);

    count_imp = 0;

    str_info += F("F: ");  str_info += String(count_imp_all / count_imp_litr, 2) + F(" л");
    str_info += F(" (");  str_info += String(liter_min) + F(" л/м)");

    if (useMQTT && mqttClient.connected()) {
      if (count_imp_all != count_imp_all_mqtt)  {
        delay(25); mqttClient.publish(mqtt_topic(F("pipe/l")).c_str(), String(count_imp_all / count_imp_litr, 2).c_str(), saveValMQTT);
        delay(25); mqttClient.publish(mqtt_topic(F("pipe/lm")).c_str(), String(liter_min).c_str(), saveValMQTT);
      }
      count_imp_all_mqtt = count_imp_all;
    }

  }


#endif
}
//------------------------------------------------------------------------------------------------------------------
