#include "esp_camera.h"
#include <WebSocketsServer.h> //https://github.com/Links2004/arduinoWebSockets
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h> //tested for version 5.13.3
#include "driver/gpio.h"

//
// WARNING!!! Make sure that you have either selected ESP32 Wrover Module,
//            or another board which has PSRAM enabled
//

#define CAMERA_MODEL_AI_THINKER
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<14))
#include "camera_pins.h"

const char* ssid = "ESP32_BOT_1";
const char* password = "0165123135abc";

/* Put IP Address details */
IPAddress local_ip(192,168,4,1);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);

unsigned char ip_flag = 0x11;

WebSocketsServer webSocket(82);

const int LedCh=1;
const int LedRCh = 2;
const int LedGCh = 3;
const int LedBCh = 4;

const int MotorA0Ch = 5;
const int MotorA1Ch = 6;
const int MotorB0Ch = 7;
const int MotorB1Ch = 8;

byte red_dec, green_dec, blue_dec;

void startCameraServer();

void blink_led(int led_color){
      red_dec = led_color >> 16;
      green_dec = led_color >> 8;
      blue_dec = led_color;

 /*
    ledcWrite(LedRCh, red_dec);
    ledcWrite(LedGCh, green_dec);
    ledcWrite(LedBCh, blue_dec);
  */
      ledcWrite(LedRCh, 255-red_dec);
    ledcWrite(LedGCh, 255-green_dec);
    ledcWrite(LedBCh, 255-blue_dec);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t payloadlength) { // When a WebSocket message is received

  int blk_count = 0;
  IPAddress localip;
  char ipaddr[26] ;
  switch (type) {
    case WStype_DISCONNECTED:             // if the websocket is disconnected
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {              // if a new websocket connection is established
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        webSocket.sendBIN(0, &ip_flag, 1);
        localip = WiFi.localIP();
        sprintf(ipaddr, "%d.%d.%d.%d", localip[0], localip[1], localip[2], localip[3]);
        webSocket.sendTXT(0, (const char *)ipaddr);

      }
   case WStype_TEXT:                     // if new text data is received

      if (payload[0] == '{') { // check if json command was received
        //DynamicJsonBuffer json_input;
        //JsonObject& doc = json_input.parseObject((const char *)payload);       
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, (const char *)payload);
        if (error)
        return;

        const char *cmd = doc["cmd"];
        const int val = doc["val"];
        const int val2 = doc["val2"];
          
        if (strstr(cmd, "power")) {
          Serial.println("Going to sleep now");
          esp_deep_sleep_start();
        }

        else if (strstr(cmd, "color")) {
 //         Serial.printf("Led color: %x\n", val);
          blink_led(val);          

        }
        else if (strstr(cmd, "light")) {
          if(val==1) {
            blink_led(0);   
          }
          else {
            blink_led(0xFFFFFF);   
          }
        }
        else if (strstr(cmd, "pos")) {
         //val:direction / leftMotor
         //val2:speed / rightMotor
          //const int val2 = doc["val2"];

          if(val==0){
               ledcWrite(MotorA0Ch, 0);
               ledcWrite(MotorA1Ch, 0);
          } else if(val > 0){ 
            ledcWrite(MotorA0Ch, abs(val));
            ledcWrite(MotorA1Ch, 0);
          } else  {
            ledcWrite(MotorA0Ch, 0);
            ledcWrite(MotorA1Ch, abs(val));
          }    

          if(val2==0){
               ledcWrite(MotorB0Ch, 0);
               ledcWrite(MotorB1Ch, 0);
          }  else if(val2>0){
            ledcWrite(MotorB0Ch, abs(val2));
            ledcWrite(MotorB1Ch, 0);
          } else {
            ledcWrite(MotorB0Ch, 0);
            ledcWrite(MotorB1Ch, abs(val2));
          } 


          Serial.printf("Motor input received: Dir:%d Speed:%d\r\n", val, val2);
//          Serial.printf("Motor input received: L:%dL R:%d\r\n", abs(val), abs(val2));
        }
        else {
          Serial.println("Unknown command");
          //sendMSG("INFO","ESP32: Unknown command received");
        }
        
      }
        break;
    case WStype_ERROR:                     // if new text data is received
      Serial.printf("Error \n");
    default:
      Serial.printf("WStype %x not handled \n", type);

  }
}


void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  //init with high specs to pre-allocate larger buffers
  if(psramFound()){
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 5;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 8;
    config.fb_count = 1;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();

  //drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_VGA);

  //create wifi access point
    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    Serial.println();
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  startCameraServer();

  gpio_config_t configG;
  configG.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
  configG.mode = GPIO_MODE_OUTPUT; 
 // configG.pull_up_en = GPIO_PULLUP_DISABLE;
 // configG.pull_down_en = GPIO_PULLDOWN_DISABLE;
  configG.intr_type = GPIO_INTR_DISABLE;
  gpio_config(&configG);

  ledcSetup(LedRCh, 490, 8);
  ledcAttachPin(2, LedRCh);
  ledcSetup(LedGCh, 490, 8);
  ledcAttachPin(1, LedGCh);  //UOT     
  ledcSetup(LedBCh, 490, 8); 
  ledcAttachPin(3, LedBCh); //UOR

  ledcAttachPin(14, MotorA0Ch);
  ledcSetup(MotorA0Ch, 490, 8);
  ledcAttachPin(15, MotorA1Ch);
  ledcSetup(MotorA1Ch, 490, 8); 
  ledcAttachPin(12, MotorB0Ch); 
  ledcSetup(MotorB0Ch, 490, 8);
  ledcAttachPin(13, MotorB1Ch); 
  ledcSetup(MotorB1Ch, 490, 8);

  //camera malfunction
//  ledcAttachPin(4, LedCh);
//  ledcSetup(LedCh, 5000, 8);


//bootloop
//  ledcSetup(R1Ch, 5000, 8);
//  ledcAttachPin(MA2, R1Ch);
 
  
}

void loop() {
  webSocket.loop();
  delay(10);
}
