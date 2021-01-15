//Notes: WIFI.h screws up analog inputs using ADC2 - lower pins 25 through 13. And possibly if using those pins, screws up wifi
#include <Stepper.h>
#include <WiFi.h>
#include <WebServer.h>

#include <Adafruit_NeoPixel.h>//Gems will be represented by lit pixels
#ifdef __AVR__      
  #include <avr/power.h>
#endif

/* Put your SSID & Password */
const char* ssid = "ESP_WiFi_1";  // Enter SSID here
const char* password = "12345678";  //Enter Password here

/* Assigned ESP IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
WebServer server(80); //default port for HTTP

int Ohms;

//these track different potential attachments a user can add to the robot
String attachmentTL; 
String attachmentC; 
String attachmentTR; 
String attachmentF; 

//NeoPixel Set up
#define PIN            16 //Neopixel pin
#define NUMPIXELS      32
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRBW + NEO_KHZ800);

int GemNum = 0; //variable to store number of gems the user collected in the exhibit
int delayval = 10; // delay
int v = 255; //brightness
int ColorIn; //value of analog read in for robots color
char inChar;

//analog in ports for attachment slots
int Fnt = 36; //front of robot
int TpLt = 38; //top left
int TpRt = 37; //top right
int TpCnt = 39; // top center

//values for analog reads for attached items
int FntV;
int TpLtV;
int TpRtV;
int TpCntV;

void setup() {
  Serial.begin(115200);

  //Wifi Connection, setting up esp32 as access point
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  //how we handle incoming HTTP requests 
  //(URL path, name of function later defined)
  server.on("/", handle_OnConnect);
  server.on("/0", handle_0);
  server.on("/1", handle_1);
  server.on("/2", handle_2);
  server.on("/3", handle_3);
  server.on("/4", handle_4);
  server.onNotFound(handle_NotFound);
  server.begin(); //start it up!
  Serial.println("HTTP server started");
  
  pixels.begin(); // This initializes the NeoPixel library.
  strip.begin();

  pinMode(19, INPUT_PULLUP);
}

void loop() { 
  server.handleClient();//handleClient ensures that the server/web page is continually supported

  //we have a ohmreader set up such that each attachment can be identified by a unique resistor
  //that interfaced with our circuit when attached
  TpLtV  =analogRead(TpLt);
  TpRtV  = analogRead(TpRt);
  TpCntV = analogRead(TpCnt);
  FntV   = analogRead(Fnt);
 
  //attachment=global variable used to determine what is printed on webpage.
  
  //Top Left attachment identification
  if(TpLtV >1850){  //Magnet 10K resistor
    attachmentTL="Magnet";}
    else if(TpLtV >1450 ){  //Plow 15K resistor
    attachmentTL="Plow";}
    else if(TpLtV > 1090){//Light 22K resistor
    attachmentTL="Light";}
    else if(TpLtV > 900){//Sound 27K resistor
    attachmentTL="Sound";}
    else if(TpLtV > 740){ //Vacuum 33K resistor
    attachmentTL="Vacuum";}
    else if(TpLtV > 640){ //Grabber 39K resisitor
    attachmentTL="Grabber";}
    else {
    attachmentTL="Nothing";}

//Center attachment identification
   if(TpCntV >1850){  //Magnet 10K resistor
    attachmentC="Magnet";}
    else if(TpCntV >1450 ){  //Plow 15K resistor
    attachmentC="Plow";}
    else if(TpCntV > 1090){//Light 22K resistor
    attachmentC="Light";}
    else if(TpCntV > 900){//Sound 27K resistor
    attachmentC="Sound";}
    else if(TpCntV > 740){ //Vacuum 33K resistor
    attachmentC="Vacuum";}
    else if(TpCntV > 640){ //Grabber 39K resisitor
    attachmentC="Grabber";}
    else {
    attachmentC="Nothing";}

//Top Right attachment identification
    if(TpRtV >1850){  //Magnet 10K resistor
    attachmentTR="Magnet";}
    else if(TpRtV >1450 ){  //Plow 15K resistor
    attachmentTR="Plow";}
    else if(TpRtV > 1090){//Light 22K resistor
    attachmentTR="Light";}
    else if(TpRtV > 900){//Sound 27K resistor
    attachmentTR="Sound";}
    else if(TpRtV > 740){ //Vacuum 33K resistor
    attachmentTR="Vacuum";}
    else if(TpRtV > 640){ //Grabber 39K resisitor
    attachmentTR="Grabber";}
    else {
    attachmentTR="Nothing";}

//Front attachment identification
    if(FntV >1850){  //Magnet 10K resistor
    attachmentF="Magnet";}
    else if(FntV >1450 ){  //Plow 15K resistor
    attachmentF="Plow";}
    else if(FntV > 1090){//Light 22K resistor
    attachmentF="Light";}
    else if(FntV > 900){//Sound 27K resistor
    attachmentF="Sound";}
    else if(FntV > 740){ //Vacuum 33K resistor
    attachmentF="Vacuum";}
    else if(FntV > 640){ //Grabber 39K resisitor
    attachmentF="Grabber";}
    else {
    attachmentF="Nothing";}
    
} 

//HTML and CSS code for webpage
String SendHTML(){//sends HTML code as one long string
  String ptr; 

   //the webpage is kept incredible simple for the exhibit application to parse
   //based on what the user has attached, the exhibit will respond accordingly
  ptr +=String(attachmentTL)+"\n"
  ptr +=String(attachmentC)+"\n";
  ptr +=String(attachmentTR)+"\n";
  ptr +=String(attachmentF)+"\n";
  
  ptr +="N/A\n";
  ptr +="N/A\n";
  ptr +="N/A\n";
  ptr +="N/A\n";
        
  return ptr;
}



void handle_OnConnect() {
 //serial prints for troubleshooting
 
  Serial.print("Top Left Ohm Read:");
  Serial.println(TpLtV);
  Serial.print("Top Left Attachment:");
  Serial.println(attachmentTL);
  Serial.println("");

  Serial.print("Center Ohm Read:");
  Serial.println(TpCntV);
  Serial.print("Center Attachment:");
  Serial.println(attachmentC);
  Serial.println("");

  Serial.print("Top Right Ohm Read:");
  Serial.println(TpRtV);
  Serial.print("Top Right Attachment:");
  Serial.println(attachmentTR);
  Serial.println("");

  Serial.print("Front Ohm Read:");
  Serial.println(FntV);
  Serial.print("Front Attachment:");
  Serial.println(attachmentF);
  Serial.println("");
  
  server.send(200, "text/html", SendHTML()); //send HTML for webpage defined above
}         


//below are the different url handles the exhibit can send back to us to indicate how many gems to light up
void handle_0() {
  GemNum=0;
  Serial.print("Gems Acquired:");
  Serial.println(GemNum);
  Serial.println("");
  server.send(200, "text/html", SendHTML()); //send HTML for webpage defined above
} 

void handle_1() {
  GemNum=1;
  Serial.print("Gems Acquired:");
  Serial.println(GemNum);
  Serial.println("");
  server.send(200, "text/html", SendHTML()); //send HTML for webpage defined above
} 

void handle_2() {
  GemNum=2;
  Serial.print("Gems Acquired:");
  Serial.println(GemNum);
  Serial.println("");
  server.send(200, "text/html", SendHTML()); //send HTML for webpage defined above
} 

void handle_3() {
  GemNum=3;
  Serial.print("Gems Acquired:");
  Serial.println(GemNum);
  Serial.println("");
  server.send(200, "text/html", SendHTML()); //send HTML for webpage defined above
} 

void handle_4() {
  GemNum=4;
  Serial.print("Gems Acquired:");
  Serial.println(GemNum);
  Serial.println("");
  server.send(200, "text/html", SendHTML()); //send HTML for webpage defined above
} 

//usuall when connection is lost, the web page drops and serial monitor stops printing
void handle_NotFound(){
  Serial.println("Lost Connection");
  server.send(404, "text/plain", "Not found");
}
