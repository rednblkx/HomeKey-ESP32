CUSTOM_CHAR(ConfigurationState, 263, PR+EV, UINT16, 0, 0, 1, true)
CUSTOM_CHAR(HardwareFinish, 264, PR, TLV_ENC, NULL_TLV, NULL_TLV, NULL_TLV, true)
CUSTOM_CHAR(NFCAccessControlPoint, 264, PR+PW+WR, TLV_ENC, NULL_TLV, NULL_TLV, NULL_TLV, true)
CUSTOM_CHAR(NFCAccessSupportedConfiguration, 265, PR, TLV_ENC, NULL_TLV, NULL_TLV, NULL_TLV, true)
CUSTOM_CHAR(LockControlPoint, 19, PW, TLV_ENC, NULL_TLV, NULL_TLV, NULL_TLV, true)

namespace Service
{
  struct LockManagement : SpanService
  {
    LockManagement() : SpanService{ "44","LockManagement",true } {
      req.push_back(&_CUSTOM_LockControlPoint);
      req.push_back(&hapChars.Version);
    }
  };
  struct NFCAccess : SpanService
  {
    NFCAccess() : SpanService{ "266","NFCAccess",true } {
      req.push_back(&_CUSTOM_ConfigurationState);
      req.push_back(&_CUSTOM_NFCAccessControlPoint);
      req.push_back(&_CUSTOM_NFCAccessSupportedConfiguration);
    }
  };
}