#pragma once
#include "driver/rmt_tx.h"
#include "esp_log.h"

#include <cstdint>
#include <cstring>

/**
 * @brief Native ESP-IDF replacement for HomeSpan's extras/Pixel.h (WS2812-class
 * single-wire addressable LEDs), built on the RMT TX driver exactly as in the
 * IDF rmt/led_strip example.
 *
 * Pixel channel order is configurable via a 3-byte map (R,G,B offsets within
 * the wire stream), mirroring HomeSpan's pixelType_t tables: RGB={0,1,2},
 * GRB={1,0,2}, etc. Only the single LED used by the feedback path is driven,
 * so the framebuffer is one pixel.
 */
class NeoPixel {
public:
  // Channel-order index, matching HomeSpan's PixelType tables.
  static constexpr uint8_t RGB = 0, RBG = 1, BRG = 2, BGR = 3, GBR = 4, GRB = 5;

  /// Initialize one LED on pin using the channel-order index; indices >= 6 use GRB.
  /// RMT setup failure leaves an inactive object whose set() calls do nothing.
  NeoPixel(int pin, uint8_t typeIndex) {
    // Each entry maps wire position -> source channel (0=r, 1=g, 2=b),
    // derived from HomeSpan's pixelType_t start-bit tables (Color union is
    // little-endian: red=bits 24-31, green=16-23, blue=8-15, white=0-7;
    // map[i] is the start bit of the channel sent i-th on the wire):
    //   RGB={31,23,15} -> R,G,B   GRB={23,31,15} -> G,R,B
    //   RBG={31,15,23} -> R,B,G   GBR={15,31,23} -> B,R,G
    //   BRG={23,15,31} -> G,B,R   BGR={15,23,31} -> B,G,R
    static constexpr uint8_t kMaps[6][3] = {
        {0, 1, 2}, // RGB
        {0, 2, 1}, // RBG
        {1, 2, 0}, // BRG
        {2, 1, 0}, // BGR
        {2, 0, 1}, // GBR
        {1, 0, 2}, // GRB
    };
    if (typeIndex >= 6) typeIndex = GRB;
    m_map = kMaps[typeIndex];

    rmt_tx_channel_config_t tx_chan_config = {};
    tx_chan_config.clk_src = RMT_CLK_SRC_DEFAULT;
    tx_chan_config.gpio_num = static_cast<gpio_num_t>(pin);
    tx_chan_config.mem_block_symbols = 64;
    tx_chan_config.resolution_hz = kResolution;
    tx_chan_config.trans_queue_depth = 4;
    if (rmt_new_tx_channel(&tx_chan_config, &m_chan) != ESP_OK) {
      ESP_LOGE(TAG, "rmt_new_tx_channel failed on pin %d", pin);
      m_chan = nullptr;
      return;
    }

    rmt_bytes_encoder_config_t bytes_encoder_config = {};
    bytes_encoder_config.bit0.level0 = 1;
    bytes_encoder_config.bit0.duration0 = 0.3 * kResolution / 1000000; // T0H=0.3us
    bytes_encoder_config.bit0.level1 = 0;
    bytes_encoder_config.bit0.duration1 = 0.9 * kResolution / 1000000; // T0L=0.9us
    bytes_encoder_config.bit1.level0 = 1;
    bytes_encoder_config.bit1.duration0 = 0.9 * kResolution / 1000000; // T1H=0.9us
    bytes_encoder_config.bit1.level1 = 0;
    bytes_encoder_config.bit1.duration1 = 0.3 * kResolution / 1000000; // T1L=0.3us
    bytes_encoder_config.flags.msb_first = 1;
    if (rmt_new_bytes_encoder(&bytes_encoder_config, &m_encoder) != ESP_OK) {
      ESP_LOGE(TAG, "rmt_new_bytes_encoder failed");
      rmt_del_channel(m_chan);
      m_chan = nullptr;
      return;
    }

    if (rmt_enable(m_chan) != ESP_OK) {
      ESP_LOGE(TAG, "rmt_enable failed");
      rmt_del_encoder(m_encoder);
      rmt_del_channel(m_chan);
      m_encoder = nullptr;
      m_chan = nullptr;
    }
  }

  ~NeoPixel() {
    if (m_encoder) rmt_del_encoder(m_encoder);
    if (m_chan) rmt_del_channel(m_chan);
  }

  NeoPixel(const NeoPixel&) = delete;
  NeoPixel& operator=(const NeoPixel&) = delete;

  /// Return whether RMT initialization succeeded.
  explicit operator bool() const { return m_chan != nullptr; }

  /// Pack 8-bit RGB intensities in this LED's wire order for set().
  uint32_t rgb(uint8_t r, uint8_t g, uint8_t b) const {
    uint8_t wire[3] = {r, g, b};
    return (static_cast<uint32_t>(wire[m_map[0]]) << 16) |
           (static_cast<uint32_t>(wire[m_map[1]]) << 8) |
           static_cast<uint32_t>(wire[m_map[2]]);
  }

  /// Transmit the low 24 bits in wire order and wait for completion without a timeout.
  /// An inactive object or rejected transmission produces no update and reports no error.
  void set(uint32_t wireRgb) {
    if (!m_chan) return;
    uint8_t bytes[3] = {
        static_cast<uint8_t>(wireRgb >> 16),
        static_cast<uint8_t>(wireRgb >> 8),
        static_cast<uint8_t>(wireRgb),
    };
    rmt_transmit_config_t tx_config = {};
    tx_config.loop_count = 0;
    if (rmt_transmit(m_chan, m_encoder, bytes, sizeof(bytes), &tx_config) == ESP_OK) {
      rmt_tx_wait_all_done(m_chan, portMAX_DELAY);
    }
  }

  /// Send a zero-intensity pixel using the same blocking behavior as set().
  void off() { set(0); }

private:
  static constexpr uint32_t kResolution = 10000000; // 10 MHz, 1 tick = 0.1us
  static constexpr const char* TAG = "NeoPixel";
  rmt_channel_handle_t m_chan = nullptr;
  rmt_encoder_handle_t m_encoder = nullptr;
  const uint8_t* m_map = nullptr;
};
