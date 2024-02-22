/*
  Code highly inspired by https://github.com/kormax/apple-home-key-reader/blob/main/util/ndef.py
 */

#ifndef NDEF_H
#define NDEF_H

#include <tuple>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <vector>
#include <initializer_list>
#include <util/utils.h>

class NDEFRecord
{
  public:
    NDEFRecord();
    NDEFRecord(const char *id, unsigned char tnf, const char *type, unsigned char *data, size_t dataLen);
    NDEFRecord(std::vector<unsigned char> id, unsigned char tnf, std::vector<unsigned char> type, std::vector<unsigned char> data);
    std::vector<unsigned char> id;
    unsigned char tnf;
    std::vector<unsigned char> type;
    std::vector<unsigned char> data;
};

class NDEFMessage
{
public:
  NDEFMessage(unsigned char *data, size_t length);
  NDEFMessage(std::initializer_list<NDEFRecord>);
  std::vector<unsigned char> pack();
  std::vector<NDEFRecord> unpack();
  NDEFRecord* findType(const char * type);

private:
  const char *TAG = "NDEFMessage";
  std::vector<NDEFRecord> records;
  std::vector<unsigned char> packedData;
};

#endif