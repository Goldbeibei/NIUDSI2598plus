#include <SimpleDHT.h> // Arduino 板通訊庫 
#include "BC26Init.h"

//狀態
bool run_init = false;
bool sub_status = false;
//LED
bool LED_r = false;
bool LED_g = false;
bool LED_b = false;
//取得字元
String data= "";
char c;
//E608 sensor    
String E608_temp;
String E608_humi;
String E608_time;

//DSI2598+ sensor
String DHT22_temp;
const int pinDHT22 = PB9; // DHT22 資料腳位於 PB9 引腳
SimpleDHT22 dht22(pinDHT22);
int err = SimpleDHTErrSuccess;
float DHT22_temp_f = 0;
float DHT22_humi_f = 0;
//DHT22 timer
unsigned long startTime; 
unsigned long interval = 3000; 
//LED timer
unsigned long LED_startTime; 
unsigned long LED_interval = 3000;

void setup()
{
  Serial.begin(115200);
  Serial1.begin(115200);

  //LED
  pinMode(PB6,OUTPUT);          // LED_b
  pinMode(PB7,OUTPUT);          // LED_g
  pinMode(PB8,OUTPUT);          // LED_r
  
  // Rest BC26 ===
  pinMode(PC13, OUTPUT);
  digitalWrite(PC13, LOW);
  delay(30);
  digitalWrite(PC13, HIGH);
  delay (3000);
  // =============

  //timer init
  startTime = millis();
  LED_startTime = millis();
  
  if (!BC26init()) {Serial.println("初始化失敗請重新開機"); delay (10000); nvic_sys_reset();}
  LED_g = true;
  digitalWrite(PB7, HIGH);
  Serial.println("初始化完成");
}

void loop()
{
  // MQTT初始化
  if(!run_init)
  {
    delay(1000);
    /*請加入程式碼*/
    run_init = true;
    //可能要等10秒才會穩定正確取得資料
    delay(2000);
  }

  // MQTT訂閱
  if(run_init && !sub_status)
  {
    /*請加入程式碼*/
  }
  
  // 取得DHT22資料
  unsigned long currentTime = millis();
  if (currentTime - startTime >= interval)
  {
    /*請加入程式碼*/
    //DHT22_temp_f = 10.0;
    DHT22_temp = String(DHT22_temp_f);
    //Serial.println(DHT22_temp);
    startTime = currentTime; //reset timer
  }
  // 取得並傳出MQTT E608_sensor資料
  if(sub_status)
  {
    //改成非阻塞式讀取資料
    String Showmsg;
    if(Serial1.available())
    {
      c = Serial1.read(); // 進行序列讀取
      data+=c; // 將資料連接在一起
      if (c=='\n')
      {
        data.trim();
        Showmsg = data;
        data="";
      }
    }
    
    //判斷收到資料是否符合格式 
    /*請加入程式碼*/
    }

    //正確收到所有資訊後輸出+LED判斷並亮燈
    if(E608_temp.length()!=0 && E608_humi.length()!=0 && E608_time.length()!=0)
    {
      /*請加入程式碼*/
    }
  }

  // 發佈LED狀態
  unsigned long LED_currentTime = millis();
  if (LED_currentTime - LED_startTime >= LED_interval && run_init && sub_status)
  {
    /*請加入程式碼*/
    LED_startTime = LED_currentTime; //reset timer
  }
}
