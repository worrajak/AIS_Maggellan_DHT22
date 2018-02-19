#include <Magellan.h>
Magellan magel;
#include <DHT.h>

#define DHTTYPE DHT22
#define DHTPIN  2
DHT dht(DHTPIN, DHTTYPE, 11); // 11 works fine for ESP8266

char auth[]="0ef72bd0-1540-11e8-969b-933c93ed7f99"; 		//Token Key you can get from magellan platform

String payload;

int Log_flag=0;
long lastMsg = 0;
float temp = 0.0;
float hum = 0.0;
float diff = 0.5;

bool checkBound(float newValue, float prevValue, float maxDiff) {
  return !isnan(newValue) &&
         (newValue < prevValue - maxDiff || newValue > prevValue + maxDiff);
}

void setup() {
  Serial.begin(9600);
  magel.begin(auth);           //init Magellan LIB
}

String Temperature;
String Humidity; 

void loop() {

    float newTemp = dht.readTemperature();
    float newHum = dht.readHumidity();

    if (checkBound(newTemp, temp, diff)) {
      temp = newTemp;
      Serial.print("New temperature:");
      Serial.println(String(temp).c_str());
      Temperature=String(temp);
    }

    if (checkBound(newHum, hum, diff)) {
      hum = newHum;
      Serial.print("New humidity:");
      Serial.println(String(hum).c_str());
      Humidity=String(hum);
    }

  payload="{\"Temperature\":"+Temperature+",\"Humidity\":"+Humidity+"}";       //please provide payload with json format

  magel.post(payload);                            							   //post payload data to Magellan IoT platform
  delay(30000);
}
