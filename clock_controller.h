#pragma once
#include <WiFi.h>
#include <TFT_eSPI.h>
#include "simple_wifi_manager.h"
#include "touch_wifi.h"
#include "web_config.h"

extern TFT_eSPI tft;
extern TouchWiFiSetup touchWiFi;

volatile bool _forceFullRedraw = false;
volatile unsigned long lastTouchTime = 0;
volatile unsigned long wifiDisconnectTime = 0;
volatile bool isDimmed = false;
volatile int currentPage = 0; // 0=clock, 1=weather
volatile bool _forceWeatherRedraw = false;

// Gear icon 16x16
static const unsigned char gear_icon[] PROGMEM = {
  0x00,0x00,0x03,0x00,0x07,0x80,0x0F,0xC0,0x1B,0x60,0x33,0x30,0x67,0x98,0x7F,0xF8,
  0x7F,0xF8,0x67,0x98,0x33,0x30,0x1B,0x60,0x0F,0xC0,0x07,0x80,0x03,0x00,0x00,0x00
};

// WiFi icon 16x16
static const unsigned char wifi_icon[] PROGMEM = {
  0x00,0x00,0x01,0x00,0x03,0x80,0x07,0xC0,0x0F,0xE0,0x1E,0x30,0x3C,0x18,0x38,0x08,
  0x1C,0x10,0x0E,0x20,0x07,0xC0,0x03,0x80,0x03,0x80,0x01,0x00,0x00,0x00,0x00,0x00
};

// Phone icon 16x16
static const unsigned char phone_icon[] PROGMEM = {
  0x0C,0x00,0x1E,0x00,0x1E,0x00,0x1E,0x00,0x1E,0x00,0x1E,0x00,0x1F,0x00,0x1F,0x00,
  0x3F,0x80,0x3F,0x80,0x3F,0x80,0x3F,0x80,0x3F,0x80,0x3F,0x80,0x1F,0x00,0x0E,0x00
};

// Trash icon 16x16
static const unsigned char trash_icon[] PROGMEM = {
  0x00,0x00,0x3F,0xFC,0x3F,0xFC,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x30,0x0C,
  0x30,0x0C,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x3F,0xFC,0x00,0x00,0x00,0x00
};

// Back arrow icon 16x16
static const unsigned char back_icon[] PROGMEM = {
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x06,0x00,0x0E,0x80,0x1E,0xC0,0x3E,
  0x80,0x1E,0x00,0x0E,0x00,0x06,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

// Power icon 16x16
static const unsigned char power_icon[] PROGMEM = {
  0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x19,0x80,0x0F,0xF0,0x07,0xE0,0x03,0xC0,
  0x07,0xE0,0x0F,0xF0,0x19,0x80,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x00,0x00
};

// Battery icon 20x12 (modern rounded design)
static const unsigned char battery_icon[] PROGMEM = {
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

// Modern battery outline 24x14
static const unsigned char battery_outline[] PROGMEM = {
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

// Battery fill patterns - modern design with rounded corners
static const unsigned char battery_fill_100[] PROGMEM = {
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
static const unsigned char battery_fill_80[] PROGMEM = {
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
static const unsigned char battery_fill_60[] PROGMEM = {
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
static const unsigned char battery_fill_40[] PROGMEM = {
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
static const unsigned char battery_fill_20[] PROGMEM = {
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

enum ConfigMode {
  MODE_TOUCH = 0,
  MODE_PHONE,
  MODE_DELETE,
  MODE_TITLE,
  MODE_POWER_OFF,
  MODE_CANCEL
};

class ClockController {
private:
  SPIClass tpSPI;
  int8_t _cs;
  int _xMin, _yMin, _xRange, _yRange;
  uint8_t _flags;
  bool _tpInited;

  // Battery voltage ADC pin (GPIO34 = ADC1_CH6)
  int _batPin = 34;
  float _batMin = 3.0;  // Minimum voltage (0%)
  float _batMax = 4.2;  // Maximum voltage (100%)

  uint16_t xpt_rd(uint8_t cmd) {
    tpSPI.beginTransaction(SPISettings(2500000, MSBFIRST, SPI_MODE0));
    digitalWrite(_cs, LOW);
    tpSPI.transfer(cmd);
    uint16_t v = (uint16_t)tpSPI.transfer(0) << 8;
    v |= tpSPI.transfer(0);
    digitalWrite(_cs, HIGH);
    tpSPI.endTransaction();
    return (v >> 3) & 0xFFF;
  }

  bool xpt_pressed() {
    uint16_t z1 = xpt_rd(0xB0);
    return (z1 > 30 && z1 < 3900);
  }

  bool xpt_read(int &x, int &y) {
    if (!xpt_pressed()) return false;
    int32_t sx = 0, sy = 0, n = 0;
    for (int i = 0; i < 4; i++) {  // 4 samples for faster response
      if (!xpt_pressed()) continue;
      sx += xpt_rd(0xD0);
      sy += xpt_rd(0x90);
      n++;
    }
    if (n < 2) return false;
    int rx = sx / n, ry = sy / n;
    int cx = (_flags & 4) ? ry : rx;
    int cy = (_flags & 4) ? rx : ry;
    if (_flags & 2) cx = 4095 - cx;
    if (_flags & 1) cy = 4095 - cy;
    int px = (int32_t)(cx - _xMin) * 240 / _xRange;
    int py = (int32_t)(cy - _yMin) * 320 / _yRange;
    x = py;
    y = 239 - px;
    return (x >= 0 && x < 320 && y >= 0 && y < 240);
  }

  bool waitTouch(int &x, int &y) {
    if (!xpt_read(x, y)) return false;
    while (xpt_pressed()) delay(10);
    delay(50);
    return true;
  }

  void beginTouch() {
    _cs = 33;
    tpSPI.begin(25, 39, 32, -1);
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH);
    _xMin = 363; _yMin = 247;
    _xRange = 3343; _yRange = 3397;
    _flags = 2;
    _tpInited = true;
  }

  void drawBtn(int x, int y, int w, int h, uint16_t bg, const char* label, const unsigned char* icon = nullptr) {
    tft.fillRoundRect(x, y, w, h, 6, bg);
    tft.setTextColor(0xFFFF);
    tft.setTextDatum(MC_DATUM);
    tft.setTextFont(2);
    tft.setTextSize(1);
    if (icon) {
      tft.drawBitmap(x + 10, y + (h - 16) / 2, icon, 16, 16, 0xFFFF);
      tft.drawString(label, x + 30 + (w - 30) / 2, y + h / 2);
    } else {
      tft.drawString(label, x + w / 2, y + h / 2);
    }
  }

  ConfigMode showConfigMenu() {
    tft.fillScreen(0x1828);

    tft.fillRect(0, 0, 320, 40, 0x0FA0);
    tft.setTextColor(0xFFFF);
    tft.setTextDatum(TC_DATUM);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.drawString("WiFi Setup", 160, 20);

    int by = 45;
    int bw = 260, bh = 36, gap = 6;
    int bx = 30;

    drawBtn(bx, by, bw, bh, 0x3B5C, "Touch Config", wifi_icon);
    by += bh + gap;
    drawBtn(bx, by, bw, bh, 0x7BEF, "Phone Config", phone_icon);
    by += bh + gap;
    drawBtn(bx, by, bw, bh, 0xF800, "Delete WiFi", trash_icon);
    by += bh + gap;
    drawBtn(bx, by, bw, bh, 0x05FF, "Edit Title", gear_icon);
    by += bh + gap;
    drawBtn(bx, by, bw, bh, 0x4208, "Power Off", power_icon);
    by += bh + gap;
    drawBtn(bx, by, bw, bh, 0xEF5D, "Cancel", back_icon);

    int btnY[6];
    by = 45;
    for (int i = 0; i < 6; i++) {
      btnY[i] = by;
      by += bh + gap;
    }

    while (true) {
      int x, y;
      if (!waitTouch(x, y)) { delay(20); continue; }

      if (x >= bx && x < bx + bw) {
        for (int i = 0; i < 6; i++) {
          if (y >= btnY[i] && y < btnY[i] + bh) {
            return (ConfigMode)i;
          }
        }
      }
    }
  }

  void showDeleteWiFi() {
    tft.fillScreen(TFT_WHITE);
    tft.fillRect(0, 0, 320, 40, 0x1828);
    tft.setTextColor(0xFFFF);
    tft.setTextDatum(TC_DATUM);
    tft.setTextFont(2);
    tft.setTextSize(1);

    String title = "Saved WiFi (" + String(wifi_count) + "/" + String(MAX_WIFI_COUNT) + ")";
    tft.drawString(title, 160, 20);

    int by = 50;
    for (int i = 0; i < wifi_count; i++) {
      String ssid = wifi_ssids[i];
      if (ssid.length() > 16) ssid = ssid.substring(0, 15) + "~";

      tft.fillRect(10, by, 250, 35, 0xEF5D);
      tft.setTextColor(0x0000);
      tft.setTextDatum(TL_DATUM);
      tft.drawString(ssid, 15, by + 10);

      tft.fillRect(265, by, 45, 35, 0xF800);
      tft.setTextColor(0xFFFF);
      tft.setTextDatum(MC_DATUM);
      tft.drawString("X", 287, by + 17);

      by += 40;
    }

    tft.fillRect(40, 195, 240, 35, 0x3B5C);
    tft.setTextColor(0xFFFF);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Back", 160, 212);

    while (true) {
      int x, y;
      if (!waitTouch(x, y)) { delay(20); continue; }

      int tempBy = 50;
      for (int i = 0; i < wifi_count; i++) {
        if (x >= 265 && x < 310 && y >= tempBy && y < tempBy + 35) {
          wifi_delete(i);
          showDeleteWiFi();
          return;
        }
        tempBy += 40;
      }

      if (x >= 40 && x < 280 && y >= 195 && y < 230) return;
    }
  }

  void showNoWiFi() {
    tft.fillScreen(TFT_WHITE);
    tft.setTextColor(TFT_RED);
    tft.setTextDatum(TC_DATUM);
    tft.setTextFont(2);
    tft.setTextSize(2);
    tft.drawString("No saved WiFi", 160, 80);
    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(1);
    tft.drawString("Touch to config", 160, 120);

    int x, y;
    while (!waitTouch(x, y)) delay(50);
  }

  void showAutoConnect(int timeoutSec) {
    tft.fillScreen(TFT_WHITE);
    tft.setTextColor(TFT_BLACK);
    tft.setTextDatum(TC_DATUM);
    tft.setTextFont(2);
    tft.drawString("Auto Connecting...", 160, 50);

    tft.drawBitmap(152, 68, wifi_icon, 16, 16, TFT_BLUE);

    tft.setTextSize(1);
    tft.setTextColor(TFT_ORANGE);
    tft.drawString("Touch to skip", 160, 180);

    unsigned long start = millis();

    while (millis() - start < (unsigned long)timeoutSec * 1000) {
      if (xpt_pressed()) {
        while (xpt_pressed()) delay(10);
        delay(50);
        return;
      }

      int progress = (millis() - start) * 100 / ((unsigned long)timeoutSec * 1000);
      tft.fillRect(40, 100, 240, 30, TFT_WHITE);
      tft.fillRect(40, 100, progress * 240 / 100, 30, TFT_GREEN);
      tft.drawRect(40, 100, 240, 30, TFT_BLACK);

      int remaining = timeoutSec - (millis() - start) / 1000;
      tft.setTextColor(TFT_BLACK);
      tft.drawString(String(remaining) + "s", 160, 115);

      delay(100);
    }
  }

public:
  void begin() {
    beginTouch();
    wifi_load_prefs();
  }

  int getBatteryLevel() {
    // Read battery voltage from ADC (GPIO34)
    int raw = analogRead(_batPin);
    // Convert to voltage (ESP32 ADC: 0-4095 = 0-3.3V, with voltage divider)
    // Assuming 2:1 voltage divider, actual voltage = ADC * 2 * 3.3 / 4095
    float voltage = raw * 2.0 * 3.3 / 4095.0;
    
    // Convert voltage to percentage
    if (voltage >= _batMax) return 100;
    if (voltage <= _batMin) return 0;
    return (int)((voltage - _batMin) / (_batMax - _batMin) * 100);
  }

  void drawBattery(int level) {
    // Modern battery design - draw with code instead of bitmap
    int x = 275, y = 4, w = 28, h = 12;
    
    // Determine color based on level
    uint16_t color;
    if (level >= 80) color = TFT_GREEN;
    else if (level >= 60) color = TFT_GREEN;
    else if (level >= 40) color = TFT_YELLOW;
    else if (level >= 20) color = TFT_ORANGE;
    else color = TFT_RED;
    
    // Draw battery outline (rounded rectangle)
    tft.drawRoundRect(x, y, w, h, 2, tft.color565(100,100,100));
    
    // Draw battery tip (small rectangle on right)
    tft.fillRect(x + w, y + 3, 3, h - 6, tft.color565(100,100,100));
    
    // Draw fill level
    int fillWidth = (w - 4) * level / 100;
    if (fillWidth > 0) {
      tft.fillRoundRect(x + 2, y + 2, fillWidth, h - 4, 1, color);
    }
    
    // Draw percentage text
    tft.setTextColor(color);
    tft.setTextFont(1);
    tft.setTextSize(1);
    tft.setTextDatum(TR_DATUM);
    char buf[5];
    sprintf(buf, "%d%%", level);
    tft.drawString(buf, x - 2, y + 2);
  }

  void forceFullRedraw() {
    _forceFullRedraw = true;
    _forceWeatherRedraw = true;
  }

  bool setupWiFi(bool skipAutoConnect = false) {
    // Silent auto-connect if has saved WiFi
    if (!skipAutoConnect && wifi_count > 0) {
      if (wifi_auto_connect(15)) {
        forceFullRedraw();
        return true;
      }
    }
    // No WiFi or failed → show config menu
    ConfigMode mode = showConfigMenu();

    if (mode == MODE_TOUCH) {
      touchWiFi.begin();
      String ssid = touchWiFi.selectWiFi();
      if (ssid.length() > 0) {
        String pass = touchWiFi.inputPassword(ssid.c_str());
        if (touchWiFi.connectWiFi(ssid, pass)) {
          forceFullRedraw();
          return true;
        }
      }
      return setupWiFi();
    } else if (mode == MODE_PHONE) {
      // Start AP mode
      startAPMode();

      // Loop: handle web + check touch for back
      while (true) {
        if (WiFi.status() == WL_CONNECTED) {
          stopAPMode();
          forceFullRedraw();
          return true;
        }
        handleWebServer();

        // Check touch using our own SPI (already initialized)
        if (xpt_pressed()) {
          int tx, ty;
          if (xpt_read(tx, ty)) {
            // Any touch = back/cancel
            while (xpt_pressed()) delay(10);
            delay(50);
            stopAPMode();
            return setupWiFi();
          }
        }
        delay(10);
      }
    } else if (mode == MODE_DELETE) {
      showDeleteWiFi();
      return setupWiFi(skipAutoConnect);
    } else if (mode == MODE_TITLE) {
      touchWiFi.begin();
      String newTitle = touchWiFi.inputTitle();
      if (newTitle.length() > 0) {
        title_save(newTitle);
        tft.fillScreen(TFT_WHITE);
        tft.setTextColor(TFT_GREEN);
        tft.setTextDatum(TC_DATUM);
        tft.setTextFont(2);
        tft.drawString("Title Saved!", 160, 100);
        delay(1500);
      }
      return setupWiFi(skipAutoConnect);
    } else if (mode == MODE_POWER_OFF) {
      tft.fillScreen(TFT_BLACK);
      tft.setTextColor(TFT_WHITE);
      tft.setTextDatum(TC_DATUM);
      tft.setTextFont(2);
      tft.drawString("Power Off...", 160, 100);
      delay(1000);
      tft.fillScreen(TFT_BLACK);
      esp_deep_sleep_start();
      return false;
    } else {
      forceFullRedraw();
      return false;
    }
  }

  // Returns: 0=nothing, 1=wifi reconnect, 2=config menu, 5=go weather, 6=go clock
  int checkButtons() {
    static bool inSwipe = false;
    static int swipeStartX = 0, swipeStartY = 0;
    static int swipeLastX = 0;
    static int swipeAccumX = 0;
    static int swipeCommitted = 0; // 1=right, -1=left
    static unsigned long lastTouchMs = 0;
    
    bool pressed = xpt_pressed();
    
    if (pressed) {
      int tx, ty;
      if (xpt_read(tx, ty)) {
        lastTouchMs = millis();
        lastTouchTime = millis();
        
        if (!inSwipe) {
          inSwipe = true;
          swipeStartX = tx;
          swipeStartY = ty;
          swipeLastX = tx;
          swipeAccumX = 0;
          swipeCommitted = 0;
          lastTouchMs = millis();
          Serial.printf("START: x=%d y=%d\n", tx, ty);
        } else {
          int dx = tx - swipeLastX;
          // Filter out tiny movements (touchscreen noise)
          if (abs(dx) >= 2 && abs(dx) < 40) {
            swipeAccumX += dx;
          }
          swipeLastX = tx;
          
          // Require larger threshold to avoid mis-detection
          if (swipeCommitted == 0 && abs(swipeAccumX) > 40) {
            int dir = (swipeAccumX > 0) ? 1 : -1;
            Serial.printf("SWIPE: dir=%d accum=%d page=%d\n", dir, swipeAccumX, currentPage);
            inSwipe = false;
            if (dir == 1) {
              if (currentPage == 0) return 2; // clock -> config
              if (currentPage == 1) return 6; // weather -> clock
            } else {
              if (currentPage == 0) return 5; // clock -> weather
              if (currentPage == 1) return 6; // weather -> clock
            }
          }
        }
        
        if (isDimmed) {
          ledcWrite(21, 204);
          isDimmed = false;
          _forceFullRedraw = true;
          _forceWeatherRedraw = true;
        }
      }
      return 0;
    }
    
    // Not pressed
    if (!inSwipe) return 0;
    
    // Bridge XPT2046 false releases (150ms grace)
    if (millis() - lastTouchMs < 150) return 0;
    
    // True release - process tap only (swipe already handled above)
    inSwipe = false;
    int totalDX = swipeLastX - swipeStartX;
    if (abs(totalDX) < 30) {
      // Tap detection - require longer press to avoid swipe mis-triggering tap
      if (currentPage == 0) {
        if (swipeStartX >= 260 && swipeStartY < 50) return 1;
        if (swipeStartY > 200) return 5;
      } else {
        return 6;
      }
    }
    
    return 0;
  }

  void checkIdle() {
    if (isDimmed) return;
    
    unsigned long idleTime = millis() - lastTouchTime;
    if (idleTime > 300000) { // 5 minutes = 300000ms
      ledcWrite(21, 13); // 5% brightness (255 * 0.05 ≈ 13)
      isDimmed = true;
      Serial.println("Screen dimmed due to idle");
    }
  }

  void checkWiFiTimeout() {
    if (WiFi.isConnected()) {
      wifiDisconnectTime = 0;
      return;
    }
    
    // No saved WiFi → 15min auto power-off
    if (wifi_count == 0) {
      if (wifiDisconnectTime == 0) {
        wifiDisconnectTime = millis();
        Serial.println("No saved WiFi! 15min auto power-off");
      }
      unsigned long d = millis() - wifiDisconnectTime;
      if (d > 900000) {
        Serial.println("15min no WiFi, powering off...");
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_WHITE);
        tft.setTextDatum(TC_DATUM);
        tft.setTextFont(2);
        tft.drawString("Auto Power Off", 160, 90);
        tft.drawString("No WiFi 15min", 160, 115);
        delay(2000);
        tft.fillScreen(TFT_BLACK);
        esp_deep_sleep_start();
      }
      return;
    }
    
    // Has saved WiFi → keep trying reconnect (non-blocking)
    wifiDisconnectTime = 0;
    static unsigned long lastRetry = 0;
    static int retryIdx = 0;
    if (millis() - lastRetry > 3000) { // try next saved WiFi every 3s
      lastRetry = millis();
      WiFi.begin(wifi_ssids[retryIdx].c_str(), wifi_passes[retryIdx].c_str());
      retryIdx = (retryIdx + 1) % wifi_count;
    }
  }

  bool handleButton(int btn) {
    Serial.printf("handleButton: btn=%d\n", btn);
    if (btn == 1 || btn == 2) {
      Serial.println("Going to config menu...");
      WiFi.disconnect();
      bool r = setupWiFi(true);
      forceFullRedraw();
      return r;
    }
    return true;
  }
};

ClockController clockCtrl;