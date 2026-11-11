#pragma once
#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_attr.h"
#include "esp_pm.h"
#include "esp_sleep.h"

namespace ps2
{

  static const char *TAG = "ps2_keyboard";

  // 协议命令与应答码
  namespace protocol
  {
    constexpr uint8_t CMD_SET_LEDS = 0xED;
    constexpr uint8_t CMD_RESEND = 0xFE;
    constexpr uint8_t CMD_ENABLE_SCANNING = 0xF4;
    constexpr uint8_t RESP_BAT_OK = 0xAA;
    constexpr uint8_t RESP_ACK = 0xFA;
    constexpr uint8_t RESP_RESEND = 0xFE;
    constexpr uint8_t PREFIX_EXTENDED = 0xE0;
    constexpr uint8_t PREFIX_PAUSE = 0xE1;
    constexpr uint8_t PREFIX_RELEASE = 0xF0;
  }

  // 使用 C++ 标准的 constexpr 函数在编译期构建数组，替代 C99 的指定初始化器
  constexpr std::array<const char *, 256> init_extended_lut()
  {
    std::array<const char *, 256> lut{}; // 自动全部初始化为 nullptr
    lut[0x11] = "R_ALT";
    lut[0x14] = "R_CTRL";
    lut[0x1F] = "L_GUI";
    lut[0x27] = "R_GUI";
    lut[0x2F] = "APPS";
    lut[0x4A] = "KP_DIVIDE";
    lut[0x5A] = "KP_ENTER";
    lut[0x69] = "END";
    lut[0x6B] = "LEFT";
    lut[0x6C] = "HOME";
    lut[0x70] = "INSERT";
    lut[0x71] = "DELETE";
    lut[0x72] = "DOWN";
    lut[0x74] = "RIGHT";
    lut[0x75] = "UP";
    lut[0x7A] = "PAGE_DOWN";
    lut[0x7C] = "PRINTSCREEN";
    lut[0x7D] = "PAGE_UP";
    lut[0x7E] = "BREAK";
    return lut;
  }

  constexpr std::array<const char *, 256> init_normal_lut()
  {
    std::array<const char *, 256> lut{}; // 自动全部初始化为 nullptr
    lut[0x01] = "F9";
    lut[0x03] = "F5";
    lut[0x04] = "F3";
    lut[0x05] = "F1";
    lut[0x06] = "F2";
    lut[0x07] = "F12";
    lut[0x09] = "F10";
    lut[0x0A] = "F8";
    lut[0x0B] = "F6";
    lut[0x0C] = "F4";
    lut[0x0D] = "TAB";
    lut[0x0E] = "GRAVE";
    lut[0x11] = "L_ALT";
    lut[0x12] = "L_SHIFT";
    lut[0x14] = "L_CTRL";
    lut[0x15] = "Q";
    lut[0x16] = "1";
    lut[0x1A] = "Z";
    lut[0x1B] = "S";
    lut[0x1C] = "A";
    lut[0x1D] = "W";
    lut[0x1E] = "2";
    lut[0x21] = "C";
    lut[0x22] = "X";
    lut[0x23] = "D";
    lut[0x24] = "E";
    lut[0x25] = "4";
    lut[0x26] = "3";
    lut[0x29] = "SPACE";
    lut[0x2A] = "V";
    lut[0x2B] = "F";
    lut[0x2C] = "T";
    lut[0x2D] = "R";
    lut[0x2E] = "5";
    lut[0x31] = "N";
    lut[0x32] = "B";
    lut[0x33] = "H";
    lut[0x34] = "G";
    lut[0x35] = "Y";
    lut[0x36] = "6";
    lut[0x3A] = "M";
    lut[0x3B] = "J";
    lut[0x3C] = "U";
    lut[0x3D] = "7";
    lut[0x3E] = "8";
    lut[0x41] = "COMMA";
    lut[0x42] = "K";
    lut[0x43] = "I";
    lut[0x44] = "O";
    lut[0x45] = "0";
    lut[0x46] = "9";
    lut[0x49] = "PERIOD";
    lut[0x4A] = "SLASH";
    lut[0x4B] = "L";
    lut[0x4C] = "SEMICOLON";
    lut[0x4D] = "P";
    lut[0x4E] = "MINUS";
    lut[0x52] = "QUOTE";
    lut[0x54] = "L_BRACKET";
    lut[0x55] = "EQUALS";
    lut[0x58] = "CAPS_LOCK";
    lut[0x59] = "R_SHIFT";
    lut[0x5A] = "ENTER";
    lut[0x5B] = "R_BRACKET";
    lut[0x5D] = "BACKSLASH";
    lut[0x66] = "BACKSPACE";
    lut[0x69] = "KP_1";
    lut[0x6B] = "KP_4";
    lut[0x6C] = "KP_7";
    lut[0x70] = "KP_0";
    lut[0x71] = "KP_DEL";
    lut[0x72] = "KP_2",
    lut[0x73] = "KP_5";
    lut[0x74] = "KP_6";
    lut[0x75] = "KP_8";
    lut[0x76] = "ESC";
    lut[0x77] = "NUM_LOCK";
    lut[0x78] = "F11";
    lut[0x79] = "KP_DOT";
    lut[0x7A] = "KP_3";
    lut[0x7B] = "KP_PLUS";
    lut[0x7C] = "KP_MULTIPLY";
    lut[0x7D] = "KP_9";
    lut[0x7E] = "SCROLL_LOCK";
    lut[0x7F] = "KP_MINUS";
    lut[0x83] = "F7";
    return lut;
  }

  // 声明为 static constexpr，它们将在编译期生成并存放在 Flash 中（省 RAM）
  static constexpr std::array<const char *, 256> EXTENDED_LUT = init_extended_lut();
  static constexpr std::array<const char *, 256> NORMAL_LUT = init_normal_lut();

  inline const char *scancode_to_name(uint8_t code, bool extended)
  {
    if (extended)
    {
      const char *name = EXTENDED_LUT[code];
      return name ? name : "EXTENDED_UNKNOWN";
    }

    const char *name = NORMAL_LUT[code];
    return name ? name : "UNKNOWN";
  }

  constexpr unsigned int str2int(const char *str, int h = 0)
  {
    return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
  }

  // 简易轻量环形缓冲区 (单写-单读场景)
  template <typename T, size_t Capacity>
  class RingBuffer
  {
  private:
    T buffer_[Capacity]{};
    size_t head_{0};
    size_t tail_{0};

  public:
    bool push(T val) volatile
    {
      size_t next_head = (head_ + 1) % Capacity;
      if (next_head == tail_)
        return false; // 队列满
      buffer_[head_] = val;
      head_ = next_head;
      return true;
    }

    bool pop(T &out) volatile
    {
      if (head_ == tail_)
        return false; // 队列空
      out = buffer_[tail_];
      tail_ = (tail_ + 1) % Capacity;
      return true;
    }

    void clear() volatile
    {
      tail_ = head_;
    }

    bool empty() const volatile { return head_ == tail_; }
  };

  class PS2KeyboardComponent : public esphome::Component
  {
  private:
    gpio_num_t clk_pin_;
    gpio_num_t data_pin_;

    // 接收状态与缓冲区
    RingBuffer<uint8_t, 64> byte_queue_;
    volatile uint16_t shift_reg_{0};
    volatile uint8_t bit_count_{0};
    volatile bool is_transmitting_{false};

    enum class LedState
    {
      PENDING,
      SENT_CMD,
      GOT_CMD_ACK,
      SENT_DATA,
      GOT_DATA_ACK,
      IDLE,
    };

    volatile LedState led_state_{LedState::IDLE};
    volatile uint8_t led_mask_{0};

    // 键盘解析状态机
    bool is_release_{false};
    bool is_extended_{false};
    bool l_ctrl_state_{false};
    bool num_lock_state_{true};
    bool caps_lock_state_{false};
    bool scroll_lock_state_{false};
    bool wait_first_release_{false};

    // 诊断统计收归结构体
    struct Diagnostics
    {
      volatile uint32_t triggers{0};
      volatile uint32_t glitches{0};
      volatile uint32_t frames{0};
      volatile uint32_t drop_bits{0};
      volatile uint32_t invalid_frames{0};
      volatile uint32_t timeout_frames{0};
      volatile uint32_t wakeups{0};

      void reset() volatile
      {
        triggers = 0;
        glitches = 0;
        frames = 0;
        drop_bits = 0;
        invalid_frames = 0;
        timeout_frames = 0;
        wakeups = 0;
      }
    } diag_;

    // 电源管理与定时
    esp_pm_lock_handle_t pm_lock_{nullptr};
    volatile int64_t last_activity_time_{0};
    volatile bool is_pm_locked_{false};
    volatile bool is_turn_off_leds{false};
    volatile bool is_sleeping_{false};

    // 引脚电平超时轮询
    inline bool wait_pin_level(gpio_num_t pin, int level, uint32_t timeout_us)
    {
      int64_t start = esp_timer_get_time();
      while (gpio_get_level(pin) != level)
      {
        if ((esp_timer_get_time() - start) > timeout_us)
          return false;
        esp_rom_delay_us(1);
      }
      return true;
    }

    inline int64_t check_idle(uint64_t timeout_us)
    {
      return (esp_timer_get_time() - last_activity_time_) > timeout_us;
    }

    // 主机到设备（Host-to-Device）协议发送
    bool write_byte(uint8_t byte_val)
    {
      is_transmitting_ = true;

      // 帧结构：[Bit 9: Stop=1] | [Bit 8: Odd Parity] | [Bit 7..0: Data]
      uint8_t odd_parity = (__builtin_popcount(byte_val) & 1) ? 0 : 1;
      uint16_t frame = byte_val | (odd_parity << 8) | (1 << 9);
      bool success = false;

      // RTS 请求：CLK 拉低 >= 100us，DATA 拉低，释放 CLK
      gpio_set_level(clk_pin_, 0);
      esp_rom_delay_us(120);
      gpio_set_level(data_pin_, 0);
      esp_rom_delay_us(15);
      gpio_set_level(clk_pin_, 1);

      for (int i = 0; i < 10; i++)
      {
        uint32_t clk_timeout = (i == 0) ? 15000 : 1000;
        if (!wait_pin_level(clk_pin_, 0, clk_timeout))
          break;

        gpio_set_level(data_pin_, (frame >> i) & 0x01);

        if (!wait_pin_level(clk_pin_, 1, 1000))
          break;

        // 第 10 位发送完毕，等待外设 ACK
        if (i == 9)
        {
          success = wait_pin_level(data_pin_, 0, 1000) &&
                    wait_pin_level(clk_pin_, 0, 1000) &&
                    wait_pin_level(clk_pin_, 1, 1000) &&
                    wait_pin_level(data_pin_, 1, 1000);
        }
      }

      // 统一恢复总线与中断
      gpio_set_level(clk_pin_, 1);
      gpio_set_level(data_pin_, 1);
      last_activity_time_ = esp_timer_get_time();
      bit_count_ = 0;
      shift_reg_ = 0;
      is_transmitting_ = false;
      return success;
    }

    bool send_reset()
    {
      bool result = write_byte(protocol::CMD_ENABLE_SCANNING);
      return result;
    }

    bool send_set_leds()
    {
      return write_byte(protocol::CMD_SET_LEDS);
    }

    bool send_set_leds_mask()
    {
      return write_byte(led_mask_);
    }

    void config_gpio_for_sleep()
    {
      gpio_wakeup_enable(clk_pin_, GPIO_INTR_LOW_LEVEL);
      esp_sleep_enable_gpio_wakeup();
    }

    void config_gpio_for_wakeup()
    {
      gpio_wakeup_disable(clk_pin_);
      gpio_set_intr_type(clk_pin_, GPIO_INTR_NEGEDGE);
    }

    // 静态 ISR 入口，通过 arg 派发到成员函数
    static void IRAM_ATTR isr_clock_handler(void *arg)
    {
      auto *self = static_cast<PS2KeyboardComponent *>(arg);
      self->diag_.triggers++;

      if (self->is_transmitting_)
        return;

      // GPIO 唤醒
      if (self->is_sleeping_)
      {
        self->is_sleeping_ = false;
        // gpio_set_level(self->clk_pin_, 0);
        // self->config_gpio_for_wakeup();
        self->diag_.wakeups++;
        return;
      }

      int64_t now = esp_timer_get_time();
      if (now - self->last_activity_time_ < 10)
      {
        self->diag_.glitches++;
        return;
      }

      // 超时复位未收完的帧
      if (self->bit_count_ > 0 && (now - self->last_activity_time_ > 200))
      {
        self->diag_.drop_bits += self->bit_count_;
        self->bit_count_ = 0;
        self->shift_reg_ = 0;
        self->diag_.timeout_frames++;
      }

      self->last_activity_time_ = now;

      uint8_t bit_val = static_cast<uint8_t>(gpio_get_level(self->data_pin_));
      self->shift_reg_ |= (bit_val << self->bit_count_);
      self->bit_count_++;

      if (self->bit_count_ == 11)
      {
        self->diag_.frames++;
        uint8_t start_bit = self->shift_reg_ & 0x01;
        uint8_t data_byte = (self->shift_reg_ >> 1) & 0xFF;
        uint8_t parity_bit = (self->shift_reg_ >> 9) & 0x01;
        uint8_t stop_bit = (self->shift_reg_ >> 10) & 0x01;
        uint8_t ones = __builtin_popcount(data_byte) + parity_bit;
        bool valid = (start_bit == 0) && (stop_bit == 1) && (ones & 1);

        self->shift_reg_ = 0;
        self->bit_count_ = 0;

        if (valid)
          self->byte_queue_.push(data_byte);
        else
          self->diag_.invalid_frames++;
      }
    }

    void handle_key_event(const char *action, const char *key_name, uint8_t key_code)
    {
      switch (str2int(key_name))
      {
      case str2int("NUM_LOCK"):
        if (strcmp(action, "down"))
          update_leds(!num_lock_state_, caps_lock_state_, scroll_lock_state_);
        break;
      case str2int("CAPS_LOCK"):
        if (strcmp(action, "down"))
          update_leds(num_lock_state_, !caps_lock_state_, scroll_lock_state_);
        break;
      case str2int("SCROLL_LOCK"):
        if (strcmp(action, "down"))
          update_leds(num_lock_state_, caps_lock_state_, !scroll_lock_state_);
        break;
      }
      on_key_event_callback.call(action, key_name, key_code);
      ESP_LOGI(TAG, "[Event] Action: %-4s Name: %-10s Code: 0x%02X", action, key_name, key_code);
    }

    void process_scancode(uint8_t code)
    {
      switch (code)
      {
      case protocol::RESP_ACK:
        switch (led_state_)
        {
        case LedState::SENT_CMD:
          led_state_ = LedState::GOT_CMD_ACK;
          ESP_LOGD(TAG, "[LedState] SENT_CMD -> GOT_CMD_ACK");
          break;
        case LedState::SENT_DATA:
          led_state_ = LedState::IDLE;
          ESP_LOGD(TAG, "[LedState] SENT_DATA -> IDLE");
          break;
        default:
          break;
        }
        break;
      case protocol::RESP_RESEND:
        sync_leds();
        break;
      case protocol::RESP_BAT_OK:
      case protocol::PREFIX_PAUSE:
        break;
      case protocol::PREFIX_EXTENDED:
        is_extended_ = true;
        break;
      case protocol::PREFIX_RELEASE:
        is_release_ = true;
        break;
      default:
        // 过滤 Extended Fake Shift (0xE0 0x12 / 0xE0 0x59)
        if (!is_extended_ || (code != 0x12 && code != 0x59))
        {
          const char *action = is_release_ ? "up" : "down";
          const char *key_name = scancode_to_name(code, is_extended_);

          // 过滤 PAUSE 虚拟出来的 Ctrl+NumLock
          if (l_ctrl_state_ && code == 0x77)
          {
            key_name = "KP_MINUS";
            code = 0x7F;
          }
          l_ctrl_state_ = code == 0x14;

          // 刚唤醒时会收到不完整的按键序列，此时忽略输入
          if (!wait_first_release_ || is_release_)
          {
            //补发因睡眠错过的key down事件
            if (wait_first_release_) {
              wait_first_release_ = false;
              handle_key_event("down", key_name, code);
            }

            handle_key_event(action, key_name, code);
          }
        }
        is_release_ = false;
        is_extended_ = false;
      }
    }

    void log_diagnostics()
    {
      ESP_LOGD(TAG,
               "[Diag] Triggers: %u, Glitches: %u, Frames: %u, Drops: %u, Invalids: %u, Timeouts: %u, Wakeups: %u, PM: %s",
               (unsigned int)diag_.triggers,
               (unsigned int)diag_.glitches,
               (unsigned int)diag_.frames,
               (unsigned int)diag_.drop_bits,
               (unsigned int)diag_.invalid_frames,
               (unsigned int)diag_.timeout_frames,
               (unsigned int)diag_.wakeups,
               is_pm_locked_ ? "LOCKED" : "UNLOCKED");

      diag_.reset();
    }

  public:
    esphome::CallbackManager<void(const char *, const char *, uint8_t)> on_key_event_callback;

    PS2KeyboardComponent(uint8_t clk_pin, uint8_t data_pin)
        : clk_pin_(static_cast<gpio_num_t>(clk_pin)),
          data_pin_(static_cast<gpio_num_t>(data_pin)) {}

    void setup() override
    {
      esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "ps2_lock", &pm_lock_);

      // GPIO 配置
      gpio_config_t io_conf = {};
      io_conf.mode = GPIO_MODE_INPUT_OUTPUT_OD;
      io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
      io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
      io_conf.intr_type = GPIO_INTR_DISABLE;

      // DATA 引脚
      io_conf.pin_bit_mask = (1ULL << data_pin_);
      gpio_config(&io_conf);
      gpio_set_level(data_pin_, 1);

      // CLK 引脚
      io_conf.pin_bit_mask = (1ULL << clk_pin_);
      gpio_config(&io_conf);
      gpio_set_level(clk_pin_, 1);

      // 配置 CLS 引脚 低电平唤醒
      esp_sleep_enable_gpio_wakeup();
      gpio_wakeup_enable(clk_pin_, GPIO_INTR_LOW_LEVEL);

      // 注册 ISR，传入 this 指针
      gpio_set_intr_type(clk_pin_, GPIO_INTR_NEGEDGE);
      gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
      gpio_isr_handler_add(clk_pin_, isr_clock_handler, this);

      // 恢复键盘状态
      send_reset();

      ESP_LOGI(TAG, "PS/2 Driver Initialized");
    }

    void loop() override
    {
      // 唤醒时或首次初始化
      if (!is_pm_locked_ && !is_sleeping_)
      {
        is_pm_locked_ = true;
        esp_pm_lock_acquire(pm_lock_);
        // write_byte(protocol::CMD_RESEND);
        // ESP_LOGD(TAG, "[PS/2] Send: CMD_RESEND");
        sync_leds();
      }

      // LED 同步
      if (led_state_ == LedState::PENDING && byte_queue_.empty() && check_idle(50000))
      {
        led_state_ = LedState::SENT_CMD;
        bool result = send_set_leds();
        ESP_LOGD(TAG, "[LedState] PENDING -> SENT_CMD: %d", result);
      }
      else if (led_state_ == LedState::GOT_CMD_ACK && byte_queue_.empty() && check_idle(50000))
      {
        led_state_ = LedState::SENT_DATA;
        bool result = send_set_leds_mask();
        bool num_lock = (led_mask_ & 0x02) != 0;
        bool caps_lock = (led_mask_ & 0x04) != 0;
        bool scroll_lock = (led_mask_ & 0x01) != 0;
        ESP_LOGD(TAG, "[LedState] GOT_CMD_ACK -> SENT_DATA: N%d C%d S%d", num_lock, caps_lock, scroll_lock);
      }
      else if (led_state_ != LedState::IDLE && byte_queue_.empty() && check_idle(1000000))
      {
        ESP_LOGI(TAG, "[LedState] Timeout, retry...");
        sync_leds(is_turn_off_leds);
      }

      // 消费接收队列
      uint8_t code = 0;
      while (byte_queue_.pop(code))
      {
        ESP_LOGD(TAG, "[Event] RawCode: 0x%02X ", code);
        process_scancode(code);
      }

      // 电源管理逻辑
      if (!is_turn_off_leds && is_pm_locked_ && check_idle(60000000))
      {
        sync_leds(true);
        is_turn_off_leds = true;
        ESP_LOGI(TAG, "Keyboard idle: Turn off LEDs");
      }
      else if (is_turn_off_leds && is_pm_locked_ && check_idle(3000000))
      {
        is_turn_off_leds = false;
        // config_gpio_for_sleep();
        is_sleeping_ = true;
        wait_first_release_ = true;

        esp_pm_lock_release(pm_lock_);
        is_pm_locked_ = false;

        log_diagnostics();
        ESP_LOGI(TAG, "Keyboard idle: Entering Sleep mode");
      }
    }

    void sync_leds(bool turnoff = false)
    {
      if (!is_pm_locked_ && is_sleeping_)
      {
        is_sleeping_ = false;
        // config_gpio_for_wakeup();
        is_pm_locked_ = true;
        esp_pm_lock_acquire(pm_lock_);
      }
      last_activity_time_ = esp_timer_get_time();
      led_mask_ = turnoff ? 0 : (num_lock_state_ ? 0x02 : 0) | (caps_lock_state_ ? 0x04 : 0) | (scroll_lock_state_ ? 0x01 : 0);
      led_state_ = LedState::PENDING;
    }

    void update_leds(bool num_lock, bool caps_lock, bool scroll_lock)
    {
      num_lock_state_ = num_lock;
      caps_lock_state_ = caps_lock;
      scroll_lock_state_ = scroll_lock;
      sync_leds();
    }

    void add_on_key_event_callback(std::function<void(const char *, const char *, uint8_t)> callback)
    {
      on_key_event_callback.add(std::move(callback));
    }
  };

} // namespace ps2