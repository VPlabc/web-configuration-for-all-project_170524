#include <Arduino.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "RTC.h"
RTCTime rtctime;
struct ts dtime;

byte Gethour = 0;
byte Getmin = 0;
byte Getsec = 0;
byte Getday = 0;
byte Getmonth = 0;
int Getyear = 0;
long GetEpoch = 0;
String Date = "";
String Time = "";
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;
void RTCGetTime(){
    #ifdef RTC_DS3231
    dtime = rtctime.GetRTC();
    #else
    LOGLN("RTC_DS3231 not defined");
    #endif//RTC_DS3231
    Getyear = dtime.year;
    Getmonth = dtime.mon;
    Getday = dtime.mday;
    Gethour = dtime.hour;
    Getmin = dtime.min;
    Getsec = dtime.sec;
    GetEpoch = dtime.unixtime;
}
void Time_setup();
void Time_loop();
void  SetTimeForRTC();
void GetTime(){
  #ifdef RTC_Onl
  // LOGLN("Wifimode: " + String(WifiMode));
if(WifiMode < 2){
    if(WiFi.status() == WL_CONNECTED) { 
    // while(!timeClient.update()) {
    //   timeClient.forceUpdate();
    // }
    for(int i = 0; i < 5; i++){
    Time_loop();
    // The formattedDate comes with the following format:
    // 2018-05-28T16:00:13Z
    // We need to extract date and time
    formattedDate = timeClient.getFormattedDate();
    Getyear = formattedDate.substring(0, 4).toInt(); //Serial.print("year: ");Serial.print(Getyear);
    Getmonth = formattedDate.substring(5, 7).toInt(); //Serial.print(" | month: ");Serial.print(Getmonth);
    Getday = formattedDate.substring(8, 10).toInt(); //Serial.print(" | day: ");Serial.println(Getday);
    Gethour = formattedDate.substring(11, 13).toInt(); //Serial.print("h: ");Serial.print(Gethour);
    Getmin = formattedDate.substring(14, 16).toInt(); //Serial.print(" m: ");Serial.print(Getmin);
    Getsec = formattedDate.substring(17, 19).toInt(); //Serial.print(" s: ");Serial.println(Getsec);
    GetEpoch = timeClient.getEpochTime();
    if(Getyear == 1900 || Getyear == 1970){}else{i = 6;}
    }
    if(Getyear == 1900 || Getyear == 1970){RTCGetTime();}
    }else{
      Serial.println("Error in WiFi connection");
      RTCGetTime();
    }
    formattedDate = "";
  }
  #endif//RTC_Onl
}
RTCTime cfRTCtime;
struct ts RTCdatetime;
void  SetTimeForRTC(){
#ifdef RTC_DS3231
  LOGLN("Check Time RTC");
  cfRTCtime.RTCSetup();
  RTCdatetime = cfRTCtime.GetRTC();
  LOG("Time RTC ");LOGLN(String(RTCdatetime.hour) + ":" + String(RTCdatetime.min) + " | " + String(RTCdatetime.mday) + "/" + String(RTCdatetime.mon) + "/" + String(RTCdatetime.year));
  GetTime();
  LOG("Time Online ");LOGLN(String(Gethour) + ":" + String(Getmin) + " | " + String(Getday) + "/" + String(Getmonth) + "/" + String(Getyear));
  if((RTCdatetime.hour == Gethour && RTCdatetime.min == Getmin && RTCdatetime.mday == Getday && RTCdatetime.mon == Getmonth && RTCdatetime.year == Getyear)&&(Getyear != 1900 && Getyear != 1970)){LOGLN("Real Timne ok!");}
  else{if((Getyear != 1900 && Getyear != 1970)){LOGLN("SetTimeForRTC");cfRTCtime.SetRTC(Getyear, Getmonth, Getday, Gethour, Getmin, Getsec, GetEpoch);RTCdatetime = cfRTCtime.GetRTC();
  LOGLN(String(RTCdatetime.hour) + ":" + String(RTCdatetime.min) + " | " + String(RTCdatetime.mday) + "/" + String(RTCdatetime.mon) + "/" + String(RTCdatetime.year)); }
  }
#endif//RTC_DS3231
}
void Time_setup() {
  // Initialize Serial Monitor

// Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(3600*7);
}
bool TimeOnce = 1;
bool TimeDSOnce = 1;
void Time_loop() {
  if(!timeClient.update() && WiFi.status() == WL_CONNECTED) {
    timeClient.forceUpdate();
  }else{
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
  // if(TimeOnce){Serial.println(formattedDate);}

  // Extract date
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  if(Getyear != 1900 && Getyear != 1970){
    if(TimeOnce){
    Serial.print("DATE: ");
    Serial.println(dayStamp);}
    Date = dayStamp;
    // Extract time
    timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
    if(TimeOnce){
    Serial.print("TIME: ");
    Serial.println(timeStamp);TimeOnce = false;}
    Time = timeStamp;
    GetEpoch = timeClient.getEpochTime();
    }
  }
//   delay(1000);
}