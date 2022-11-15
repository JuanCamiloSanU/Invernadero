#include <WiFiClientSecure.h>
#include "ThingSpeak.h"
#include "EEPROM.h"

#include <WiFi.h>
#include "WiFiClientSecure.h"
#include "DHT.h"

#define DHTPIN 23
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


int addr = 0;
const int boton = 2;
#define EEPROM_SIZE 24
int valor_boton;

// user config:
const char* wifi_ssid = "Usergioarboleda";             // replace MySSID with your WiFi network name
const char* wifi_password = "ceskqyw2012";         // replace MyPassword with your WiFi password

unsigned long myChannelNumber = 1516035;
const char * myWriteAPIKey = "DB1LKNQFUYESYNCD";
const unsigned int writeInterval = 500; // write interval (in ms)


WiFiClient client;


void setup() {
  Serial.begin(115200);
  dht.begin();
  ThingSpeak.begin(client);  // Initialize ThingSpeak
 
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
 

  pinMode(boton, INPUT);
 
  Serial.println("start...");
  if (!EEPROM.begin(EEPROM_SIZE))
  {
    Serial.println("failed to initialise EEPROM"); delay(1000000);
  }
   
  Serial.println(" bytes read from Flash . Values are:");
  for (int i = 0; i < EEPROM_SIZE; i++)
  {
    Serial.print(byte(EEPROM.read(i))); Serial.print(" ");
  }
  Serial.println();
  Serial.println("writing random n. in memory");
}

void loop() {
         getTem();
  delay(2000);

}


void getTem()
{
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
  flash(t,h);

  thingspeak(t,h);
 
}


void flash(float temp, float hum){

  EEPROM.write(0, 't');
  EEPROM.commit();
  for (int address = 1; address < EEPROM_SIZE; address++)
    {
      EEPROM.write(address, temp);
      EEPROM.commit();
    }
  EEPROM.write(12, 'h');
  EEPROM.commit();
  for (int address_2 = 13; address_2 < EEPROM_SIZE; address_2++)
    {
      EEPROM.write(address_2, hum);
      EEPROM.commit();
    }
 
  valor_boton = digitalRead(boton);
  if (valor_boton ==HIGH)
  {
    Serial.println();
   
    Serial.print(EEPROM_SIZE);
    Serial.println(" bytes written on Flash . Values are:");
    for (int i = 0; i < EEPROM_SIZE; i++)
    {
      Serial.print(byte(EEPROM.read(i))); Serial.print(" ");
    }
    Serial.println(); Serial.println("----------------------------------");
  }

  delay(100);
}



void thingspeak(float temp, float hum){
   
  Serial.print("Enviado");
 

 
  ThingSpeak.setField(1, temp);
  ThingSpeak.setField(3, hum);

  // set the status
  //ThingSpeak.setStatus(myStatus);
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  //client.connected();
  delay(15000);
}
