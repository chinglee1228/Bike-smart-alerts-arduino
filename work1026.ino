#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>
MFRC522 mfrc522;   // 建立MFRC522實體
SoftwareSerial BT(8,9); //RX | TX
#define RST_PIN         6          
#define SS_PIN          10 //RC522卡上的SDA
//byte uid[]={0x0C, 0xEF, 0xF2, 0x37};  //這是我們指定的卡片UID，可由讀取UID的程式取得特定卡片的UID，再修改這行
String read_rfid;                   // Add how many you need and don't forget to include the UID.
String ok_rfid_1="ceff237";        // 卡片ID
const int Red = 2; //RGB LED針腳分配 //共陰極 長針腳接地
const int Green = 3;
const int Blue = 4;
int vibr_pin = 5; //震動感應器  
int buzzer = 7; // 蜂鳴器輸出接腳
int LED_Pin = 13; //板上LED
int shock ;//震動感應變數
bool bell = true; //警鈴開關
char blue;//藍芽接收資料變數 程式資料
char bluesee;//藍芽接收資料變數 監控視窗
bool lock = true; //宣告預設為鎖住


    
void setup()
{  
  Serial.begin(9600);  //與電腦序列埠連線
  Serial.println("BT is ready!");
  BT.begin(9600); // 設定藍牙模組的連線速率HC-05 9600
  int shock = digitalRead(vibr_pin);//從D3讀取感測值
  SPI.begin();// 初始化SPI介面
  mfrc522.PCD_Init(SS_PIN, RST_PIN); // 初始化MFRC522卡
  Serial.print(F("\nReader : "));
  mfrc522.PCD_DumpVersionToSerial(); // 顯示讀卡設備的版本
  pinMode(buzzer,OUTPUT); //蜂鳴器輸出
  pinMode(Red, OUTPUT); //紅LED
  pinMode(Green, OUTPUT);
  pinMode(Blue, OUTPUT);
  pinMode(vibr_pin, INPUT);//震動開關
  pinMode(LED_Pin, OUTPUT);
  start();
}
void start() //起始判斷
{ 
  if (lock == true) {//鎖定狀態
    Serial.print(F("\nLOCK!!"));
    lockcode();}
  else{
    Serial.print(F("\nUNLOCK!!"));
    unlockcode();
    }}



    
void dump_byte_array(byte *buffer, byte bufferSize) {
    read_rfid="";
    for (byte i = 0; i < bufferSize; i++) {
        read_rfid=read_rfid + String(buffer[i], HEX);
    }
}
void lockcode()//鎖住程式碼
{
  analogWrite(Red,255);
  analogWrite(Green,LOW);
  delay(100);
  shockcode();
}

    
void unlockcode()//解鎖時程式碼
{
  analogWrite(Red,LOW);//RGB
  analogWrite(Green,255);//RGB
  delay(100);
  }
  
void  shockcode(){  
  blue = '震動警告!!';
  //Serial.write(blue);
  if(shock ==1){
      //震動開關
      analogWrite(Red,255);//RGB
      delay(100);
      analogWrite(Red,LOW);//RGB
      delay(100);
      Serial.print(F("\n震動!!"));
      /*
      digitalWrite(buzzer,HIGH);//bb聲
      delay(100);
      digitalWrite(buzzer,LOW);//bb聲
      delay(100);
      digitalWrite(buzzer,HIGH);//bb聲
      delay(100);
      digitalWrite(buzzer,LOW);//bb聲
      delay(100);
      */
      //Serial.print(F("\nWarring,someone move your bike!!"));
      //Serial.print(F("\nLOCK!!"));       
      
     }
     else {
      
    // 關閉蜂鳴器及LED
   
    digitalWrite(buzzer,LOW);
   }
  }

  
void loop() {
 // 若收到「序列埠監控視窗」的資料，則送到藍牙模組 //藍芽到手機
 if(Serial.available()) 
    { blue = Serial.read();
      BT.write(byte(blue)); 
      Serial.print(blue);
      delay(300);
  }
  
  // 若收到藍牙模組的資料，則送到「序列埠監控視窗」//手機到藍芽
  if (BT.available()) {
    char i = BT.read();
    Serial.println(i);
   if(i == 1){
      Serial.println("1");
      Serial.println("閃紅燈");
      analogWrite(Red,LOW);
      analogWrite(Green,LOW);
      analogWrite(Red,255);//RGB
      delay(100);
      analogWrite(Red,LOW);//RGB
      delay(100);
      analogWrite(Red,255);//RGB
      delay(100);
      analogWrite(Red,LOW);//RGB
      delay(100);
    }
    if(i == 2){
      Serial.println("2");
      Serial.println("閃綠燈");
      analogWrite(Red,LOW);
      analogWrite(Blue,LOW);
      analogWrite(Green,255);//RGB
      delay(100);
      analogWrite(Green,LOW);//RGB
      delay(100);
      analogWrite(Green,255);//RGB
      delay(100);
      analogWrite(Green,LOW);//RGB
      delay(100);
    }
    if(i == 3){
     Serial.println("3");
     Serial.println("閃藍燈");
     analogWrite(Red,LOW);
     analogWrite(Green,LOW);
     analogWrite(Blue,255);
     delay(100);
     analogWrite(Blue,LOW);
     delay(100);
     analogWrite(Blue,255);
     delay(100);
     analogWrite(Blue,LOW);
     delay(100);
    }
  }

  
 shock = digitalRead(vibr_pin);//從D3讀取感測值
 
 if (lock){lockcode();}else{unlockcode();} //判斷狀態選擇副程式
   
 if (read_rfid==ok_rfid_1) {//RFID 成功 解鎖
    if ( ! mfrc522.PICC_IsNewCardPresent())
       return;
    if ( ! mfrc522.PICC_ReadCardSerial())
       return;
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    lock =! lock;
    //digitalWrite(buzzer,HIGH); //感應到卡的BB聲
     delay(100);
     digitalWrite(buzzer,LOW);
     delay(1000);
     start();          
        }
   else if (read_rfid != ok_rfid_1){
        if ( ! mfrc522.PICC_IsNewCardPresent())
          return;
        if ( ! mfrc522.PICC_ReadCardSerial())
          return;
   dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
         //error card
         Serial.print(F("\nERROR CARD!!"));
         lock = true;
         start();
         delay(100);
         /*
         digitalWrite(buzzer,HIGH);//感應到卡的BB聲
         delay(100);
         digitalWrite(buzzer,LOW);
         delay(100);
         digitalWrite(7,1);//感應到卡的BB聲
         delay(100);
         digitalWrite(7,0);
         delay(100);
         digitalWrite(7,1);
         delay(100);
         digitalWrite(7,0);
         */
         
           }  



           

}
