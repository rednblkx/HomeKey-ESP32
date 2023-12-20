#include <ndef.h>

NDEFRecord::NDEFRecord(const char *id, uint8_t tnf, const char *type, uint8_t *data, size_t dataLen)
{
  this->id = (uint8_t *)id;
  this->tnf = tnf;
  this->type = (uint8_t *)type;
  this->data = (uint8_t *)data;
  this->dataLen = dataLen;
}

NDEFMessage::NDEFMessage(NDEFRecord *records, size_t length)
{
  this->records = records;
  this->recordsLen = length;
}

std::tuple<uint8_t *, size_t> NDEFMessage::pack()
{
  uint8_t *result = new uint8_t[128];
  size_t olen = 0;
  for (size_t i = 0; i < this->recordsLen; i++)
  {
    uint8_t mb = (1 << 8) >> 1 * (i == 0);
    uint8_t me = (1 << 8) >> 2 * (i == recordsLen - 1);
    uint8_t ch = 0x0;
    uint8_t sr = (1 << 8) >> 4 * (this->records[i].dataLen <= 255);
    uint8_t il = (1 << 8) >> 5 * (strlen((const char *)this->records[i].id) > 0);
    uint8_t tnf = this->records[i].tnf;
    uint8_t header = mb + me + ch + sr + il + tnf;
    // printf("%x\n", header);
    uint8_t id_length = strlen((const char *)this->records[i].id) > 0 ? strlen((const char *)this->records[i].id) : 0;
    uint8_t packed[sizeof(header) + 1 + 1 + (id_length > 0 ? sizeof(id_length) : 0) + strlen((const char *)this->records[i].type) + strlen((const char *)this->records[i].id) + this->records[i].dataLen] = {
        header,
        (uint8_t)strlen((const char *)this->records[i].type),
        static_cast<uint8_t>(this->records[i].dataLen),
        id_length,
    };
    memcpy(packed + sizeof(header) + 1 + 1 + (id_length > 0 ? sizeof(id_length) : 0), this->records[i].type, strlen((const char *)this->records[i].type));
    memcpy(packed + sizeof(header) + 1 + 1 + (id_length > 0 ? sizeof(id_length) : 0) + strlen((const char *)this->records[i].type), this->records[i].id, strlen((const char *)this->records[i].id));
    memcpy(packed + sizeof(header) + 1 + 1 + (id_length > 0 ? sizeof(id_length) : 0) + strlen((const char *)this->records[i].type) + strlen((const char *)this->records[i].id), this->records[i].data, this->records[i].dataLen);
    memcpy(result + olen, packed, sizeof(packed));
    olen += sizeof(packed);
  }
  return std::make_tuple(result, olen);
}
