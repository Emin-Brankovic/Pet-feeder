#include <Arduino.h>
#include <Servo.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "REPLACE_WITH_YOUR_SSID"
#define WIFI_PASSWORD "REPLACE_WITH_YOUR_PASSWORD"

// Insert Firebase project API Key
#define API_KEY "REPLACE_WITH_YOUR_FIREBASE_PROJECT_API_KEY"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "REPLACE_WITH_YOUR_FIREBASE_DATABASE_URL" 

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
String tekst;
bool signupOK = false;
Servo servo;

// varijable potrebne za logiku izvršavanja
String vrijeme1;
String vrijeme2;
String vrijeme3;

bool isHranioVrijeme1=false;
bool isHranioVrijeme2=false;
bool isHranioVrijeme3=false;
String currentTime;

int hours1,hours2,hours3=0;
int minutes1,minutes2,minutes3=0;

bool nacinRada;
int otvori;


void parseTime(String timeStr, int &hours, int &minutes) {
    int colonPos = timeStr.indexOf(':'); // Find the position of the colon
    if (colonPos != -1) {
        hours = timeStr.substring(0, colonPos).toInt();       // Part before the colon
        minutes = timeStr.substring(colonPos + 1).toInt();    // Part after the colon
    } else {
        hours = -1;   // Indicate error with invalid values
        minutes = -1;
    }
}




void setup() {
  servo.attach(D5);

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin("WIFI_SSID","WIFI_PASSWORD");
  Serial.print("connecting to wifi");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }

  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("MacAddress:");
  Serial.println(WiFi.macAddress());

    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;

      if (Firebase.signUp(&config, &auth, "", ""))
    {
      Serial.println("ok");
      signupOK = true;
    }
  else {
    Serial.println( config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

    timeClient.begin();
    timeClient.setTimeOffset(3600);

  servo.write(180);

}

void loop() 
{
  timeClient.update();
  if(Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 3000 || sendDataPrevMillis == 0))
  {
      sendDataPrevMillis = millis();
      if(Firebase.RTDB.getBool(&fbdo,"automatski/upali"))
        {
          nacinRada=fbdo.boolData();
          if(nacinRada){
            Serial.println("Automatski");
          }
          else{
            Serial.println("Manualni");
          }
        }

            if(Firebase.RTDB.getString(&fbdo,"autoNahrani/prvi")){
              vrijeme1=fbdo.stringData();
              parseTime(vrijeme1,hours1,minutes1);
            }

            if(Firebase.RTDB.getString(&fbdo,"autoNahrani/drugi")){
              vrijeme2=fbdo.stringData();
              parseTime(vrijeme2,hours2,minutes2);
            }

            if(Firebase.RTDB.getString(&fbdo,"autoNahrani/treci")){
              vrijeme3=fbdo.stringData();
              parseTime(vrijeme3,hours3,minutes3);
            }

       if(nacinRada)
      {

            currentTime=String(timeClient.getHours())+":"+String(timeClient.getMinutes());
              
            Serial.println(vrijeme1);
            Serial.println(vrijeme2);
            Serial.println(vrijeme3);

            Serial.println(currentTime);

            
            if(currentTime==vrijeme1 && isHranioVrijeme1==false)
            {
              Serial.println("Prvo otvaranje");
              servo.write(0);
              delay(1500);
              servo.write(180);
              isHranioVrijeme1=true;
              parseTime(vrijeme1,hours1,minutes1);
            }
            
            if(currentTime==vrijeme2 && isHranioVrijeme2==false)
            {
              Serial.println("Drugo otvaranje");
              servo.write(0);
              delay(1500);
              servo.write(180);
              isHranioVrijeme2=true;
              parseTime(vrijeme2,hours2,minutes2);
            }
            
            if(currentTime==vrijeme3 && isHranioVrijeme3==false)
            {
              Serial.println("Trece otvaranje");
              servo.write(0);
              delay(1500);
              servo.write(180);
              isHranioVrijeme3=true;
              parseTime(vrijeme3,hours3,minutes3);
            }

            if(timeClient.getHours() > hours1 || 
              (timeClient.getHours() == hours1 && timeClient.getMinutes() > minutes1)){
              Serial.println("Postavljeno vrijeme1");
              isHranioVrijeme1=false;
            }

             if (timeClient.getHours() > hours2 || 
              (timeClient.getHours() == hours2 && timeClient.getMinutes() > minutes2)) {
               Serial.println("Postavljeno vrijeme2");
              isHranioVrijeme2=false;
             }
            
             if (timeClient.getHours() > hours3 || 
                (timeClient.getHours() == hours3 && timeClient.getMinutes() > minutes3)){
               Serial.println("Postavljeno vrijeme3");
              isHranioVrijeme3=false;
             }
      } 

      else
        {      
          if(Firebase.RTDB.getInt(&fbdo,"nahrani/otvori"))
          {
            if(fbdo.dataType()=="int")
            {
                  otvori=fbdo.intData();
                  servo.write(otvori);
                  if(otvori==0)
                  {
                    delay(1500);
                    otvori=180;
                    if(Firebase.RTDB.setInt(&fbdo,"nahrani/otvori",otvori))
                    {
                        Serial.println("PASSED");
                    }
                    else 
                    {
                        Serial.println("FAILED");
                        Serial.println("REASON: " + fbdo.errorReason());
                    }
                  }
            }
          }
        } 

  }
        
}

