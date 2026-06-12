#pragma once
#include <WiFi.h>
#include <Preferences.h>

#define MAX_WIFI_COUNT 3

String wifi_ssids[MAX_WIFI_COUNT];
String wifi_passes[MAX_WIFI_COUNT];
int wifi_count = 0;

void wifi_load_prefs() {
  Preferences prefs;
  prefs.begin("wifi_cfg", false);
  wifi_count = prefs.getInt("count", 0);
  for (int i = 0; i < wifi_count && i < MAX_WIFI_COUNT; i++) {
    wifi_ssids[i] = prefs.getString(("ssid_" + String(i)).c_str(), "");
    wifi_passes[i] = prefs.getString(("pass_" + String(i)).c_str(), "");
  }
  prefs.end();
}

void wifi_save_prefs() {
  Preferences prefs;
  prefs.begin("wifi_cfg", false);
  prefs.putInt("count", wifi_count);
  for (int i = 0; i < wifi_count; i++) {
    prefs.putString(("ssid_" + String(i)).c_str(), wifi_ssids[i]);
    prefs.putString(("pass_" + String(i)).c_str(), wifi_passes[i]);
  }
  prefs.end();
}

void wifi_move_to_front(int idx) {
  if (idx <= 0 || idx >= wifi_count) return;
  String ssid = wifi_ssids[idx];
  String pass = wifi_passes[idx];
  for (int i = idx; i > 0; i--) {
    wifi_ssids[i] = wifi_ssids[i-1];
    wifi_passes[i] = wifi_passes[i-1];
  }
  wifi_ssids[0] = ssid;
  wifi_passes[0] = pass;
  wifi_save_prefs();
}

void wifi_add(const String &ssid, const String &pass) {
  for (int i = 0; i < wifi_count; i++) {
    if (wifi_ssids[i] == ssid) {
      wifi_passes[i] = pass;
      wifi_move_to_front(i);
      return;
    }
  }
  
  if (wifi_count >= MAX_WIFI_COUNT) wifi_count = MAX_WIFI_COUNT - 1;
  
  for (int i = wifi_count; i > 0; i--) {
    wifi_ssids[i] = wifi_ssids[i-1];
    wifi_passes[i] = wifi_passes[i-1];
  }
  
  wifi_ssids[0] = ssid;
  wifi_passes[0] = pass;
  wifi_count++;
  wifi_save_prefs();
}

void wifi_delete(int idx) {
  if (idx < 0 || idx >= wifi_count) return;
  for (int i = idx; i < wifi_count - 1; i++) {
    wifi_ssids[i] = wifi_ssids[i+1];
    wifi_passes[i] = wifi_passes[i+1];
  }
  wifi_count--;
  wifi_save_prefs();
}

bool wifi_try_connect(const String &ssid, const String &pass, int timeoutSec = 10) {
  WiFi.begin(ssid.c_str(), pass.c_str());
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < timeoutSec * 2) {
    delay(500);
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED) return true;
  WiFi.disconnect();
  return false;
}

bool wifi_auto_connect(int timeoutSec = 30) {
  if (wifi_count == 0) return false;
  
  int totalAttempts = 0;
  int maxAttempts = timeoutSec * 2;
  
  for (int i = 0; i < wifi_count && totalAttempts < maxAttempts; i++) {
    if (wifi_try_connect(wifi_ssids[i], wifi_passes[i], 10)) {
      wifi_move_to_front(i);
      return true;
    }
    totalAttempts += 20;
  }
  
  return false;
}

void wifi_clear_all() {
  wifi_count = 0;
  Preferences prefs;
  prefs.begin("wifi_cfg", false);
  prefs.clear();
  prefs.end();
}

// Title storage
String clock_title = "BG8TGT";

void title_load() {
  Preferences prefs;
  prefs.begin("clock_cfg", false);
  clock_title = prefs.getString("title", "BG8TGT");
  prefs.end();
}

void title_save(const String &title) {
  clock_title = title;
  Preferences prefs;
  prefs.begin("clock_cfg", false);
  prefs.putString("title", title);
  prefs.end();
}