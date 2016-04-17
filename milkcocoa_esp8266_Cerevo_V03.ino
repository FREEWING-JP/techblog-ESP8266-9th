//mini4wd control with milkcocoa
//Add LED light
//Add BT Module Control
//2016 April 18
//Cerevo Inc.

#include <ESP8266WiFi.h>
#include <Milkcocoa.h>


#include <Wire.h>
#define ADDR1  0x64

#define command_start  0
#define command_stop   1
#define command_back  2
#define forward       0x01
#define reverse       0x02

//LED comunication inicator
#define LED_H       (digitalWrite( 12, HIGH ))
#define LED_L       (digitalWrite( 12, LOW ))
//TP1 GPIO13
#define BLED_H       (analogWrite( 13, 1023 ))
#define BLED_M       (analogWrite( 13, 512 ))
#define BLED_L       (analogWrite( 13, 0 ))
//GPIO5(SW)
#define MAIN_H       (digitalWrite( 5, HIGH ))
#define MAIN_L       (digitalWrite( 5, LOW ))
//GPIO2(Mode)
#define VOLP_H       (digitalWrite( 2, HIGH ))
#define VOLP_L       (digitalWrite( 2, LOW ))
//GPIO15(Mode)
#define VOLM_H       (digitalWrite( 15, HIGH ))
#define VOLM_L       (digitalWrite( 15, LOW ))


char state = command_stop;
int offset = 0;

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "HG8045-734B-bg" //自宅で使用のSSID名
#define WLAN_PASS       "u86s6ar9"       //自宅で使用のSSID パスワード


/************************* Your Milkcocoa Setup *********************************/


#define MILKCOCOA_APP_ID      "catimlk5yjv"               //milkcocoa idを記載
#define MILKCOCOA_DATASTORE   "ESP8266_Cerevo"            //milkcocoa data store 名を記載

/************* Milkcocoa Setup (you don't need to change this!) ******************/

#define MILKCOCOA_SERVERPORT  1883

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

const char MQTT_SERVER[] PROGMEM    = MILKCOCOA_APP_ID ".mlkcca.com";
const char MQTT_CLIENTID[] PROGMEM  = __TIME__ MILKCOCOA_APP_ID;

Milkcocoa milkcocoa = Milkcocoa(&client, MQTT_SERVER, MILKCOCOA_SERVERPORT, MILKCOCOA_APP_ID, MQTT_CLIENTID);

void onpush(DataElement *elem) {
  int command;    //1:f_left 2:forward 3:f_right 0:stop 7:r_left 8:back 9:r_right
  
  Serial.println("onpush");
  command = elem->getInt("com");

  if(command == 1){
    handle_f_left();
  }else if(command == 2){
    handle_drive();
  }else if(command == 3){
    handle_f_right();
  }else if(command == 0){
    handle_stop();
  }else if(command == 7){
    handle_r_left();
  }else if(command == 8){
    handle_back();
  }else if(command == 9){
    handle_r_right();
  }else if(command == 4){
    handle_btpower();
  }else if(command == 5){
    handle_btplus();
  }else if(command == 6){
    handle_btminus();
  }
 

  
};

void setupWiFi() {
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("Milkcocoa SDK demo");

  Wire.begin(4, 14);
  delay(40);
  
  setupWiFi();

  pinMode(16,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(13,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(2,OUTPUT);
  pinMode(15,OUTPUT); 
  BLED_H;   
  MAIN_L;
  VOLP_L;
  VOLM_L;
    
  Serial.println( milkcocoa.on(MILKCOCOA_DATASTORE, "push", onpush) );
};

void loop() {
  milkcocoa.loop();
  DataElement elem = DataElement();

  delay(2000);
};

void handle_stop() {
  Serial.print("stop\r\n");
  LED_H;
  BLED_H;
    stop_motor();
  LED_L;
    state = command_stop;
}

void handle_drive() {
 Serial.print("drive\r\n");
  LED_H;
  BLED_M;
  drive();
  servo_control(90);
  LED_L;
}

void handle_back() {
  Serial.print("back\r\n");
  BLED_M;  
  back();
  servo_control(90);
}

void handle_left(){
  Serial.print("left\r\n");
  BLED_M;
   drive();
  servo_control(60);
}

void handle_right(){
  Serial.print("right\r\n");
   BLED_M;
   drive();
  servo_control(120);
}


void handle_f_left(){
  Serial.print("f_left\r\n");
  LED_H;
  BLED_M;
  drive();
  servo_control(60);
  LED_L;
}


void handle_f_right(){
  Serial.print("f_right\r\n");
  LED_H;
  BLED_M;
  drive();
  servo_control(120);
  LED_L;
}

void handle_r_left(){
  Serial.print("r_left\r\n");
  LED_H;
  BLED_M;
  back();
  servo_control(60);
  LED_L;
}


void handle_r_right(){
  Serial.print("r_right\r\n");
  LED_H;
  BLED_M;
  back();
  servo_control(120);
  LED_L;
}


void handle_btpower(){
  Serial.print("BT Power ON:H wait 5sec\r\n");
  MAIN_H;
  delay(5000);
  MAIN_L;
  Serial.print("BT Power ON:L\r\n");
}

void handle_btplus(){
  Serial.print("BT VOL+ \r\n");
  VOLP_H;
  delay(1000);
  VOLP_L;
}

void handle_btminus(){
  Serial.print("BT VOL- \r\n");
  VOLM_H;
  delay(1000);
  VOLM_L;
}


void drive(){
    if(state == command_back){
    
     stop_motor();

     delay(10);

     start_motor();
  
  }else if(state == command_stop){
    start_motor();
  }
  state = command_start;
}

void back(){
    if(state == command_start){
    
     stop_motor();

     delay(10);

     reverse_motor();
  
  }else if(state == command_stop){
    reverse_motor();

  }
  state = command_back;
}

void start_motor(){
  char i, volt;
  volt = 0x20;
  for(i=0;i<5;i++){ 
    volt = volt + ((0x40)*i);
    volt = volt | forward;
    motor_func(ADDR1 , volt);
    delay(10);
  }
}

void reverse_motor(){
    char i, volt;
  volt = 0x20;
  for(i=0;i<5;i++){ 
    volt = volt + ((0x40)*i);
    volt = volt | reverse;
    motor_func(ADDR1 , volt);
    delay(10);
  }
   
}


void stop_motor(){
  motor_func(ADDR1 , 0x18);
  delay(10);

  motor_func(ADDR1 , 0x1B);
  delay(10);
}

void motor_func(char add , char duty){
  Wire.beginTransmission(add);
  Wire.write(0x00);
  Wire.write(duty);
  Wire.endTransmission();
}

void servo_control(int angle){
int microsec,i;

      microsec = (5*(angle+offset))+ 1000;
       
      for(i=0; i<20 ;i++){
        digitalWrite( 16, HIGH );
        delayMicroseconds( microsec ); 
        digitalWrite( 16, LOW );
        delayMicroseconds( 10000 - microsec ); 
      }
}

