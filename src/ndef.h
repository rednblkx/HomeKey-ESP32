#include <cstddef>
#include <tuple>
#include <string.h>

class NDEFRecord
{
public:
  NDEFRecord(const char *id, uint8_t tnf, const char *type, uint8_t *data, size_t dataLen);
  uint8_t *id;
  uint8_t tnf;
  uint8_t *type;
  uint8_t *data;
  size_t dataLen;
};

class NDEFMessage
{
public:
  NDEFMessage(NDEFRecord *records, size_t length);
  std::tuple<uint8_t *, size_t> pack();

private:
  NDEFRecord *records;
  size_t recordsLen = 0;
};