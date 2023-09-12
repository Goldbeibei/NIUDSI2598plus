#include <SimpleDHT.h> // Arduino 板通訊庫 
#include "BC26Init.h"
String MQTT_Server="iiot.ideaschain.com.tw";  //MQTT Server 的 IP 位址
String MQTT_Port="1883";                          //MQTT 通訊埠號
String MQTT_Access_token="eviD0n3tB1qMOEyBo9yI";      //通訊鑰匙用的 Token ==> IDEAS Chain提供
String MQTTtopic="v1/devices/me/telemetry";   //上傳感測資料到 IDEAS Chain
String MQTTmessage="";

String MQTTattrestopic="v1/devices/me/attributes/response/+";  //接收 IDEAS Chain 傳送的控制資料
String MQTTattreqtopic="v1/devices/me/attributes/request/1";   //接收 IDEAS Chain 傳送的控制資料
String DelayTime_Text="DelayTime";             // 延遲時間的屬性參數名稱
String TempHigh_Text="TempHigh";               // 溫度高閾值的屬性參數名稱
String TempLow_Text="TempLow";                 // 溫度低閾值的屬性參數名稱
byte mode=0;                                   // 0 => 用硬體顯示溫度狀態  1 => 用序列阜顯示溫度狀態

String DelayTime;
String TempHigh;
String TempLow;

String Sensor1="Temperature";
String Sensor2="Humidity";

//"AT+QMTPUB=0,0,0,0,\"v1/devices/me/telemetry\",\"{\"key1\":data, \"key2\":data}\""

const int pinDHT11 = PB9;                           // DHT11 資料腳位於 PB9 引腳
SimpleDHT11 dht11(pinDHT11);
int err = SimpleDHTErrSuccess;
float temperature = 0;
float humidity = 0;
float CorrectionTEMP = 0.0;                       // 溫度校正值
float CorrectionHUMI = 20.0;                      // 濕度校正值

bool Get_ideaschain_data(String attrestopic,String attreqtopic)  //接收 IDEAS Chain 傳送的控制資料
{
  Sub_Ideaschain(attrestopic);
  DelayTime=Get_Publish_MQTT(mode,attreqtopic,DelayTime_Text);
  TempHigh=Get_Publish_MQTT(mode,attreqtopic,TempHigh_Text);
  TempLow=Get_Publish_MQTT(mode,attreqtopic,TempLow_Text);
  
  if (DelayTime.toInt()!=0 && TempHigh.toInt()!=0 && TempLow.toInt()!=0 )
  {
    Serial.println("接收延遲時間 : " + DelayTime + " 秒");
    Serial.println("接收溫度高閾值 : " + TempHigh + " 度C");
    Serial.println("接收溫度低閾值 : " + TempLow + " 度C");
    return true;
  }
  return false;
}

void setup()
{
  Serial.begin(115200);
  Serial1.begin(115200);
  pinMode(PB6,OUTPUT);          // 發送資料完成後,重設模組的腳位定義
  pinMode(PB7,OUTPUT);          // 溫度低於低閾值時,點亮紅燈的腳位定義
  pinMode(PB8,OUTPUT);          // 溫度高於高閾值時,點亮紅燈的腳位定義
  pinMode(PC13,OUTPUT);
  digitalWrite(PB6, HIGH);
  
  // Rest BC26 ===
  
  digitalWrite(PC13, LOW);
  delay(30);
  digitalWrite(PC13, HIGH);
  delay (3000);
  // =============
  
  if (!BC26init()) {delay (10000); nvic_sys_reset();}
  Serial.println("準備連線至雲端 ....");
}

void loop()
{
  if (connect_MQTT(MQTT_Server,MQTT_Port,MQTT_Access_token,MQTT_Access_token))
  {
    Rset_Count++;
    if (Get_ideaschain_data(MQTTattrestopic,MQTTattreqtopic))
    {
      Serial.println("=================================");
      Serial.println("開始讀取 DHT11 溫濕度資料....");
      int err = SimpleDHTErrSuccess;
      if ((err = dht11.read2(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess)
      {
        Serial.print("Read DHT11 failed, err=");
        Serial.println(err);
        Close_MQTT();
        delay(1500);
        return;
      }
      delay(500);
      Serial.print("偵測到溫度值");Serial.print((int)temperature - CorrectionTEMP); Serial.println(" 度C");
      Serial.print("偵測到濕度值");Serial.print((int)humidity - CorrectionHUMI); Serial.println(" RH");
      String DHTtemp = String(temperature - CorrectionTEMP);
      String DHThumi = String(humidity - CorrectionHUMI);
      MQTTmessage="\"{\"" + Sensor1 + "\":" +  DHTtemp + "," + "\"" + Sensor2 + "\":" +  DHThumi + "}\"";
      if (Publish_MQTT(MQTTtopic,MQTTmessage))
      {
        if (TempHigh.toInt() < (temperature - CorrectionTEMP)) digitalWrite(PB8, HIGH);
        else digitalWrite(PB8, LOW);
        if (TempLow.toInt() > (temperature - CorrectionTEMP)) digitalWrite(PB7, HIGH);
        else digitalWrite(PB7, LOW);
        Rset_Count=0;
      }
    }
    Close_MQTT();
    Serial.println("延遲 " + DelayTime +" 秒後再傳送 !!");
    delay (DelayTime.toInt()*1000);
    if (Rset_Count>20) {delay (10000);nvic_sys_reset();}
  }
} 
