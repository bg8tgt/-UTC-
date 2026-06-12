#include <Arduino.h>

#define ST7789_DRIVER
#define TFT_FREQUENCY 80000000
#define TFT_WIDTH 240
#define TFT_HEIGHT 320
#define TFT_MISO 12
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_CS 15
#define TFT_DC 2
#define TFT_RST -1
#define TFT_BL -1
#define TFT_BACKLIGHT_ON HIGH
#define TFT_RGB_ORDER TFT_RGB
#define USE_HSPI_PORT
#include "simple_wifi_manager.h"
#include "web_config.h"
#include "clock_controller.h"
#include <Preferences.h>
#include <WebServer.h>
#include "weather_manager.h"
#include <HTTPClient.h>

#include <TFT_eSPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

unsigned long lastDisplayTime = 0;
int prevSec = (-1);
TFT_eSPI tft = TFT_eSPI();

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 28800000);

void setup() {
  Serial.begin(115200);

  Serial.println("=== BG8TGT Clock ===");
  tft.init();
  tft.setRotation(1);
  ledcAttach(21, 5000, 8);
  ledcWrite(21, 204);
  lastTouchTime = millis();
  clockCtrl.begin();
  title_load();
  clockCtrl.setupWiFi();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected!");
    Serial.println((WiFi.localIP().toString()));
    timeClient.begin();
    timeClient.update();
    configTime(28800, 0, "pool.ntp.org");
    delay(1000);
    Serial.println("NTP synced UTC+8");
    lastDisplayTime = 0;
  }

}

void loop() {

  int _btn = clockCtrl.checkButtons();
  if (_btn == 5) { currentPage = 1; _forceWeatherRedraw = true; }
  if (_btn == 6) { currentPage = 0; _forceFullRedraw = true; }
  if (_btn == 1 || _btn == 2) { clockCtrl.handleButton(_btn); return; }
  unsigned long now = millis();
  if (now - lastDisplayTime < 200) return;
  lastDisplayTime = now;
  clockCtrl.checkIdle();
  clockCtrl.checkWiFiTimeout();
  if (currentPage == 1 && WiFi.isConnected() && weatherNeedsUpdate()) { weatherFetch(); _forceWeatherRedraw = true; }
  if (!WiFi.isConnected()) {
    if (wifiDisconnectTime == 0) wifiDisconnectTime = millis();
    return;
  }
  if (currentPage == 1) { drawWeatherPage(_forceWeatherRedraw); _forceWeatherRedraw = false; return; }
  timeClient.update();
  time_t _now = time(nullptr);
  struct tm* ti = localtime(&_now);
  int h = ti->tm_hour;
  int m = ti->tm_min;
  int s = ti->tm_sec;
  if (s == prevSec) return;
  prevSec = s;
  int yr = ti->tm_year + 1900;
  int mo = ti->tm_mon + 1;
  int d = ti->tm_mday;
  int wd = ti->tm_wday;
  static const unsigned char ch_zhou[] PROGMEM = {0x3F,0xFE,0x18,0xC6,0x18,0xC6,0x1B,0xF6,0x18,0xC6,0x18,0xC6,0x1F,0xFE,0x18,0x06,0x1B,0xF6,0x1B,0x36,0x1B,0x36,0x33,0xF6,0x30,0x06,0x60,0x1E,0x00,0x00,0x00,0x00};
  static const unsigned char ch_yi[] PROGMEM = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  static const unsigned char ch_er[] PROGMEM = {0x00,0x00,0x3F,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  static const unsigned char ch_san[] PROGMEM = {0x3F,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xFE,0x00,0x00,0x00,0x00,0x00,0x00};
  static const unsigned char ch_si[] PROGMEM = {0x3F,0xFE,0x33,0x66,0x33,0x66,0x33,0x66,0x33,0x66,0x33,0x66,0x33,0x66,0x33,0x66,0x36,0x3E,0x3C,0x06,0x30,0x06,0x3F,0xFE,0x30,0x06,0x00,0x00,0x00,0x00,0x00,0x00};
  static const unsigned char ch_wu[] PROGMEM = {0x3F,0xFC,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x3F,0xF8,0x06,0x18,0x06,0x18,0x06,0x18,0x0C,0x18,0x0C,0x18,0x0C,0x18,0x7F,0xFE,0x00,0x00,0x00,0x00,0x00,0x00};
  static const unsigned char ch_liu[] PROGMEM = {0x01,0x80,0x00,0xC0,0x00,0xC0,0x7F,0xFE,0x00,0x00,0x00,0x00,0x06,0x60,0x06,0x30,0x0C,0x18,0x0C,0x0C,0x18,0x0C,0x30,0x06,0x60,0x06,0x00,0x00,0x00,0x00,0x00,0x00};
  static const unsigned char ch_ri[] PROGMEM = {0x3F,0xFC,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x3F,0xFC,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x3F,0xFC,0x30,0x0C,0x00,0x00,0x00,0x00,0x00,0x00};
  static const unsigned char* const _wdChars[] = {ch_ri,ch_yi,ch_er,ch_san,ch_si,ch_wu,ch_liu};
  int uh = h - 8;
  if (uh < 0) uh += 24;
  static const byte _seg[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
  uint16_t wh=tft.color565(50,50,50), bk=TFT_WHITE;
  auto _ds=[&](int x,int y,int w,int h,int t,uint16_t c,byte s){tft.fillRect(x+t,y,w-2*t,t,(s&1)?c:bk);tft.fillRect(x+w-t,y+t,t,h/2-t,(s&2)?c:bk);tft.fillRect(x+w-t,y+h/2+t/2,t,h/2-t,(s&4)?c:bk);tft.fillRect(x+t,y+h-t,w-2*t,t,(s&8)?c:bk);tft.fillRect(x,y+h/2+t/2,t,h/2-t,(s&16)?c:bk);tft.fillRect(x,y+t,t,h/2-t,(s&32)?c:bk);tft.fillRect(x+t,y+h/2-t/2,w-2*t,t,(s&64)?c:bk);};
  auto _dd=[&](int x,int y,int d){_ds(x,y,30,60,6,bk,0x7F);_ds(x,y,30,60,6,wh,_seg[d]);};
  auto _dt=[&](int x,int y,int hh,int mm,int ss){int dx=x;_dd(dx,y,hh/10);dx+=35;_dd(dx,y,hh%10);dx+=35;tft.fillRect(dx+3,y+14,5,5,wh);tft.fillRect(dx+3,y+41,5,5,wh);dx+=12;_dd(dx,y,mm/10);dx+=35;_dd(dx,y,mm%10);dx+=35;tft.fillRect(dx+3,y+14,5,5,wh);tft.fillRect(dx+3,y+41,5,5,wh);dx+=12;_dd(dx,y,ss/10);dx+=35;_dd(dx,y,ss%10);};
  static int _prevDay=-1;
  static int _psh=-1,_psm=-1;
  if(_forceFullRedraw){_forceFullRedraw=false;_prevDay=-1;prevSec=-1;_psh=-1;_psm=-1;}
  if(d!=_prevDay){_prevDay=d;tft.fillScreen(TFT_WHITE);tft.setTextDatum(TC_DATUM);tft.setTextColor(TFT_CYAN);tft.setTextFont(4);tft.setTextSize(1);tft.drawString(clock_title,160,10);tft.setTextColor(TFT_YELLOW);tft.setTextFont(2);tft.setTextSize(2);tft.setTextDatum(TL_DATUM);char dateStr[20];sprintf(dateStr,"%04d-%02d-%02d",yr,mo,d);tft.drawString(dateStr,17,42);int bw=tft.textWidth(dateStr);tft.drawBitmap(37+bw-1,52,ch_zhou,16,16,tft.color565(255,255,0));tft.drawBitmap(37+bw+17,52,_wdChars[wd],16,16,tft.color565(255,255,0));tft.setTextFont(2);tft.setTextSize(2);tft.setTextColor(TFT_ORANGE);tft.drawString("BJT",12,80);tft.setTextColor(TFT_GREEN);tft.drawString("UTC",12,160);clockCtrl.drawBattery(clockCtrl.getBatteryLevel());}
  if(h!=_psh||m!=_psm){_psh=h;_psm=m;tft.fillRect(63,76,155,64,TFT_WHITE);tft.fillRect(63,156,155,64,TFT_WHITE);_dd(65,78,h/10);_dd(100,78,h%10);tft.fillRect(138,92,5,5,wh);tft.fillRect(138,119,5,5,wh);_dd(147,78,m/10);_dd(182,78,m%10);_dd(65,158,uh/10);_dd(100,158,uh%10);tft.fillRect(138,172,5,5,wh);tft.fillRect(138,199,5,5,wh);_dd(147,158,m/10);_dd(182,158,m%10);}
  tft.fillRect(215,76,85,64,TFT_WHITE);tft.fillRect(215,156,85,64,TFT_WHITE);tft.fillRect(220,92,5,5,wh);tft.fillRect(220,119,5,5,wh);_dd(229,78,s/10);_dd(264,78,s%10);tft.fillRect(220,172,5,5,wh);tft.fillRect(220,199,5,5,wh);_dd(229,158,s/10);_dd(264,158,s%10);
  Serial.printf("%04d-%02d-%02d BJT:%02d:%02d:%02d UTC:%02d:%02d:%02d\n", yr, mo, d, h, m, s, uh, m, s);

}