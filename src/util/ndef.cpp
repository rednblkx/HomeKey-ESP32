/*
  Code highly inspired by https://github.com/kormax/apple-home-key-reader/blob/main/util/ndef.py
 */

#include <util/ndef.h>
NDEFRecord::NDEFRecord(){
  this->id.assign(1, '\0');
  this->type.assign(1, '\0');
  this->data.assign(1, '\0');
  this->tnf = 0x00;
}

NDEFRecord::NDEFRecord(const char *id, unsigned char tnf, const char *type, unsigned char *data, size_t dataLen)
{
  this->id.insert(this->id.begin(), id, id + strlen(id));
  this->id.push_back('\0');
  this->type.insert(this->type.begin(), type, type + strlen(type));
  this->type.push_back('\0');
  this->data.insert(this->data.begin(), data, data + dataLen);
  this->data.push_back('\0');
  this->tnf = tnf;
}

NDEFRecord::NDEFRecord(std::vector<unsigned char> id, unsigned char tnf, std::vector<unsigned char> type, std::vector<unsigned char> data){
  this->id = id;
  this->type = type;
  this->data = data;
  this->tnf = tnf;
}

NDEFMessage::NDEFMessage(unsigned char *data, size_t length){
  this->packedData.insert(this->packedData.begin(), data, data + length);
}

NDEFMessage::NDEFMessage(std::initializer_list<NDEFRecord> records)
{
  this->records.assign(records);
}

std::vector<unsigned char> NDEFMessage::pack()
{
  unsigned char result[256];
  size_t olen = 0;
  for (size_t i = 0; i < this->records.size(); i++)
  {
    unsigned char id_length = this->records.data()[i].id.size() - 1;
    unsigned char type_length = this->records.data()[i].type.size() - 1;
    unsigned char payload_length = this->records.data()[i].data.size() - 1;
    unsigned char mb = (1 << 8) >> 1 * (i == 0);
    unsigned char me = (1 << 8) >> 2 * (i == this->records.size() - 1);
    unsigned char ch = 0x0;
    unsigned char sr = (1 << 8) >> 4 * (payload_length <= 255);
    unsigned char il = (1 << 8) >> 5 * (id_length > 0);
    unsigned char tnf = this->records.data()[i].tnf;
    unsigned char header = mb + me + ch + sr + il + tnf;
    unsigned char packed[sizeof(header) + 1 + 1 + (id_length > 0 ? 1 : 0) + type_length + id_length + payload_length] = {
        header,
        (unsigned char)type_length,
        (unsigned char)payload_length,
        id_length,
    };
    memcpy(packed + sizeof(header) + 1 + 1 + (id_length > 0 ? 1 : 0), this->records.data()[i].type.data(), type_length);
    memcpy(packed + sizeof(header) + 1 + 1 + (id_length > 0 ? 1 : 0) + type_length, this->records.data()[i].id.data(), id_length);
    memcpy(packed + sizeof(header) + 1 + 1 + (id_length > 0 ? 1 : 0) + type_length + id_length, this->records.data()[i].data.data(), payload_length);
    memcpy(result + olen, packed, sizeof(packed));
    olen += sizeof(packed);
  }
  this->packedData.clear();
  this->packedData.insert(this->packedData.begin(), result, result + olen);
  LOG(D, "NDEF MSG PACKED - LENGTH: %d, DATA: %s", packedData.size(), utils::bufToHexString(packedData.data(), packedData.size()).c_str());
  return this->packedData;
}

std::vector<NDEFRecord> NDEFMessage::unpack(){
  std::vector<NDEFRecord> records;
  unsigned char header[1];
  unsigned char type_length[1];
  unsigned char payload_length[1];
  unsigned char id_length[1];
  size_t i = 0;
  while(i < this->packedData.size())
  {
    header[0] = this->packedData.data()[i];
    i++;
    unsigned char sr = ((header[0] >> 4 << 7) & 0xFF) >> 3;
    unsigned char il = ((header[0] >> 3 << 7) & 0xFF) >> 4;
    unsigned char tnf = ((header[0] << 5) & 0xFF) >> 5;

    type_length[0] = this->packedData.data()[i];
    i++;

    if(sr){
      payload_length[0] = this->packedData.data()[i];
      i++;
    }
    else
    {
      payload_length[0] = this->packedData.data()[i];
      i += 4;
    }

    if(il){
      id_length[0] = this->packedData.data()[i];
      i++;
    }
    else
    {
      id_length[0] = 0;
    }

    std::vector<unsigned char> type_vec;
    type_vec.insert(type_vec.begin(), this->packedData.data() + i, this->packedData.data() + i + type_length[0]);
    type_vec.push_back('\0');
    i += type_length[0];

    std::vector<unsigned char> id_vec;
    id_vec.resize(id_length[0] + 1);
    id_vec.insert(id_vec.begin(), this->packedData.data() + i, this->packedData.data() + i + id_length[0]);
    id_vec.push_back('\0');
    i += id_length[0];

    std::vector<unsigned char> payload_vec;
    payload_vec.insert(payload_vec.begin(), this->packedData.data() + i, this->packedData.data() + i + payload_length[0]);
    payload_vec.push_back('\0');
    i += payload_length[0];
    
    LOG(D, "NDEF RECORD ID: %s, TNF: %d, TYPE: %s, PAYLOAD: %s", utils::bufToHexString(id_vec.data(), id_vec.size()).c_str(), utils::bufToHexString(&tnf, 1).c_str(), utils::bufToHexString(type_vec.data(), type_vec.size()).c_str(), utils::bufToHexString(payload_vec.data(), payload_vec.size()).c_str());
    records.emplace_back(id_vec, tnf, type_vec, payload_vec);
  }
  this->records.insert(this->records.begin(), records.data(), records.data() + records.size());
  return this->records;
}

NDEFRecord* NDEFMessage::findType(const char * type){
  NDEFRecord *foundRecord = nullptr;
  for (auto &&record : records)
  {
    if(!strcmp(type, (const char *)record.type.data())){
      foundRecord = &record;
      break;
    }
  }
  LOG(D, "NDEF RECORD ID: %s, TNF: %s, TYPE: %s, PAYLOAD: %s", utils::bufToHexString(foundRecord->id.data(), foundRecord->id.size()).c_str(), utils::bufToHexString(&foundRecord->tnf, 1).c_str(), utils::bufToHexString(foundRecord->type.data(), foundRecord->type.size()).c_str(), utils::bufToHexString(foundRecord->data.data(), foundRecord->data.size()).c_str());
  return foundRecord;
}