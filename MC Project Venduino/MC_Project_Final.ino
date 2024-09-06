#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

//Create software serial object to communicate with SIM900
SoftwareSerial mySerial(7, 8); //SIM900 Tx & Rx is connected to Arduino #7 & #8

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x3F for a 16 chars and 2 line display

Servo myservo1,myservo2; 
#define BUTTON1 3
#define BUTTON2 4
#define BUTTON3 2
#define BUTTON4 0

#define SERVO1_PWM 5
#define SERVO2_PWM 6

#define RST_PIN 9
#define SS_PIN 10

byte readCard[4];
String ValidCards[10]={"1A45E1B"};
uint32_t tagInt;

//String[] ValidCards = new String*[10];
int vcards=1;
//String MasterTag = "B39559AD";	// REPLACE this Tag ID with your Tag ID!!!
String tagID = "0 0 0 0";

// Create instances
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key; 
//LiquidCrystal lcd(7, 6, 5, 4, 3, 2); //Parameters: (rs, enable, d4, d5, d6, d7) 

byte bufferATQA[18];
byte bufferSize = sizeof(bufferATQA);

int angle = 90;

int reg_mode=0;

void user_logged_in(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("User ID : "+tagID);
  lcd.setCursor(0, 1);
  lcd.print("Product 1  Product 2");
  lcd.setCursor(0, 2);
  lcd.print("  20Tk       30Tk");
  lcd.setCursor(0, 3);
  lcd.print("Press a button...");  
  delay(2000);      
}

void user_set_initial(){
  lcd.clear();
  lcd.print("    **VENDUINO**   ");
  lcd.setCursor(0, 1);
  lcd.print("   Scan Your Card  ");        
}

void user_register(){
  lcd.clear();
  lcd.print(" Scan And Register "); 
  reg_mode=1;  
}

void setup() {
  Serial.begin(9600);
  myservo1.attach(SERVO1_PWM);
  pinMode(BUTTON1,INPUT_PULLUP);
  // Servo is stationary.
  myservo1.write(90);

  myservo2.attach(SERVO2_PWM);
  pinMode(BUTTON2,INPUT_PULLUP);
  // Servo is stationary.
  myservo2.write(90);
  Serial.println("Controling Servo with signal");

   SPI.begin(); // SPI bus
  mfrc522.PCD_Init(); // MFRC522
  lcd.init();
  lcd.clear();         
  lcd.backlight();      // Make sure backlight is on

  user_set_initial();

  Serial.println("Scan Your Card : ");  
  // lcd.begin(16, 2); // LCD screen

  // Prepare the security key for the read and write functions.
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;  //keyByte is defined in the "MIFARE_Key" 'struct' definition in the .h file of the library
  }

  //Begin serial communication with Arduino and SIM900
  mySerial.begin(9600);

  Serial.println("Initializing..."); 
  delay(1000);

  mySerial.println("AT"); //Handshaking with SIM900
  updateSerial();
  
  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CNMI=2,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
  updateSerial();
}



void loop() {
  byte readbackblock[18];

  while(getID(2, readbackblock)) 
  {
    bool f=false;
    Serial.println(vcards);  
      
    for(int i=0;i<10;i++){
      Serial.println(ValidCards[i]);      
      if(tagID==ValidCards[i]) 
        f=true;
    }

    if(reg_mode==1){
      if(f==true){
        lcd.setCursor(0, 1);
        lcd.print("     ID :"+tagID);
        lcd.setCursor(0, 2);
        lcd.print("Already Registered!");         
        delay(2000);
        user_set_initial();
        reg_mode=0;
        break;
      }
      
      lcd.setCursor(0, 1);
      lcd.print("     ID :"+tagID);
      lcd.setCursor(0, 2);
      lcd.print("Pay 20Tk... ");         
      delay(2000);
                            
      String str, msg;
      int amnt=0;
      bool end=false;
      while(true){ 
        while(str.length()==0){
          if(digitalRead(BUTTON3) == LOW){
            user_set_initial();
            end=true;
            break;
          }
          str=updateSerial();
        }
        
        if(end) break; 

        Serial.println(str);
        amnt+=str.toInt();

        if(amnt<20){
          lcd.setCursor(0, 3);
          lcd.print("Insufficient Amount");   
          delay(2000);
          lcd.clear();
          user_register(); 
          lcd.setCursor(0, 1);
          lcd.print("     ID :"+tagID);
          lcd.setCursor(0, 2);
          delay(2000);
          lcd.setCursor(0, 3);              
          lcd.print("Pay "+String(20-amnt)+" more...");   
          delay(2000);
          str="";         
        }
        else {
          Serial.println(tagID);
          Serial.println(tagInt);
          ValidCards[vcards++]=tagID;
          lcd.setCursor(0, 3);
          lcd.print("Registered!"); 
          delay(2000); 
        
          user_set_initial();
          break;            
        }          
      }
      reg_mode=0;
    }
    else if (f) 
    {
      Serial.println("Access Granted for "+ tagID);

      lcd.setCursor(0, 2);
      lcd.print("  Access Granted!  ");

      lcd.setCursor(0, 3);
      lcd.print("     ID :"+tagID);

      delay(2000);

      user_logged_in();

      while(true) {
        if(digitalRead(BUTTON1) == LOW){
          lcd.setCursor(0, 3);
          lcd.print("Button 1 Pressed");
          delay(2000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("User ID : "+tagID);
          lcd.setCursor(0, 1);
          lcd.print("Product 1 selected");  
          lcd.setCursor(0, 2);
          lcd.print("Pay 20Tk...");

                                     
          String str, msg;
          int amnt=0;
          bool end=false;
          while(true){         
            while(str.length()==0){
              if(digitalRead(BUTTON3) == LOW){
                user_logged_in();
                end=true;
                break;
              }
              str=updateSerial();
            }
            
            if(end) break; 

            Serial.println(str);

            amnt+=str.toInt();

            if(amnt<20){
              lcd.setCursor(0, 3);
              lcd.print("Insufficient Amount");   
              delay(2000); 
              lcd.setCursor(0, 3);
              lcd.print("");
              delay(2000);
              lcd.setCursor(0, 3);              
              lcd.print("Pay "+String(20-amnt)+" more...");   
              delay(2000);
              str="";     
            }
            else {
              lcd.setCursor(0, 3);
              lcd.print("Payment Successful"); 
              delay(2000); 
              Serial.println("Got signal from button1");
              myservo1.write(0);
              delay(2500);
              myservo1.write(90);
              delay(1000);
              Serial.println("Whats Happening1");

              user_logged_in();
              break;            
            }          
          }       
        }
        if(digitalRead(BUTTON2) == LOW){
          lcd.setCursor(0, 3);
          lcd.print("Button 2 Pressed");
          delay(2000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("User ID : "+tagID);
          lcd.setCursor(0, 1);          
          lcd.print("Product 2 selected");  
          lcd.setCursor(0, 2);
          lcd.print("Pay 30Tk...");

          int amnt=0;                       
          String str, msg;
          bool end=false;
          while(true){         
            while(str.length()==0){
              if(digitalRead(BUTTON3) == LOW){
                user_logged_in();
                end=true;
                break;
              }
              str=updateSerial();
            }
            
            if(end) break; 

            Serial.print(str);

            amnt+=str.toInt();

            if(amnt<30){
              lcd.setCursor(0, 3);
              lcd.print("Insufficient Amount");   
              delay(2000); 
              lcd.setCursor(0, 3);
              lcd.print("");
              delay(2000);
              lcd.setCursor(0, 3);              
              lcd.print("Pay "+String(30-amnt)+" more...");   
              delay(2000);
              str="";        
            }
            else {
              lcd.setCursor(0, 3);
              lcd.print("Payment Successful"); 
              delay(2000); 
              Serial.println("Got signal from button1");
              myservo2.write(0);
              delay(2500);
              myservo2.write(90);
              delay(1000);
              Serial.println("Whats Happening1");

              user_logged_in();
              break;            
            }          
          }        
        }  
        if(digitalRead(BUTTON3) == LOW){
          user_set_initial();
          break;
          reg_mode=0;
        }      
      }     
    }
    else
    {
      Serial.println("Access Denied For "+ tagID);

      lcd.setCursor(0, 2);
      lcd.print("  Access Denied  ");

      lcd.setCursor(0, 3);
      lcd.print("     ID :"+tagID);

      delay(2000);

      //lcd.print(" Access Denied!");
    }

    if(digitalRead(BUTTON3) == LOW){
      user_set_initial();
      break;
    } 
     
    delay(1000);
  }

  if(digitalRead(BUTTON4) == LOW){
    user_register();
  }

  if(digitalRead(BUTTON3) == LOW){
    user_set_initial();
  }
}


String updateSerial()
{
  delay(500);
  String text;
  char ch;
  int mode=0;
  while(mySerial.available()) 
  {
    ch=mySerial.read();
    Serial.write(ch);//Forward what Software Serial received to Serial Port
    //text+=ch;
    if(ch=='Y')mode=1;
    if(mode==1&&ch>='0'&&ch<='9') text+=ch;
    if(ch=='.')mode=0;    
    delay(10);
  }

  return text;
}

//Read new tag if available
boolean getID(int blockNumber, byte arrayAddress[]) 
{
  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
     //If a new PICC placed to RFID reader continue
      //Serial.println("in error 1");      return false;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) { 
    //Serial.println("in error 2");//Since a PICC placed get Serial and continue
  return false;
  }
  tagID = "";
  for ( uint8_t i = 0; i < 4; i++) { // The MIFARE PICCs that we use have 4 byte UID
  //readCard[i] = mfrc522.uid.uidByte[i];
  tagID.concat(String(mfrc522.uid.uidByte[i], HEX)); // Adds the 4 bytes in a single String variable
  }
  tagID.toUpperCase();

  tagInt = (mfrc522.uid.uidByte[0]) | (mfrc522.uid.uidByte[1] << 8) |( mfrc522.uid.uidByte[2]<< 16) | (mfrc522.uid.uidByte[3] << 24);


  mfrc522.PICC_HaltA(); // Stop reading
  Serial.println("Card Check Complete.");
  return true;
}
