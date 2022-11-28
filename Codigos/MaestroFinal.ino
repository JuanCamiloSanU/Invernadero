

#include <esp_now.h>
#include <WiFi.h>

#include "ThingSpeak.h"
#include "DHT.h"


#define DHTPIN 23
#define sensor_piso 22 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);



const char* wifi_ssid = "Usergioarboleda";             // replace MySSID with your WiFi network name
const char* wifi_password = "ceskqyw2012";         // replace MyPassword with your WiFi password

unsigned long myChannelNumber = 1516035;
const char * myWriteAPIKey = "DB1LKNQFUYESYNCD";
const unsigned int writeInterval = 500; // write interval (in ms)


WiFiClient client;

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Structure example to send data
// Must match the receiver structure

typedef struct temp_humidity {
  float temperature;
  float humidity;
  float humedadP;
};
// Create a struct_message called myData
temp_humidity dhtData;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 
void setup() {

  
  // Init Serial Monitor
  Serial.begin(115200);
  Serial.println("-------INICIO DEL PROGRAMA----------");
  dht.begin();
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  
// Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {
  
  float sensorValue = analogRead(sensor_piso);
  Serial.print("SENSOR PISO HP: ");
  Serial.println(sensorValue);
  //
  //delay(30000);
  
  float h = dht.readHumidity();
  float t = dht.readTemperature();
   
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  Serial.print("Tempe: ");
  Serial.println(t);
  Serial.print("Hum: ");
  Serial.println(h);;
  
  //int humedad_Piso = analogRead(SensorPin);
  //Serial.println(humedad);
  
//---------------------------------------------

//-------- ENVIO THINGSPEAK Y ESP - NOW--------
  thingspeak(t,h);

  dhtData.temperature = t;
  dhtData.humidity = h;

  //dhtData.humedadP = humedad_Piso;

  thingspeak(dhtData.temperature, dhtData.humidity);
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &dhtData, sizeof(dhtData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(2000);
}






void thingspeak(float temp, float hum){
   
  Serial.print("Enviado");
 
  ThingSpeak.setField(1, temp);
  ThingSpeak.setField(3, hum);

  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  //client.connected();
  delay(15000);
}
