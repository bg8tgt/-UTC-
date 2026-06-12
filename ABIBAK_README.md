# BG8TGT 智能时钟 ESP32

## 项目概述

基于 ESP32 + ST7789 2.8寸触摸屏的智能网络时钟，支持双页面切换（时钟页 / 天气页），具备 WiFi 管理、触摸交互、天气查询、自动节能等功能。

## 硬件配置

| 硬件 | 参数 |
|------|------|
| 主控 | ESP32 |
| 屏幕 | ST7789 240x320 SPI (横屏模式 rotation=1, 320x240) |
| 触摸 | XPT2046 SPI (CS=33, CLK=25, MISO=39, MOSI=32) |
| 背光 | GPIO21 PWM (ledc 5000Hz 8bit) |
| 电池 | GPIO34 ADC 电压检测 (2:1分压, 3.0V~4.2V) |

### TFT 引脚

| 引脚 | GPIO |
|------|------|
| MISO | 12 |
| MOSI | 13 |
| SCLK | 14 |
| CS   | 15 |
| DC   | 2  |
| RST  | -1 |
| BL   | -1 |

## 功能说明

### 1. 时钟页面 (Page 0)

- **七段数码管时钟**: BJT(北京时间) + UTC 双行显示，自绘段码风格
- **日期显示**: 年-月-日 + 星期X (汉字位图)
- **标题栏**: 可自定义标题 (默认 "BG8TGT")
- **电池图标**: 右上角显示电量百分比和填充条
- **NTP 同步**: 连接 WiFi 后自动从 pool.ntp.org 同步时间 (UTC+8)

### 2. 天气页面 (Page 1)

- **当前天气**: 天气图标 + 温度(大字体) + 海拔1314m(同行显示) + 天气描述
- **详细信息**: 风力(级) | 湿度(%) | 更新时间
- **未来预报**: 4天预报卡片 (日期、天气图标、天气文字、高/低温)
- **数据来源**: 高德天气 API (restapi.amap.com)

### 3. 页面切换

| 操作 | 效果 |
|------|------|
| 右滑 | 时钟页→WiFi配置 / 天气页→时钟页 |
| 左滑 | 时钟页→天气页 / 天气页→时钟页 |
| 点击时钟页底部 | 跳转天气页 |
| 点击时钟页右上角 | WiFi重连 |

### 4. WiFi 配置系统

支持三种 WiFi 配置方式：

- **自动连接**: 优先自动连接已保存的 WiFi (最多保存3个)
- **触摸配置**: 通过屏幕触摸键盘扫描并输入密码连接
- **手机配置**: ESP32 开启 AP 热点 (BG8TGT_Setup/12345678)，手机浏览器访问 192.168.4.1 配置

### 5. 节能与保护

- **自动息屏**: 5分钟无操作自动降低亮度至5%
- **自动关机**: 无保存WiFi时15分钟无连接自动深度休眠
- **WiFi断线重连**: 有保存WiFi时每3秒轮询尝试重连

### 6. 配置菜单功能

| 功能 | 说明 |
|------|------|
| Touch Config | 触摸屏选择WiFi并输入密码 |
| Phone Config | 手机热点模式配置 |
| Delete WiFi | 删除已保存的WiFi |
| Edit Title | 修改时钟标题文字 |
| Power Off | 深度休眠 |

## 文件结构

```
source_code/
├── sketch.ino              # 主程序 (setup/loop, 时钟页面绘制, 页面切换逻辑)
├── simple_wifi_manager.h   # WiFi管理 (存储/读取/自动连接/优先级排序)
├── clock_controller.h      # 时钟控制器 (触摸驱动, 配置菜单, 按键检测, 电池, 节能)
├── touch_wifi.h            # 触摸屏WiFi配置界面 (虚拟键盘, WiFi列表, 密码输入)
├── weather_manager.h       # 天气管理 (高德API数据获取与天气页面绘制)
├── web_config.h            # Web配置服务器 (AP模式, 手机配置页面, Captive Portal)
└── README.md               # 本文档
```

## 依赖库

| 库 | 说明 |
|----|------|
| TFT_eSPI | ST7789 屏幕驱动 |
| WiFi | ESP32 WiFi |
| NTPClient | NTP 时间同步 |
| HTTPClient | HTTP 请求 (天气API) |
| WebServer | Web 配置服务器 |
| Preferences | NVS 存储 (WiFi/标题) |
| SPI | XPT2046 触摸驱动 |

## 天气 API 配置

使用高德天气 REST API，密钥和城市代码定义在 `weather_manager.h` 中：
- API Key: `226bf59672f9c3d8702635a901d68ed8`
- 城市编码: `532625`
- 更新间隔: 30分钟

## 编译说明

1. 使用 Aily Blockly 打开项目
2. 开发板选择 ESP32
3. 安装依赖库: lib-tft-espi, lib-ntpclient, lib-esp32-wifi, lib-esp32-webserver, lib-esp32-preferences
4. TFT_eSPI User_Setup 需配置 ST7789 驱动及对应引脚
5. 编译并烧录
