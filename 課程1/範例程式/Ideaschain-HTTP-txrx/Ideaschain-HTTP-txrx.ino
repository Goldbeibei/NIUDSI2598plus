#include "BC26-HTTP.h"

String Server_Name="iiot.ideaschain.com.tw";  //HTTP Server çš„ IP ä½å€
String Access_Token="L4ke8GEimjpc6gPbmu8E";   //ä½¿ç”¨è€…è³‡è¨Š ==> IDEAS Chainå­˜å–æ¬Šæ–
String Attrib_Key="TestValue";             //å±¬æ€§çš„ Key 
String Attrib_Data_String="telemetry";
String Attrib_Client_String="attributes";
String DATA_Attrib;
String DATA_Message;
byte Attrib_Mode=1;                           // 0 -> sharedKeys ; 1 -> clientKeys
int test_value= 20020304;

// ** iiot.ideaschain.com.tw/api/v1/$ACCESS_TOKEN/telemetry  //æ™‚é–“åºåˆ—è³‡æ–™ä¸Šå‚³
// ** iiot.ideaschain.com.tw/api/v1/$ACCESS_TOKEN/attributes //ä¸Šå‚³å®¢æˆ¶ç«¯è¨­å‚™å±¬æ€§å€¼
// ** iiot.ideaschain.com.tw/api/v1/$ACCESS_TOKEN/attributes?clientKeys=key å–å¾—å®¢æˆ¶ç«¯è¨­å‚™å±¬æ€§å€¼
// ** Return of Data Format "{clientKeys:{"key":"value"}}

void setup()
{ 
 Serial.begin(115200);
 Serial1.begin(115200);
 pinMode(PC13,OUTPUT); 
 
 // Rest BC26 ===
 digitalWrite(PC13, LOW);
 delay(30);
 digitalWrite(PC13, HIGH);
 delay (3000);
 // ============= 
 if (!BC26init()) {delay (10000); nvic_sys_reset();}
 Serial.println("initialization OK ....");
} 

void loop()
{
 Serial.println("Star Send Data ...."); 
 if (Connect_HTTP_Server(Server_Name))
 {
  DATA_Attrib="/api/v1/" + Access_Token + "/" + Attrib_Client_String;
  DATA_Message="{\"" + Attrib_Key + "\":" +  String(test_value) + "}";
  if (Send_Data_IdeasChain(DATA_Attrib,DATA_Message))
   Serial.println("Data Send OK !!");
  Close_HTTP();  
 }
 else Serial.println("Data Send Fail .....");

 Serial.println("\n\nDelay 10 second .....\n\n");
 delay (10000);
 Serial.println("Star Get Data ....");
 
 String G_data;
 if (Connect_HTTP_Server(Server_Name))
 {
  DATA_Attrib="/api/v1/" + Access_Token + "/" + Attrib_Client_String;
  G_data=Get_Data_IdeasChain(Attrib_Mode,DATA_Attrib,Attrib_Key);
  if (G_data!="Empty")
   Serial.println("\nGet " + Attrib_Key + " of Data is : " + G_data);
  Rset_Count=0;     
 }
 Close_HTTP();
 Serial.println("Data Get OK !!");
 delay(3000);
 exit(0);  
}
