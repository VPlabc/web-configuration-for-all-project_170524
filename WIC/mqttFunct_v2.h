/*
Command Description
Get backup Data
{"id":"0","type":"getbackup","time":1721926897}
Disable MQTT
{"id":"0","type":"disableMQTT"}
Enable MQTT
{"i":"0","type":"enableMQTT"}

//SICK Sig350
http://192.168.0.200"
/iolink/v1/devices/master1port1/processdata/getdata/value
//SICK Sig200
/********************************
http://192.168.2.253
/iolink/v1/devices/master1port1/processdata/getdata/value


Get backup Data
{"id":"0","type":"getbackup","port":1,"d":31,"m":7,"y":2024,"h":12,"min":8}
Delete backup Data
{"id":"0","type":"deletebackup","port":1,"d":31,"m":7,"y":2024,"h":12,"min":8}
*/

#include "config.h"
CONFIG MQTT_Cofig;
#include <WiFi.h>
extern "C" {
	#include "freertos/FreeRTOS.h"
	#include "freertos/timers.h"
}
#include <AsyncMqttClient.h>

#ifdef MQTT
// TaskHandle_t Task3;
#endif//MQTT
void MQTT_loop();
// #ifdef MQTT
// #include "Tsk3.h"
// #endif//MQTT
// #define WIFI_SSID "yourSSID"
// #define WIFI_PASSWORD "yourpass"

// #define MQTT_HOST IPAddress(192, 168, 1, 10)
// #define MQTT_PORT 1883

AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
// TimerHandle_t wifiReconnectTimer;
  String MessageInput = "";

// void connectToWifi() {
//   Serial.println("Connecting to Wi-Fi...");
//   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
// }
bool MQTTOnce = false;
bool MQTTOnceDis = true;
String Topic = "";
uint32_t GetBackupTime = 0;

void updateLog();
void MQTTPush(String Topic,String Payload);
void Log_MQTT(String msg);
void Log_setting_MQTT(String msg);
void connectToMqtt() {
  if(WiFi.status() == WL_CONNECTED){LOGLN("WiFi connected");}
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void onMqttConnect(bool sessionPresent) {
  MQTT_Cofig.MQTTConnected = true;
  Serial.println("Connected to MQTT.");
    #ifdef RTC_Onl
      GetTime();
    #endif//RTC_Onl≈
  // FS_SQL_log("Connected from MQTT.", GetEpoch);
  // Serial.print("Session present: ");
  // Topic  = SettingData[18] +  "/node "+String(SettingData[21]) + "/in" + String(SettingData[21]);    
  // Serial.println(sessionPresent);
  // LOGLN("Sub topic: " + Topic);
  uint16_t packetIdSub = mqttClient.subscribe(Topic.c_str(), 2);
  Serial.print("Subscribing at QoS 2, packetId: ");
  Serial.println(packetIdSub);
  // TopicSub  = SettingData[18] +  "/node "+String(SettingData[21]) + "/out" + String(SettingData[21]);    
  // Serial.println(sessionPresent);
  // LOGLN("Sub topic: " + TopicSub);
  // packetIdSub = mqttClient.subscribe(TopicSub.c_str(), 2);
  // Serial.print("Subscribing at QoS 2, packetId: ");
  // Serial.println(packetIdSub);
  Topic  = "/iSoft/node/getting";    
  Serial.println(sessionPresent);
  LOGLN("Sub topic: " + Topic);
  uint16_t packetIdSub1 = mqttClient.subscribe(Topic.c_str(), 2);
  Serial.print("Subscribing 1 at QoS 2, packetId: ");
  Serial.println(packetIdSub1);
  if(MQTTOnceDis){MQTTOnceDis = false;
    #ifdef Log
    #ifdef RTC_Onl
    logContent = "Start at " + String(Getday) + "/" + String(Getmonth) + "/" +  String(Getyear) + " | "   + Time + "\n";
    #endif//
    Topic  = SettingData[18] +  "/node "+String(SettingData[21]) + "/status" + String(SettingData[21]);    
    mqttClient.publish(Topic.c_str(), 2, true ,logContent.c_str());
    #endif// log
  }
  // FS_SQL_log("Connected from MQTT.", GetEpoch);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  #ifdef RTC_Onl
    GetTime();
  #endif//RTC_Onl
  Serial.println("Disconnected from MQTT.");
    #ifdef RTC_Onl
    FS_SQL_log("MQTT0", GetEpoch);

      MQTTConnected = false;
    pinMode(BUZZ, OUTPUT);
    BUZZ_ON;delay(50); BUZZ_OFF;
    #endif//RTC_Onl
  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("Subscribe OK");
  // Serial.println("Subscribe acknowledged.");
  // Serial.print("  packetId: ");
  // Serial.println(packetId);
  // Serial.print("  qos: ");
  // Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("Unsubscribe OK.");
  // Serial.println("Unsubscribe acknowledged.");
  // Serial.print("  packetId: ");
  // Serial.println(packetId);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.print(topic);
  Serial.print(" | qos: ");
  Serial.print(properties.qos);
  Serial.print(" | dup: ");
  Serial.print(properties.dup);
  Serial.print(" | retain: ");
  Serial.println(properties.retain);
  
  for (int i=0;i<len;i++) {
    // Serial.print((char)payload[i]);
    MessageInput+= (char)payload[i];
  }
  MQTTOnce = true;
}

void onMqttPublish(uint16_t packetId) {
//   Serial.println("Publish acknowledged.");
//   Serial.print("  packetId: ");
//   Serial.println(packetId);
  Serial.print("->");
  if(MQTTOnceDis){MQTTOnceDis = false;
    #ifdef Log
    #ifdef RTC_Onl
    logContent = "Start at " + String(Getday) + "/" + String(Getmonth) + "/" +  String(Getyear) + " | "   + Time + "\n";
    #endif//RTC_Onl
    Topic  = SettingData[18] +  "/node "+String(SettingData[21]) + "/status" + String(SettingData[21]);    
    if(MQTTConnected)mqttClient.publish(Topic.c_str(), 2, true ,logContent.c_str());
  #endif// log
  }
}


const char* mqttserver = "";
const char* charmqttUserName = "";
const char* charmqttUserPassword = "";
String mqttbroker = "";
String mqttUserName="";
String mqttUserPassword="";
int32_t MQTTcount = 0;
bool PrintOnce = 1;
void MQTT_setup() {
//   Serial.begin(115200);
//   Serial.println();
//   Serial.println();
LOGLN("MQTT Init");
  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
//   wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

  // WiFi.onEvent(WiFiEvent);
 #ifdef FILECONFIG
  int mqttPort;
  CFRespondNetworkData MQTTNetworkDatas;
  MQTTNetworkDatas = CONFIG::init_Network_config();
  if(PrintOnce){
    // LOGLN("___ MQTT ___ \n MQTT Host: " + MQTTNetworkDatas.MQhost);
    // LOGLN("MQTT Port: " + MQTTNetworkDatas.MQport);
    // LOGLN("MQTT User: " + MQTTNetworkDatas.MQuser);
    // LOGLN("MQTT Password: " + MQTTNetworkDatas.MQpass);
  }
  mqttPort = MQTTNetworkDatas.MQport.toInt();
  mqttbroker = MQTTNetworkDatas.MQhost;mqttbroker.replace(" ","");
  mqttserver = mqttbroker.c_str();
  mqttUserName = MQTTNetworkDatas.MQuser;mqttUserName.replace(" ","");
  charmqttUserName = mqttUserName.c_str();
  mqttUserPassword = MQTTNetworkDatas.MQpass;mqttUserPassword.replace(" ","");
  charmqttUserPassword = mqttUserPassword.c_str();
  #else//EEPROM
    if(CONFIG::read_string (EP_MQTT_BROKER, mqttbroker, MAX_MQTT_BROKER_LENGTH)){
      if(PrintOnce)LOGLN("mqtt broker:" + String(mqttbroker));
    }
      mqttbroker.replace(" ","");
      mqttserver = mqttbroker.c_str();
    if(CONFIG::read_string (EP_MQTT_USER, mqttUserName, MAX_MQTT_USER_LENGTH)){
      if(String(mqttUserName) == "_"){charmqttUserName = "";}
      mqttUserName.replace("_","");
      if(PrintOnce)LOGLN("mqtt user:" + String(mqttUserName));
    }
      mqttUserName.replace(" ","");
      charmqttUserName = mqttUserName.c_str();
    if(CONFIG::read_string (EP_MQTT_PASS, mqttUserPassword, MAX_MQTT_PASS_LENGTH)){
      if(String(mqttUserPassword) == "_"){charmqttUserPassword = "";}
      mqttUserPassword.replace("_","");
      if(PrintOnce)LOGLN("mqtt pass:" + String(mqttUserPassword));
    } 

    mqttUserPassword.replace(" ","");
    charmqttUserPassword = mqttUserPassword.c_str();

    int mqttPort;
    if(CONFIG::read_buffer (EP_MQTT_PORT,  (byte *) &mqttPort, INTEGER_LENGTH)){
      if(PrintOnce)LOGLN("mqtt port:" + String(mqttPort));
    }
  #endif//#ifdef FILECONFIG
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(mqttserver, mqttPort);
  mqttClient.setCredentials(charmqttUserName, charmqttUserPassword);
  // LOGLN("MQTT info:" + mqttserver + " | " + charmqttUserName + " | " +  charmqttUserPassword + " | " +  mqttPort);
  connectToMqtt();
  
  delay(5000);
//Task3 : MQTT task
  // xTaskCreatePinnedToCore(
  //   Task3code,   /* Task function. */
  //   "Task3",     /* name of task. */
  //   5000,       /* Stack size of task */
  //   NULL,        /* parameter of the task */
  //   1,           /* priority of the task*/
  //   &Task3,      /* Task handle to keep track of created task */
  //   1);          /* pin task to core 0/1 */
  // delay(500);
LOGLN("MQTT Running");
//   connectToWifi();
}

void MQTT_loop() {


}//MQTT_loop

void MQTTPush(String Topic,String Payload) {
  if(WiFi.status() == WL_CONNECTED && MQTT_Cofig.MQTTConnected)mqttClient.publish(Topic.c_str(), 0, true ,Payload.c_str());
}

///////////////////////////////// MQTT ////////////////////////////////////
// void Log_MQTT(String msg){
//   if(WiFi.status() == WL_CONNECTED){
//       Topic  = SettingData[18] +  "/node "+ String(SettingData[21])+"/log" + String(SettingData[21]);    
//       if(MQTTConnected)mqttClient.publish(Topic.c_str(), 2 , true ,msg.c_str());
//       LOGLN("MQTT Log");
//   }
// }
///////////////////////////////// MQTT ////////////////////////////////////
// void Log_setting_MQTT(String msg){
//   if(WiFi.status() == WL_CONNECTED){
//       Topic  = SettingData[18] +  "/node "+ String(SettingData[21])+"/setting" + String(SettingData[21]);    
//       mqttClient.publish(Topic.c_str(), 2 , true ,msg.c_str());
//       LOGLN("MQTT setting");
//   }
// }
void updateLog(){
    #ifdef BOARD_4I4O2AER
  State5 = State[4];//digitalRead(RL1);
  State6 = State[5];//digitalRead(RL2);
  State7 = State[6];//digitalRead(RL3);
  State8 = State[7];//digitalRead(RL4);
  #endif//BOARD_4I4O2AER
    #ifdef BOARD_4I10O2AER
    State5 = State[4];// mcp.digitalRead(0);
    State6 = State[5];//mcp.digitalRead(1);
    State7 = State[6];//mcp.digitalRead(2);
    State8 = State[7];//mcp.digitalRead(3);
    #endif//BOARD_4I10O2AER
  // String StateLists = "{\"id\":0,\"type\":\"get\",\"ready\":\""+ String(ready) + "\",\"state1\":\""+ String(State[0])  + "\",\"status1\":\""+ String(statusState[0]) + "\",\"state2\":\"" + String(State[1])  + "\",\"status2\":\""+ String(statusState[1])  + "\",\"state3\":\"" + String(State[2])  + "\",\"status3\":\""+ String(statusState[2])  + "\",\"state4\":\"" + String(State[3])  + "\",\"status4\":\""+ String(statusState[3])  + "\",\"state5\":\"" + String(State[4])  + "\",\"status5\":\""+ String(statusState[4])  + "\",\"state6\":\"" + String(State[5])  + "\",\"status6\":\""+ String(statusState[5])  + "\",\"state7\":\"" + String(State[6])  + "\",\"status7\":\""+ String(statusState[6])  + "\",\"state8\":\"" + String(State[7])  + "\",\"status8\":\""+ String(statusState[7]) + "\"}";
  // Log_MQTT(StateLists);StateLists = "";
}

