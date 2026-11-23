#include <Arduino.h>
#include "esp_mac.h"

//Script para leer la mac del ESP32

void setup() {
  Serial.begin(115200);  
  delay(2000); 

  uint8_t baseMac[6]; 
  esp_err_t ret = esp_read_mac(baseMac, ESP_MAC_WIFI_STA);

  if (ret == ESP_OK) {
    Serial.printf("Dirección MAC (Wi-Fi STA): %02X:%02X:%02X:%02X:%02X:%02X\n",
                  baseMac[0], baseMac[1], baseMac[2],
                  baseMac[3], baseMac[4], baseMac[5]);
  } else {
    Serial.println("Error al leer la dirección MAC.");
  }
}

void loop() {

}
