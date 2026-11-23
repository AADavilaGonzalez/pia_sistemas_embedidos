#include <WiFi.h>
#include <esp_now.h>

// LEDs
const int encendido   = 4;
const int decrementar = 5;
const int incrementar = 6;
const int error       = 7;

// Botones
const int btn_encender = 10;
const int btn_apagar   = 11;
const int analog_in    = 9;

// Parametros
const int MARGEN_P = 200;
const unsigned long TIEMPO_N = 3000;

// MAC de la placa A (02:4a:7c:b1:93:e5)
// uint8_t macRemota[] = {0x02, 0x4A, 0x7C, 0xB1, 0x93, 0xE5};

// MAC de la placa B (7c:4f:ad:ba:ca:dc)
uint8_t macRemota[] = {0x7C, 0x4F, 0xAD, 0xBA, 0xCA, 0xDC};

struct datos_maquina {
  uint32_t timestamp;
  uint16_t potencia;
  bool maquinaActiva;
};

struct datos_maquina datosLocales;
struct datos_maquina datosRemotos;

esp_now_peer_info_t peerInfo;
unsigned long inicioDesincronizacion = 0;
bool sistemaBloqueadoPorError = false;
bool nuevaInformacionRecibida = false;
bool estadoMaquina = false;  // arranca apagada

void OnDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
}

void OnDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  memcpy(&datosRemotos, incomingData, sizeof(datosRemotos));
  nuevaInformacionRecibida = true;
}

void leerBotones() {
  if (digitalRead(btn_apagar) == LOW)        estadoMaquina = false;
  else if (digitalRead(btn_encender) == LOW) estadoMaquina = true;
}

void setup() {
  Serial.begin(115200);

  pinMode(encendido, OUTPUT);
  pinMode(decrementar, OUTPUT);
  pinMode(incrementar, OUTPUT);
  pinMode(error, OUTPUT);

  pinMode(btn_encender, INPUT_PULLUP);
  pinMode(btn_apagar, INPUT_PULLUP);
  analogReadResolution(12);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error al inicializar ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  memcpy(peerInfo.peer_addr, macRemota, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Error al agregar el dispositivo contrario");
    return;
  }

  digitalWrite(encendido, LOW);
  digitalWrite(decrementar, LOW);
  digitalWrite(incrementar, LOW);
  digitalWrite(error, LOW);
}

void loop() {
  datosLocales.timestamp     = millis() / 1000;
  datosLocales.potencia      = analogRead(analog_in);
  datosLocales.maquinaActiva = estadoMaquina;

  esp_now_send(macRemota, (uint8_t *)&datosLocales, sizeof(datosLocales));

  if (nuevaInformacionRecibida) {

    digitalWrite(encendido, datosRemotos.maquinaActiva);
    int diferencia = (int)datosRemotos.potencia - (int)datosLocales.potencia;

    if (abs(diferencia) > MARGEN_P && datosLocales.maquinaActiva && datosRemotos.maquinaActiva) {
      if (inicioDesincronizacion == 0) {
        inicioDesincronizacion = millis();
      } else if (millis() - inicioDesincronizacion >= TIEMPO_N) {
        sistemaBloqueadoPorError = true;
      }
    } else {
      inicioDesincronizacion = 0;
    }

    if (sistemaBloqueadoPorError) {
      digitalWrite(error, HIGH);
      digitalWrite(decrementar, LOW);
      digitalWrite(incrementar, LOW);
    } else {
      digitalWrite(error, LOW);

      if (diferencia > MARGEN_P) {
        digitalWrite(incrementar, HIGH);
        digitalWrite(decrementar, LOW);
      }
      else if (diferencia < -MARGEN_P) {
        digitalWrite(incrementar, LOW);
        digitalWrite(decrementar, HIGH);
      }
      else {
        digitalWrite(incrementar, LOW);
        digitalWrite(decrementar, LOW);
      }
    }

    Serial.printf("[Local] Time: %us | Pot: %d | On: %s\n",
                  datosLocales.timestamp, datosLocales.potencia, datosLocales.maquinaActiva ? "SI" : "NO");
    Serial.printf("[Remota] Time: %us | Pot: %d | On: %s\n",
                  datosRemotos.timestamp, datosRemotos.potencia, datosRemotos.maquinaActiva ? "SI" : "NO");
    Serial.printf("Diferencia: %d | Estado Error: %s\n\n",
                  diferencia, sistemaBloqueadoPorError ? "BLOQUEADO" : "OK");
  }

  // Mensaje cada segundo, para saber si llegan datos aunque no pase nada
  static unsigned long t = 0;
  if (millis() - t > 1000) {
    t = millis();
    Serial.printf("PRUEBA | Mi maquina: %s | Recibiendo del otro: %s\n",
                  estadoMaquina ? "ENCENDIDA" : "APAGADA",
                  nuevaInformacionRecibida ? "SI" : "NO");
  }

  // Espera 100 ms, pero revisando los botones cada 5 ms
  for (int i = 0; i < 20; i++) {
    leerBotones();
    delay(5);
  }
}
