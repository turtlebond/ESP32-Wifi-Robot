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

const char* ssid = "ESP32_BOT";
const char* password = "123456";

/* Put IP Address details */
IPAddress local_ip(192,168,4,1);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);

unsigned char ip_flag = 0x11;

WebSocketsServer webSocket(82);

// motor outputs
const int MA1 = 12;
const int MA2 = 13;
const int MB1 = 14;
const int MB2 = 15;

const int L1Ch = 5;
const int L2Ch = 6;
const int R1Ch = 7;
const int R2Ch = 8;


void startCameraServer();


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
        DynamicJsonBuffer json_input;
        JsonObject& root = json_input.parseObject((const char *)payload);
        const char *cmd = root["cmd"];
        const int val = root["val"];

        if (strstr(cmd, "power")) {
          Serial.println("Going to sleep now");
          esp_deep_sleep_start();
        }
        /*
        else if (strstr(cmd, "light")) {
          if(val==1) {
            ledcWrite(LedCh, 20);
          }
          else {
              ledcWrite(LedCh, 0);
          }
        }
        */
        else if (strstr(cmd, "pos")) {
         //val:leftmotor
         //val2:rightmotor
          const int val2 = root["val2"];
          if(val==0){
               ledcWrite(L1Ch, 0);
               ledcWrite(L2Ch, 0);
          } else if(val > 0){ //4xPWM motor control
            ledcWrite(L1Ch, abs(val));
            ledcWrite(L2Ch, 0);
          } else  {
            ledcWrite(L1Ch, 0);
            ledcWrite(L2Ch, abs(val));
          }    
           
          if(val2==0){
               ledcWrite(R1Ch, 0);
               ledcWrite(R2Ch, 0);
          }  else if(val2>0){
            ledcWrite(R1Ch, abs(val2));
            ledcWrite(R2Ch, 0);
          } else {
            ledcWrite(R1Ch, 0);
            ledcWrite(R2Ch, abs(val2));
          } 

          Serial.printf("Motor input received: L:%dL R:%d\r\n", val, val2);
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
  configG.intr_type = GPIO_INTR_DISABLE;
  gpio_config(&configG);

    

  ledcSetup(L1Ch, 490, 8);
  ledcSetup(L2Ch, 490, 8);
  ledcSetup(R1Ch, 490, 8);
  ledcSetup(R2Ch, 490, 8);
 
  ledcAttachPin(MA1, L1Ch);
  ledcAttachPin(MA2, L2Ch);
  ledcAttachPin(MB1, R1Ch);
  ledcAttachPin(MB2, R2Ch);
  
}

void loop() {
  webSocket.loop();
}

