#include <DHT.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266_Seniverse.h>
#include <PubSubClient.h>
#include <Ticker.h>
#include "FACE_Angry.h"

const char* mqttServer = "MQTTServer";
const char* mqttUserName = "MQTTSecretKey";
const char* mqttPassword = NULL;
const char* WiFiSSID = "12345";
const char* WiFiPswd = "12345";

DHT dht11(14, 11);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

float tempread;
float humiread;
char dispflag;
char sendflag;
int count;

WeatherNow weatherNow;
Ticker ticker;
Forecast forecast;
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void page1();
void connectMQTTServer();
void tickerisp();
void dht11read();
void pubMQTTmsg();

void setup()
{
  Serial.begin(9600);

  u8g2.enableUTF8Print();
  u8g2.setI2CAddress(0x3C * 2);
  u8g2.begin();
  dht11.begin();
  ticker.attach(1,tickerisp);

  WiFi.begin(WiFiSSID, WiFiPswd);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Local IP:");
  Serial.print(WiFi.localIP());
  
  weatherNow.config("XZWeatherKey", "ip", "c", "zh-Hans");
  forecast.config("XZWeatherKey", "ip", "c", "zh-Hans");

  mqttClient.setServer(mqttServer,1883);
  connectMQTTServer();

  dispflag = 0;

  u8g2.firstPage();
  do
  {
    u8g2.clearDisplay();
  } while (u8g2.nextPage());
  u8g2.firstPage();
  do
  {
    u8g2.clearDisplay();
    u8g2.drawXBMP(20, 0, 89, 64, FACE_Angry);
  } while (u8g2.nextPage());

  //delay(2000);
}

void loop()
{
  if(!mqttClient.connected())
  {
    connectMQTTServer();
  }
  mqttClient.loop();

  delay(500);
  while(!weatherNow.update())
  delay(500);
  while(!forecast.update());
  if(count>=5)
  {
    dht11read();
    pubMQTTmsg();
    pubMQTTmsg();
    count=0;
  }
  u8g2.firstPage();
  do
  {
    page1();
  } while (u8g2.nextPage());

  delay(10000);
  dispflag = (dispflag + 1) % 2;
}

void page1()
{
  switch (dispflag)
  {
  case 0:
    u8g2.setFont(u8g2_font_wqy16_t_gb2312);
    u8g2.setFontPosTop();
    u8g2.setCursor(0, 4);
    u8g2.print(String(String("实时天气:") + String(weatherNow.getWeatherText())) + String(String(weatherNow.getDegree()) + String("°C")));
    u8g2.setCursor(0, 36);
    u8g2.print(String(String("全天温度:") + String(forecast.getLow(0))) + String("~") + String(forecast.getHigh(0))+ String("°C"));
    break;
  case 1:
    //temp = tempread;
    //humi = humiread;
    u8g2.setFont(u8g2_font_wqy16_t_gb2312);
    u8g2.setFontPosTop();
    u8g2.setCursor(0, 4);
    u8g2.print(String(String("室内温湿度:")));
    u8g2.setCursor(0, 36);
    u8g2.print(String(String(tempread) + String("°C")) + String(String(" ") + String(humiread) + String("%")));
    break;
  }
}

void connectMQTTServer() 
{
  // 根据ESP8266的MAC地址生成客户端ID（避免与其它ESP8266的客户端ID重名）
  String clientId = "esp8266-" + WiFi.macAddress();

  // 连接MQTT服务器。此处使用了程序首部定义的用户名和密码来实现MQTT服务端认证
  if (mqttClient.connect(clientId.c_str(), mqttUserName, mqttPassword)) {
    Serial.println("MQTT Server Connected.");
    Serial.print("Server Address: ");
    Serial.println(mqttServer);
    Serial.print("ClientId: ");
    Serial.println(clientId);
  } else {
    Serial.print("MQTT Server Connect Failed. Client State:");
    Serial.println(mqttClient.state());
    delay(3000);
  }
}

void dht11read()
{
  delay(10);
  float h = dht11.readHumidity();
  delay(10);
  float t = dht11.readTemperature();
  if (isnan(h) || isnan(t))
  {
    Serial.println("Failed to read from DHT sensor!");
  }
  else
  {
    humiread = h;
    tempread = t;
  }
}

void pubMQTTmsg() {
  //static int value;


  // 建立发布主题。主题名称以taichi/Pub-为前缀，后面添加设备的MAC地址。
  // 这么做是为确保不同用户进行MQTT信息发布时，ESP8266客户端名称各不相同，
  String topicString = "v1/devices/me/telemetry";
  char publishTopic[topicString.length() + 1];
  strcpy(publishTopic, topicString.c_str());

  // 建立发布信息。信息内容以Hello World为起始，后面添加发布次数。
  String messageString;
  if(sendflag==0)
  {
    messageString = "{\"temp\":" + String(tempread) + "}";
  }

  else
  {
    messageString = "{\"humi\":" + String(humiread) + "}";
  }

    char publishMsg[messageString.length() + 1];
    strcpy(publishMsg, messageString.c_str());
  
  // 实现ESP8266向主题发布信息
  if (mqttClient.publish(publishTopic, publishMsg)) 
  {
    Serial.println("Publish Topic:"); Serial.println(publishTopic);
    if(sendflag==0)
    {
      Serial.println("Temperature:"); Serial.println(publishMsg);
    }
    else
    {
      Serial.println("Humidity:"); Serial.println(publishMsg);
    }
  } 
  else 
  {
    Serial.println("Message Publish Failed.");
  }
  sendflag=(sendflag+1)%2;
  delay(500);
}

void tickerisp()
{
    count++;
}