#pragma once
#ifndef BERTLV_H
#define BERTLV_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <util/utils.h>

enum BERTLVTagClass {
    UNIVERSAL = 0b00,
    APPLICATION = 0b01,
    CONTEXT_SPECIFIC = 0b10,
    PRIVATE = 0b11
};

class BERTLVLength {
public:
    std::vector<uint8_t> data;

    BERTLVLength();

    BERTLVLength(const std::vector<uint8_t>& data);

    int getInt() const;

    bool isIndefinite() const;

    int getValue() const;

    std::vector<uint8_t> pack() const;

    static BERTLVLength unpack(const std::vector<uint8_t>& data);

private:
    std::vector<uint8_t> toBytes(int value) const;

    int fromBytes(const std::vector<uint8_t>& bytes) const;
};

class BERTLV {
public:
    std::vector<uint8_t> tag;
    BERTLVLength length;
    std::vector<uint8_t> value;

    BERTLV();

    BERTLV(int tag, const std::vector<uint8_t> length, const std::vector<uint8_t>& value);

    BERTLV(const std::vector<uint8_t>& tag, const BERTLVLength& length, const std::vector<uint8_t>& value);

    std::vector<uint8_t> pack() const;
    static BERTLV findTag(int tag, std::vector<BERTLV> data);

    static BERTLV unpack(const std::vector<uint8_t>& data);
    static std::vector<BERTLV> unpack_array(const std::vector<uint8_t> &data);
    static std::vector<BERTLV> unpack_array(const uint8_t *data, const size_t len);
};

#endif // BERTLV_H
