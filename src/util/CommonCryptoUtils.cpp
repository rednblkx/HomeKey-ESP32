#include <util/CommonCryptoUtils.h>
#include "CommonCryptoUtils.h"

CommonCryptoUtils::CommonCryptoUtils()
{
}

int CommonCryptoUtils::esp_rng(void *, uint8_t *buf, size_t len)
{
  esp_fill_random(buf, len);
  return 0;
}

/**
 * The function generates an ephemeral key pair using the secp256r1 elliptic curve and returns the
 * private and public keys as vectors of uint8_t.
 * 
 * @return The function `generateEphemeralKey` returns a tuple containing two vectors of type
 * `uint8_t`. The first vector `bufPriv` contains the private key generated, and the second vector
 * `bufPub` contains the public key generated.
 */
std::tuple<std::vector<uint8_t>, std::vector<uint8_t>> CommonCryptoUtils::generateEphemeralKey()
{
  mbedtls_ecp_keypair ephemeral;
  mbedtls_ecp_keypair_init(&ephemeral);
  int gen_key = mbedtls_ecp_gen_key(MBEDTLS_ECP_DP_SECP256R1, &ephemeral, esp_rng, NULL);
  if(gen_key != 0){
    ESP_LOGE(TAG, "generateEphemeralKey > gen_key - %s: %s", mbedtls_high_level_strerr(gen_key), mbedtls_low_level_strerr(gen_key));
    return std::make_tuple(std::vector<uint8_t>(), std::vector<uint8_t>());
  }
  std::vector<uint8_t> bufPriv(mbedtls_mpi_size(&ephemeral.d));
  int mpi_write = mbedtls_mpi_write_binary(&ephemeral.d, bufPriv.data(), bufPriv.capacity());
  if(mpi_write != 0){
    ESP_LOGE(TAG, "generateEphemeralKey > mpi_write - %s: %s", mbedtls_high_level_strerr(mpi_write), mbedtls_low_level_strerr(mpi_write));
    return std::make_tuple(std::vector<uint8_t>(), std::vector<uint8_t>());
  }
  std::vector<uint8_t> bufPub(MBEDTLS_ECP_MAX_BYTES);
  size_t olen = 0;
  int ecp_write = mbedtls_ecp_point_write_binary(&ephemeral.grp, &ephemeral.Q, MBEDTLS_ECP_PF_UNCOMPRESSED, &olen, bufPub.data(), bufPub.capacity());
  if(!ecp_write){
    ESP_LOGD(TAG, "Ephemeral Key generated -- private: %s, public: %s", utils::bufToHexString(bufPriv.data(), bufPriv.size()).c_str(), utils::bufToHexString(bufPub.data(), bufPub.size()).c_str());
  } else{
    ESP_LOGE(TAG, "generateEphemeralKey > ecp_write - %s : %s", mbedtls_high_level_strerr(ecp_write), mbedtls_low_level_strerr(ecp_write));
    return std::make_tuple(std::vector<uint8_t>(), std::vector<uint8_t>());
  }
  bufPub.resize(olen);
  mbedtls_ecp_keypair_free(&ephemeral);
  return std::make_tuple(std::move(bufPriv), std::move(bufPub));
}

/**
 * The function `get_x` takes a public key as input, converts it to a point on an elliptic curve,
 * extracts the X coordinate of the point, and returns it as a vector of bytes.
 * 
 * @param pubKey The `pubKey` parameter is a reference to a `std::vector<uint8_t>` object, which
 * represents a public key. It is passed by reference so that the function can read the key and perform
 * operations on it without making a copy of the data.
 * 
 * @return a std::vector<uint8_t> object, which contains the X coordinate of the given public key.
 */
std::vector<uint8_t> CommonCryptoUtils::get_x(std::vector<uint8_t> &pubKey)
{
  mbedtls_ecp_group grp;
  mbedtls_ecp_point point;
  mbedtls_ecp_point_init(&point);
  mbedtls_ecp_group_init(&grp);
  mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1);
  int ecp_read = mbedtls_ecp_point_read_binary(&grp, &point, pubKey.data(), pubKey.size());
  if(ecp_read != 0)
    ESP_LOGE(TAG, "get_x > ecp_read - %s: %s", mbedtls_high_level_strerr(ecp_read), mbedtls_low_level_strerr(ecp_read));
  size_t buffer_size_x = mbedtls_mpi_size(&point.X);
  std::vector<uint8_t> X(buffer_size_x);
  int ecp_write = mbedtls_mpi_write_binary(&point.X, X.data(), buffer_size_x);
  if(ecp_write != 0)
    ESP_LOGE(TAG, "get_x > ecp_write - %s: %s", mbedtls_high_level_strerr(ecp_write), mbedtls_low_level_strerr(ecp_write));
  ESP_LOGV(TAG, "PublicKey: %s, X Coordinate: %s", utils::bufToHexString(pubKey.data(), pubKey.size()).c_str(), utils::bufToHexString(X.data(), X.size()).c_str());
  mbedtls_ecp_group_free(&grp);
  mbedtls_ecp_point_free(&point);
  return X;
}

/**
 * The function `get_x` takes a public key as input, converts it to a point on an elliptic curve,
 * extracts the X coordinate of the point, and returns it as a vector of bytes.
 * 
 * @param pubKey The `pubKey` parameter is a pointer to an array of `uint8_t` (unsigned 8-bit integers)
 * that represents the public key. It is assumed that the array contains `len` number of elements.
 * @param len The parameter `len` represents the length of the `pubKey` array.
 * 
 * @return a std::vector<uint8_t> object named X.
 */
std::vector<uint8_t> CommonCryptoUtils::get_x(uint8_t *pubKey, size_t len)
{
  mbedtls_ecp_group grp;
  mbedtls_ecp_point point;
  mbedtls_ecp_point_init(&point);
  mbedtls_ecp_group_init(&grp);
  mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1);
  int ecp_read = mbedtls_ecp_point_read_binary(&grp, &point, pubKey, len);
  if(ecp_read != 0){
    ESP_LOGE(TAG, "get_x > ecp_read - %s: %s", mbedtls_high_level_strerr(ecp_read), mbedtls_low_level_strerr(ecp_read));
    return std::vector<uint8_t>();
  }
  size_t buffer_size_x = mbedtls_mpi_size(&point.X);
  std::vector<uint8_t> X(buffer_size_x);
  int ecp_write = mbedtls_mpi_write_binary(&point.X, X.data(), buffer_size_x);
  if(ecp_write != 0){
    ESP_LOGE(TAG, "get_x > ecp_write - %s: %s", mbedtls_high_level_strerr(ecp_write), mbedtls_low_level_strerr(ecp_write));
    return std::vector<uint8_t>();
  }
  ESP_LOGV(TAG, "PublicKey: %s, X Coordinate: %s", utils::bufToHexString(pubKey, len).c_str(), utils::bufToHexString(X.data(), X.size()).c_str());
  mbedtls_ecp_group_free(&grp);
  mbedtls_ecp_point_free(&point);
  return X;
}

/**
 * The function signSharedInfo signs the given data using the private key and returns the signature in
 * TLV format.
 *
 * @param stdTlv A pointer to an array of uint8_t values representing a TLV (Tag-Length-Value)
 * structure.
 * @param len The `len` parameter represents the length of the `stdTlv` array.
 *
 * @return a std::vector<uint8_t> containing a TLV (Tag-Length-Value) structure. The TLV structure
 * consists of a tag (0x9E) and the signature point (sigPoint) as the value.
 */
std::vector<uint8_t> CommonCryptoUtils::signSharedInfo(uint8_t *stdTlv, size_t len, uint8_t *privateKey, size_t keyLen)
{
  mbedtls_ecp_keypair keypair;
  mbedtls_ecp_keypair_init(&keypair);

  mbedtls_mpi sigMpi1;
  mbedtls_mpi sigMpi2;
  mbedtls_mpi_init(&sigMpi1);
  mbedtls_mpi_init(&sigMpi2);

  uint8_t hash[32];

  mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), stdTlv, len, hash);

  int ecp_read = mbedtls_ecp_read_key(MBEDTLS_ECP_DP_SECP256R1, &keypair, privateKey, keyLen);
  if(ecp_read != 0){
    ESP_LOGE(TAG, "signSharedInfo > ecp_read - %s: %s", mbedtls_high_level_strerr(ecp_read), mbedtls_low_level_strerr(ecp_read));
    return std::vector<uint8_t>();
  }
  int ecdsa_sign = mbedtls_ecdsa_sign_det_ext(&keypair.grp, &sigMpi1, &sigMpi2, &keypair.d, hash, keyLen, MBEDTLS_MD_SHA256, esp_rng, NULL);
  if(ecdsa_sign != 0){
    ESP_LOGE(TAG, "signSharedInfo > ecdsa_sign - %s: %s", mbedtls_high_level_strerr(ecdsa_sign), mbedtls_low_level_strerr(ecdsa_sign));
    return std::vector<uint8_t>();
  }
  uint8_t sigPoint[mbedtls_mpi_size(&sigMpi1) + mbedtls_mpi_size(&sigMpi2)];
  int ecp_write_1 = mbedtls_mpi_write_binary(&sigMpi1, sigPoint, mbedtls_mpi_size(&sigMpi1));
  if(ecp_write_1 != 0){
    ESP_LOGE(TAG, "signSharedInfo > ecp_write_1 - %s: %s", mbedtls_high_level_strerr(ecp_write_1), mbedtls_low_level_strerr(ecp_write_1));
    return std::vector<uint8_t>();
  }
  int ecp_write_2 = mbedtls_mpi_write_binary(&sigMpi2, sigPoint + mbedtls_mpi_size(&sigMpi1), mbedtls_mpi_size(&sigMpi2));
  if(ecp_write_2 != 0){
    ESP_LOGE(TAG, "signSharedInfo > ecp_write_2 - %s: %s", mbedtls_high_level_strerr(ecp_write_2), mbedtls_low_level_strerr(ecp_write_2));
    return std::vector<uint8_t>();
  }
  mbedtls_ecp_keypair_free(&keypair);
  mbedtls_mpi_free(&sigMpi1);
  mbedtls_mpi_free(&sigMpi2);
  return utils::simple_tlv(0x9E, sigPoint, sizeof(sigPoint));
}

std::vector<uint8_t> CommonCryptoUtils::getPublicKey(uint8_t *privKey, size_t len)
{
  mbedtls_ecp_keypair keypair;
  mbedtls_ecp_keypair_init(&keypair);
  int ecp_key = mbedtls_ecp_read_key(MBEDTLS_ECP_DP_SECP256R1, &keypair, privKey, len);
  int ret = mbedtls_ecp_mul(&keypair.grp, &keypair.Q, &keypair.d, &keypair.grp.G, esp_rng, NULL);

  size_t olenPub = 0;
  std::vector<uint8_t> readerPublicKey(MBEDTLS_ECP_MAX_BYTES);
  mbedtls_ecp_point_write_binary(&keypair.grp, &keypair.Q, MBEDTLS_ECP_PF_UNCOMPRESSED, &olenPub, readerPublicKey.data(), readerPublicKey.capacity());
  readerPublicKey.resize(olenPub);

  // Cleanup
  mbedtls_ecp_keypair_free(&keypair);
  return readerPublicKey;
}