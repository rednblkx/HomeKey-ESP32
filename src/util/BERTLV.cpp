#include <iostream>
#include <vector>
#include <cstdint>
#include <util/BERTLV.h>

BERTLVLength::BERTLVLength(){}

BERTLVLength::BERTLVLength(const std::vector<uint8_t> &data)
{
  if (data[0] <= 127)
  {
    this->data.push_back(data[0]);
  }
  else
  {
    this->data = data;
  }
}

int BERTLVLength::getInt() const
{
  return getValue();
}

bool BERTLVLength::isIndefinite() const
{
  return (data[0] & 0b01111111) == 0;
}

int BERTLVLength::getValue() const
{
  uint8_t lengthBaseData = data[0];
  bool lengthFormIsSimple = !(lengthBaseData & 0b10000000);
  if (lengthFormIsSimple)
  {
    return lengthBaseData & 0b01111111;
  }
  else
  {
    uint8_t lengthLength = lengthBaseData & 0b01111111;
    if (lengthLength)
    {
      // Definite form
      std::vector<uint8_t> lengthData(data.begin() + 1, data.begin() + 1 + lengthLength);
      return fromBytes(lengthData);
    }
    else
    {
      // Indefinite form
      std::vector<uint8_t> lengthData;
      size_t index = 1;
      while (!lengthData.empty() && !(lengthData[lengthData.size() - 1] == 0x00 && lengthData[lengthData.size() - 2] == 0x00))
      {
        lengthData.push_back(data[index]);
        ++index;
      }
      return fromBytes(lengthData);
    }
  }
}

std::vector<uint8_t> BERTLVLength::pack() const
{
  return data;
}

BERTLVLength BERTLVLength::unpack(const std::vector<uint8_t> &data)
{
    size_t index = 0;
    std::vector<uint8_t> result;
    uint8_t lengthBaseData = data[index];
    result.push_back(data[index]);
    ++index;

    bool lengthFormIsSimple = !(lengthBaseData & 0b10000000);
    if (lengthFormIsSimple) {
        return BERTLVLength(result);
    } else {
        uint8_t lengthLength = lengthBaseData & 0b01111111;

        if (lengthLength) {
            // Definite form
            result.insert(result.end(), data.begin() + index, data.begin() + index + lengthLength);
            index += lengthLength;
            if (result.size() != lengthLength + 1) {
                // throw std::runtime_error("Bad format");
            }
        } else {
            // Indefinite form
            while (!result.empty() && !(result[result.size() - 1] == 0x00 && result[result.size() - 2] == 0x00)) {
                result.push_back(data[index]);
                ++index;
            }
        }
        return BERTLVLength(result);
    }
}

int BERTLVLength::fromBytes(const std::vector<uint8_t> &bytes) const
{
  int result = 0;
  for (size_t i = 0; i < bytes.size(); ++i)
  {
    result <<= 8;
    result |= bytes[i];
  }
  return result;
}

BERTLV::BERTLV() : tag(std::vector<uint8_t>()), length(BERTLVLength()), value(std::vector<uint8_t>()) {}

BERTLV::BERTLV(int tag, const std::vector<uint8_t> length, const std::vector<uint8_t> &value)
    : tag(tag), length(length), value(value) {}

BERTLV::BERTLV(const std::vector<uint8_t> &tag, const BERTLVLength &length, const std::vector<uint8_t> &value)
    : tag(tag), length(length), value(value) {}

std::vector<uint8_t> BERTLV::pack() const
{
  std::vector<uint8_t> result;
  result.insert(result.end(), tag.begin(), tag.end());
  result.insert(result.end(), length.data.begin(), length.data.end());
  result.insert(result.end(), value.begin(), value.end());
  return result;
}

std::vector<BERTLV> BERTLV::unpack_array(const std::vector<uint8_t>& data) {
  std::vector<BERTLV> result;
  size_t index = 0;

  while (index < data.size()) {
    std::vector<uint8_t> tag(data.begin() + index, data.begin() + index + 1);
    index += 1;
    BERTLVLength length = BERTLVLength::unpack(std::vector<uint8_t>(data.begin() + index, data.begin() + index + 8));
    index += length.data.size();
    std::vector<uint8_t> value(data.begin() + index, data.begin() + index + length.getValue());
    result.emplace_back(tag, length, value);
    index += value.size();
    ESP_LOGD("BERTLV::unpack_array", "TLV %02x[%d]: %s", tag.data()[0], length.data.data()[0], utils::bufToHexString(value.data(), value.size()).c_str());
  }

  return result;
}

std::vector<BERTLV> BERTLV::unpack_array(const uint8_t *data, const size_t len) {
  std::vector<BERTLV> result;
  size_t index = 0;

  while (index < len) {
    std::vector<uint8_t> tag(data + index, data + index + 1);
    index += 1;
    BERTLVLength length = BERTLVLength::unpack(std::vector<uint8_t>(data + index, data + index + 8));
    index += length.data.size();
    std::vector<uint8_t> value(data + index, data + index + length.getValue());
    result.emplace_back(tag, length, value);
    index += value.size();
    ESP_LOGD("BERTLV::unpack_array", "TLV %02x[%d]: %s", tag.data()[0], length.data.data()[0], utils::bufToHexString(value.data(), value.size()).c_str());
  }

  return result;
}

BERTLV BERTLV::unpack(const std::vector<uint8_t> &data)
{
  size_t index = 0;
  std::vector<uint8_t> tag(data.begin() + index, data.begin() + index + 1);
  index += tag.size();
  BERTLVLength length = BERTLVLength::unpack(std::vector<uint8_t>(data.begin() + index, data.begin() + index + 8));
  index += length.data.size();
  std::vector<uint8_t> value(data.begin() + index, data.begin() + index + length.getValue());
  ESP_LOGD("BERTLV::unpack", "tag: %02x, length: %02x, value: %s", tag.data()[0], length.data.data()[0], utils::bufToHexString(value.data(), value.size()).c_str());
  return BERTLV(tag, length, value);
}

BERTLV BERTLV::findTag(int tag, std::vector<BERTLV> data){
  for(auto& tlv:data){
    if(tlv.tag.data()[0] == tag){
      return tlv;
    }
  }
  return BERTLV();
}