#pragma once
#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

#define WEATHER_API_KEY "226bf59672f9c3d8702635a901d68ed8"
#define WEATHER_CITY "532625"
#define WEATHER_UPDATE_INTERVAL 1800000  // 30 minutes

struct WeatherData {
  char province[16];
  char city[16];
  char weather[16];
  char temperature[8];
  char winddirection[8];
  char windpower[4];
  char humidity[8];
  char reporttime[24];
  bool valid;
};

struct ForecastDay {
  char date[12];
  char dayweather[16];
  char nightweather[16];
  char daytemp[8];
  char nighttemp[8];
  char daywind[8];
  char daypower[4];
};

struct FullWeather {
  WeatherData current;
  ForecastDay forecast[4];
  int forecastCount;
  bool valid;
};

static FullWeather _weather;
static unsigned long _lastWeatherFetch = 0;
static bool _weatherFetching = false;

// Weather icons 24x24 (outline-style: sun/cloud/overcast/rain/snow/fog)
static const unsigned char w_qing[] PROGMEM = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x04,0x08,0x10,0x02,0x00,0x20,0x01,0x3E,0x40,0x00,0x41,0x00,0x00,0x80,0x80,0x01,0x00,0x40,0x01,0x00,0x40,0x3D,0x00,0x5E,0x01,0x00,0x40,0x01,0x00,0x40,0x00,0x80,0x80,0x00,0x41,0x00,0x01,0x3E,0x40,0x02,0x00,0x20,0x04,0x08,0x10,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x00,0x00};
static const unsigned char w_yun[] PROGMEM = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0E,0x00,0x00,0x31,0x80,0x00,0x40,0x40,0x01,0x80,0x20,0x06,0x00,0x10,0x08,0x00,0x08,0x08,0x00,0x08,0x10,0x00,0x08,0x10,0x00,0x08,0x10,0x00,0x08,0x1F,0xFF,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static const unsigned char w_yin[] PROGMEM = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0E,0x00,0x00,0x31,0x80,0x00,0xC0,0x40,0x01,0x00,0x30,0x02,0x70,0x08,0x03,0x8C,0x04,0x0E,0x02,0x04,0x13,0xFF,0xFE,0x10,0x00,0x80,0x20,0x00,0x40,0x20,0x00,0x40,0x20,0x00,0x20,0x3F,0xFF,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static const unsigned char w_yu[] PROGMEM = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7E,0x00,0x01,0x81,0x80,0x02,0x00,0x40,0x04,0x00,0x20,0x04,0x00,0x10,0x04,0x00,0x10,0x07,0xFF,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x08,0x40,0x01,0x08,0x40,0x02,0x10,0x80,0x02,0x10,0x80,0x02,0x52,0x80,0x04,0x63,0x00,0x04,0xA5,0x00,0x00,0x84,0x00,0x00,0x84,0x00,0x01,0x08,0x00,0x01,0x08,0x00,0x00,0x00,0x00};
static const unsigned char w_xue[] PROGMEM = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7E,0x00,0x01,0x81,0x80,0x02,0x00,0x40,0x04,0x00,0x20,0x04,0x00,0x10,0x04,0x00,0x10,0x07,0xFF,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x08,0x10,0x04,0x08,0x10,0x04,0x08,0x10,0x1F,0x3E,0x7C,0x04,0x08,0x10,0x04,0x88,0x90,0x04,0x88,0x90,0x03,0xE3,0xE0,0x00,0x80,0x80,0x00,0x80,0x80,0x00,0x00,0x00};
static const unsigned char w_wu[] PROGMEM = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xFF,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFF,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xFF,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFF,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};


static const unsigned char ch_fc_ri[] PROGMEM = {0x3F,0xFC,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x3F,0xFC,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x3F,0xFC,0x30,0x0C,0x00,0x00,0x00,0x00,0x00,0x00};

// Arrow icon for page indicator
static const unsigned char arrow_l[] PROGMEM = {
  0x00,0x00,0x00,0x00,0x20,0x00,0x30,0x00,0x38,0x00,0x3C,0x00,
  0x38,0x00,0x30,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
static const unsigned char arrow_r[] PROGMEM = {
  0x00,0x00,0x00,0x00,0x08,0x00,0x18,0x00,0x38,0x00,0x78,0x00,
  0x38,0x00,0x18,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

// Chinese 16x16 bitmaps for labels (SimSun Bold)
static const unsigned char ch_wind0[] PROGMEM = {0x1F,0xFC,0x18,0x0C,0x18,0x3C,0x1E,0x3C,0x1B,0x6C,0x19,0xEC,0x18,0xCC,0x18,0xCC,0x19,0xEC,0x19,0xEC,0x1B,0x3F,0x1E,0x3F,0x18,0x07,0x30,0x03,0x00,0x00,0x00,0x00};
static const unsigned char ch_wind1[] PROGMEM = {0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x3F,0xFE,0x01,0x86,0x03,0x06,0x03,0x06,0x03,0x06,0x06,0x06,0x06,0x06,0x0C,0x06,0x18,0x0C,0x30,0x38,0x00,0x00,0x00,0x00};
static const unsigned char ch_hum0[] PROGMEM = {0x00,0x00,0x1B,0xFE,0x0F,0x06,0x03,0x06,0x33,0xFE,0x1B,0x06,0x03,0x06,0x0F,0xFE,0x0C,0xD8,0x1E,0xDB,0x1B,0xDE,0x31,0xDC,0x30,0xD8,0x07,0xFF,0x00,0x00,0x00,0x00};
static const unsigned char ch_hum1[] PROGMEM = {0x00,0xC0,0x00,0x60,0x0F,0xFF,0x0D,0x8C,0x0F,0xFF,0x0D,0x8C,0x0D,0xFC,0x0C,0x00,0x0F,0xFE,0x0D,0x8C,0x0C,0xD8,0x18,0x70,0x19,0xDC,0x37,0x07,0x00,0x00,0x00,0x00};
static const unsigned char ch_upd0[] PROGMEM = {0x00,0x00,0x3F,0xFF,0x00,0xC0,0x1F,0xFE,0x18,0xC6,0x1F,0xFE,0x18,0xC6,0x18,0xC6,0x1F,0xFE,0x0C,0xC0,0x07,0x80,0x03,0x80,0x0E,0xF0,0x38,0x1F,0x00,0x00,0x00,0x00};
static const unsigned char ch_upd1[] PROGMEM = {0x0C,0x07,0x06,0x3C,0x3F,0xF0,0x19,0xB0,0x0F,0x30,0x3F,0xFF,0x00,0x36,0x06,0x36,0x3F,0xF6,0x06,0x36,0x1F,0xB6,0x36,0xF6,0x06,0x66,0x1E,0xC6,0x00,0x00,0x00,0x00};
static const unsigned char ch_yubao0[] PROGMEM = {0x00,0x00,0x3F,0xFF,0x03,0x18,0x1E,0x30,0x0C,0x7E,0x3F,0x66,0x0F,0x7E,0x0C,0x7E,0x0C,0x7E,0x0C,0x7E,0x0C,0x7E,0x0C,0x3C,0x0C,0x66,0x3C,0xC6,0x00,0x00,0x00,0x00};
static const unsigned char ch_yubao1[] PROGMEM = {0x0C,0x00,0x0C,0xFE,0x0C,0xC6,0x3F,0xC6,0x0C,0xDE,0x0C,0xC0,0x0F,0xFE,0x0E,0xF6,0x3C,0xF6,0x0C,0xFC,0x0C,0xD8,0x0C,0xFC,0x0C,0xE6,0x3C,0xC3,0x00,0x00,0x00,0x00};
// 当前 (current)
static const unsigned char ch_dq0[] PROGMEM = {0x00,0xC0,0x18,0xC6,0x0C,0xCC,0x06,0xD8,0x3F,0xFE,0x00,0x06,0x00,0x06,0x00,0x06,0x1F,0xFE,0x00,0x06,0x00,0x06,0x00,0x06,0x3F,0xFE,0x00,0x06,0x00,0x00,0x00,0x00};
static const unsigned char ch_dq1[] PROGMEM = {0x0C,0x0C,0x06,0x18,0x3F,0xFF,0x00,0x00,0x1F,0xC6,0x18,0xF6,0x18,0xF6,0x1F,0xF6,0x18,0xF6,0x18,0xF6,0x1F,0xF6,0x18,0xC6,0x18,0xC6,0x1B,0xDE,0x00,0x00,0x00,0x00};

// Simple JSON field extractor
static String jsonField(const String& json, const char* key, int fromIdx = 0) {
  String search = "\"";
  search += key;
  search += "\":\"";
  int idx = json.indexOf(search, fromIdx);
  if (idx < 0) return "";
  idx += search.length();
  int end = json.indexOf("\"", idx);
  if (end < 0) return "";
  return json.substring(idx, end);
}

static int jsonFieldInt(const String& json, const char* key, int fromIdx = 0) {
  String search = "\"";
  search += key;
  search += "\":";
  int idx = json.indexOf(search, fromIdx);
  if (idx < 0) return -1;
  idx += search.length();
  // skip whitespace
  while (idx < (int)json.length() && (json[idx] == ' ' || json[idx] == '\n' || json[idx] == '\r')) idx++;
  int end = idx;
  while (end < (int)json.length() && json[end] >= '0' && json[end] <= '9') end++;
  if (end == idx) return -1;
  return json.substring(idx, end).toInt();
}

static String jsonFieldRaw(const String& json, const char* key, int fromIdx = 0) {
  String search = "\"";
  search += key;
  search += "\":";
  int idx = json.indexOf(search, fromIdx);
  if (idx < 0) return "";
  idx += search.length();
  while (idx < (int)json.length() && (json[idx] == ' ' || json[idx] == '\n' || json[idx] == '\r')) idx++;
  if (json[idx] == '"') {
    idx++;
    int end = json.indexOf("\"", idx);
    return json.substring(idx, end);
  }
  // number or other
  int end = idx;
  while (end < (int)json.length() && json[end] != ',' && json[end] != '}' && json[end] != ']') end++;
  return json.substring(idx, end);
}

void weatherFetch() {
  if (!WiFi.isConnected()) return;
  if (_weatherFetching) return;
  _weatherFetching = true;

  HTTPClient http;
  String url = "https://restapi.amap.com/v3/weather/weatherInfo?city=" 
               + String(WEATHER_CITY) + "&key=" + String(WEATHER_API_KEY) + "&extensions=all&output=JSON";
  
  Serial.println("Fetching weather...");
  http.begin(url);
  int code = http.GET();
  
  if (code == 200) {
    String resp = http.getString();
    Serial.println("Weather response received (" + String(resp.length()) + " bytes)");
    
    // Check status
    String status = jsonField(resp, "status");
    if (status == "1") {
      // Parse forecast array - find first "casts":[
      int castsIdx = resp.indexOf("\"casts\":[");
      if (castsIdx > 0) {
        // Parse city info from forecast level
        String fcCity = jsonField(resp, "city");
        String fcProvince = jsonField(resp, "province");
        String fcReport = jsonField(resp, "reporttime");
        
        strncpy(_weather.current.city, fcCity.c_str(), 15);
        strncpy(_weather.current.province, fcProvince.c_str(), 15);
        strncpy(_weather.current.reporttime, fcReport.c_str(), 23);
        
        // Parse each cast entry
        int searchFrom = castsIdx + 9;
        _weather.forecastCount = 0;
        
        for (int i = 0; i < 4 && searchFrom > 0; i++) {
          int objStart = resp.indexOf("{", searchFrom);
          if (objStart < 0) break;
          int objEnd = resp.indexOf("}", objStart);
          if (objEnd < 0) break;
          
          String obj = resp.substring(objStart, objEnd + 1);
          
          String date = jsonFieldRaw(obj, "date");
          String dayw = jsonFieldRaw(obj, "dayweather");
          String nightw = jsonFieldRaw(obj, "nightweather");
          String dayt = jsonFieldRaw(obj, "daytemp");
          String nightt = jsonFieldRaw(obj, "nighttemp");
          String daywind = jsonFieldRaw(obj, "daywind");
          String daypower = jsonFieldRaw(obj, "daypower");
          
          strncpy(_weather.forecast[i].date, date.c_str(), 11);
          strncpy(_weather.forecast[i].dayweather, dayw.c_str(), 15);
          strncpy(_weather.forecast[i].nightweather, nightw.c_str(), 15);
          strncpy(_weather.forecast[i].daytemp, dayt.c_str(), 7);
          strncpy(_weather.forecast[i].nighttemp, nightt.c_str(), 7);
          strncpy(_weather.forecast[i].daywind, daywind.c_str(), 7);
          strncpy(_weather.forecast[i].daypower, daypower.c_str(), 3);
          
          // First cast = today's data, use as current
          if (i == 0) {
            strncpy(_weather.current.weather, dayw.c_str(), 15);
            strncpy(_weather.current.temperature, dayt.c_str(), 7);
            strncpy(_weather.current.winddirection, daywind.c_str(), 7);
            strncpy(_weather.current.windpower, daypower.c_str(), 3);
            strncpy(_weather.current.humidity, "-", 7);
            _weather.current.valid = true;
          }
          
          _weather.forecastCount++;
          searchFrom = objEnd + 1;
        }
        
        _weather.valid = true;
        Serial.println("Weather parsed OK: " + String(_weather.current.weather) + " " + String(_weather.current.temperature) + "C");
      }
      
      // Also fetch live weather for humidity
      String url2 = "https://restapi.amap.com/v3/weather/weatherInfo?city=" 
                     + String(WEATHER_CITY) + "&key=" + String(WEATHER_API_KEY) + "&extensions=base&output=JSON";
      http.begin(url2);
      int code2 = http.GET();
      if (code2 == 200) {
        String resp2 = http.getString();
        String hum = jsonField(resp2, "humidity");
        String temp = jsonField(resp2, "temperature");
        if (hum.length() > 0) {
          strncpy(_weather.current.humidity, hum.c_str(), 7);
        }
        if (temp.length() > 0 && !_weather.current.valid) {
          strncpy(_weather.current.temperature, temp.c_str(), 7);
          strncpy(_weather.current.weather, jsonField(resp2, "weather").c_str(), 15);
          strncpy(_weather.current.winddirection, jsonField(resp2, "winddirection").c_str(), 7);
          strncpy(_weather.current.windpower, jsonField(resp2, "windpower").c_str(), 3);
          _weather.current.valid = true;
          _weather.valid = true;
        }
        Serial.println("Live: " + temp + "C humidity=" + hum + "%");
      }
    } else {
      Serial.println("Weather API error: status=" + status);
    }
  } else {
    Serial.println("Weather HTTP error: " + String(code));
  }
  
  http.end();
  _lastWeatherFetch = millis();
  _weatherFetching = false;
}

bool weatherNeedsUpdate() {
  if (!_weather.valid) return true;
  if (millis() - _lastWeatherFetch > WEATHER_UPDATE_INTERVAL) return true;
  return false;
}

// Get weather icon based on weather text
const unsigned char* weatherIcon(const char* w) {
  String ws = String(w);
  ws.toLowerCase();
  if (ws.indexOf("晴") >= 0) return w_qing;
  if (ws.indexOf("云") >= 0) return w_yun;
  if (ws.indexOf("阴") >= 0) return w_yin;
  if (ws.indexOf("雨") >= 0) return w_yu;
  if (ws.indexOf("雪") >= 0) return w_xue;
  if (ws.indexOf("雾") >= 0 || ws.indexOf("霾") >= 0) return w_wu;
  return w_qing;
}

// Get color for weather
uint16_t weatherColor(const char* w) {
  String ws = String(w);
  if (ws.indexOf("晴") >= 0) return tft.color565(230, 140, 0);
  if (ws.indexOf("云") >= 0 || ws.indexOf("阴") >= 0) return tft.color565(80, 100, 120);
  if (ws.indexOf("雨") >= 0) return tft.color565(40, 90, 200);
  if (ws.indexOf("雪") >= 0) return tft.color565(60, 130, 210);
  return tft.color565(230, 140, 0);
}

void drawWeatherPage(bool forceRedraw) {
  static bool prevValid = false;
  
  if (forceRedraw || prevValid != _weather.valid) {
    prevValid = _weather.valid;
    tft.fillScreen(TFT_WHITE);
    
    // Top bar - gradient blue (height 36)
    for (int i = 0; i < 36; i++) {
      uint16_t c = tft.color565(30 + i, 100 + i*2, 200 + i);
      tft.drawFastHLine(0, i, 320, c);
    }
    
    tft.setTextColor(TFT_WHITE);
    tft.setTextDatum(TC_DATUM);
    tft.setTextFont(2);
    tft.setTextSize(1);
    
    // Page indicator dots
    tft.fillCircle(148, 7, 4, tft.color565(100,100,200));
    tft.fillCircle(160, 7, 4, TFT_WHITE);
    tft.fillCircle(172, 7, 4, tft.color565(100,100,200));
    
    if (!_weather.valid || !_weather.current.valid) {
      tft.drawString("Loading...", 160, 22);
      tft.setTextColor(TFT_BLACK);
      tft.setTextFont(2);
      tft.drawString("Pulling weather...", 160, 100);
      return;
    }
    
    // --- Current weather section (y: 40 ~ 120) ---
    
    // 当前 label
    tft.drawBitmap(8, 42, ch_dq0, 16, 16, tft.color565(70,130,180));
    tft.drawBitmap(24, 42, ch_dq1, 16, 16, tft.color565(70,130,180));
    
    // Weather icon (graphical)
    tft.drawBitmap(48, 42, weatherIcon(_weather.current.weather), 24, 24, weatherColor(_weather.current.weather));
    
    // Temperature - large font
    tft.setTextColor(TFT_BLACK);
    tft.setTextFont(4);
    tft.setTextSize(1);
    tft.setTextDatum(TL_DATUM);
    String tempStr = String(_weather.current.temperature) + "C";
    tft.drawString(tempStr, 78, 42);
    
    // Altitude on same line as temperature (right side): 海拔1314m
    static const unsigned char ch_hai[] PROGMEM = {0x11,0x00,0x09,0xFC,0x02,0x00,0x05,0xF8,0x11,0x08,0x09,0x48,0x07,0xFC,0x02,0x08,0x02,0x48,0x0B,0xFC,0x10,0x08,0x00,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    static const unsigned char ch_ba[] PROGMEM = {0x04,0x50,0x04,0x48,0x04,0x40,0x1E,0xFC,0x04,0x40,0x04,0x7C,0x06,0xA4,0x1C,0xA4,0x04,0xA8,0x05,0x10,0x05,0x28,0x0E,0x44,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    int altX = 82 + tft.textWidth(tempStr) + 4;
    tft.drawBitmap(altX, 44, ch_hai, 16, 16, tft.color565(100,100,100));
    tft.drawBitmap(altX + 16, 44, ch_ba, 16, 16, tft.color565(100,100,100));
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(tft.color565(100,100,100));
    tft.drawString("1314m", altX + 34, 46);
    
    // Weather description text
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(weatherColor(_weather.current.weather));
    tft.drawString(String(_weather.current.weather), 78, 68);
    
    // Detail info row (y: 88~108)
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextDatum(TL_DATUM);
    
    // Wind: 风力 label + value
    tft.drawBitmap(8, 88, ch_wind0, 16, 16, tft.color565(70,130,180));
    tft.drawBitmap(24, 88, ch_wind1, 16, 16, tft.color565(70,130,180));
    tft.setTextColor(TFT_BLACK);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextDatum(TL_DATUM);
    String windVal = String(_weather.current.winddirection) + String(_weather.current.windpower) + "级";
    tft.drawString(windVal, 42, 90);
    
    // Humidity: 湿度 label + value
    tft.drawBitmap(115, 88, ch_hum0, 16, 16, tft.color565(70,130,180));
    tft.drawBitmap(131, 88, ch_hum1, 16, 16, tft.color565(70,130,180));
    tft.setTextColor(TFT_BLACK);
    String humVal = String(_weather.current.humidity) + "%";
    tft.drawString(humVal, 150, 90);
    
    // Update time: 更新 label + value
    tft.drawBitmap(205, 88, ch_upd0, 16, 16, tft.color565(70,130,180));
    tft.drawBitmap(221, 88, ch_upd1, 16, 16, tft.color565(70,130,180));
    tft.setTextColor(tft.color565(100,100,100));
    String rt = String(_weather.current.reporttime);
    int sp = rt.indexOf(' ');
    if (sp > 0) rt = rt.substring(sp + 1);
    tft.drawString(rt, 240, 90);
    
    // Separator line
    tft.drawFastHLine(8, 110, 304, tft.color565(200,200,200));
    
    // --- Forecast section (y: 110 ~ 235) ---
    // 未来预报 label
    static const unsigned char ch_wei[] PROGMEM = {0x00,0x80,0x00,0x80,0x0F,0xF8,0x00,0x80,0x00,0x80,0x1F,0xFC,0x01,0xC0,0x02,0xA0,0x04,0x90,0x18,0x8C,0x00,0x80,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    static const unsigned char ch_lai[] PROGMEM = {0x00,0x80,0x00,0x80,0x0F,0xF8,0x00,0x80,0x04,0x90,0x02,0xA0,0x1F,0xFC,0x01,0xC0,0x02,0xA0,0x04,0x90,0x18,0x8C,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    static const unsigned char ch_yu[] PROGMEM = {0x00,0x00,0x0E,0x7C,0x02,0x10,0x0A,0x20,0x04,0x7C,0x1F,0x44,0x05,0x54,0x04,0x54,0x04,0x54,0x04,0x10,0x04,0x28,0x0C,0x44,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    static const unsigned char ch_bao[] PROGMEM = {0x04,0x00,0x04,0xFC,0x04,0x84,0x1E,0x84,0x04,0x98,0x04,0x80,0x06,0xFC,0x1C,0xA4,0x04,0xA8,0x04,0x90,0x04,0xA8,0x0C,0xC4,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    tft.setTextDatum(TC_DATUM);
    tft.setTextColor(tft.color565(80,80,80));
    tft.setTextFont(2);
    tft.drawBitmap(100, 118, ch_wei, 16, 16, tft.color565(80,80,80));
    tft.drawBitmap(116, 118, ch_lai, 16, 16, tft.color565(80,80,80));
    tft.drawBitmap(132, 118, ch_yu, 16, 16, tft.color565(80,80,80));
    tft.drawBitmap(148, 118, ch_bao, 16, 16, tft.color565(80,80,80));
    
    int fcY = 136;
    int cardW = 70;
    int gap = 8;
    int startX = (320 - (_weather.forecastCount * (cardW + gap) - gap)) / 2;
    
    for (int i = 0; i < _weather.forecastCount && i < 4; i++) {
      int cx = startX + i * (cardW + gap);
      
      // Card background
      uint16_t bgColor = (i == 0) ? tft.color565(220, 240, 255) : tft.color565(242, 242, 242);
      tft.fillRoundRect(cx, fcY, cardW, 82, 4, bgColor);
      tft.drawRoundRect(cx, fcY, cardW, 82, 4, tft.color565(180,180,180));
      
      // Date label
      tft.setTextDatum(TC_DATUM);
      tft.setTextFont(1);
      tft.setTextSize(1);
      tft.setTextColor(TFT_DARKGREY);
      String dt = String(_weather.forecast[i].date);
      int dash = dt.lastIndexOf('-');
      if (dash > 0) dt = dt.substring(dash + 1);
      if (i == 0) {
        tft.drawString("Today", cx + cardW/2, fcY + 8);
      } else {
        int dtW = tft.textWidth(dt);
        int totalW = dtW + 18; // date + "日" 16px + 2px gap
        int dx = cx + cardW/2 - totalW/2;
        tft.setTextDatum(TL_DATUM);
        tft.drawString(dt, dx, fcY + 8);
        tft.drawBitmap(dx + dtW + 2, fcY + 3, ch_fc_ri, 16, 16, TFT_DARKGREY);
        tft.setTextDatum(TC_DATUM);
      }
      
      // Weather icon (graphical)
      tft.drawBitmap(cx + (cardW-24)/2, fcY + 18, weatherIcon(_weather.forecast[i].dayweather), 24, 24, weatherColor(_weather.forecast[i].dayweather));
      
      // Weather text
      tft.setTextFont(1);
      tft.setTextColor(TFT_BLACK);
      String shortW = String(_weather.forecast[i].dayweather);
      if (shortW.length() > 4) shortW = shortW.substring(0, 3) + ".";
      tft.drawString(shortW, cx + cardW/2, fcY + 47);
      
      // High temp (red)
      tft.setTextColor(tft.color565(220, 50, 50));
      tft.drawString(String(_weather.forecast[i].daytemp) + "C", cx + cardW/2, fcY + 60);
      
      // Low temp (blue)
      tft.setTextColor(tft.color565(50, 100, 220));
      tft.drawString(String(_weather.forecast[i].nighttemp) + "C", cx + cardW/2, fcY + 72);
    }
  }
}
