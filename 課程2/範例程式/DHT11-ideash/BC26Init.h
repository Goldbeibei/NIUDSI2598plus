#include <ArduinoJson.h>

byte Rset_Count=0;            //  ç³»çµ±é‡æ–°å•Ÿå‹•è¨ˆæ™‚å™¨å®£å‘Š
int waitingTime = 30000;      // ç­‰å€™ 30 ç§’çš„å›žè¦†.

String Check_RevData()  // è®€å–æ”¶åˆ°çš„æ¯ä¸€å­—å…ƒè³‡æ–™ ï¼Œå½™æ•´æˆä¸€å€‹å­—ä¸²
{
 String data= "";	
 char c;
 while (Serial1.available())
 {
  delay(50);
  c = Serial1.read(); //Conduct a serial read
  data+=c; //Shorthand for data = data + c
  if (c=='\n') break;
 }
 data.trim();
 return data;  
}

byte Send_ATcommand(String msg,byte stepnum) // å‚³é€ AT command ï¼Œä¸¦åŠ ä»¥åˆ¤æ–·
{
 String Showmsg,C_temp;
 Serial.println(msg);
 Serial1.println(msg);
 Showmsg=Check_RevData();
 //Serial.println(Showmsg);
 long StartTime=millis();
 switch (stepnum)
  {
    case 0:  // Reset BC26
         C_temp="+IP:";
         break;    
    case 1:         // Other Data
         C_temp="OK";
         break; 
    case 2:         // Check IPAddress 
         C_temp="+CGPADDR:"; 
         break; 
    case 10:  // build MQTT Server
         C_temp="+QMTOPEN: 0,0";
         break;        
    case 11: // Connect to MQTT server by username and password
         C_temp="+QMTCONN: 0,0,0";
         break;  
    case 12: // Publisher MQTT Data 
         C_temp="+QMTPUB: 0,0,0";
         break;        
    case 13: // Sub MQTT Data 
         C_temp="+QMTSUB: 0,1,0,0";
         break;
  }
  while (!Showmsg.startsWith(C_temp))
  {
   Showmsg=Check_RevData();
   if (Showmsg.startsWith("+")) Serial.println(Showmsg);
   if ((StartTime+waitingTime) < millis()) return stepnum;
  }          
  return 99;
}

bool BC26init()  // åˆå§‹åŒ– BC26
{
 Send_ATcommand("AT+QGACT=1,1,\"apn\",\"internet.iot\"",1);
 Send_ATcommand("AT+QCGDEFCONT=\"IP\",\"internet.iot\"",1);
 Send_ATcommand("AT+QBAND=1,8",1); 
 Send_ATcommand("AT+QRST=1",0);
 if (Send_ATcommand("ATE0",1)==99)
  if (Send_ATcommand("AT+CGPADDR=1",2)==99) return true;
 return false;   

}

bool connect_MQTT(String Serverx,String port,String user,String pass) // å»ºç«‹ MQTT é€£ç·šé€šé“
{
 String S_temp;
 S_temp = "\"" + Serverx + "\"" + "," + port;
 S_temp="AT+QMTOPEN=0," + S_temp;
 //Serial.println(S_temp);
 if (Send_ATcommand(S_temp,10)!=99) return false;
 //delay(100);
 S_temp= "\"" + user + "\"" + "," + "\"" + pass + "\"";
 S_temp="AT+QMTCONN=0,0," + S_temp;
 //Serial.println(S_temp);
 if (Send_ATcommand(S_temp,11)!=99) return false;
 //delay(100);
 return true;
}

bool Publish_MQTT(String topic, String message) // ç™¼ä½ˆè³‡æ–™
{ 
 String S_temp;
 S_temp = "\"" + topic + "\"" + "," + message ;
 S_temp = "AT+QMTPUB=0,0,0,0," + S_temp ;
 //Serial.println(S_temp);
 if (Send_ATcommand(S_temp,12)!=99) return false; 
 //delay(100);
 return true;
}

bool Sub_MQTT(String topic) // è¨‚é–±è³‡æ–™
{
 String S_temp;
 S_temp = "\"" + topic + "\"" + "," + "0";
 S_temp = "AT+QMTSUB=0,1," + S_temp; 
 //Serial.println(S_temp);
 if (Send_ATcommand(S_temp,13)!= 99) return false;  
 //delay(100); 
 return true; 
}

bool Close_MQTT() // é—œé–‰é€£ç·š
{ 
 String S_temp;
 S_temp="AT+QMTCLOSE=0";
 if (Send_ATcommand(S_temp,1)!=99) return false; 
 //delay(100);
 return true;
}


String JSON_DEC_data (String input,String findData)
{
 int index = input.indexOf(',');
 int x = input.substring(0, index).toInt();
 String json = input.substring(index + 1, input.length());
 //Serial.println(json);
 index = json.indexOf(':');
 x = json.substring(0, index).toInt();
 json = json.substring(index + 1, json.length());
 //Serial.println(json);
 DynamicJsonDocument doc(1024);
 deserializeJson(doc, json);
 JsonObject obj = doc.as<JsonObject>();
 return obj[findData];
}

bool Sub_Ideaschain(String attrestopic)
{
 String S_temp;
 S_temp="\""+ attrestopic + "\"" + "," + "0";
 S_temp="AT+QMTSUB=0,1," + S_temp;  // Qos 0 
 Serial.println(S_temp);
 Serial1.println(S_temp);
 delay (2000);
 return true; 
}

String Get_Publish_MQTT(byte mode,String attreqtopic , String message) // å–å¾— Ideaschain å±¬æ€§è³‡æ–™
{
 String Showmsg; 
 String S_temp,T_temp;
 //delay (1000);
 if (mode==0) T_temp="sharedKeys";
 if (mode==1) T_temp="clientKeys";
 S_temp="\"" + attreqtopic + "\"" + "," + "\"{\"" + T_temp + "\":\"" + message + "\"}\"";
 S_temp="AT+QMTPUB=0,0,0,0," + S_temp; 
 Serial.println(S_temp);
 Serial1.println(S_temp);
 Showmsg=Check_RevData();
 long StartTime=millis();
 while (!Showmsg.startsWith("+QMTRECV:"))
 {
  delay(100);
  Showmsg=Check_RevData();  
  if (Showmsg.length()>30) break;
  //Serial.println(Showmsg);
  if ((StartTime+waitingTime) < millis()) return "error";
 }
 //Serial.println(Showmsg);         
 return JSON_DEC_data (Showmsg,message);
}
