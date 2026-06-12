#pragma once
#include <SPI.h>
#include <WiFi.h>
#include <TFT_eSPI.h>
#include "simple_wifi_manager.h"

extern TFT_eSPI tft;

class TouchWiFiSetup {
  SPIClass tpSPI;
  int8_t _cs;
  int _xMin, _yMin, _xRange, _yRange;
  uint8_t _flags;
  char _buf[65];
  int _bufLen;
  bool _shift;
  int _selIdx;

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
    for (int i = 0; i < 8; i++) {
      if (!xpt_pressed()) continue;
      sx += xpt_rd(0xD0);
      sy += xpt_rd(0x90);
      n++;
    }
    if (n < 3) return false;
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

  void waitRelease() { while (xpt_pressed()) delay(10); delay(50); }

  bool waitTouch(int &x, int &y) {
    if (!xpt_read(x, y)) return false;
    waitRelease();
    return true;
  }

  struct Key { int x, y, w, h; char ch; uint8_t type; };
  Key keys[50];
  int nK;

  void buildKeys() {
    nK = 0;
    const int kw = 29, kh = 27, gap = 1;
    int sx = (320 - 10*kw - 9*gap) / 2;
    const char* r;
    r = "1234567890";
    for (int i = 0; i < 10; i++) keys[nK++] = {sx+i*(kw+gap), 55, kw, kh, r[i], 0};
    r = "QWERTYUIOP";
    for (int i = 0; i < 10; i++) keys[nK++] = {sx+i*(kw+gap), 83, kw, kh, r[i], 0};
    int sx2 = (320 - 9*kw - 8*gap) / 2;
    r = "ASDFGHJKL";
    for (int i = 0; i < 9; i++) keys[nK++] = {sx2+i*(kw+gap), 111, kw, kh, r[i], 0};
    keys[nK++] = {sx, 139, 40, kh, 0, 1};
    r = "ZXCVBNM";
    for (int i = 0; i < 7; i++) keys[nK++] = {sx+42+i*(kw+gap), 139, kw, kh, r[i], 0};
    keys[nK++] = {sx+42+7*(kw+gap), 139, 42, kh, 0, 2};
    int sx4 = 6;
    r = ".@-_";
    for (int i = 0; i < 4; i++) keys[nK++] = {sx4+i*(kw+gap), 167, kw, kh, r[i], 0};
    keys[nK++] = {sx4+4*(kw+gap)+3, 167, 80, kh, 0, 3};
    keys[nK++] = {sx4+4*(kw+gap)+87, 167, 50, kh, 0, 5};
    keys[nK++] = {sx4+4*(kw+gap)+141, 167, 50, kh, 0, 4};
  }

  void drawKey(int i, bool hi) {
    Key& k = keys[i];
    uint16_t bg, fg;
    if (k.type == 4) { bg = hi ? 0x04E0 : 0x07E0; fg = 0x0000; }
    else if (k.type == 5) { bg = hi ? 0x8410 : 0x4208; fg = 0xFFFF; }
    else if (k.type == 1) {
      if (_shift) { bg = hi ? 0xB400 : 0xFD20; fg = 0x0000; }
      else { bg = hi ? 0x3B5C : 0xEF5D; fg = hi ? 0xFFFF : 0x0000; }
    }
    else { bg = hi ? 0x3B5C : 0xEF5D; fg = hi ? 0xFFFF : 0x0000; }
    tft.fillRect(k.x, k.y, k.w, k.h, bg);
    tft.drawRect(k.x, k.y, k.w, k.h, 0x8410);
    tft.setTextColor(fg);
    tft.setTextDatum(MC_DATUM);
    tft.setTextFont(2);
    tft.setTextSize(1);
    if (k.type == 0) {
      char c = k.ch;
      if (c >= 'A' && c <= 'Z') c += _shift ? 0 : 32;
      char s[2] = {c, 0};
      tft.drawString(s, k.x + k.w/2, k.y + k.h/2);
    } else {
      const char* lbl;
      if (k.type == 1) lbl = _shift ? "SH" : "sh";
      else if (k.type == 2) lbl = "DEL";
      else if (k.type == 3) lbl = "SPACE";
      else if (k.type == 5) lbl = "BACK";
      else lbl = "OK";
      tft.drawString(lbl, k.x + k.w/2, k.y + k.h/2);
    }
  }

  void drawAllKeys() { for (int i = 0; i < nK; i++) drawKey(i, false); }

  void updateInput(const char* title) {
    tft.fillRect(0, 0, 320, 50, 0x1828);
    tft.setTextColor(0xFFFF);
    tft.setTextDatum(TC_DATUM);
    tft.setTextFont(2);
    tft.drawString(title, 160, 8);
    tft.fillRect(8, 26, 304, 20, 0xFFFF);
    tft.drawRect(8, 26, 304, 20, 0x1C7F);
    tft.setTextColor(0x0000);
    tft.setTextDatum(TL_DATUM);
    tft.drawString(_buf, 12, 28);
    tft.drawString("_", 12 + tft.textWidth(_buf), 28);
  }

  int findKey(int tx, int ty) {
    for (int i = 0; i < nK; i++) {
      Key& k = keys[i];
      if (tx >= k.x && tx < k.x+k.w && ty >= k.y && ty < k.y+k.h) return i;
    }
    return -1;
  }

public:
  void begin() {
    _cs = 33;
    tpSPI.begin(25, 39, 32, -1);
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH);
    _xMin = 363; _yMin = 247;
    _xRange = 3343; _yRange = 3397;
    _flags = 2;
    buildKeys();
    _shift = true;
    _selIdx = -1;
  }

  String selectWiFi() {
    _selIdx = -1;
    tft.fillScreen(0xFFFF);
    tft.setTextColor(0x0000);
    tft.setTextDatum(TC_DATUM);
    tft.setTextFont(2);
    tft.drawString("Scanning WiFi...", 160, 110);
    int n = WiFi.scanNetworks(false, true);
    if (n <= 0) {
      tft.fillScreen(0xFFFF);
      tft.setTextColor(0xF800);
      tft.drawString("No networks found!", 160, 100);
      tft.setTextColor(0x0000);
      tft.drawString("Touch to rescan", 160, 130);
      int x, y;
      while (!waitTouch(x, y)) delay(50);
      return selectWiFi();
    }
    int scroll = 0;
    const int IH = 30, MS = 5;
    while (true) {
      tft.fillScreen(0x1828);
      tft.setTextColor(0xFFFF);
      tft.setTextDatum(TC_DATUM);
      tft.setTextFont(2);
      tft.drawString("Select WiFi", 160, 10);
      char cnt[30]; sprintf(cnt, "%d networks", n);
      tft.setTextFont(1);
      tft.drawString(cnt, 160, 25);
      int sn = n - scroll; if (sn > MS) sn = MS;
      for (int i = 0; i < sn; i++) {
        int idx = i + scroll;
        int iy = 38 + i * IH;
        String ssid = WiFi.SSID(idx);
        int rssi = WiFi.RSSI(idx);
        uint16_t bg;
        if (_selIdx == idx) {
          bg = 0x3B5C;
        } else {
          bg = (i % 2 == 0) ? 0xEF5D : 0xFFFF;
        }
        tft.fillRect(5, iy, 310, IH, bg);
        tft.drawRect(5, iy, 310, IH, 0xB5B6);
        tft.setTextColor(_selIdx == idx ? 0xFFFF : 0x0000);
        tft.setTextDatum(TL_DATUM);
        tft.setTextFont(2);
        if (ssid.length() > 18) ssid = ssid.substring(0, 17) + "~";
        tft.drawString(ssid.c_str(), 10, iy + 8);
        if (WiFi.encryptionType(idx) != WIFI_AUTH_OPEN) {
          tft.drawString("*", 200, iy + 8);
        }
        int bars = (rssi + 100) / 10;
        if (bars < 0) bars = 0; if (bars > 5) bars = 5;
        for (int b = 0; b < 5; b++) {
          int bh = (b + 1) * 4;
          uint16_t c = (b < bars) ? 0x04E0 : 0xB5B6;
          tft.fillRect(278 + b * 5, iy + IH - 2 - bh, 4, bh, c);
        }
      }
      int by = 210;
      tft.fillRect(5, by, 55, 25, 0x3B5C); tft.setTextColor(0xFFFF);
      tft.setTextDatum(MC_DATUM); tft.setTextFont(2);
      tft.drawString("Scan", 32, by + 12);
      if (scroll > 0) {
        tft.fillRect(65, by, 45, 25, 0x7BEF); tft.setTextColor(0x0000);
        tft.drawString("Up", 87, by + 12);
      }
      if (scroll + MS < n) {
        tft.fillRect(115, by, 55, 25, 0x7BEF); tft.setTextColor(0x0000);
        tft.drawString("Down", 142, by + 12);
      }
      tft.fillRect(250, by, 60, 25, 0xF800); tft.setTextColor(0xFFFF);
      tft.drawString("Skip", 280, by + 12);
      if (_selIdx >= 0) {
        tft.fillRect(178, by, 65, 25, 0x07E0); tft.setTextColor(0x0000);
        tft.drawString("OK", 210, by + 12);
      }
      int tx, ty;
      while (!waitTouch(tx, ty)) delay(20);
      for (int i = 0; i < sn; i++) {
        int iy = 38 + i * IH;
        if (ty >= iy && ty < iy + IH && tx >= 5 && tx < 315) {
          _selIdx = i + scroll;
        }
      }
      if (ty >= by && ty < by + 25) {
        if (tx >= 5 && tx < 60) { WiFi.scanDelete(); return selectWiFi(); }
        if (tx >= 65 && tx < 110) scroll = (scroll - MS < 0) ? 0 : scroll - MS;
        if (tx >= 115 && tx < 170) scroll += MS;
        if (tx >= 250 && tx < 310) { WiFi.scanDelete(); return ""; }
        if (tx >= 178 && tx < 243 && _selIdx >= 0) {
          String sel = WiFi.SSID(_selIdx);
          WiFi.scanDelete();
          return sel;
        }
      }
    }
  }

  String inputPassword(const char* ssid) {
    _bufLen = 0; _buf[0] = 0; _shift = true;
    tft.fillScreen(0xE73C);
    updateInput(ssid);
    drawAllKeys();
    while (true) {
      int tx, ty;
      if (!waitTouch(tx, ty)) { delay(20); continue; }
      int ki = findKey(tx, ty);
      if (ki < 0) continue;
      drawKey(ki, true); delay(80);
      Key& k = keys[ki];
      switch (k.type) {
        case 0: {
          char c = k.ch;
          if (c >= 'A' && c <= 'Z') c += _shift ? 0 : 32;
          if (_bufLen < 63) _buf[_bufLen++] = c;
          break;
        }
        case 1: _shift = !_shift; break;
        case 2: if (_bufLen > 0) _buf[--_bufLen] = 0; break;
        case 3: if (_bufLen < 63) _buf[_bufLen++] = ' '; break;
        case 5: return "";
        case 4: _buf[_bufLen] = 0; return String(_buf);
      }
      drawKey(ki, false);
      if (k.type == 1) drawAllKeys();
      updateInput(ssid);
    }
  }

  String inputTitle() {
    _bufLen = clock_title.length();
    if (_bufLen > 63) _bufLen = 63;
    memcpy(_buf, clock_title.c_str(), _bufLen);
    _buf[_bufLen] = 0;
    _shift = true;
    tft.fillScreen(0xE73C);
    updateInput("Edit Clock Title");
    drawAllKeys();
    while (true) {
      int tx, ty;
      if (!waitTouch(tx, ty)) { delay(20); continue; }
      int ki = findKey(tx, ty);
      if (ki < 0) continue;
      drawKey(ki, true); delay(80);
      Key& k = keys[ki];
      switch (k.type) {
        case 0: {
          char c = k.ch;
          if (c >= 'A' && c <= 'Z') c += _shift ? 0 : 32;
          if (_bufLen < 63) _buf[_bufLen++] = c;
          break;
        }
        case 1: _shift = !_shift; break;
        case 2: if (_bufLen > 0) _buf[--_bufLen] = 0; break;
        case 3: if (_bufLen < 63) _buf[_bufLen++] = ' '; break;
        case 5: return "";
        case 4: _buf[_bufLen] = 0; return String(_buf);
      }
      drawKey(ki, false);
      if (k.type == 1) drawAllKeys();
      updateInput("Edit Clock Title");
    }
  }

  bool connectWiFi(const String &ssid, const String &pass) {
    WiFi.begin(ssid.c_str(), pass.c_str());
    tft.fillScreen(0xFFFF);
    tft.setTextColor(0x0000);
    tft.setTextDatum(TC_DATUM);
    tft.setTextFont(2);
    unsigned long t0 = millis();
    int dots = 0;
    while (millis() - t0 < 15000) {
      if (WiFi.status() == WL_CONNECTED) {
        wifi_add(ssid, pass);
        tft.fillScreen(0xFFFF);
        tft.setTextColor(0x04E0);
        tft.drawString("Connected!", 160, 95);
        tft.setTextColor(0x0000);
        tft.drawString(WiFi.localIP().toString().c_str(), 160, 125);
        delay(2000);
        return true;
      }
      tft.fillRect(60, 100, 200, 20, 0xFFFF);
      String msg = "Connecting";
      for (int i = 0; i <= dots % 4; i++) msg += ".";
      tft.drawString(msg, 160, 100);
      dots++;
      delay(500);
    }
    tft.fillScreen(0xFFFF);
    tft.setTextColor(0xF800);
    tft.drawString("Failed!", 160, 95);
    tft.setTextColor(0x0000);
    tft.drawString("Touch to retry", 160, 125);
    int x, y;
    while (!waitTouch(x, y)) delay(50);
    return false;
  }

  bool run(String &outSSID, String &outPass) {
    begin();
    while (true) {
      String ssid = selectWiFi();
      if (ssid.length() == 0) return false;
      String pass = inputPassword(ssid.c_str());
      if (connectWiFi(ssid, pass)) {
        outSSID = ssid;
        outPass = pass;
        return true;
      }
    }
  }
};

TouchWiFiSetup touchWiFi;