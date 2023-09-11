#include "BC26-HTTP.h"

String Server_Name = "iiot.ideaschain.com.tw";  // HTTP Server 的 IP 位址
String Access_Token = "L4ke8GEimjpc6gPbmu8E";   // 使用者資訊 ==> IDEAS Chain存取權杖
String Attrib_Key = "TestValue";             // 屬性的 Key 
String Attrib_Data_String = "telemetry";
String Attrib_Client_String = "attributes";
String DATA_Attrib;
String DATA_Message;
byte Attrib_Mode = 1;                           // 0 -> sharedKeys ; 1 -> clientKeys
int test_value = 20020304;

// ** iiot.ideaschain.com.tw/api/v1/$ACCESS_TOKEN/telemetry  // 時間序列資料上傳
// ** iiot.ideaschain.com.tw/api/v1/$ACCESS_TOKEN/attributes // 上傳客戶端設備屬性值
// ** iiot.ideaschain.com.tw/api/v1/$ACCESS_TOKEN/attributes?clientKeys=key 取得客戶端設備屬性值
// ** 回傳的資料格式 "{clientKeys:{"key":"value"}}"

void setup()
{ 
 Serial.begin(115200);
 Serial1.begin(115200);
 pinMode(PC13, OUTPUT); 
 
 // 重置 BC26 ===
 digitalWrite(PC13, LOW);
 delay(30);
 digitalWrite(PC13, HIGH);
 delay(3000);
 // ============= 

 if (!BC26init()) {delay(10000); nvic_sys_reset();}
 Serial.println("初始化完成....");
} 

void loop()
{
 Serial.println("開始傳送資料...."); 
 if (Connect_HTTP_Server(Server_Name))
 {
  DATA_Attrib = "/api/v1/" + Access_Token + "/" + Attrib_Client_String;
  DATA_Message = "{\"" + Attrib_Key + "\":" + String(test_value) + "}";
  if (Send_Data_IdeasChain(DATA_Attrib, DATA_Message))
   Serial.println("資料傳送成功!!");
  Close_HTTP();  
 }
 else Serial.println("資料傳送失敗.....");

 Serial.println("\n\n延遲 10 秒.....\n\n");
 delay(10000);
 Serial.println("開始取得資料....");
 
 String G_data;
 if (Connect_HTTP_Server(Server_Name))
 {
  DATA_Attrib = "/api/v1/" + Access_Token + "/" + Attrib_Client_String;
  G_data = Get_Data_IdeasChain(Attrib_Mode, DATA_Attrib, Attrib_Key);
  if (G_data != "Empty")
   Serial.println("\n獲取 " + Attrib_Key + " 的資料為: " + G_data);
  Rset_Count = 0;     
 }
 Close_HTTP();
 Serial.println("資料獲取成功!!");
 delay(3000);
 exit(0);  
}
