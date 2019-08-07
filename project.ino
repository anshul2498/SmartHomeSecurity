#ifndef __CC3200R1M1RGC__
#endif
#include <WiFi.h>
#include <WiFiClient.h>
#include <Temboo.h>
#include "temboo_header.h" // Contains Temboo account information
#include "LiquidCrystal.h"

int keyIndex = 0;

WiFiServer server(80); 

boolean checkDistance();
WiFiClient client;
float TempC = 24.0;           //Current Temperature will affect speed of sound.
float speedOfSound;
#define trig 23             
#define echo 24  
#define buzzer 8
long duration, distance;   //Duration will used to calculate the distance
boolean activated = false; 

int calls = 1;   // Execution count, so this doesn't run forever
int maxCalls = 11;   // Maximum number of times the Choreo should be executed
int buttonState = 0;
LiquidCrystal lcd(12,11,5,4,3,2);


void setup() {
    Serial.begin(9600);
   pinMode(trig, OUTPUT);
   pinMode(echo, INPUT);
   pinMode(buzzer, OUTPUT);
   pinMode(PUSH2, INPUT_PULLUP);
   lcd.begin(16,2);
   
  int wifiStatus = WL_IDLE_STATUS;
  Serial.print("\n\nShield:");
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("FAIL");
    while(true);
  }
  Serial.println("OK");
  while(wifiStatus != WL_CONNECTED) {
    Serial.print("WiFi:");
    wifiStatus = WiFi.begin(WIFI_SSID,WIFI_PASSWORD);

    if (wifiStatus == WL_CONNECTED) {9*
      Serial.println("OK");
    } else {
      Serial.println("FAIL");
    }
    delay(5000);
  }

  Serial.println("Setup complete.\n");
   IPAddress ip = WiFi.localIP();
}

void loop() {
  buttonState = digitalRead(PUSH2);
  if(buttonState == HIGH)
  {
  if (calls <= maxCalls)
  {
    Serial.println("Running MakeCall - Run #" + String(calls++));
    String choice = "";

    TembooChoreo MakeCallChoreo(client);

    // Invoke the Temboo client
    MakeCallChoreo.begin();

    // Set Temboo account credentials
    MakeCallChoreo.setAccountName(TEMBOO_ACCOUNT);
    MakeCallChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    MakeCallChoreo.setAppKey(TEMBOO_APP_KEY);

    // Set Choreo inputs
   String AuthTokenValue = "b440f544e70e2aaffb45f690ff5caeb2";
    MakeCallChoreo.addInput("AuthToken", AuthTokenValue);
    String ToValue = "+919736893596";
    MakeCallChoreo.addInput("To", ToValue);
    String FromValue = "+16413246427";
    MakeCallChoreo.addInput("From", FromValue);
   String AnswerURLValue = "https://handler.twilio.com/twiml/EH1b0c31ee2367d0b88c5c0eead2d114a3";
   MakeCallChoreo.addInput("AnswerURL",AnswerURLValue);
   String GoodbyeURLValue = "https://handler.twilio.com/twiml/EHe0529e34868dba21506db0ce2393fa3c"; 
    MakeCallChoreo.addInput("GoodbyeURL",GoodbyeURLValue);
    String AccountSIDValue = "AC5884ce63c47e4c8ae693f0f97a58ca6e";

   MakeCallChoreo.addInput("AccountSID", AccountSIDValue);

    // Identify the Choreo to run from the temboo Library
    MakeCallChoreo.setChoreo("/Library/Twilio/Calls/CaptureKeyPadEntry"); 
// use an output filter to filter the required output
MakeCallChoreo.addOutputFilter("choice", "", "Digits");

  // run Choreo
 MakeCallChoreo.run(901,USE_SSL);
 //delay(15000);
  // parse the results
  
  while(MakeCallChoreo.available()) {
    // read the name of the next output item
    String name = MakeCallChoreo.readStringUntil('\x1F');
    name.trim(); // use “trim” to get rid of newlines
   Serial.println(name);
   // Serial.println("hi");
    // read the value of the next output item
    String data = MakeCallChoreo.readStringUntil('\x1E');
    data.trim(); // use “trim” to get rid of newlines
  Serial.println(data);
    // parse the user's choice out of the response data
    if (name == "choice") 
     {  Serial.println(" fetch");
      choice = data;
    } 
  }
  // close the Choreo 
 MakeCallChoreo.close();

  // return the choice that the user made 
  if (choice=="1")
  {
      Serial.println("The user pressed 1.\n");
      lcd.print("Please come later!");
      lcd.display();
  }   

     
    //else {
      //Serial.println("The user did not press 1.");
      //  }

    
}
}

 activated = checkDistance();    //If distance is less than 30cm, the "activated" variable will be TRUE.
  if(activated){
  
    digitalWrite(buzzer, HIGH); 
     int i = 0;
  WiFiClient client = server.available();   // listen for incoming clients

  if (client)  {                             // if you get a client,
                  Serial.println("new client"); 
                  boolean currentLineIsBlank =true;
                 // print a message out the serial port
                  char buffer[500] = {0};                 // make a buffer to hold incoming data
                  while (client.connected())
                    {
                      // loop while the client's connected
                       if (client.available())
                           {     // if there's bytes to read from the client,
                              char c = client.read();             // read a byte, then
                              Serial.write(c);                    // print it out the serial monitor
                                if (c == '\n' && currentLineIsBlank)
                                  {   
                                      client.println("HTTP/1.1 200 OK");
                                      client.println("Content-type:text/html");
                                      client.println();

                                      // the content of the HTTP response follows the header:
                                      client.println("<html><head><title> DANGER </title></head><body align=center>");
                                      client.println("<h1 align=center><font color=\"red\"> Choose what you want to do </font></h1>");
                                      
                                      client.print(" button1 <button onclick=\"location.href='/H'\">HIGH</button>");
                                      
                                      client.println(" <button onclick=\"location.href='/L'\">LOW</button><br>");
                            
                                         
                                                                               // The HTTP response ends with another blank line:
                                      client.println();
                                      // break out of the while loop:
                                      break;
                                 }   //3rd if
          
          else if (c =='\n')
           {
              currentLineIsBlank =true;  //// you're starting a new line
              //memset(buffer, 0, 150);
              //i = 0;
           }
         
    
        else if (c != '\r') //// you've gotten a character on the current line
        {    
          buffer[i++] = c;      // add it to the end of the currentLine
         currentLineIsBlank =false;
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (endsWith(buffer, "GET /H"))
        {
          digitalWrite(buzzer, HIGH);               // GET /H turns the LED on
        }
        if (endsWith(buffer, "GET /L"))
        {
          digitalWrite(buzzer, LOW);                // GET /L turns the LED off
        }
  
        
      } //if for checking the client response
    
      }  // connected
    client.stop();
    Serial.println("client disonnected");
  } // client
       
    Serial.println("Running SendSMS - Run #" + String(calls++));

     
     
     
    TembooChoreo SendSMSChoreo(client);

    // Invoke the Temboo client
    SendSMSChoreo.begin();

    // Set Temboo account credentials
    SendSMSChoreo.setAccountName(TEMBOO_ACCOUNT);
    SendSMSChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    SendSMSChoreo.setAppKey(TEMBOO_APP_KEY);

    // Set Choreo inputs
    String AuthTokenValue = "b440f544e70e2aaffb45f690ff5caeb2";
    SendSMSChoreo.addInput("AuthToken", AuthTokenValue);
    String ToValue = "+919736893596";
    SendSMSChoreo.addInput("To", ToValue);
    String FromValue = "+16413246427";
    SendSMSChoreo.addInput("From", FromValue);
    String BodyValue = "ip";
    SendSMSChoreo.addInput("Body", BodyValue);
    String AccountSIDValue = "AC5884ce63c47e4c8ae693f0f97a58ca6e";
    SendSMSChoreo.addInput("AccountSID", AccountSIDValue);

    // Identify the Choreo to run
    SendSMSChoreo.setChoreo("/Library/Twilio/SMSMessages/SendSMS");

    // Run the Choreo; when results are available, print them to serial
    // 901 time to wait for a Choreo response. Can be edited as needed
    // USE_SSL input to tell library to use HTTPS
    SendSMSChoreo.run(901, USE_SSL);
    
    while(SendSMSChoreo.available()) {
      char c = SendSMSChoreo.read();
      Serial.print(c);
    }
    SendSMSChoreo.close();
  }
  
}  
boolean checkDistance()
{
  digitalWrite(trig, LOW); 
  delayMicroseconds(2); 

  digitalWrite(trig, HIGH);
  delayMicroseconds(10); 
 
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
 
//  speedOfSound = (331.3 + (0.6*TempC)) / 10000.0;   //Speed of sound in "cm per microsecond"
  distance = (duration)/29.1;             //Calculate the distance (in cm) based on the speed of sound.

  if(distance<30){                                  //When the distance detected is <30cm, it will return TRUE
    return true;
  }
  return false;
}

boolean endsWith(char* inString, char* compString) {
  int compLength = strlen(compString);
  int strLength = strlen(inString);
  
  //compare the last "compLength" values of the inString
  int i;
  for (i = 0; i < compLength; i++) {
    char a = inString[(strLength - 1) - i];
    char b = compString[(compLength - 1) - i];
    if (a != b) {
      return false;
    }
  }
  return true;
}










