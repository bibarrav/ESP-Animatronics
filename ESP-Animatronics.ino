#include <NewPing.h>
 
//const int UltrasonicPin = 5;
const int TrigerPin = 13;
const int EchoPin = 12;
const int MaxDistance = 400;

int SonarDistance = 0;

#include <DFRobotDFPlayerMini.h>
 
HardwareSerial Serial2(2);
DFRobotDFPlayerMini myDFPlayer;
const int PlayerBusyPin = 4;  //Busy Ping of DFPlayer Mini to check playing status

//Leds
const int LedAPin = 15;
const int LedBPin = 2;
 
//NewPing sonar(UltrasonicPin, UltrasonicPin, MaxDistance);
NewPing sonar(TrigerPin, EchoPin, MaxDistance);

//Servos
#include <ESP32_Servo.h>
const int ServoAPin = 5; 
Servo myServoA;  // crea el objeto servo
int ServoApos = 0;    // posicion del servo

void setup() {
  Serial.begin(9600);
  pinMode(LedAPin,OUTPUT);
  pinMode(LedBPin,OUTPUT);
  pinMode(PlayerBusyPin,INPUT);
  //myServoA.attach(ServoAPin);
  myServoA.attach(ServoAPin, 771, 2740);
  Serial2.begin(9600);
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  //myDFPlayer.begin(Serial2);
  //delay(5000);

  while (!myDFPlayer.begin(Serial2)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    delay(50); // Code to compatible with ESP8266 watch dog.
    }
  Serial.println(F("DFPlayer Mini online."));
  
  myDFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms
  
  //----Set volume----
  myDFPlayer.volume(30);  //Set volume value (0~30).
//  myDFPlayer.volumeUp(); //Volume Up
//  myDFPlayer.volumeDown(); //Volume Down
  
  //----Set different EQ----
//  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
//  myDFPlayer.EQ(DFPLAYER_EQ_POP);
//  myDFPlayer.EQ(DFPLAYER_EQ_ROCK);
//  myDFPlayer.EQ(DFPLAYER_EQ_JAZZ);
//  myDFPlayer.EQ(DFPLAYER_EQ_CLASSIC);
  myDFPlayer.EQ(DFPLAYER_EQ_BASS);
  
  //----Set device we use SD as default----
//  myDFPlayer.outputDevice(DFPLAYER_DEVICE_U_DISK);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
//  myDFPlayer.outputDevice(DFPLAYER_DEVICE_AUX);
//  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SLEEP);
//  myDFPlayer.outputDevice(DFPLAYER_DEVICE_FLASH);
  
  //----Mp3 control----
//  myDFPlayer.sleep();     //sleep
//  myDFPlayer.reset();     //Reset the module
//  myDFPlayer.enableDAC();  //Enable On-chip DAC
//  myDFPlayer.disableDAC();  //Disable On-chip DAC
//  myDFPlayer.outputSetting(true, 15); //output setting, enable the output and set the gain to 15
  
}

bool Playing = false;

void loop() {
  delay(100);                      // esperar 50ms entre pings (29ms como minimo)
  SonarDistance = sonar.ping_cm();
  Playing = !digitalRead(PlayerBusyPin);
  Serial.print(SonarDistance); // obtener el valor en cm (0 = fuera de rango)
  Serial.println("cm");
  Serial.print("DFPlayer Busy: ");
  Serial.println(Playing); //read mp3 state
  
  if (SonarDistance < 120 && SonarDistance > 0)
  {
    digitalWrite(LedAPin,HIGH);
    digitalWrite(LedBPin,HIGH);
    if (!Playing)
    {
      myDFPlayer.volume(15);  //Set volume value (0~30).
      myDFPlayer.play(1);
    }
    //myDFPlayer.next();
    else
    {
      
    }
  }
  else
  {
    if(!Playing)
    {
      digitalWrite(LedAPin,LOW);
      digitalWrite(LedBPin,LOW);
      //myDFPlayer.pause();  //pause the mp3
    }
  }

  if (Playing)
  {
       int ServoAPosInit = myServoA.read();
       Serial.print("ServoA Pos: ");
       Serial.println(ServoAPosInit);
       if (ServoAPosInit > 0)
       {
         for (ServoApos = ServoAPosInit; ServoApos >= 0; ServoApos -= 1) 
         {
           if (!Playing) break;
           myServoA.write(ServoApos);              
           delay(12);   
           Playing = !digitalRead(PlayerBusyPin);                     
         }
       }
       //varia la posicion de 0 a 180, con esperas de 15ms
       for (ServoApos = 0; ServoApos <= 180; ServoApos += 1) 
       {
          if (!Playing) break;
          myServoA.write(ServoApos);              
          delay(12);   
          Playing = !digitalRead(PlayerBusyPin);                     
       }
 
       //varia la posicion de 0 a 180, con esperas de 15ms
       for (ServoApos = 180; ServoApos >= 0; ServoApos -= 1) 
       {
         if (!Playing) break;
         myServoA.write(ServoApos);              
         delay(12);      
         Playing = !digitalRead(PlayerBusyPin);                 
       }
  }
}
