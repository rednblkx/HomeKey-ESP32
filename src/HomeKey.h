#pragma once
#include <ctime>
#include <list>
#include <nlohmann/json.hpp>
#include <TLV8.h>

using nlohmann::literals::operator""_json;
using namespace nlohmann::literals;
using namespace nlohmann::json_literals;
using namespace nlohmann::literals::json_literals;
using namespace nlohmann;

typedef enum
{
  kTLVType1_Operation = 0x01,
  kTLVType1_Device_Credential_Request = 0x04,
  kTLVType1_Device_Credential_Response = 0x05,
  kTLVType1_Reader_Key_Request = 0x06,
  kTLVType1_Reader_Key_Response = 0x07,
} Reader_Tags;

typedef enum
{
  kReader_Req_Key_Type = 0x01,
  kReader_Req_Reader_Private_Key = 0x02,
  kReader_Req_Identifier = 0x03,
  kReader_Req_Key_Identifier = 0x04, // This is only relevant for "remove" operation
  kRequest_Reader_Key_Request = 0x06
} Reader_Key_Request;

const TLV8_names RK_REQ_TLV_NAMES[] = {
  {kReader_Req_Key_Type,"KEY.TYPE"},
  {kReader_Req_Reader_Private_Key,"PRIV.KEY"},
  {kReader_Req_Identifier,"UNIQUE.IDENTIFIER"},
  {kReader_Req_Key_Identifier,"KEY.IDENTIFIER"},
  {kRequest_Reader_Key_Request,"READER.REQ"}
};

typedef enum
{
  kReader_Res_Key_Identifier = 0x01,
  kReader_Res_Status = 0x02,
  kReader_Res_Reader_Key_Response = 0x07
} Reader_Key_Response;

const TLV8_names RK_RES_TLV_NAMES[] = {
  {kReader_Res_Key_Identifier,"KEY.IDENTIFIER"},
  {kReader_Res_Status,"STATUS"},
  {kReader_Res_Reader_Key_Response,"READER.RES"}
};

typedef enum
{
  kDevice_Req_Key_Type = 0x01,
  kDevice_Req_Public_Key = 0x02,
  kDevice_Req_Issuer_Key_Identifier = 0x03,
  kDevice_Req_Key_State = 0x04,
  kDevice_Req_Key_Identifier = 0x05 // This is only relevant for "remove" operation
} Device_Credential_Request;

const TLV8_names DCR_REQ_TLV_NAMES[] = {
  {kDevice_Req_Key_Type,"KEY.TYPE"},
  {kDevice_Req_Public_Key,"PUBLIC.KEY"},
  {kDevice_Req_Issuer_Key_Identifier,"ISSUER.IDENTIFIER"},
  {kDevice_Req_Key_State,"KEY.STATE"},
  {kDevice_Req_Key_Identifier,"KEY.IDENTIFIER"},
};

typedef enum
{
  kDevice_Res_Key_Identifier = 0x01,
  kDevice_Res_Issuer_Key_Identifier = 0x02,
  kDevice_Res_Status = 0x03,
  kDevice_Credential_Response = 0x05
} Device_Credential_Response;

const TLV8_names DCR_RES_TLV_NAMES[] = {
  {kDevice_Req_Issuer_Key_Identifier,"ISSUER.IDENTIFIER"},
  {kDevice_Res_Status,"STATUS"},
  {kDevice_Res_Status,"STATUS"},
  {kDevice_Credential_Response,"DCR.RES"}
};

typedef enum
{
  kEndpoint_Public_Key = 0x86,
  kAuth0_Cryptogram = 0x9D,
  kAuth0Status = 0x90
} AUTH0_RESPONSE;

typedef enum
{
  kNDEF_MESSAGE = 0x53,
  kEnv1Status = 0x90
} ENVELOPE_RESPONSE;

  namespace endpointEnrollment
  {
    struct enrollment_t
    {
      std::time_t unixTime = 0;
      std::list<uint8_t> payload;
    };
    inline void to_json(json &j, const enrollment_t &p)
    {
      j = json{{"unixTime", p.unixTime}, {"payload", p.payload}};
    }

    inline void from_json(const json &j, enrollment_t &p)
    {
      j.at("unixTime").get_to(p.unixTime);
      j.at("payload").get_to(p.payload);
    }
  }

  namespace endpointEnrollments
  {
    struct enrollments_t
    {
      endpointEnrollment::enrollment_t hap;
      endpointEnrollment::enrollment_t attestation;
    };
    inline void to_json(json &j, const enrollments_t &p)
    {
      j = json{{"hap", p.hap}, {"attestation", p.attestation}};
    }

    inline void from_json(const json &j, enrollments_t &p)
    {
      j.at("hap").get_to(p.hap);
      j.at("attestation").get_to(p.attestation);
    }
  }

  namespace homeKeyEndpoint
  {
    struct endpoint_t
    {
      uint8_t endpointId[6] = {};
      uint32_t last_used_at = 0;
      int counter = 0;
      int key_type = 0;
      uint8_t publicKey[65] = {};
      uint8_t endpoint_key_x[32] = {};
      uint8_t persistent_key[32] = {};
      endpointEnrollments::enrollments_t enrollments;
    };
    inline void to_json(json &j, const endpoint_t &p)
    {
      j = json{{"endpointId", p.endpointId}, {"last_used_at", p.last_used_at}, {"counter", p.counter}, {"key_type", p.key_type}, {"publicKey", p.publicKey}, {"endpoint_key_x", p.endpoint_key_x}, {"persistent_key", p.persistent_key}, {"enrollments", p.enrollments}};
    }

    inline void from_json(const json &j, endpoint_t &p)
    {
      j.at("endpointId").get_to(p.endpointId);
      j.at("last_used_at").get_to(p.last_used_at);
      j.at("counter").get_to(p.counter);
      j.at("key_type").get_to(p.key_type);
      j.at("publicKey").get_to(p.publicKey);
      j.at("endpoint_key_x").get_to(p.endpoint_key_x);
      j.at("persistent_key").get_to(p.persistent_key);
      j.at("enrollments").get_to(p.enrollments);
    }
  }
  namespace homeKeyIssuer
  {
    struct issuer_t
    {
      uint8_t issuerId[8] = {};
      uint8_t publicKey[32] = {};
      uint8_t issuer_key_x[32] = {};
      std::list<homeKeyEndpoint::endpoint_t> endpoints;
    };
    inline void to_json(json &j, const issuer_t &p)
    {
      j = json{{"issuerId", p.issuerId}, {"publicKey", p.publicKey}, {"issuer_key_x", p.issuer_key_x}, {"endpoints", p.endpoints}};
    }

    inline void from_json(const json &j, issuer_t &p)
    {
      j.at("issuerId").get_to(p.issuerId);
      j.at("publicKey").get_to(p.publicKey);
      j.at("issuer_key_x").get_to(p.issuer_key_x);
      j.at("endpoints").get_to(p.endpoints);
    }
  }

namespace homeKeyReader
{
  typedef enum
  {
    kCmdFlowFailed = 0x0,
    kCmdFlowSuccess = 0x01,
    kCmdFlowAttestation = 0x40
  } CommandFlowStatus;
  typedef enum
  {
    UNLOCK = 0x01
  } KeyTransactionType;

  typedef enum
  {
    kTransactionSTANDARD = 0x0,
    kTransactionFAST = 0x01
  } KeyTransactionFlags;

  typedef enum
  {
    kFlowFAST = 0x00,
    kFlowSTANDARD = 0x01,
    kFlowATTESTATION = 0x02,
    kFlowFailed = 0x99
  } KeyFlow;
  typedef enum
  {
    SUCCESS = 0,
    OUT_OF_RESOURCES = 1,
    DUPLICATE = 2,
    DOES_NOT_EXIST = 3,
    NOT_SUPPORTED = 4
  } OPERATION_STATUS;

  struct readerData_t
  {
    uint8_t reader_private_key[32] = {};
    uint8_t reader_public_key[65] = {};
    uint8_t reader_key_x[32] = {};
    uint8_t reader_identifier[8] = {};
    uint8_t identifier[8] = {};
    std::list<homeKeyIssuer::issuer_t> issuers;
  };

  inline void to_json(json &j, const readerData_t &p)
  {
    j = json{{"reader_private_key", p.reader_private_key}, {"reader_public_key", p.reader_public_key}, {"reader_key_x", p.reader_key_x}, {"unique_identifier", p.identifier}, {"group_identifier", p.reader_identifier}, {"issuers", p.issuers}};
  }

  inline void from_json(const json &j, readerData_t &p)
  {
    j.at("reader_private_key").get_to(p.reader_private_key);
    j.at("reader_public_key").get_to(p.reader_public_key);
    j.at("reader_key_x").get_to(p.reader_key_x);
    j.at("unique_identifier").get_to(p.identifier);
    j.at("group_identifier").get_to(p.reader_identifier);
    j.at("issuers").get_to(p.issuers);
  }
}