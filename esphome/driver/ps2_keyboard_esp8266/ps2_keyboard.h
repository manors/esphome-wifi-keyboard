#pragma once
#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"
#include <Arduino.h>

namespace ps2 {

static const char *TAG = "ps2_keyboard";

inline const char* scancode_to_name(uint8_t code, bool extended) {
  if (extended) {
    switch (code) {
      // 导航与编辑键
      case 0x75: return "UP";
      case 0x72: return "DOWN";
      case 0x6B: return "LEFT";
      case 0x74: return "RIGHT";
      case 0x70: return "INSERT";
      case 0x71: return "DELETE";
      case 0x6C: return "HOME";
      case 0x69: return "END";
      case 0x7D: return "PAGE_UP";
      case 0x7A: return "PAGE_DOWN";

      // 扩展控制键
      case 0x14: return "R_CTRL";
      case 0x11: return "R_ALT";

      // Windows 徽标键与应用菜单键
      case 0x1F: return "L_GUI";     // 左 Win 键
      case 0x27: return "R_GUI";     // 右 Win 键
      case 0x2F: return "APPS";      // Menu / 菜单键

      // 小键盘中的扩展键
      case 0x4A: return "KP_DIVIDE"; // 小键盘 /
      case 0x5A: return "KP_ENTER";  // 小键盘 回车

      // Print Screen (硬件实际发送 E0 12 E0 7C，核心匹配 0x7C)
      case 0x7C: return "PRINTSCREEN";

      default:   return "EXTENDED_UNKNOWN";
    }
  }

  switch (code) {
    // 字母区 (A-Z)
    case 0x1C: return "A";
    case 0x32: return "B";
    case 0x21: return "C";
    case 0x23: return "D";
    case 0x24: return "E";
    case 0x2B: return "F";
    case 0x34: return "G";
    case 0x33: return "H";
    case 0x43: return "I";
    case 0x3B: return "J";
    case 0x42: return "K";
    case 0x4B: return "L";
    case 0x3A: return "M";
    case 0x31: return "N";
    case 0x44: return "O";
    case 0x4D: return "P";
    case 0x15: return "Q";
    case 0x2D: return "R";
    case 0x1B: return "S";
    case 0x2C: return "T";
    case 0x3C: return "U";
    case 0x2A: return "V";
    case 0x1D: return "W";
    case 0x22: return "X";
    case 0x35: return "Y";
    case 0x1A: return "Z";

    // 主键盘数字区 (0-9)
    case 0x45: return "0";
    case 0x16: return "1";
    case 0x1E: return "2";
    case 0x26: return "3";
    case 0x25: return "4";
    case 0x2E: return "5";
    case 0x36: return "6";
    case 0x3D: return "7";
    case 0x3E: return "8";
    case 0x46: return "9";

    // 小键盘区 (Keypad 数字与运算符，标准非扩展码)
    case 0x70: return "KP_0";
    case 0x69: return "KP_1";
    case 0x72: return "KP_2";
    case 0x7A: return "KP_3";
    case 0x6B: return "KP_4";
    case 0x73: return "KP_5";
    case 0x74: return "KP_6";
    case 0x6C: return "KP_7";
    case 0x75: return "KP_8";
    case 0x7D: return "KP_9";
    case 0x71: return "KP_DOT";      // 小键盘 .
    case 0x7C: return "KP_MULTIPLY"; // 小键盘 *
    case 0x7B: return "KP_MINUS";    // 小键盘 -
    case 0x79: return "KP_PLUS";     // 小键盘 +
    case 0x77: return "NUM_LOCK";

    // 功能与控制键
    case 0x29: return "SPACE";
    case 0x5A: return "ENTER";
    case 0x66: return "BACKSPACE";
    case 0x0D: return "TAB";
    case 0x76: return "ESC";
    case 0x12: return "L_SHIFT";
    case 0x59: return "R_SHIFT";
    case 0x14: return "L_CTRL";
    case 0x11: return "L_ALT";
    case 0x58: return "CAPS_LOCK";
    case 0x7E: return "SCROLL_LOCK";

    // 常用标点符号
    case 0x0E: return "GRAVE";         // ` ~
    case 0x4E: return "MINUS";         // - _
    case 0x55: return "EQUALS";        // = +
    case 0x54: return "L_BRACKET";     // [ {
    case 0x5B: return "R_BRACKET";     // ] }
    case 0x5D: return "BACKSLASH";     // \ |
    case 0x4C: return "SEMICOLON";     // ; :
    case 0x52: return "QUOTE";         // ' "
    case 0x41: return "COMMA";         // , <
    case 0x49: return "PERIOD";        // . >
    case 0x4A: return "SLASH";         // / ?

    // 功能键 (F1 - F12)
    case 0x05: return "F1";
    case 0x06: return "F2";
    case 0x04: return "F3";
    case 0x0C: return "F4";
    case 0x03: return "F5";
    case 0x0B: return "F6";
    case 0x83: return "F7";
    case 0x0A: return "F8";
    case 0x01: return "F9";
    case 0x09: return "F10";
    case 0x78: return "F11";
    case 0x07: return "F12";

    default:   return "UNKNOWN";
  }
}

class PS2KeyboardComponent : public esphome::Component {
 private:
  uint8_t clk_pin_;
  uint8_t data_pin_;

  // 环形队列缓冲区（避免紧凑连续字节被单变量覆盖）
  static const uint8_t QUEUE_SIZE = 16;
  inline static volatile uint8_t byte_queue_[QUEUE_SIZE];
  inline static volatile uint8_t queue_head_ = 0;
  inline static volatile uint8_t queue_tail_ = 0;

  // 移位与采样变量
  inline static volatile uint16_t shift_reg_ = 0;
  inline static volatile uint8_t bit_count_ = 0;
  inline static volatile uint32_t last_edge_time_ = 0;
  inline static PS2KeyboardComponent *instance_ = nullptr;

  // 协议状态机变量
  bool is_break_{false};
  bool is_extended_{false};

  static void IRAM_ATTR isr_clock_handler() {
    uint32_t now = micros();
    // 1. 去抖滤波：PS/2 时钟半周期约 30~50µs，小于 20µs 的下降沿直接判定为杂散毛刺
    if (now - last_edge_time_ < 20) {
      return;
    }
    last_edge_time_ = now;

    uint8_t bit_val = digitalRead(instance_->data_pin_);
    shift_reg_ |= (bit_val << bit_count_);
    bit_count_++;

    // 2. 满 11 位完成一帧数据
    if (bit_count_ == 11) {
      uint8_t data_byte = (shift_reg_ >> 1) & 0xFF; // 取出第 1~8 位有效数据
      uint8_t stop_bit  = (shift_reg_ >> 10) & 0x01; // 停止位应为 1

      // 仅在停止位有效时压入 FIFO 队列
      if (stop_bit == 1) {
        uint8_t next_head = (queue_head_ + 1) % QUEUE_SIZE;
        if (next_head != queue_tail_) {
          byte_queue_[queue_head_] = data_byte;
          queue_head_ = next_head;
        }
      }

      // 重置位计数器
      shift_reg_ = 0;
      bit_count_ = 0;
    }
  }

 public:
  esphome::CallbackManager<void(uint8_t, bool, const char*, const char*)> on_key_event_callback;

  PS2KeyboardComponent(uint8_t clk_pin, uint8_t data_pin)
      : clk_pin_(clk_pin), data_pin_(data_pin) {
    instance_ = this;
  }

  void setup() override {
    pinMode(clk_pin_, INPUT_PULLUP);
    pinMode(data_pin_, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(clk_pin_), isr_clock_handler, FALLING);
    ESP_LOGI(TAG, "PS/2 Driver Initialized with Debounced Queue Engine");
  }

  void loop() override {
    // 1. 帧间超时恢复：如果位计数停在半途超过 15ms，说明漏脉冲，重置对齐
    if (bit_count_ > 0 && (micros() - last_edge_time_ > 15000)) {
      bit_count_ = 0;
      shift_reg_ = 0;
    }

    // 2. 依次消费环形队列中的字节
    while (queue_tail_ != queue_head_) {
      uint8_t code = byte_queue_[queue_tail_];
      queue_tail_ = (queue_tail_ + 1) % QUEUE_SIZE;

      if (code == 0xE0) {
        is_extended_ = true;
      } else if (code == 0xF0) {
        is_break_ = true;  // 标记松开状态
      } else {
        // 静默丢弃扩展模式下的伪装码 0x12 (L_SHIFT) 0x59 (R_SHIFT)
        if (!is_extended_ || (code != 0x12 && code != 0x59)) {
          const char *action = is_break_ ? "up" : "down";
          const char *key_name = scancode_to_name(code, is_extended_);
          this->on_key_event_callback.call(code, is_extended_, key_name, action);
        }
        
        // 重置瞬态标志
        is_break_ = false;
        is_extended_ = false;
      }
    }
  }

  void add_on_key_event_callback(std::function<void(uint8_t, bool, const char*, const char*)> callback) {
    this->on_key_event_callback.add(std::move(callback));
  }
};

}  // namespace ps2