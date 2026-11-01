# ESPHome Wi-Fi Keyboard (PS/2 智能中控键盘)

[![ESPHome](https://img.shields.io/badge/ESPHome-Supported-blue.svg)](https://esphome.io/)
[![Home Assistant](https://img.shields.io/badge/Home%20Assistant-Integration-41BDF5.svg)](https://www.home-assistant.io/)
[![Hardware](https://img.shields.io/badge/Hardware-ESP32--C6%20%7C%20ESP8266-green.svg)](#硬件准备)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](LICENSE)

基于 ESP32 / ESP8266 与 ESPHome 实现的 **PS/2 Wi-Fi 物理按键中控台**。通过将老式 PS/2 键盘改造为无线键盘，无缝接入 Home Assistant，作为全屋智能的专属物理控制台使用。

---

## ✨ 核心特性

- 🔌 **复古焕新**：让闲置的 PS/2 有线机械键盘/薄膜键盘变身为 Wi-Fi 桌面智能控制台。
- 🔄 **双向通讯（ESP32-C6）**：
  - 支持全键位扫描码精确解码与按键状态（按下/松开）捕获。
  - 支持向键盘发送指令控制键盘指示灯（Num Lock / Caps Lock / Scroll Lock），支持系统联网与就绪状态指示。
- ⚡ **超低延迟与省电优化**：
  - 基于 ESP-IDF Light Sleep 与 FreeRTOS Tickless Idle 机制，深度优化待机能耗。
  - 支持电池供电，自带 ADC 电池电压检测与电量百分比计算。
  - 内置充电宝防休眠脉冲机制（周期模拟按键触发），解决低功耗下充电宝自动断电关机的问题。
- 🏠 **Home Assistant 深度联动**：
  - 监听单键及丰富组合键（`Ctrl`、`Alt`、`Shift`、`Win`）。
  - **创新 Alt 提示模式**：按下单键直接执行操作（如开关灯）；按下 `Alt + 对应按键` 则仅语音/文字播报该设备当前状态与按键定义，不执行动作，避免盲操误触。
  - 支持空调模式/风量/温度梯级调节、一键全开/全关灯光、PC 网络唤醒（Wake-on-LAN）等复杂场景。

---

## 🛠️ 硬件准备与接线

### 1. 硬件清单

1. **PS/2 键盘** 一个（标准 6 针 Mini-DIN 接口或引出的 4 根线：VCC、GND、CLK、DATA）。
2. **ESP32 开发板**（推荐 ESP32-C6，支持完整双向通讯与低功耗特性）或 **ESP8266**。
3. **5V 转 3.3V 双向逻辑电平转换模块**（如常用的基于 MOSFET 的 I2C 双向电平转换板，用于安全转换 CLK/DATA 信号）。
4. **供电模块**：USB 5V 直供 或 锂电池 / 充电宝。
5. *(可选)* 2 颗阻值相同的精密电阻（推荐阻值范围 **10kΩ ~ 100kΩ**，。用于电池电压 1:1 分压检测，过小阻值会增加待机静态功耗，过大阻值会影响 ADC 采样精度）。

### 2. 引脚接线说明

| ESP32 引脚 | 经电平转换后连接目标 | 功能说明 | 必选/可选 |
| :--- | :--- | :--- | :--- |
| **GPIO 5** (3.3V端) | PS/2 **Clock** (5V端) | 键盘时钟引脚（**必须经电平转换**） | **必选** |
| **GPIO 4** (3.3V端) | PS/2 **Data** (5V端) | 键盘数据引脚（**必须经电平转换**） | **必选** |
| **GPIO 6** | 充电宝开机键引脚 | 开漏输出模拟开机按键，周期性唤醒充电宝防断电 | 可选（电池供电时用） |
| **GPIO 0** | 电池正极分压中点 | ADC 采样引脚（2 颗 100kΩ 串联 1:1 分压，将 0~4.2V 降至 0~2.1V） | 可选（电量检测） |
| **5V** | PS/2 **VCC** 及转换板高压端(HV) | PS/2 键盘与电平转换板供电 | **必选** |
| **3.3V** | 电平转换板低压端(LV) | 为电平转换芯片低压参考端供电 | **必选** |
| **GND** | PS/2 **GND** 及转换板 GND | 全系统共地 | **必选** |

> [!WARNING]
> **电平安全警告**：
> - PS/2 键盘工作标准为 **5V 逻辑电平**，其内部 CLK 和 DATA 默认上拉到 5V；
> - ESP32 的 GPIO 额定耐压仅为 **3.3V**；
> - **切勿将 ESP32 引脚直接与 PS/2 的 5V 信号线相连**，否则极易损坏 ESP32 引脚或芯片。在 GPIO 5、GPIO 4 与键盘 CLK、DATA 之间**必须串接双向电平转换模块**。

---

## 📂 项目结构

```text
esphome-wifi-keyboard/
├── esphome/
│   ├── driver/
│   │   ├── ps2_keyboard_esp32c6/  # ESP32-C6 专用双向通信驱动（支持读取按键与控制LED）
│   │   └── ps2_keyboard_esp8266/  # ESP8266 基础单向驱动（仅接收按键输入）
│   ├── secrets.example.yaml       # 密钥配置文件模板
│   ├── wifi_wrap.h                # WiFi 底层优化包装头文件
│   └── wifi-keyboard.yaml         # ESPHome 设备固件配置文件
├── homeassistant/
│   └── automations.yaml           # Home Assistant 自动化事件处理脚本（包含按键映射与TTS提示）
├── LICENSE                        # GNU General Public License v3.0
└── README.md                      # 项目说明文档
```

---

## 🚀 快速上手

### 步骤 1：配置 ESPHome

1. 将本仓库克隆至本地：
   ```bash
   git clone https://github.com/your-username/esphome-wifi-keyboard.git
   ```
2. 在 ESPHome 运行环境的 `secrets.yaml` 中配置相关凭据：
   ```yaml
   wifi_ssid: "Your_WiFi_SSID"
   wifi_password: "Your_WiFi_Password"
   api_password: "Your_ESPHome_API_Key"
   web_password: "Your_Web_Server_Password"
   ```
3. 检查并根据实际情况修改 `esphome/wifi-keyboard.yaml`：
   - 修改 `external_components` 的驱动路径（指向 `driver` 目录）。
   - 按需配置或注释掉固定的静态 IP（`use_address`）与测试 Ping 目标。
4. 编译并烧录固件到 ESP32：
   ```bash
   esphome run esphome/wifi-keyboard.yaml
   ```

### 步骤 2：配置 Home Assistant 自动化

1. 打开 Home Assistant 前端控制台，进入 **设置** -> **自动化与场景** -> **自动化**。
2. 点击右下角 **创建自动化**，右上角选择 **以 YAML 模式编辑**。
3. 复制 `homeassistant/automations.yaml` 的内容并粘贴进去。
4. 根据自己的设备修改 `key_map` 与控制实体：
   - 替换灯光、开关、空调、WOL 等实体 ID 为自己的设备实体。
   - 替换通知播报设备（如 `text.xiaomi_...` 播放服务）。
5. 保存并启用自动化。

---

## ⌨️ 按键控制映射与逻辑

本项目的 Home Assistant 自动化脚本内置了极为灵活的按键分发逻辑：

### 1. 按键定义格式

在 `automations.yaml` 的 `key_map` 中，支持以下几种配置方式：

- **简单切换（Toggle）**：直接配置实体名称：
  ```yaml
  F1: switch.living_room_light
  ```
- **完整动作（Action / Target / Data / Msg）**：
  ```yaml
  F12:
    action: switch.turn_on
    target: "{{ b2_lights }}"
    msg: "开启所有灯光"
  ```
- **快捷键组合（全大写、字母顺序）**：
  ```yaml
  CTRL+S: switch.study_socket
  INSERT:
    action: wake_on_lan.send_magic_packet
    data:
      mac: "00:11:22:33:44:55"
    msg: "唤醒台式机"
  ```

### 2. Alt 查询/提示功能

- 直接按下按键（如 `F1`）：直接触发对应实体开/关。
- 按下 `Alt + 按键`（如 `Alt + F1`）：**不会改变开关状态**，而是通过语音音箱播报该实体当前状态（例如：“*书房灯 已开启*”）。

### 3. LED 指示灯状态对应表（ESP32-C6）

| 键盘指示灯状态 | 对应设备阶段 |
| :--- | :--- |
| **Num Lock 亮** | ESP32 已开机，启动就绪 |
| **Num Lock + Caps Lock 亮** | Wi-Fi 已成功连接 |
| **三灯全亮** | Home Assistant API 已成功建立连接，中控台就绪 |

---

## 📋 常见问题 (FAQ)

<details>
<summary><b>Q: 为什么按键没有反应？</b></summary>

1. 检查键盘指示灯是否正常点亮；
2. 确认 ESPHome 核心日志中是否成功捕获 PS/2 键码输出；
3. 确认 Home Assistant 是否收到 `esphome.keyboard_02_event` 事件。
</details>

<details>
<summary><b>Q: 充电宝依然会自动关机断电？</b></summary>

充电宝的自动休眠检测电流通常在 50~100mA 左右。若开漏模拟按键的时间不足以激活充电宝，可在 `wifi-keyboard.yaml` 的 `sim_press_power_btn` 中适当延长 `delay`（如调至 300~500ms），或调整定时周期。
</details>

---

## 📄 开源协议

本项目采用 [GNU General Public License v3.0 (GPL-3.0)](LICENSE) 开源协议。
