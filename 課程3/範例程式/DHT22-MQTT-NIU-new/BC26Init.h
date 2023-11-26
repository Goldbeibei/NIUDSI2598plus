//byte Rset_Count=0; // 系統重新開機計時器變數
int waitingTime = 30000; // 等待30秒的回覆

String Check_RevData() // 讀取收到的每一個字元資料,當成一個字串
{
  String data= "";
  char c;
  while (Serial1.available())
  {
    delay(50);
    c = Serial1.read(); // 進行序列讀取
    data+=c; // 將資料連接在一起
    if (c=='\n') break;
  }
  data.trim();
  return data;
}

byte Send_ATcommand(String msg,byte stepnum) // 傳送AT指令,並加以判斷 
{
  String Showmsg,C_temp;
  Serial.println(msg);
  Serial1.println(msg);
  Showmsg=Check_RevData();
  
  long StartTime=millis();
  switch (stepnum)
  {
    case 0: // 重設 BC26
         C_temp="+IP:";
         break;
         
    case 1: // 其他資料
         C_temp="OK";
         break;
         
    case 2: // 檢查 IPAddress  
         C_temp="+CGPADDR:";
         break;
         
    case 10: // 建立 MQTT 伺服器
         C_temp="+QMTOPEN: 0,0";
         break;
         
    case 11: // 使用帳號密碼連線 MQTT 伺服器
         C_temp="+QMTCONN: 0,0,0";
         break;
         
    case 12: // 發佈 MQTT 資料
         C_temp="+QMTPUB: 0,0,0";
         break;
         
    case 13: // 訂閱 MQTT 資料
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

bool BC26init() // 初始化 BC26
{
  Send_ATcommand("AT+QGACT=1,1,\"apn\",\"internet.iot\"",1);
  Send_ATcommand("AT+QCGDEFCONT=\"IP\",\"internet.iot\"",1); 
  Send_ATcommand("AT+QBAND=1,8",1);
  Send_ATcommand("AT+QRST=1",0);
  
  if (Send_ATcommand("ATE0",1)==99) 
    if (Send_ATcommand("AT+CGPADDR=1",2)==99) return true;
        
  return false;
}

bool MQTT_SUB() // MQTT訂閱
{
  //if(Send_ATcommand("AT+QMTSUB=0,1,\"sensor1/#\",0",13)!=99) return false;
  Serial1.println("AT+QMTSUB=0,1,\"sensor1/#\",0");
  return true;
}

bool MQTT_UNS() // MQTT退訂
{
  //if(Send_ATcommand("AT+QMTUNS=0,1,\"sensor1/#\"",1)!=99) return false;
  Serial1.println("AT+QMTUNS=0,1,\"sensor1/#\"");
  return true;
}

bool MQTT_PUB(String message) // MQTT發佈
{
  //if(Send_ATcommand("AT+QMTPUB=0,0,0,0,\"xxx/LED\",\""+message+"\"",12)!=99) return false;
  Serial1.println("AT+QMTPUB=0,0,0,0,\"xxx/LED\",\""+message+"\"");/*請修改此行程式碼*/
  return true;
}

bool LED_r_status(String E608_temp,String DHT22_temp)//LED_r判斷並亮燈，回傳LED_r狀態
{
  /*請加入程式碼*/
}
