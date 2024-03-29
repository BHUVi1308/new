#include <Wire.h>
#include <MPU6050.h>
#include <SoftwareSerial.h>
#include <math.h>
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"

MPU6050 mpu;

int stepCount = 0;       // step count variable
float distance = 0;      // distance traveled variable
bool isWalking = false;  // flag for walking status
int threshold = 35;

uint8_t gateway_address=1;


const char* ssid = "Random"; // replace with your WiFi network name (SSID)
const char* password = "12345678"; // replace with your WiFi network password
const char* server_ip = "192.168.137.175"; // replace with your PC's IP address
int server_port = 1234;

void setup() {
  Serial.begin(115200);

  Serial.println("Initialize MPU6050");

  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }

  // If you want, you can set accelerometer offsets
  // mpu.setAccelOffsetX();
  // mpu.setAccelOffsetY();
  // mpu.setAccelOffsetZ();



  checkSettings();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
}

void checkSettings()
{
  Serial.println();
  
  Serial.print(" * Sleep Mode:            ");
  Serial.println(mpu.getSleepEnabled() ? "Enabled" : "Disabled");
  
  Serial.print(" * Clock Source:          ");
  switch(mpu.getClockSource())
  {
    case MPU6050_CLOCK_KEEP_RESET:     Serial.println("Stops the clock and keeps the timing generator in reset"); break;
    case MPU6050_CLOCK_EXTERNAL_19MHZ: Serial.println("PLL with external 19.2MHz reference"); break;
    case MPU6050_CLOCK_EXTERNAL_32KHZ: Serial.println("PLL with external 32.768kHz reference"); break;
    case MPU6050_CLOCK_PLL_ZGYRO:      Serial.println("PLL with Z axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_YGYRO:      Serial.println("PLL with Y axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_XGYRO:      Serial.println("PLL with X axis gyroscope reference"); break;
    case MPU6050_CLOCK_INTERNAL_8MHZ:  Serial.println("Internal 8MHz oscillator"); break;
  }
  
  Serial.print(" * Accelerometer:         ");
  switch(mpu.getRange())
  {
    case MPU6050_RANGE_16G:            Serial.println("+/- 16 g"); break;
    case MPU6050_RANGE_8G:             Serial.println("+/- 8 g"); break;
    case MPU6050_RANGE_4G:             Serial.println("+/- 4 g"); break;
    case MPU6050_RANGE_2G:             Serial.println("+/- 2 g"); break;
  }  

  Serial.print(" * Accelerometer offsets: ");
  Serial.print(mpu.getAccelOffsetX());
  Serial.print(" / ");
  Serial.print(mpu.getAccelOffsetY());
  Serial.print(" / ");
  Serial.println(mpu.getAccelOffsetZ());
  
  Serial.println();
}



void loop() {
  WiFiClient client;
 
    Vector normAccel_i = mpu.readNormalizeAccel();

    float x_val = normAccel_i.XAxis;
    float y_val = normAccel_i.YAxis;
    float z_val = normAccel_i.ZAxis;

    unsigned long time = millis();

  while(time<240000000)
  {    
 
    // magnitude of acceleration vector
    int n = 0;


    while(n<10)
    {
 
      Vector normAccel = mpu.readNormalizeAccel();
      float x_val_i = normAccel.XAxis;
      float y_val_i = normAccel.YAxis;
      float z_val_i = normAccel.ZAxis;


      float accelMag = pow(pow(x_val_i - x_val, 2) + pow(y_val_i - y_val, 2) + pow(z_val_i - z_val, 2),0.5); 
      Serial.println(accelMag);
 
      if (accelMag > threshold) {
      stepCount++;
      
      }
      n = n +1;

      if (accelMag >30 ){
        delay(750*30/accelMag);
      }
      else{
        delay(750);
      }
      
      normAccel_i = normAccel;

      x_val = normAccel_i.XAxis;
      y_val = normAccel_i.YAxis;
      z_val = normAccel_i.ZAxis;
    
    }

    Serial.print("step Count: ");
    Serial.println(stepCount);

    if (client.connect(server_ip, server_port)) {
      Serial.println("Connected to server");

      String a = String(stepCount);
      client.println(a);

      client.stop();
      Serial.println("Connection closed");
    } else {
      Serial.println("Connection failed");
    }
  }

}
