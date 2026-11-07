#include <array>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <vector>

using TickType_t = uint32_t;
using esp_err_t = int;
constexpr int ESP_OK = 0, ESP_FAIL = -1, ESP_ERR_TIMEOUT = 0x107,
              ESP_ERR_INVALID_STATE = 0x103, ESP_ERR_NO_MEM = 0x101;
constexpr unsigned portTICK_PERIOD_MS = 1;
uint32_t ticks = 0;
uint32_t xTaskGetTickCount() { return ticks; }
#define pdMS_TO_TICKS(value) (value)
#define ESP_LOGW(...) ((void)0)
#define ESP_LOGI(...) ((void)0)
#define ESP_LOGE(...) ((void)0)
#define ESP_LOGD(...) ((void)0)
#define ESP_LOG_BUFFER_HEXDUMP(...) ((void)0)

namespace nci {
constexpr int STATUS_OK = 0, INTF_ISODEP = 2, PROT_ISODEP = 4,
  DEACTIVATION_TYPE_DISCOVERY = 3, PKT_MT_CTRL_COMMAND = 1, CORE_GID = 0,
  CORE_GET_CONFIG_OID = 3, PN7160_DEFAULT_TIMEOUT_MS = 1000;
}
struct NciMessage {
  std::vector<uint8_t> bytes = {0, 0, 0};
  NciMessage() = default;
  template<size_t N> NciMessage(int, int, int, const uint8_t (&)[N]) {}
  const uint8_t* get_payload_ptr() const { return bytes.data() + 3; }
  uint8_t get_len() const { return bytes[2]; }
  size_t size() const { return bytes.size(); }
  uint8_t operator[](size_t index) const { return bytes[index]; }
};
enum class NciEventType { RF_INTF_ACTIVATED, RF_DISCOVER, RF_DEACTIVATE, SHUTDOWN };
struct NciEvent { NciEventType type = NciEventType::SHUTDOWN; NciMessage msg; };
struct ScriptedNci {
  bool initialized = true, present = true;
  int eventStatus = ESP_ERR_TIMEOUT, probeStatus = ESP_OK, healthStatus = ESP_OK;
  unsigned polls = 0, probes = 0, releases = 0, healthProbes = 0;
  NciEvent event;
  bool is_initialized() const { return initialized; }
  int get_event(NciEvent& out, uint32_t) { ++polls; out = event; return eventStatus; }
  int rf_iso_dep_presence_check() { ++probes; return probeStatus; }
  bool tag_in_field() const { return present; }
  int rf_deactivate(uint8_t) { ++releases; return ESP_OK; }
  int send_command_wait_response(const NciMessage&, NciMessage&, uint32_t) {
    ++healthProbes; return healthStatus;
  }
};
struct Pn7160Reader {
  ScriptedNci* m_nci = nullptr;
  bool m_connected = true;
  uint8_t m_currentProtocol = nci::PROT_ISODEP, m_fwMajor = 1, m_fwMinor = 2;
  uint32_t m_lastPresenceCheck = 0, m_lastHealthCheckTick = 0, m_lastActivation = 0;
  static constexpr uint32_t kPresenceCheckIntervalMs = 500,
    kHealthCheckIntervalMs = 10000, kActivationCooldownWindowMs = 300;
  bool pollForTag(std::vector<uint8_t>&, std::array<uint8_t, 2>&, uint8_t&, uint32_t);
  bool isTagStillPresent();
  void releaseTag();
  bool healthCheck();
  bool beginDiscovery() { return true; }
};
#include "reader_methods.inc"

int main() {
  ScriptedNci nci;
  Pn7160Reader reader; reader.m_nci = &nci;
  std::vector<uint8_t> uid; std::array<uint8_t, 2> atqa{}; uint8_t sak = 0;
  nci.eventStatus = ESP_OK; nci.event.type = NciEventType::RF_INTF_ACTIVATED;
  nci.event.msg.bytes = {0x61, 5, 17, 1, 2, 4, 0, 255, 1, 10,
                        0x44, 0, 4, 1, 2, 3, 4, 1, 0x20, 0};
  assert(reader.pollForTag(uid, atqa, sak, 10) && reader.m_connected);
  assert((uid == std::vector<uint8_t>{1, 2, 3, 4}) && atqa[0] == 0x44 && sak == 0x20);
  nci.eventStatus = ESP_ERR_TIMEOUT;
  assert(!reader.pollForTag(uid, atqa, sak, 10) && reader.m_connected);
  assert(reader.healthCheck() && nci.healthProbes == 0); // Idle timeout is healthy.
  nci.initialized = false;
  assert(!reader.pollForTag(uid, atqa, sak, 10) && !reader.m_connected);
  nci.initialized = true;
  for (int error : {ESP_FAIL, ESP_ERR_INVALID_STATE, ESP_ERR_NO_MEM}) {
    reader.m_connected = true; nci.eventStatus = error;
    assert(!reader.pollForTag(uid, atqa, sak, 10) && !reader.m_connected);
    assert(!reader.healthCheck() && nci.healthProbes == 0);
    unsigned releases = nci.releases;
    reader.releaseTag(); assert(nci.releases == releases);
  }
  reader.m_connected = true; nci.eventStatus = ESP_ERR_TIMEOUT;
  ticks = 499; assert(reader.isTagStillPresent() && nci.probes == 0);
  ticks = 500; assert(reader.isTagStillPresent() && nci.probes == 1);
  nci.present = false; ticks = 501; assert(!reader.isTagStillPresent());
  nci.present = true; nci.probeStatus = ESP_FAIL; ticks = 1000;
  assert(!reader.isTagStillPresent() && !reader.m_connected);
  unsigned probes = nci.probes, polls = nci.polls, releases = nci.releases;
  ticks = 1001;
  assert(!reader.isTagStillPresent() && nci.probes == probes);
  assert(!reader.pollForTag(uid, atqa, sak, 10) && nci.polls == polls);
  assert(!reader.healthCheck() && nci.healthProbes == 0);
  reader.releaseTag(); assert(nci.releases == releases);
  reader.m_connected = true; reader.m_nci = nullptr;
  assert(!reader.isTagStillPresent() && !reader.m_connected);
  reader.m_nci = &nci; reader.m_connected = true; nci.probeStatus = ESP_OK;
  assert(reader.isTagStillPresent()); // Reinitialized state resumes probing.
  reader.releaseTag(); assert(nci.releases == releases + 1);
  ticks = 10000; nci.healthStatus = ESP_FAIL;
  assert(!reader.healthCheck() && !reader.m_connected && nci.healthProbes == 1);
  std::puts("PN7160 idle timeout, polling faults and cached-presence recovery passed");
}
