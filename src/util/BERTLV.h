/*
  Code highly inspired by https://github.com/kormax/apple-home-key-reader/blob/main/util/tlv.py
 */

#ifndef BERTLV_H
#define BERTLV_H

#include <iostream>
#include <vector>
#include <cstdint>

enum BERTLVTagClass {
    UNIVERSAL = 0b00,
    APPLICATION = 0b01,
    CONTEXT_SPECIFIC = 0b10,
    PRIVATE = 0b11
};

class BERTLVTag {
public:
    std::vector<uint8_t> data;

    BERTLVTag(int data);

    BERTLVTag(const std::vector<uint8_t>& data);

    std::vector<uint8_t> pack() const;

    static BERTLVTag unpack(const std::vector<uint8_t>& data);
};

class BERTLVLength {
public:
    std::vector<uint8_t> data;

    BERTLVLength(const std::vector<uint8_t>& data);

    int getInt() const;

    bool isIndefinite() const;

    int getValue() const;

    std::vector<uint8_t> pack() const;

    static BERTLVLength unpack(const std::vector<uint8_t>& data);

private:
    int fromBytes(const std::vector<uint8_t>& bytes) const;
};

class BERTLV {
public:
    BERTLVTag tag;
    BERTLVLength length;
    std::vector<uint8_t> value;

    BERTLV(int tag, const std::vector<uint8_t> length, const std::vector<uint8_t>& value);

    BERTLV(const BERTLVTag& tag, const BERTLVLength& length, const std::vector<uint8_t>& value);

    std::vector<uint8_t> pack() const;

    static BERTLV unpack(const std::vector<uint8_t>& data);
    static std::vector<BERTLV> unpack_array(const std::vector<uint8_t> &data);
};

#endif // BERTLV_H
