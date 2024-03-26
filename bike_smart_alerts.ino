#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>
MFRC522 mfrc522;   // 建立MFRC522實體
SoftwareSerial BT(8,9); //RX | TX
#define RST_PIN         6
#define SS_PIN          10 //RC522卡上的SDA
char *reference;
byte uid[]={0x0C, 0xEF, 0xF2, 0x37};  //這是我們指定的卡片UID，可由讀取UID的程式取得特定卡片的UID，再修改這行
String read_rfid;                   // Add how many you need and don't forget to include the UID.
String ok_rfid_1="ceff237";        // 卡片ID
const int Red = 2; //RGB LED針腳分配 //共陰極 長針腳接地
const int Green = 3;
const int Blue = 4;
int vibr_pin = 5; //震動感應器
int buzzer = 7; // 蜂鳴器輸出接腳
//程式變數
int shock = 0 ;//震動感應變數
bool alarm = false; //警報狀態
bool bell = true; //警鈴開關
bool lock =true; //宣告預設為鎖住
//藍芽
byte cmmd[20];//藍芽接收資料變數
int insize;
int bluedata;//藍芽接收資料變數 存
byte Data[3];

void setup()
{
  SPI.begin();// 初始化SPI介面
  mfrc522.PCD_Init(SS_PIN, RST_PIN); // 初始化MFRC522卡
  Serial.begin(9600);  //與電腦序列埠連線
  BT.begin(9600); // 設定藍牙模組的連線速率HC-05 9600
  int shock = digitalRead(vibr_pin);//從D3讀取感測值
  pinMode(buzzer,OUTPUT); //蜂鳴器輸出
  pinMode(Red, OUTPUT); //紅LED
  pinMode(Green, OUTPUT);
  pinMode(Blue, OUTPUT);
  pinMode(vibr_pin, INPUT);//震動開關
  Serial.println("BT is ready!");
}
void start() //起始判斷
  {
  if (lock == true) {//鎖定狀態
    analogWrite(Red,255);
    analogWrite(Green,LOW);
    shockcode();
    }
  else{
    shock = 0;
    alarm = false;
    analogWrite(Red,LOW);//RGB
    analogWrite(Green,255);
    }
   }//start end
void buzzercode()//警報聲
  {
    digitalWrite(buzzer,HIGH);//bb聲
    delay(100);
    digitalWrite(buzzer,LOW);//bb聲
    delay(10);
  }
void  shockcode(){
  if(shock ==1){
    if (alarm ==false)
    {Data[0]='1';//輸出1警告通知
      BT.write(Data[0]);
      alarm = true;
      Serial.print("震動!!輸出資料:");
      Serial.println(Data[0]);
      //Serial.println("");
       }
     }
  }
void loop() {
  start();
  shock = digitalRead(vibr_pin);//從D3讀取感測值
  if (alarm){
     analogWrite(Red,255);//RGB
     delay(100);
     analogWrite(Red,LOW);//RGB
     delay(10);
    if (bell){buzzercode();}
   }
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    bool they_match = true; // 初始值是假設為真
     for ( int i = 0; i < 4; i++ ) { // 卡片UID為4段，分別做比對
        if ( uid[i] != mfrc522.uid.uidByte[i] ) {
          they_match = false;
          break;
        }
      }
      mfrc522.PICC_HaltA();
      if (they_match) {//RFID 成功 解鎖
        lock =! lock;
        buzzercode();//蜂鳴器叫
        start();
        if(lock){  Data[0]='2';//2=鎖定
             BT.write(Data[0]);
             }
        else{
             Data[0]='3';//3=鎖定
             BT.write(Data[0]);
             }
        Serial.print("鎖定狀態:");//RGB
        Serial.println(lock);
        mfrc522.PICC_HaltA();
          }//成功END
       else {//error card
         Serial.print(F("\nERROR CARD!!"));
         for ( int i = 0; i < 3; i++ ) {buzzercode();}//蜂鳴器叫
         start();
           }//error END
   }//RFID感應結束

// 若收到藍牙模組的資料，則送到「序列埠監控視窗」//手機到藍芽 電腦顯示
  if ((insize=(BT.available()))>0)
    {  //判斷有沒有訊息接收
    for (int i=0; i<insize; i++) {
      cmmd[i]=char(BT.read());//將接收的訊息顯示出來
      bluedata = cmmd[i];
      Serial.print("輸入資料:");
      Serial.print(bluedata);
    }

  if(bluedata == 49){
      Serial.println("49 鎖定");
    lock = true;
    }
  if(bluedata == 50){
      Serial.println("50 解除");
      lock = false;
      alarm = false;
      shock = 0;
      }
  if(bluedata == 51){
     Serial.println("51警鈴開");
     bell = true;
     buzzercode();
    }
  if(bluedata == 52){
     Serial.println("52警鈴關");
     bell = false;
    }
  if(bluedata == 53){
     Serial.println("53警報關");
     alarm = false;
     shock = 0;
    }
  }//for

}
