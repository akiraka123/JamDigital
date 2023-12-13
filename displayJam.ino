#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <Wire.h>
#include <RTClib.h>
#include <Bonezegei_DHT11.h>
#include "fontDigitalJam.h"
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CS_PIN    10
#define mode 9
#define select_adj  8
#define BUTTON_UP_PIN  7
#define BUTTON_DOWN_PIN  6
#define buzzer 3

MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

RTC_DS1307 rtc;
Bonezegei_DHT11 dht(5);

void setup() {
  Serial.begin(57600);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  dht.begin();
  myDisplay.begin();
  myDisplay.setFont(fontJam);
  myDisplay.setTextAlignment(PA_CENTER);
  
  // if (!rtc.isrunning()) {
  //   Serial.println("RTC is NOT running, let's set the time!");
  //   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // }
  pinMode(mode, INPUT_PULLUP);
  pinMode(select_adj, INPUT_PULLUP);
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
}

void cahaya(){
  int x = analogRead(A3);
  float y = (x -700) / 22;
  Serial.println(y);
  myDisplay.setIntensity(y);
}

// catatan
// const int mode = 5;
// const int select_adj = 6;
// const int BUTTON_UP_PIN = 7;
// const int BUTTON_DOWN_PIN = 8;
// const int buzzer = 2;
unsigned long pressbutton=0;
unsigned long prevmil=0;
uint8_t select = 0;
bool animasiStop = false;
bool animasiStop1 = false;
bool flag  = false;
bool flagButtonPressed = false;
bool alarmState1 = false; 
bool alarmState2 = false; 
bool alarmState3 = false; 
bool setalarm1 = false;
bool setalarm2 = false;
bool setalarm3 = false;
bool alarmFlag1 = false;
bool alarmFlag2 = false;
bool alarmFlag3 = false;
bool alarmFlag = false;
bool hourminute= true;

uint8_t hour1 = 0;
uint8_t minute1 = 0;
uint8_t hour2 = 0;
uint8_t minute2 = 0;
uint8_t hour3 = 0;
uint8_t minute3 = 0;
const char* monthNames[] = {"   ","jan", "feb", "mar", "apr", "may", "jun", 
                              "jul", "aug", "sep", "oct", "nov", "dec"};
const char* offon[] = {"off","on"};
uint8_t daysInMonth[] = {31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void sound(int frequency) {
  if (alarmFlag == false){
    alarmFlag = true; 
    tone(buzzer, frequency);
    prevmil = millis();  
  }
}
bool isLeapYear(int year) {
  if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
    return true;  // Leap year
  } else {
    return false; // Not a leap year
  }
}
bool isLeapYear1(int year) {
  year++;
  if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
    return true;  // Leap year
  } else {
    return false; // Not a leap year
  }
}

void set(){
  pressbutton = 0;
  flag = true;
  flagButtonPressed = false;

//SETTING Time
  while(flag == true){
    DateTime now = rtc.now();
      bool isLeap = isLeapYear(now.year());
      uint8_t month = now.month();
      uint8_t year = now.year()%100; 
      if (isLeap) {
        daysInMonth[2] = 29; // Tahun kabisat, Februari memiliki 29 hari
      } else {
        daysInMonth[2] = 28; // Bukan tahun kabisat, Februari memiliki 28 hari                
      }
      uint8_t maxDay = daysInMonth[month];
      uint8_t day = min(now.day(), maxDay);
      rtc.adjust(DateTime(year, month, day, now.hour(), now.minute(), now.second()));

    if (millis() - prevmil >= 100) {
      noTone(buzzer);
      alarmFlag = false;
    }

    char timeString[11];
    
    if      (select >= 3 && select < 6) sprintf(timeString, "%02d.%s.%02d", now.day(), monthNames[month], now.year() % 100);
    else if (select >= 0 && select < 3) sprintf(timeString, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
    else {}
    cahaya();

      if (digitalRead (mode) == LOW){ 
        pressbutton++; 
        if (pressbutton >= 250) {
            pressbutton = 0;
            select =0;
            flag = false;
            sound(1000);
          }
      }
      else 
        pressbutton = 0;

    switch (select)
    {
      case 0:
        if (millis()%1000>=500 && animasiStop ==false){
          timeString [0] = ' ';
          timeString [1] = ' ';
          myDisplay.print(timeString);
        }
        else
          myDisplay.print(timeString);

        if (digitalRead(select_adj) == LOW){
          Serial.println(digitalRead(select_adj));
          select = 1;
          sound(1000);
          delay(300);
          }
        // Tombol penambahan waktu
        if (digitalRead(BUTTON_UP_PIN) == LOW) {
          animasiStop = true;
          if (!flagButtonPressed) {
            prevmil = millis();
            rtc.adjust(now + TimeSpan(3600)); // 24 hours
            flagButtonPressed = true;
          }
          else if (millis() -prevmil >= 200){
            rtc.adjust(now + TimeSpan(3600));
            prevmil = millis();
          }
        }else if(digitalRead(BUTTON_UP_PIN) == HIGH && digitalRead(BUTTON_DOWN_PIN) == HIGH) {
          flagButtonPressed = false;
          animasiStop = false;
        }
        
       // Tombol pengurangan waktu
        if (digitalRead(BUTTON_DOWN_PIN) == LOW) {
          animasiStop = true;
          if (!flagButtonPressed) {
            prevmil = millis();
            rtc.adjust(now - TimeSpan(3600)); // Tambah satu detik     
            flagButtonPressed = true;
          }
          else if (millis() -prevmil >= 200){
            rtc.adjust(now - TimeSpan(3600));
            prevmil = millis();
          }
        }else if(digitalRead(BUTTON_UP_PIN) == HIGH && digitalRead(BUTTON_DOWN_PIN) == HIGH) {
          flagButtonPressed = false;
          animasiStop = false;
          
        }
        break;

      case 1:
        if (millis()%1000>=500 && animasiStop ==false){
        timeString [3] = ' ';
        timeString [4] = ' ';
        myDisplay.print(timeString);
        }
        else
          myDisplay.print(timeString);

        if (digitalRead(select_adj) == LOW){
          Serial.println(digitalRead(select_adj));
          select = 2;
          sound(1000);
          delay(300);
          }
        // Tombol penambahan waktu
        if (digitalRead(BUTTON_UP_PIN) == LOW) {
          animasiStop = true;
          if (!flagButtonPressed) {
            prevmil = millis();
            rtc.adjust(now + TimeSpan(60)); // 24 hours
            flagButtonPressed = true;
          }
          else if (millis() -prevmil >= 200){
            rtc.adjust(now + TimeSpan(60));
            prevmil = millis();
          }
        }else if(digitalRead(BUTTON_UP_PIN) == HIGH && digitalRead(BUTTON_DOWN_PIN) == HIGH) {
          flagButtonPressed = false;
          animasiStop = false;
        }
        
       // Tombol pengurangan waktu
        if (digitalRead(BUTTON_DOWN_PIN) == LOW) {
          animasiStop = true;
          if (!flagButtonPressed) {
            prevmil = millis();
            rtc.adjust(now - TimeSpan(60)); // Tambah satu detik     
            flagButtonPressed = true;
          }
          else if (millis() -prevmil >= 200){
            rtc.adjust(now - TimeSpan(60));
            prevmil = millis();
          }
        }else if(digitalRead(BUTTON_UP_PIN) == HIGH && digitalRead(BUTTON_DOWN_PIN) == HIGH) {
          flagButtonPressed = false;
          animasiStop = false;
        }
        break;
      case 2:
        if (millis()%1000>=500 && animasiStop ==false){
          timeString [6] = ' ';
          timeString [7] = ' ';
          myDisplay.print(timeString);
        }
        else
          myDisplay.print(timeString);

        if (digitalRead(select_adj) == LOW){
          Serial.println(digitalRead(select_adj));
          select = 3;
          sound(1000);
          delay(300);
          }
        // Tombol penambahan waktu
        if (digitalRead(BUTTON_UP_PIN) == LOW) {
          animasiStop = true;
          if (!flagButtonPressed) {
            prevmil = millis();
            rtc.adjust(now + TimeSpan(1)); // 24 hours
            flagButtonPressed = true;
          }
          else if (millis() -prevmil >= 200){
            rtc.adjust(now + TimeSpan(1));
            prevmil = millis();
          }
        }else if(digitalRead(BUTTON_UP_PIN) == HIGH && digitalRead(BUTTON_DOWN_PIN) == HIGH) {
          flagButtonPressed = false;
          animasiStop = false;
        }
        
       // Tombol pengurangan waktu
        if (digitalRead(BUTTON_DOWN_PIN) == LOW) {
          animasiStop = true;
          if (!flagButtonPressed) {
            prevmil = millis();
            rtc.adjust(now - TimeSpan(1)); // Tambah satu detik     
            flagButtonPressed = true;
          }
          else if (millis() -prevmil >= 200){
            rtc.adjust(now - TimeSpan(1));
            prevmil = millis();
          }
        }else if(digitalRead(BUTTON_UP_PIN) == HIGH && digitalRead(BUTTON_DOWN_PIN) == HIGH) {
          flagButtonPressed = false;
          animasiStop = false;
        }
        break;

      case 3:
        if (millis()%1000>=500 && animasiStop ==false){
          timeString [0] = ' ';
          timeString [1] = ' ';
          myDisplay.print(timeString);
        }
        else
          myDisplay.print(timeString);

        if (digitalRead(select_adj) == LOW){
          Serial.println(digitalRead(select_adj));
          select = 4;
          sound(1000);
          delay(300);
          }
        // Tombol penambahan waktu
        if (digitalRead(BUTTON_UP_PIN) == LOW) {
          animasiStop = true;
          if (!flagButtonPressed) {
            prevmil = millis();
            rtc.adjust(now + TimeSpan(86400)); // 24 hours
            flagButtonPressed = true;
          }
          else if (millis() -prevmil >= 200){
            rtc.adjust(now + TimeSpan(86400));
            prevmil = millis();
          }
        }else if(digitalRead(BUTTON_UP_PIN) == HIGH && digitalRead(BUTTON_DOWN_PIN) == HIGH) {
          flagButtonPressed = false;
          animasiStop = false;
        }
        
       // Tombol pengurangan waktu
        if (digitalRead(BUTTON_DOWN_PIN) == LOW) {
          animasiStop = true;
          if (!flagButtonPressed) {
            prevmil = millis();
            rtc.adjust(now - TimeSpan(86400)); // Tambah satu detik     
            flagButtonPressed = true;
          }
          else if (millis() -prevmil >= 200){
            rtc.adjust(now - TimeSpan(86400));
            prevmil = millis();
          }
        }else if(digitalRead(BUTTON_UP_PIN) == HIGH && digitalRead(BUTTON_DOWN_PIN) == HIGH) {
          flagButtonPressed = false;
          animasiStop = false;
        }
        break;
      
      case 4:
        if (millis()%1000>=500 && animasiStop ==false){
          timeString [3] = ' ';
          timeString [4] = ' ';
          timeString [5] = ' ';
          myDisplay.print(timeString);
        }
        else
          myDisplay.print(timeString);

        if (digitalRead(select_adj) == LOW){
          Serial.println(digitalRead(select_adj));
          select = 5;
          sound(1000);
          delay(300);
          }
        // Tombol penambahan waktu
        if (digitalRead(BUTTON_UP_PIN) == LOW) {
          animasiStop = true;
          if (!flagButtonPressed) {
            prevmil = millis();
            rtc.adjust(now + TimeSpan(86400*daysInMonth[month])); // 24 hours
            flagButtonPressed = true;
          }
          else if (millis() -prevmil >= 200){
            rtc.adjust(now + TimeSpan(86400*daysInMonth[month]));
            prevmil = millis();
          }
        }else if(digitalRead(BUTTON_UP_PIN) == HIGH && digitalRead(BUTTON_DOWN_PIN) == HIGH) {
          flagButtonPressed = false;
          animasiStop = false;
        }
        
       // Tombol pengurangan waktu
        if (digitalRead(BUTTON_DOWN_PIN) == LOW) {
          animasiStop = true;
          if (!flagButtonPressed) {
            prevmil = millis();
            rtc.adjust(now - TimeSpan(86400*daysInMonth[month-1])); // Tambah satu detik     
            flagButtonPressed = true;
          }
          else if (millis() -prevmil >= 200){
            rtc.adjust(now - TimeSpan(86400*daysInMonth[month-1]));
            prevmil = millis();
          }
        }else if(digitalRead(BUTTON_UP_PIN) == HIGH && digitalRead(BUTTON_DOWN_PIN) == HIGH) {
          flagButtonPressed = false;
          animasiStop = false;  
        }
        break;

      case 5:
        
        if (millis()%1000>=500 && animasiStop ==false){
          timeString [7] = ' ';
          timeString [8] = ' ';
          myDisplay.print(timeString);
        }
        else
          myDisplay.print(timeString);

        if (digitalRead(select_adj) == LOW){
          Serial.println(digitalRead(select_adj));
          select = 6;
          sound(1000);
          delay(300);
          }
        // Tombol penambahan waktu
        if (digitalRead(BUTTON_UP_PIN) == LOW) {
          animasiStop = true;
          if (!flagButtonPressed) {
            prevmil = millis();
            rtc.adjust(now + TimeSpan(86400*(isLeapYear1(now.year()) ? 366 : 365)));
            flagButtonPressed = true;
          }
          else if (millis() -prevmil >= 200){
            rtc.adjust(now + TimeSpan(86400*(isLeapYear1(now.year()) ? 366 : 365)));
            prevmil = millis();
          }
        }else if(digitalRead(BUTTON_UP_PIN) == HIGH && digitalRead(BUTTON_DOWN_PIN) == HIGH) {
          flagButtonPressed = false;
          animasiStop = false;
        }
        
       // Tombol pengurangan waktu
        if (digitalRead(BUTTON_DOWN_PIN) == LOW) {
          animasiStop = true;
          if (!flagButtonPressed) {
            prevmil = millis();
            rtc.adjust(now - TimeSpan(86400*(isLeap ? 366 : 365)));
            flagButtonPressed = true;
          }
          else if (millis() -prevmil >= 200){
            rtc.adjust(now - TimeSpan(86400*(isLeap ? 366 : 365)));
            prevmil = millis();
          }
        }else if(digitalRead(BUTTON_UP_PIN) == HIGH && digitalRead(BUTTON_DOWN_PIN) == HIGH) {
          flagButtonPressed = false;
          animasiStop = false;
        }
        break;
      
      case 6:
        if (digitalRead(select_adj) == LOW){
          Serial.println(digitalRead(select_adj));
          select = 7;
          sound(1000);
          delay(300);
          }
        if ((digitalRead(mode) == LOW && alarmState1 == true) ||setalarm1 == true){
          setalarm1 = true;
          sprintf(timeString, "Al1:%02d:%02d", hour1, minute1);
          myDisplay.print(timeString);

          if(digitalRead(mode) == LOW){
          hourminute = !hourminute;
          sound(1000);
          }
          if(hourminute){
              // Tombol penambahan waktu
            if (digitalRead(BUTTON_UP_PIN) == LOW) {
              
              if (!flagButtonPressed) {
                prevmil = millis();
                hour1++;
                if (hour1>23) hour1 = 0;
                flagButtonPressed = true;
              }
              else if (millis() -prevmil >= 200){
                hour1++;
                if (hour1>23) hour1 = 0;
                prevmil = millis();
              }
            }else if(digitalRead(BUTTON_UP_PIN) == HIGH && digitalRead(BUTTON_DOWN_PIN) == HIGH) {
              flagButtonPressed = false; 
            }
            
          // Tombol pengurangan waktu
            if (digitalRead(BUTTON_DOWN_PIN) == LOW) {
              if (!flagButtonPressed) {
                prevmil = millis();
                hour1--;
                if (hour1<0) hour1 = 23;
                flagButtonPressed = true;
              }
              else if (millis() -prevmil >= 200){
                hour1--;
                if (hour1<0) hour1 = 23;
                prevmil = millis();
              }
            }else if(digitalRead(BUTTON_UP_PIN) == HIGH && digitalRead(BUTTON_DOWN_PIN) == HIGH) {
              flagButtonPressed = false;
              
            }
          }
          else if (!hourminute){
           if (digitalRead(BUTTON_UP_PIN) == LOW) {
              
              if (!flagButtonPressed) {
                prevmil = millis();
                minute1++;
                if (minute1>59) minute1 = 0;
                flagButtonPressed = true;
              }
              else if (millis() -prevmil >= 200){
                minute1++;
                if (minute1>59) minute1 = 0;
                prevmil = millis();
              }
            }else if(digitalRead(BUTTON_UP_PIN) == HIGH && digitalRead(BUTTON_DOWN_PIN) == HIGH) {
              flagButtonPressed = false; 
            }
            
          // Tombol pengurangan waktu
            if (digitalRead(BUTTON_DOWN_PIN) == LOW) {
              if (!flagButtonPressed) {
                prevmil = millis();
                minute1--;
                if (minute1<0) minute1 = 59;
                flagButtonPressed = true;
              }
              else if (millis() -prevmil >= 200){
                minute1--;
                if (minute1<0) minute1 = 59;
                prevmil = millis();
              }
            }else if(digitalRead(BUTTON_UP_PIN) == HIGH && digitalRead(BUTTON_DOWN_PIN) == HIGH) {
              flagButtonPressed = false;
              
            }

          }

          break;
        }

        else {
          sprintf(timeString, "Alm1:%s", offon[alarmState1]);
          myDisplay.print(timeString);
          if (digitalRead(BUTTON_UP_PIN) == LOW || digitalRead(BUTTON_DOWN_PIN) == LOW ) {
            if (!flagButtonPressed) {
              prevmil = millis();
              alarmState1 = !alarmState1;
              flagButtonPressed = true;
            } 
          } 
          else if (digitalRead(BUTTON_UP_PIN) == HIGH && digitalRead(BUTTON_DOWN_PIN) == HIGH)
            flagButtonPressed = false;
          }
        break;
        
      case 7:
        if (digitalRead(select_adj) == LOW){
          Serial.println(digitalRead(select_adj));
          select = 8;
          sound(1000);
          delay(300);
          }
        if ((digitalRead(mode) == LOW && alarmState2 == true) ||setalarm2 == true){
          setalarm2 = true;
          sprintf(timeString, "Al2:%02d:%02d", hour2, minute2);
          myDisplay.print(timeString);

          if(digitalRead(mode) == LOW){
          hourminute = !hourminute;
          sound(1000);
          }
          if(hourminute){
              // Tombol penambahan waktu
            if (digitalRead(BUTTON_UP_PIN) == LOW) {
              
              if (!flagButtonPressed) {
                prevmil = millis();
                hour2++;
                if (hour2>23) hour2 = 0;
                flagButtonPressed = true;
              }
              else if (millis() -prevmil >= 200){
                hour2++;
                if (hour2>23) hour2 = 0;
                prevmil = millis();
              }
            }else if(digitalRead(BUTTON_UP_PIN) == HIGH && digitalRead(BUTTON_DOWN_PIN) == HIGH) {
              flagButtonPressed = false; 
            }
            
          // Tombol pengurangan waktu
            if (digitalRead(BUTTON_DOWN_PIN) == LOW) {
              if (!flagButtonPressed) {
                prevmil = millis();
                hour2--;
                if (hour2<0) hour2 = 23;
                flagButtonPressed = true;
              }
              else if (millis() -prevmil >= 200){
                hour2--;
                if (hour2<0) hour2 = 23;
                prevmil = millis();
              }
            }else if(digitalRead(BUTTON_UP_PIN) == HIGH && digitalRead(BUTTON_DOWN_PIN) == HIGH) {
              flagButtonPressed = false;
              
            }
          }
          else if (!hourminute){
           if (digitalRead(BUTTON_UP_PIN) == LOW) {
              
              if (!flagButtonPressed) {
                prevmil = millis();
                minute2++;
                if (minute2>59) minute2 = 0;
                flagButtonPressed = true;
              }
              else if (millis() -prevmil >= 200){
                minute2++;
                if (minute2>59) minute2 = 0;
                prevmil = millis();
              }
            }else if(digitalRead(BUTTON_UP_PIN) == HIGH && digitalRead(BUTTON_DOWN_PIN) == HIGH) {
              flagButtonPressed = false; 
            }
            
          // Tombol pengurangan waktu
            if (digitalRead(BUTTON_DOWN_PIN) == LOW) {
              if (!flagButtonPressed) {
                prevmil = millis();
                minute2--;
                if (minute2<0) minute2 = 59;
                flagButtonPressed = true;
              }
              else if (millis() -prevmil >= 200){
                minute2--;
                if (minute2<0) minute2 = 59;
                prevmil = millis();
              }
            }else if(digitalRead(BUTTON_UP_PIN) == HIGH && digitalRead(BUTTON_DOWN_PIN) == HIGH) {
              flagButtonPressed = false;
              
            }

          }

          break;
        }

        else {
          sprintf(timeString, "Alm2:%s", offon[alarmState2]);
          myDisplay.print(timeString);
          if (digitalRead(BUTTON_UP_PIN) == LOW || digitalRead(BUTTON_DOWN_PIN) == LOW ) {
            if (!flagButtonPressed) {
              prevmil = millis();
              alarmState2 = !alarmState2;
              flagButtonPressed = true;
            } 
          } 
          else if (digitalRead(BUTTON_UP_PIN) == HIGH && digitalRead(BUTTON_DOWN_PIN) == HIGH)
            flagButtonPressed = false;
          }
        break;
      case 8:
        if (digitalRead(select_adj) == LOW){
          Serial.println(digitalRead(select_adj));
          select = 0;
          sound(1000);
          delay(300);
          }
        if ((digitalRead(mode) == LOW && alarmState3 == true) ||setalarm3 == true){
          setalarm3 = true;
          sprintf(timeString, "Al3:%02d:%02d", hour3, minute3);
          myDisplay.print(timeString);

          if(digitalRead(mode) == LOW){
          hourminute = !hourminute;
          sound(1000);
          }
          if(hourminute){
              // Tombol penambahan waktu
            if (digitalRead(BUTTON_UP_PIN) == LOW) {
              
              if (!flagButtonPressed) {
                prevmil = millis();
                hour3++;
                if (hour3>23) hour3 = 0;
                flagButtonPressed = true;
              }
              else if (millis() -prevmil >= 200){
                hour3++;
                if (hour3>23) hour3 = 0;
                prevmil = millis();
              }
            }else if(digitalRead(BUTTON_UP_PIN) == HIGH && digitalRead(BUTTON_DOWN_PIN) == HIGH) {
              flagButtonPressed = false; 
            }
            
          // Tombol pengurangan waktu
            if (digitalRead(BUTTON_DOWN_PIN) == LOW) {
              if (!flagButtonPressed) {
                prevmil = millis();
                hour3--;
                if (hour3<0) hour3 = 23;
                flagButtonPressed = true;
              }
              else if (millis() -prevmil >= 200){
                hour3--;
                if (hour3<0) hour3 = 23;
                prevmil = millis();
              }
            }else if(digitalRead(BUTTON_UP_PIN) == HIGH && digitalRead(BUTTON_DOWN_PIN) == HIGH) {
              flagButtonPressed = false;
              
            }
          }
          else if (!hourminute){
           if (digitalRead(BUTTON_UP_PIN) == LOW) {
              
              if (!flagButtonPressed) {
                prevmil = millis();
                minute3++;
                if (minute3>59) minute3 = 0;
                flagButtonPressed = true;
              }
              else if (millis() -prevmil >= 200){
                minute3++;
                if (minute3>59) minute3 = 0;
                prevmil = millis();
              }
            }else if(digitalRead(BUTTON_UP_PIN) == HIGH && digitalRead(BUTTON_DOWN_PIN) == HIGH) {
              flagButtonPressed = false; 
            }
            
          // Tombol pengurangan waktu
            if (digitalRead(BUTTON_DOWN_PIN) == LOW) {
              if (!flagButtonPressed) {
                prevmil = millis();
                minute3--;
                if (minute3<0) minute3 = 59;
                flagButtonPressed = true;
              }
              else if (millis() -prevmil >= 200){
                minute3--;
                if (minute3<0) minute3 = 59;
                prevmil = millis();
              }
            }else if(digitalRead(BUTTON_UP_PIN) == HIGH && digitalRead(BUTTON_DOWN_PIN) == HIGH) {
              flagButtonPressed = false;
              
            }

          }

          break;
        }

        else {
          sprintf(timeString, "Alm3:%s", offon[alarmState3]);
          myDisplay.print(timeString);
          if (digitalRead(BUTTON_UP_PIN) == LOW || digitalRead(BUTTON_DOWN_PIN) == LOW ) {
            if (!flagButtonPressed) {
              prevmil = millis();
              alarmState3 = !alarmState3;
              flagButtonPressed = true;
            } 
          } 
          else if (digitalRead(BUTTON_UP_PIN) == HIGH && digitalRead(BUTTON_DOWN_PIN) == HIGH)
            flagButtonPressed = false;
          }
        break;

      }
    
    }

  }

void MODE(){
  if (digitalRead (mode) == LOW){ 
    pressbutton++; 
    if (pressbutton >= 250) {
      setalarm1 = false;
      setalarm2 = false;
      setalarm3 = false;
      pressbutton = 0;
      sound(1000);
      set();
    }
  }  
  else 
    pressbutton = 0;
}

void loop() {
  DateTime now = rtc.now();
  char timeString[11];
  
  if (millis() - prevmil >= 100) {
      noTone(buzzer);
      alarmFlag = false;
    }
  MODE();

  if (digitalRead(BUTTON_UP_PIN) == LOW && digitalRead(BUTTON_DOWN_PIN) == LOW){
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  sound(1000);
  }
//  Serial.println(digitalRead(mode));
//  Serial.println(digitalRead(select_adj));
//  Serial.println(digitalRead(BUTTON_UP_PIN));
//  Serial.println(digitalRead(BUTTON_DOWN_PIN));
  if (millis()%150 >= 100) 
    cahaya();
  if(alarmFlag1==false&&alarmFlag2==false&&alarmFlag3==false)
  myDisplay.print(timeString);
  if (alarmFlag1==true) {
    sound(1000);sound(1000);sound(1000);
    if (myDisplay.displayAnimate()) {
      myDisplay.displayText("5024211051", PA_CENTER, 100, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
      }
    if (millis() - prevmil >= 60000||digitalRead(mode)==LOW) alarmFlag1 = false;
  }

  else if (now.hour() == hour1 && now.minute() == minute1 && now.second() == 0&& alarmState1 == true) {
    prevmil = millis();
    alarmFlag1 = true;
    sprintf(timeString, "" );
    myDisplay.print(timeString);
  }
  else if (alarmFlag2) {
    sound(1000);sound(1000);sound(1000);
    if (myDisplay.displayAnimate()) {
      myDisplay.displayText("5024211051 MOCHAMAD AKIRAKA HARIFANDA", PA_CENTER, 100, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
      }
    if (millis() - prevmil >= 60000||digitalRead(mode)==LOW) alarmFlag2 = false;
  }
  else if (now.hour() == hour2 && now.minute() == minute2 && now.second() == 0 && alarmState2 == true) {
    prevmil = millis();
    alarmFlag2 = true;
    sprintf(timeString, "" );
    myDisplay.print(timeString);
  }
  else if (alarmFlag3) {
    sound(1000);sound(1000);sound(1000);
    if (myDisplay.displayAnimate()) {
      myDisplay.displayText("Customize", PA_CENTER, 100, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
      }
    if (millis() - prevmil >= 60000||digitalRead(mode)==LOW) alarmFlag3 = false;
  }
  else if (now.hour() == hour3 && now.minute() == minute3 && now.second() == 0&& alarmState3 == true) {
    prevmil = millis();
    alarmFlag3 = true;
    sprintf(timeString, "" );
    myDisplay.print(timeString);
  }
  else if ((millis()%30000) <= 10000)
    sprintf(timeString, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  else if ((millis()%30000) <= 13000)
    sprintf(timeString, "%02d.%s.%02d", now.day(), monthNames[now.month()], now.year() % 100);
  else if ((millis()%30000) <= 16000){
      if (dht.getData()) {                         // get All data from DHT11
      float tempDeg = dht.getTemperature();      // return temperature in celsius
      if (tempDeg != 0) {
        sprintf(timeString, "suhu:%d°C", (int)tempDeg);
      }
    }
  }
  else if ((millis()%30000) <= 30000)
    sprintf(timeString, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());

}
