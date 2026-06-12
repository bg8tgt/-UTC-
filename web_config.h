#pragma once
#include <WiFi.h>
#include <WebServer.h>
#include <TFT_eSPI.h>
#include "simple_wifi_manager.h"

extern TFT_eSPI tft;

WebServer webServer(80);

const char apSSID[] = "BG8TGT_Setup";
const char apPass[] = "12345678";

const char htmlPage[] PROGMEM = 
"<!DOCTYPE html><html><head><meta charset='UTF-8'>"
"<meta name='viewport' content='width=device-width,initial-scale=1,maximum-scale=1'>"
"<title>BG8TGT WiFi Setup</title>"
"<style>*{box-sizing:border-box;margin:0;padding:0}"
"body{font-family:-apple-system,Arial;background:#1a1a2e;color:#fff;padding:15px}"
"h1{color:#00d4ff;text-align:center;font-size:20px;margin:10px 0}"
".container{max-width:400px;margin:auto;background:#16213e;padding:15px;border-radius:10px}"
"button{width:100%;padding:14px;background:#00d4ff;border:none;border-radius:5px;color:#000;font-weight:bold;cursor:pointer;margin:8px 0;font-size:16px}"
"button:hover{background:#00a8cc}"
"button:disabled{background:#555;cursor:default}"
".wifi-item{padding:12px;margin:5px 0;background:#0f3460;border-radius:5px;cursor:pointer;display:flex;justify-content:space-between;align-items:center;font-size:14px}"
".wifi-item:hover{background:#1a4a7e}"
".wifi-item.selected{background:#1a6a3e;border:2px solid #00ff00}"
".rssi{color:#aaa;font-size:11px}"
"input{width:100%;padding:12px;margin:6px 0;border:1px solid #0f3460;border-radius:5px;background:#1a1a2e;color:#fff;font-size:16px}"
"#status{text-align:center;margin-top:10px;font-size:14px;color:#aaa}"
".btn-row{display:flex;gap:8px}"
".btn-row button{flex:1}"
"</style></head>"
"<body><div class='container'>"
"<h1>BG8TGT WiFi Setup</h1>"
"<button id='scanBtn' onclick='scanWiFi()'>Scan WiFi</button>"
"<div id='list'></div>"
"<div id='form' style='display:none'>"
"<p style='color:#aaa;font-size:13px;margin:8px 0 4px'>Password:</p>"
"<input type='text' id='ssid' readonly style='display:none'>"
"<input type='text' id='pass' placeholder='WiFi Password'>"
"<div class='btn-row'>"
"<button onclick='cancelSelect()' style='background:#e74c3c'>Cancel</button>"
"<button onclick='connect()' style='background:#27ae60'>Connect</button>"
"</div></div>"
"<div id='status'></div></div>"
"<script>"
"function scanWiFi(){"
"var b=document.getElementById('scanBtn');"
"b.disabled=true;b.textContent='Scanning...';"
"document.getElementById('status').innerHTML='Scanning WiFi networks...';"
"document.getElementById('list').innerHTML='';"
"fetch('/scan').then(function(r){return r.json();}).then(function(d){"
"var html='';"
"d.sort(function(a,b){return b.rssi-a.rssi;});"
"d.forEach(function(w){"
"var lock=w.enc?'[LOCK]':'[OPEN]';"
"html+='<div class=\"wifi-item\" data-ssid=\"'+w.ssid.replace(/\"/g,'&quot;')+'\" onclick=\"selectWiFi(this)\">';"
"html+='<span>'+lock+' '+w.ssid+'</span>';"
"html+='<span class=\"rssi\">'+w.rssi+'dBm</span></div>';"
"});"
"if(html==='')html='<p style=\"text-align:center;color:#888\">No WiFi found. Try again.</p>';"
"document.getElementById('list').innerHTML=html;"
"b.disabled=false;b.textContent='Scan WiFi';"
"document.getElementById('status').innerHTML='Found '+d.length+' networks. Tap one to connect.';"
"}).catch(function(e){"
"b.disabled=false;b.textContent='Scan WiFi';"
"document.getElementById('status').innerHTML='Scan failed! Retrying...';"
"setTimeout(scanWiFi,3000);"
"});}"
"function selectWiFi(el){"
"var items=document.querySelectorAll('.wifi-item');"
"items.forEach(function(i){i.classList.remove('selected');});"
"el.classList.add('selected');"
"document.getElementById('ssid').value=el.getAttribute('data-ssid');"
"document.getElementById('form').style.display='block';"
"document.getElementById('pass').value='';"
"document.getElementById('pass').focus();"
"}"
"function cancelSelect(){"
"document.getElementById('form').style.display='none';"
"var items=document.querySelectorAll('.wifi-item');"
"items.forEach(function(i){i.classList.remove('selected');});"
"}"
"function connect(){"
"var s=document.getElementById('ssid').value;"
"var p=document.getElementById('pass').value;"
"if(!s){alert('Please select a network');return;}"
"document.getElementById('status').innerHTML='Connecting to '+s+'...';"
"fetch('/connect?ssid='+encodeURIComponent(s)+'&pass='+encodeURIComponent(p))"
".then(function(r){return r.text();}).then(function(t){"
"if(t==='OK'){document.getElementById('status').innerHTML='Connected! Device will restart...';}"
"else{document.getElementById('status').innerHTML=t;}"
"}).catch(function(e){"
"document.getElementById('status').innerHTML='Connection error. Please try again.';"
"});}"
"</script></body></html>";

void startAPMode() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(apSSID, apPass);
  delay(100);
  
  tft.fillScreen(TFT_WHITE);
  tft.fillRect(0, 0, 320, 40, 0x1828);
  tft.setTextColor(0xFFFF);
  tft.setTextDatum(TC_DATUM);
  tft.setTextFont(2);
  tft.setTextSize(1);
  tft.drawString("Phone Config Mode", 160, 12);
  
  tft.setTextColor(TFT_BLACK);
  tft.setTextFont(2);
  tft.drawString("WiFi:", 50, 55);
  tft.setTextColor(TFT_GREEN);
  tft.drawString((String)apSSID, 180, 55);
  
  tft.setTextColor(TFT_BLACK);
  tft.drawString("Pass:", 50, 80);
  tft.setTextColor(TFT_ORANGE);
  tft.drawString((String)apPass, 180, 80);
  
  tft.setTextColor(TFT_CYAN);
  tft.drawString("Open 192.168.4.1", 160, 110);
  
  // Back button
  tft.fillRect(60, 190, 200, 40, 0x4208);
  tft.setTextColor(0xFFFF);
  tft.setTextDatum(MC_DATUM);
  tft.setTextFont(2);
  tft.drawString("Back / Cancel", 160, 210);
  
  webServer.on("/", []() {
    webServer.send(200, "text/html", htmlPage);
  });
  
  // Captive portal detection - respond to common probe URLs
  webServer.on("/generate_204", []() {
    webServer.sendHeader("Location", "http://" + WiFi.softAPIP().toString());
    webServer.send(302, "text/plain", "");
  });
  webServer.on("/hotspot-detect.html", []() {
    webServer.sendHeader("Location", "http://" + WiFi.softAPIP().toString());
    webServer.send(302, "text/plain", "");
  });
  webServer.on("/fwlink", []() {
    webServer.sendHeader("Location", "http://" + WiFi.softAPIP().toString());
    webServer.send(302, "text/plain", "");
  });
  
  webServer.on("/scan", []() {
    int n = WiFi.scanNetworks(false, true);
    String json = "[";
    for (int i = 0; i < n; i++) {
      if (i > 0) json += ",";
      json += "{\"ssid\":\"" + WiFi.SSID(i) + "\",";
      json += "\"rssi\":" + String(WiFi.RSSI(i)) + ",";
      json += "\"enc\":" + String(WiFi.encryptionType(i) != WIFI_AUTH_OPEN ? "true" : "false") + "}";
    }
    json += "]";
    WiFi.scanDelete();
    webServer.send(200, "application/json", json);
  });
  
  webServer.on("/connect", []() {
    String ssid = webServer.arg("ssid");
    String pass = webServer.arg("pass");
    
    if (ssid.length() == 0) {
      webServer.send(400, "text/plain", "No SSID");
      return;
    }
    
    webServer.send(200, "text/plain", "OK");
    delay(500);
    
    // Stop server first
    webServer.stop();
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 40) {
      delay(500);
      attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      wifi_add(ssid, pass);
      tft.fillScreen(TFT_WHITE);
      tft.setTextColor(TFT_GREEN);
      tft.setTextDatum(TC_DATUM);
      tft.setTextSize(2);
      tft.drawString("Connected!", 160, 100);
      tft.setTextSize(1);
      tft.setTextColor(TFT_BLACK);
      tft.drawString(WiFi.localIP().toString(), 160, 130);
      delay(2000);
    } else {
      tft.fillScreen(TFT_WHITE);
      tft.setTextColor(TFT_RED);
      tft.setTextDatum(TC_DATUM);
      tft.setTextSize(2);
      tft.drawString("Failed!", 160, 100);
      delay(2000);
    }
  });
  
  // Handle all other requests - redirect to captive portal
  webServer.onNotFound([]() {
    webServer.sendHeader("Location", "http://" + WiFi.softAPIP().toString());
    webServer.send(302, "text/plain", "");
  });
  
  webServer.begin();
}

void stopAPMode() {
  webServer.stop();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
}

void handleWebServer() {
  webServer.handleClient();
}