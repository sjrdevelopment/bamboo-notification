#include <SPI.h>
#include <Ethernet.h>


//Hardware address of ethernet shield
byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0xF8, 0x8F };

// initialize the library instance:
EthernetClient client;

char herokuServer[] = "blooming-temple-5808.herokuapp.com";  

unsigned long lastConnectionTime = 0;             // last time you connected to the server, in milliseconds
boolean lastConnected = false;                    // state of the connection last time through the main loop
const unsigned long postingInterval = 5L * 1000L; // delay between updates, in milliseconds
// the "L" is needed to use long type numbers

boolean currentLineIsBlank = false;
boolean lineBeforePost = false;
// boolean firstTimeRunFlag = true;

String stat = "";
String statB = "";
String buildNum = "";
  
int solenoidPin = 8;


void setup() {
  // start serial port:
  Serial.begin(9600);
    // initialize the digital pin as an output.
  pinMode(solenoidPin, OUTPUT);
  
  // give the ethernet module time to boot up:
  delay(1000);

  // start the Ethernet connection:
  Ethernet.begin(mac);
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
}

void loop()
{
  listenForHerokuData();
  delay(50);
}

void listenForHerokuData() {
    // if there are incoming bytes available
  // from the server, read them and print them:
  while (client.available()) {
 
    char c = client.read();
    Serial.print(c);
    
    if (lineBeforePost && c != '\n') {
          while(client.available()) {
           char ic = client.read();
           if(ic == '=') {
               char nextChar = client.read();
               if(nextChar == 'f') {
                 //if(stat != "fail") {
                   stat = "fail";
                   failNotification();
                // } else {
                 //  Serial.println("No change");
                 //}
               } else if (nextChar == 'p') {
                 // if(stat != "pass") {
                    stat = "pass";
                    passNotification(); 
                 // } else {
                 //    Serial.println("No change");
                 // }
               }
               
               if (stat != "") {
                 Serial.println("Build has " + stat + ".");
               } else {
                 stat = "";
                   Serial.println("No change");
               }
           }
           
          }
        } else if (c == '\n' && lineBeforePost) {
          
        } else if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c == '\r'  && currentLineIsBlank) {
           lineBeforePost = true;
        } else {
          currentLineIsBlank = false;
        }
   // Serial.print(c);
   
    
  }

  
  // if you're not connected, and ten seconds have passed since
  // your last connection, then connect again and send data:
  if (!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
    makeHerokuRequest();

  }
  // store the state of the connection for next time through
  // the loop:
  lastConnected = client.connected();
}




void makeHerokuRequest() {
 // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting to heroku...");

  // if you get a connection, report back via serial:
  if (client.connect(herokuServer, 80)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.println("GET /status HTTP/1.1");
    client.println("Host: blooming-temple-5808.herokuapp.com");
    client.println("Connection: close");
    client.println();
    
    // note the time that the connection was made:
    lastConnectionTime = millis();
  }
  else {
    // kf you didn't get a connection to the server:
    Serial.println("connection failed");
    client.stop();
  }
}

/*
boolean checkFirstTimeRun() {
  if(firstTimeRunFlag) {
    firstTimeRunFlag = false;
    Serial.println("First time run, skipping notification");
    return true;
  } else {
    return false;
  }
}
*/

void passNotification() {
//  if(!checkFirstTimeRun()) {
     Serial.println("running pass notification");
     digitalWrite(solenoidPin, HIGH);   // turn the LED on (HIGH is the voltage level)
     delay(10);               // wait for a second
     digitalWrite(solenoidPin, LOW);    // turn the LED off by making the voltage LOW
     delay(100); 
 // }
}

void failNotification() {
  // if(!checkFirstTimeRun()) {
     Serial.println("running fail notification");
     digitalWrite(solenoidPin, HIGH);   // turn the LED on (HIGH is the voltage level)
     delay(10);               // wait for a second
     digitalWrite(solenoidPin, LOW);    // turn the LED off by making the voltage LOW
     delay(250); 
     digitalWrite(solenoidPin, HIGH);   // turn the LED on (HIGH is the voltage level)
     delay(10);               // wait for a second
     digitalWrite(solenoidPin, LOW);    // turn the LED off by making the voltage LOW
     delay(250);
     digitalWrite(solenoidPin, HIGH);   // turn the LED on (HIGH is the voltage level)
     delay(10);               // wait for a second
     digitalWrite(solenoidPin, LOW);    // turn the LED off by making the voltage LOW
     delay(100);
  // }
}
