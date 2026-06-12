# BG8TGT 智能时钟 - ESP32
<img width="3840" height="2160" alt="64578ee79770208cf3ed4aa10e756a9a" src="https://github.com/user-attachments/assets/0d96db9e-ed10-48ef-99e5-bcc45594d565" />
<img width="3840" height="2160" alt="64578ee79770208cf3ed4aa10e756a9a" src="https://github.com/user-attachments/assets/a8d11a6f-d395-4ebf-9d37-86e27b1e6c5d" />
<img width="602" height="450" alt="744f74bd-a284-42a4-be89-813e43c87811" src="https://github.com/user-attachments/assets/91e8ea72-8612-4a5d-9797-c97d5a7b7b8d" />

## 硬件配置
- **MCU**: ESP32
- **显示屏**: ST7789 240x320 (横屏320x240)
- **触摸**: XPT2046 SPI
- **电池**: ADC GPIO34 电压检测 (2:1分压)
- **背光**: GPIO21 PWM

## 文件结构

```
├── sketch.ino              # 主程序 (时钟页面、页面切换、NTP同步)
├── simple_wifi_manager.h   # WiFi管理 (NVS存储/自动连接/最多3个)
├── clock_controller.h      # 时钟控制器 (触摸驱动、配置菜单、电池、节能)
├── touch_wifi.h            # 触摸屏WiFi配置 (虚拟键盘、WiFi列表)
├── web_config.h            # Web配置 (AP热点、手机配置、天气参数管理)
├── weather_manager.h       # 天气管理 (高德API、海拔自动查询、天气页绘制)
└── README.md               # 本文档
```

## 功能总结

| 模块 | 功能 | 关键文件 |
|------|------|----------|
| 🕐 双时区时钟 | BJT+UTC七段数码管、日期星期、电池电量、自定义标题 | sketch.ino |
| 🌤️ 天气预报 | 当前天气(温度+海拔+风力+湿度)、未来4天预报卡片 | weather_manager.h |
| ⛰️ 海拔查询 | 高德行政区API获取坐标→Open Elevation API获取海拔 | weather_manager.h |
| 📡 WiFi管理 | 自动连接/触摸配置/手机热点配置，最多保存3个 | simple_wifi_manager.h |
| 👆 触摸交互 | 左右滑动切页、点击操作、虚拟键盘输入 | clock_controller.h, touch_wifi.h |
| 📱 Web配置 | WiFi配置 + 高德API Key + 城市编码，手机浏览器操作 | web_config.h |
| ⚡ 节能 | 5分钟息屏(5%亮度)、15分钟无WiFi自动关机、断线自动重连 | clock_controller.h |
| 🔋 电池监控 | ADC电压检测→百分比显示→彩色电池图标 | clock_controller.h |
| 🔧 配置菜单 | 触摸配置WiFi、手机配置、删除WiFi、修改标题、关机 | clock_controller.h |

## 数据存储 (NVS)

| 键名空间 | 键 | 说明 |
|-----------|-----|------|
| wifi_cfg | count, ssid_0~2, pass_0~2 | WiFi凭据 |
| clock_cfg | title | 时钟标题 |
| weather_cfg | api_key, city, altitude | 天气配置+海拔 |

## API依赖
高德天气API文档https://lbs.amap.com/api/webservice/guide/api/weatherinfo
| API | 用途 | URL |
|-----|------|-----|
| 高德天气 | 天气预报+实况 | restapi.amap.com/v3/weather/weatherInfo |
| 高德行政区 | 城市坐标查询 | restapi.amap.com/v3/config/district |
| Open Elevation | 海拔查询 | api.open-elevation.com/api/v1/lookup |
| NTP | 时间同步 | pool.ntp.org |

## 触摸手势

| 操作 | 功能 |
|------|------|
| 左滑 (时钟页) | → 天气页 |
| 右滑 (天气页) | → 时钟页 |
| 右滑 (时钟页) | → 配置菜单 |
| 点击右上角 (时钟页) | WiFi重连 |
| 点击底部 (时钟页) | → 天气页 |
| 任意触摸 (息屏时) | 唤醒屏幕 |

## 编译说明

Arduino IDE / PlatformIO，需安装库：
- TFT_eSPI
- NTPClient
- WiFi (ESP32内置)
- WebServer (ESP32内置)
