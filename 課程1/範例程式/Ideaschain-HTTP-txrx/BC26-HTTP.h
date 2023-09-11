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
    case 20:  // Connect HTTP Server
         C_temp="+QIOPEN: 0,0";
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

bool Connect_HTTP_Server(String S_name) // å»ºç«‹ HTTP é€£ç·šé€šé“
{
 String S_temp="";
 S_temp="AT+QIOPEN=1,0,\"TCP\",\"" + S_name + "\",80,0,0";
 //Serial.println(S_temp);
 if (Send_ATcommand(S_temp,20)!=99) return false;
 //Send_ATcommand("AT+QICFG=\"viewmode\",1",4);
 delay(100);
 return true;  
}

bool Close_HTTP()
{
 if (Send_ATcommand("AT+QICLOSE=0",1)!=99) return false;
 delay(100);
 return true; 
}

bool Send_Data_IdeasChain(String token,String D_Message) // HTTP æ–¹å¼å‚³é€è³‡æ–™çµ¦ç¶²ç«™
{
 String Send_check=""; 
 Serial.println("AT+QISEND=0");   
 Serial1.println("AT+QISEND=0"); 
 Send_check=Check_RevData();
 Serial.println(Send_check);
 
 while (!Send_check.startsWith(">"))
  Send_check=Check_RevData();
 Serial.println("POST " + token + " HTTP/1.1");
 Serial1.println("POST " + token + " HTTP/1.1");
 Serial.println("Host: iiot.ideaschain.com.tw");
 Serial1.println("Host: iiot.ideaschain.com.tw");
 Serial.println("Content-Type: application/json");
 Serial1.println("Content-Type: application/json");
 Serial.println("Content-Length:" + String(D_Message.length()));
 Serial1.println("Content-Length:" + String(D_Message.length()));
 Serial1.println();
 Serial.println(D_Message); 
 Serial1.print(D_Message); //JSON Data
 Serial1.print("\x1A");

 Send_check=Check_RevData(); 
 Serial.println(Send_check);
 while (!Send_check.startsWith("SEND OK"))
  Send_check=Check_RevData();
 Serial.println(Send_check);
 Send_check=Check_RevData();
 while (!Send_check.startsWith("+QIURC: \"recv\",0"))
  Send_check=Check_RevData();
 Serial.println(Send_check); 
 return true; 
}

//================ Get ==================
String DEC_data (String input)
{
 int index = input.indexOf('{');
 int x = input.substring(0, index).toInt();
 String temp = input.substring(index , input.length());
 //Serial.println(temp);
 index = temp.indexOf(':');
 x = temp.substring(0, index).toInt();
 temp = temp.substring(index +1 , temp.length());
 //Serial.println(temp);
 index = temp.indexOf(':');
 x = temp.substring(0, index).toInt();
 temp = temp.substring(index +1 , temp.length());
 //Serial.println(temp);
 index = temp.indexOf('}');
 temp = temp.substring(0 , index);
 return temp;
}

String Get_QIRD_DATA() // å–å¾—å›žå‚³è³‡æ–™
{ 
 String S_temp="";
 S_temp="AT+QIRD=0,512";
 Serial.println(S_temp);
 Serial1.println(S_temp);
 while(!Serial1.available());
 while(Serial1.available())
  S_temp=Serial1.readString();
 if (S_temp.length()>30)
 {
  Serial.println(S_temp);
  return (DEC_data(S_temp));
 }
 else return "Empty";
}

String Get_Data_IdeasChain(byte mode,String token,String key) // HTTP æ–¹å¼å–å¾—ç¶²ç«™è³‡æ–™
{
 String Send_check=""; 
 String Get_data="";
 Serial.println("AT+QISEND=0");   
 Serial1.println("AT+QISEND=0"); 
 Send_check=Check_RevData();
 //Serial.println(Send_check);
 while (!Send_check.startsWith(">"))
  Send_check=Check_RevData();
 if (mode==0)
 {
  Serial.println("GET " + token + "?sharedKeys=" + key +" HTTP/1.1");
  Serial1.println("GET " + token + "?sharedKeys=" + key +" HTTP/1.1");
 }
 if (mode==1)
 {
  Serial.println("GET " + token + "?clientKeys=" + key +" HTTP/1.1");
  Serial1.println("GET " + token + "?clientKeys=" + key +" HTTP/1.1");
 } 
 Serial.println("Host: iiot.ideaschain.com.tw");
 Serial1.println("Host: iiot.ideaschain.com.tw");
 Serial1.print("\r\n\r\n\x1A");
 Send_check=Check_RevData(); 
 //Serial.println(Send_check);
 while (!Send_check.startsWith("SEND OK"))
  Send_check=Check_RevData();
 Serial.println(Send_check);
 Send_check=Check_RevData();
 while (!Send_check.startsWith("+QIURC: \"recv\",0"))
  Send_check=Check_RevData();
 Serial.println(Send_check);
 return Get_QIRD_DATA();
}
