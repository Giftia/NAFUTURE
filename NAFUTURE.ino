/*
  Giftina：https://giftia.moe

  使用说明：
  1.使用M-TTL烧录器或第三方TTL连接光和未来温湿度传感计(以下简称设备)；
  2.按住M-TTL烧录器上的FLASH按钮 并打开设备开关，应能观察到设备蓝色指示灯微光常亮，进入烧录模式；
  3.使用安装了对应库的 Arduino IDE 打开本工程，修改25、26行的 ssid 和 password 为自己的WiFi；
  4.点击菜单-工具中的 ESP8266 Sketch Data Upload 刷入 SPIFFS，等待完毕后重复第二步；
  5.点击上传，等待完毕后重启设备开关；
  6.打开电脑或手机的WiFi，连接一个名字类似 ESP_1A2B3C 的WiFi；
  7.使用浏览器访问 192.168.4.1，查看页面上显示的 本地ip地址，接着切换到自己的WiFi；
  8.访问上面显示的 本地ip地址 即可在局域网查看数据页。进入睡眠状态后，设备将会在1小时后醒来10分钟。

  推销M-TTL烧录器：https://market.m.taobao.com/app/idleFish-F2e/widle-taobao-rax/page-detail?wh_weex=true&wx_navbar_transparent=true&id=626032002165&ut_sk=1.XrybCpHeoWADAMQYe2jVnfL5_21407387_1599787921395.Copy.detail.626032002165.1783941160&forceFlush=1&qq-pf-to=pcqq.c2c

  推销开源硬件：https://oshwhub.com/Giftina/guang-ge-wei-lai-shao-lu-zhuai-j
*/
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <DHT.h>
#include <Ticker.h>

const char *ssid = "";
const char *password = "";

Ticker Wait10mTicket;

#define DHTPIN 4      //DHT11位于GPIO4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

float t = 0.0;               //温度
float h = 0.0;               //湿度
float b = 0.0;               //电量
float SleepTime = 60 * 60e6; //睡眠时间，默认1小时

IPAddress temp;
String ip;

unsigned long previousMillis = 0; //前一次温湿度改变时间

const long interval = 3000; //每隔几秒检测一次

const int ledPin = 12; //指示灯位于GPIO12
String ledState;

AsyncWebServer server(80);

String processor(const String &var)
{
  if (var == "LOCALIP")
  {
    return ip;
  }
  else if (var == "STATE")
  {
    if (digitalRead(ledPin))
    {
      ledState = "常亮";
    }
    else
    {
      ledState = "常灭";
    }
    return ledState;
  }
  else if (var == "TEMPERATURE")
  {
    return String(t);
  }
  else if (var == "HUMIDITY")
  {
    return String(h);
  }
  else if (var == "BATTERY")
  {
    return String(b);
  }
  return String();
}

void setup()
{
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  dht.begin();

  //初始化SPIFFS
  if (!SPIFFS.begin())
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  //连接WiFi
  WiFi.begin(ssid, password);
  Serial.println("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }

  Serial.println(WiFi.localIP()); //串口打印访问地址

  temp = WiFi.localIP();
  int buf[3];
  for (int i = 0; i < 4; i++)
  {
    buf[i] = temp[i];
  }
  ip += String(buf[0]);
  ip += '.';
  ip += String(buf[1]);
  ip += '.';
  ip += String(buf[2]);
  ip += '.';
  ip += String(buf[3]);

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to set GPIO to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(ledPin, HIGH);
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(ledPin, LOW);
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", String(t).c_str());
  });

  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", String(h).c_str());
  });

  server.on("/battery", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", String(b).c_str());
  });

  server.on("/sleep", HTTP_GET, [](AsyncWebServerRequest * request) {
    sleep;
  });

  server.begin();

  Wait10mTicket.attach(10 * 60, sleep);//闲置10分钟自动睡眠
}

void sleep() { //睡眠
  ESP.deepSleep(SleepTime);
}

void loop()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    b = analogRead(A0);
    b = (b / 1024) * 100;
    // save the last time you updated the DHT values
    previousMillis = currentMillis;
    // Read temperature as Celsius (the default)
    float newT = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //float newT = dht.readTemperature(true);
    // if temperature read failed, don't change t value
    if (isnan(newT))
    {
      Serial.println("Failed to read from DHT sensor!");
    }
    else
    {
      t = newT;
      //Serial.println(t);
    }
    // Read Humidity
    float newH = dht.readHumidity();
    // if humidity read failed, don't change h value
    if (isnan(newH))
    {
      Serial.println("Failed to read from DHT sensor!");
    }
    else
    {
      h = newH;
      //Serial.println(h);
    }
  }
}
