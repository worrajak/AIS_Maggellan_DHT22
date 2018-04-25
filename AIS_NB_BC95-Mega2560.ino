#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Average.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#include "AIS_NB_BC95.h"
//#include <DHT.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C #define BME280_ADDRESS                (0x76)

//#define DHTTYPE DHT22
//#define DHTPIN  2
//DHT dht(DHTPIN, DHTTYPE, 11); // 11 works fine for ESP8266

int measurePin = A0;
int ledPower = 3;

unsigned int samplingTime = 280;
unsigned int deltaTime = 40;
unsigned int sleepTime = 9680;

float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

float minute_data = 0.0;
float hour_data = 0.0;

unsigned long delayTime;

Average<float> ave(30);

String apnName = "devkit.nb";

String serverIP = "35.201.5.100"; // Your Server IP
String serverPort = "12345"; // Your Server Port

String udpData = "HelloWorld";

AIS_NB_BC95 AISnb;

const long interval = 20000;  //millisecond
unsigned long previousMillis = 0;

String ip1; 

void setup()
{ 
  bool status;
  pinMode(ledPower,OUTPUT);
     
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done
  // text display tests
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  Serial.begin(9600);
  display.println("Connecting to network");  
  display.println();
  display.display();
  delay(1000);

  AISnb.debug = true;
  AISnb.setupDevice(serverPort);
  ip1 = AISnb.getDeviceIP();  

  display.print("IP: ");
  display.println(ip1);
  display.display();
    
  delayTime = 10000;
  pingRESP pingR = AISnb.pingIP(serverIP);

  status = bme.begin();  
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }
  
  previousMillis = millis();
}

int i = 0;
  
void loop(){ 

  unsigned long currentMillis = millis();
    int minat = 0;
    int maxat = 0;
    
   digitalWrite(ledPower,LOW);
  delayMicroseconds(samplingTime);

  voMeasured = analogRead(measurePin);

  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH);
  delayMicroseconds(sleepTime);

  calcVoltage = voMeasured*(5.0/1024);
  dustDensity = 0.17*calcVoltage-0.1;

  if ( dustDensity < 0)
  {
    dustDensity = 0.00;
  }

   if(i<=30){
      ave.push(dustDensity);
      i++; 
   }else{
      i=0;
      Serial.println();
      Serial.print("Mean:   "); Serial.println(ave.mean());
      Serial.print("Mode:   "); Serial.println(ave.mode());
      Serial.print("Max:    "); Serial.println(ave.maximum(&maxat));
      Serial.print(" at:    "); Serial.println(maxat);
      Serial.print("Min:    "); Serial.println(ave.minimum(&minat));
      Serial.print(" at:    "); Serial.println(minat);
      Serial.println("StdDev: "); Serial.println(ave.stddev());   
   }

  delay(5000);

if (currentMillis - previousMillis >= interval){
 
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  float pressure = (bme.readPressure() / 100.0F);
  float alt = bme.readAltitude(SEALEVELPRESSURE_HPA);
  
  printValues();

 String DataSend ="{\"id\":\"NB-IoT-Home\",\"temperature\":"+String(temperature)+",\"humidity\":"+String(humidity)+",\"pressure\":"+String(pressure)+",\"alt\":"+String(alt)+",\"dustDensity\":"+String(dustDensity)+",\"dustm\":"+String(ave.mean())+"}";     

      UDPSend udp = AISnb.sendUDPmsgStr(serverIP, serverPort, DataSend);
   
      previousMillis = currentMillis;
    }
  UDPReceive resp = AISnb.waitResponse();     
}

void printValues() {
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");

    display.clearDisplay();
    display.setCursor(0,0);
    display.println("BME280 & Dust Sensor");
    display.setCursor(0,20);
    display.print("Temp.:  ");
    display.println(bme.readTemperature());
    display.print("Humid.: ");  
    display.println(bme.readHumidity());
    display.print("Pres.:  ");
    display.print(bme.readPressure() / 100.0F);
    display.println(" hPa");
    display.print("Alt:    ");
    display.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    display.println(" m");  
    //voMeasured
    //calcVoltage
    display.print("MC:     ");
    display.print(ave.mean()*1000);
    display.println(" ug/m3");
    display.display();
}


