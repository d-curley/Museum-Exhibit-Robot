#include <WiFi.h>
#include <WebServer.h>

WebServer server(80); //Default server channel for Arduino
//Pin set up
uint8_t LED1pin = 12;
bool LED1status = LOW;

uint8_t LED2pin = 13;
bool LED2status = LOW;

uint8_t LED3pin = 5; 
bool LED3status = LOW;

uint8_t Buzzer = 15;
bool BUZZstatus = LOW;

uint8_t Drivef = 16; //Forward Drive
uint8_t Driveb = 17; //Backward Drive
bool DRIVE;          // Drive Status

uint8_t HeadCW = 18; //Head Spin Clockwise

uint8_t HeadCCW = 19;//Head Spin Counter-Clockwise
bool SPIN;           //Head Spin Status

uint8_t Front = 22;
int HeadFS;

void setup() {

  Serial.begin(115200);
  pinMode(LED1pin, OUTPUT);
  pinMode(LED2pin, OUTPUT);
  pinMode(LED3pin, OUTPUT);
  pinMode(Drivef, OUTPUT);
  pinMode(Driveb, OUTPUT);
  pinMode(HeadCW, OUTPUT);
  pinMode(HeadCCW, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(Front,INPUT);

  // Future featue: Hot Swap: Put IP Address, ssid, and password in a swap statement
  // Note: The goal here is for the tablet to stay on the same WiFi and auto-connect

  /* Put your SSID & Password */
  const char* ssid = "Edward";  // Enter SSID here
  const char* password = "12345678";  //Enter Password here
  
    /* Assigned ESP IP Address details */
  IPAddress local_ip(192,168,1,1);
  IPAddress gateway(192,168,1,1);
  IPAddress subnet(255,255,255,0);

//Wifi Connection
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  //how we handle incoming HTTP requests 
  //(URL path, name of function later defined)
  //One URL path per function

  //establish url handles for our different functions
  server.on("/", handle_OnConnect);
  server.on("/led1on", handle_led1on);
  server.on("/led1off", handle_led1off);
  server.on("/led2on", handle_led2on);
  server.on("/led2off", handle_led2off);
  server.on("/led3on", handle_led3on);
  server.on("/led3off", handle_led3off);
  server.on("/buzzon", handle_buzzon);
  server.on("/buzzoff", handle_buzzoff);
  server.on("/drivef", handle_drivef);
  server.on("/driveb", handle_driveb);
  server.on("/headcw", handle_headcw);
  server.on("/headccw", handle_headccw);
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() { 
   server.handleClient();

  //status variables are set based on url handle requests
  //these are for LED control and the buzzer, which we have in the void loop since they are always active
  if(LED1status)
  {digitalWrite(LED1pin, HIGH);}
  else
  {digitalWrite(LED1pin, LOW);}
  
  if(LED2status)
  {digitalWrite(LED2pin, HIGH);}
  else
  {digitalWrite(LED2pin, LOW);}

  if(LED3status) 
  {digitalWrite(LED3pin, HIGH);}
  else
  {digitalWrite(LED3pin, LOW);}

  if(BUZZstatus) 
  {digitalWrite(Buzzer, HIGH);}
  else
  {digitalWrite(Buzzer, LOW);}

//HTML and CSS code for webpage
//this web page has a series of labeled buttons that when pressed, will control the robot
//we need to sendHTML() with all the variables so it can make changes to the robot while aware of its current state
String SendHTML(uint8_t led1stat,uint8_t led2stat,uint8_t led3stat,uint8_t buzzstat,uint8_t forward, uint8_t head){

  String ptr = "<!DOCTYPE html> <html>\n"; //indiactes we're sending HTML
 
  //creates one huge HTML string of code
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n"; //works in any browser
  ptr +="<title>Edward</title>\n"; //name of page
  
  //<style>
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n"; //places Helvetica font for buttons
  ptr +="body{margin-top: 5px;} h1 {color: #444444;margin: 5px auto 10px;} h3 {color: #444444;margin-bottom: 20px;}\n"; //font color
  ptr +=".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 20px;text-decoration: none;font-size: 30px;margin: 0px auto 10px;cursor: pointer;border-radius: 4px;}\n";//font margins
  ptr +=".button-on {background-color: #3498db;}\n";
  ptr +=".button-on:active {background-color: #2980b9;}\n";
  ptr +=".button-off {background-color: #34495e;}\n"; 
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>MoS Robot Demo: Edward</h1>\n"; //name of web page
  ptr +="<h3>ESP32 Web Server</h3>\n";

  //Reads the current status of booleans on robot, and makes buttons accordingly
  //When a button is pushed it will change the url, triggering the corresponding Arduino function on the robot
   if(led1stat) 
  {ptr +="<p>Red Status: ON</p><a class=\"button button-off\" href=\"/led1off\">OFF</a>\n";}
  else
  {ptr +="<p>Red Status: OFF</p><a class=\"button button-on\" href=\"/led1on\">ON</a>\n";}

  if(led2stat)
  {ptr +="<p>Green Status: ON</p><a class=\"button button-off\" href=\"/led2off\">OFF</a>\n";}
  else
  {ptr +="<p>Green Status: OFF</p><a class=\"button button-on\" href=\"/led2on\">ON</a>\n";}

  if(led3stat)
  {ptr +="<p>Blue Status: ON</p><a class=\"button button-off\" href=\"/led3off\">OFF</a>\n";}
  else
  {ptr +="<p>Blue Status: OFF</p><a class=\"button button-on\" href=\"/led3on\">ON</a>\n";}

  if(buzzstat)
  {ptr +="<p>Buzzer Status: ON</p><a class=\"button button-off\" href=\"/buzzoff\">OFF</a>\n";}
  else
  {ptr +="<p>Buzzer Status: OFF</p><a class=\"button button-on\" href=\"/buzzon\">ON</a>\n";}

  if(forward)
  {ptr +="<p>Drive: Backwards</p><a class=\"button button-off\" href=\"/driveb\">BACK</a>\n";}
  else
  {ptr +="<p>Drive: Forwards</p><a class=\"button button-on\" href=\"/drivef\">FORW.</a>\n";}

  if(head)
  {ptr +="<p>Spin: CCW</p><a class=\"button button-off\" href=\"/headccw\">CCW</a>\n";}
  else
  {ptr +="<p>Spin: CW</p><a class=\"button button-on\" href=\"/headcw\">CW</a>\n";}

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}


void handle_OnConnect() {
  //initialize variables when connected
  LED1status = LOW;
  LED2status = LOW;
  LED3status = LOW;
  BUZZstatus = LOW;
  DRIVE = LOW;
  SPIN = LOW;
  Serial.println("Light OFF.  Motors OFF");
  server.send(200, "text/html", SendHTML(LED1status,LED2status,LED3status,BUZZstatus,DRIVE,SPIN)); 
} 
                                                                                      
void handle_led1on() {
  LED1status = HIGH;
  Serial.println("Red Status: ON");
  server.send(200, "text/html", SendHTML(true,LED2status,LED3status,BUZZstatus,DRIVE,SPIN));  
}

void handle_led1off() {
  LED1status = LOW;
  Serial.println("Red Status: OFF");
  server.send(200, "text/html", SendHTML(false,LED2status,LED3status,BUZZstatus,DRIVE,SPIN)); 
}

void handle_led2on() {
  LED2status = HIGH;
  Serial.println("Green Status: ON");
  server.send(200, "text/html", SendHTML(LED1status,true,LED3status,BUZZstatus,DRIVE,SPIN)); 
}

void handle_led2off() {
  LED2status = LOW;
  Serial.println("Green Status: OFF");
  server.send(200, "text/html", SendHTML(LED1status,false,LED3status,BUZZstatus,DRIVE,SPIN));  
}

void handle_led3on() {
  LED3status = HIGH;
  Serial.println("Blue Status: ON");
  server.send(200, "text/html", SendHTML(LED1status,LED2status,true,BUZZstatus,DRIVE,SPIN)); 
}

void handle_led3off() {
  LED3status = LOW;
  Serial.println("Blue Status: OFF");
  server.send(200, "text/html", SendHTML(LED1status,LED2status,false,BUZZstatus,DRIVE,SPIN));
}

void handle_buzzon() {
  BUZZstatus = HIGH;
  Serial.println("Buzzer Status: ON");
  server.send(200, "text/html", SendHTML(LED1status,LED2status,LED3status,true,DRIVE,SPIN)); 
}

void handle_buzzoff() {
  BUZZstatus = LOW;
  Serial.println("Buzzer Status: OFF");
  server.send(200, "text/html", SendHTML(LED1status,LED2status,LED3status,true,DRIVE,SPIN));
}

void handle_drivef() {
  DRIVE = HIGH;
  digitalWrite(Drivef, HIGH); //turn on pin used for motors ‘forward’ direction
  Serial.println("We're Moving Forward!");
  delay(5000); //5 seconds of ‘motor on’ time
  digitalWrite(Drivef, LOW); ////turn off pin used for motors ‘forward’ direction
  Serial.println("We've Stopped"); 
  server.send(200, "text/html", SendHTML(LED1status,LED2status,LED3status,BUZZstatus,true,SPIN));
}

void handle_driveb() {
  DRIVE = LOW;
  digitalWrite(Driveb, HIGH); //turn on pin used for motors ‘backward’ direction
  Serial.println("We're Moving Back!");
  delay(5000); //5 seconds of ‘motor on’ time
  digitalWrite(Driveb, LOW); ////turn off pin used for motors ‘backward’ direction 
  Serial.println("We've Stopped");
  server.send(200, "text/html", SendHTML(LED1status,LED2status,LED3status,BUZZstatus,false,SPIN));
}

void handle_headcw() {
  SPIN=HIGH;
digitalWrite(HeadCW, HIGH); //turn on motor
Serial.println("Spinning Clockwise");
//  for (int i=1; i <= 4; i++){ //setting rotation count routine
//    HeadFS = digitalRead(Front); //read the ‘front of robot’ sensor. 
//    while (HeadFS > 10){ //hanging out here while moving off front sensor
//      HeadFS = digitalRead(Front); //killing time while head spins
//    }
//  }
  delay(2000); //2 seconds of ‘motor on’ time
  digitalWrite(HeadCW, LOW); //turn off motor
  Serial.println("Done Spinning");
  server.send(200, "text/html", SendHTML(LED1status,LED2status,LED3status,BUZZstatus,DRIVE,true));
}

void handle_headccw() {
  SPIN= LOW;
digitalWrite(HeadCCW, HIGH); //turn on motor
Serial.println("Spinning Counter-Clockwise");
//  for (int i=1; i <= 4; i++){ //setting rotation count routine
//    HeadFS = digitalRead(Front); //read the ‘front of robot’ sensor. 
//    while (HeadFS > 10){ //hanging out here while moving off front sensor
//      HeadFS = digitalRead(Front); //killing time while head spins
//    }
//  }
  
  delay(2000); //2 seconds of ‘motor on’ time
  digitalWrite(HeadCCW, LOW); //turn off motor
  Serial.println("Done Spinning");
  server.send(200, "text/html", SendHTML(LED1status,LED2status,LED3status,BUZZstatus,DRIVE,false));
}

void handle_NotFound(){

  server.send(404, "text/plain", "Not found");
}
