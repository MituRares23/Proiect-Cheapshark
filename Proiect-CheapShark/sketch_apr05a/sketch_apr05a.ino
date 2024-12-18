#include <BluetoothSerial.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
 
#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif
 
BluetoothSerial ESP_BT;
char i=0; 
char data[40]; 
int secret_password = 555;
int team_number = 128;
int numar_random;
const unsigned int MAX_INPUT = 40; 
int LED_PIN = 16; 
 
#define BT_DISCOVER_TIME  10000
static bool btScanSync = true;

uint16_t crc16_xmodem(uint8_t *data1, uint32_t len)
{
    uint16_t crc = 0;
    uint32_t i, j;
    
    for (i = 0; i < len; i++) {
        crc ^= (uint16_t)data1[i] << 8;
        for (j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc <<= 1;
            }
        }
    }
    
    return crc;
}

//void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) { 
// if(event == ESP_SPP_SRV_OPEN_EVT){ 
// Serial.println("Client Connected"); 
// delay(1000);
// ESP_BT.print("AT+U");
// ESP_BT.print(team_number);
// ESP_BT.write('\n');
// connected = true; 
// }
// 
// if(event == ESP_SPP_CLOSE_EVT ){ 
// Serial.println("Client disconnected"); 
// connected = false; 
// } 
//}

void process_data (const char * data) { 
 Serial.println (data); 
if(strncmp(data,"210",3)==0){
char* start = strstr(data, "Authorize ");
start += strlen("Authorize ");
int random_number = atoi(start);
int y=random_number+secret_password+team_number;
Serial.print(y);
  ESP_BT.print(y);
uint16_t num = y;
    char num_str[6];
    sprintf(num_str, "%hu", num);
    
    uint32_t len = strlen(num_str);
    uint8_t *data1 = (uint8_t*)malloc(len);
    memcpy(data1, num_str, len);
    
    uint16_t crc = crc16_xmodem(data1, len);
    
    ESP_BT.println(crc,DEC);
    Serial.println(crc,DEC);
}

}

void processIncomingByte (const byte inByte) { 
 static char input_line [MAX_INPUT]; 
 static unsigned int input_pos = 0; 
 switch (inByte) 
 { 
 case '\n': // end of text 
 input_line [input_pos] = 0; // terminating null byte 
 
 // terminator reached! process input_line here ... 
 process_data (input_line); 
 
 // reset buffer for next time 
 input_pos = 0; 
 break; 
 case '\r': // discard carriage return 
 break; 
 default: 
 // keep adding if not full ... allow for terminating null byte 
 if (input_pos < (MAX_INPUT - 1)) 
 input_line [input_pos++] = inByte; 
 break; 
 } //switch 
 
 }
 
void setup() {
  Serial.begin(115200);
  ESP_BT.begin("ESP32test",true); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  if (btScanSync) {
    Serial.println("Starting discover...");
    BTScanResults *pResults = ESP_BT.discover(BT_DISCOVER_TIME);
    if (pResults)
      pResults->dump(&Serial);
    else
      Serial.println("Error on BT Scan, no result!");
  }
}
 
void loop() {
  delay(100);
  if(ESP_BT.connected()==true)
  {
    Serial.println("Connected");
    processIncomingByte(ESP_BT.read());
  }
  else
  {
    SerialBT.connect("PHNT5");
  }
  delay(500);
}
