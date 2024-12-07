/*
 * APN3 PPI string generator - sample code
 *
 * NOTE: All code is provided as sample code for informational purposes only, and should not be used for any testing or production workloads.
 * All code is provided “as is” and AWS expressly disclaims all warranties, including, without limitation: any implied warranties of
 * noninfringement, merchantability, or fitness for a particular purpose; any warranty that operation of the code will be error-free
 * or free of harmful components; or any warranty arising out of any course of dealing or usage of trade. In no event shall AWS or
 * any of its affiliates be liable for any damages arising out of the use of this code, including, without limitation, any direct,
 * indirect, special, incidental or consequential damages.
 */
#include <stdio.h>
#include <string.h>
#include <sdkconfig.h>
#include <esp_wifi.h>
#include <esp_log.h>
#ifdef CONFIG_ESP_RMAKER_NETWORK_OVER_THREAD
#include <esp_mac.h>
#endif

static const char *TAG = "aws_ppi";

#define PPI_STRING_GEN_VERSION 0.4
#define MAX_PPI_STRING_LEN 64
#define MAX_LEN_FIELDS_1_2_3 37  /* max length of fields ProductName + ProductUID + ProductVersion */

#define PPI_PREFIX_1 "?"
#define PPI_PREFIX_1_ALT "&"

#define PPI_PREFIX_2 "Platform=APN3|"
#define PPI_RESERVED "A0|" /* reserved for AWS use */

#define PPI_PLATFORM_AWS "AZ|"
#define PPI_PLATFORM_RAINMAKER "RM|"
#define PPI_PLATFORM PPI_PLATFORM_RAINMAKER

#define PPI_SILICON_SKU_CODE    CONFIG_ESP_RMAKER_MQTT_PRODUCT_SKU
#define PPI_PRODUCT_NAME        CONFIG_ESP_RMAKER_MQTT_PRODUCT_NAME
#define PPI_PRODUCT_VERSION     CONFIG_ESP_RMAKER_MQTT_PRODUCT_VERSION

#define PPI_SEPARATOR_CHAR '|'
#define PPI_SEPARATOR "|"

#define PPI_FIXED_PART PPI_PREFIX_1 PPI_PREFIX_2 PPI_RESERVED PPI_PLATFORM PPI_SILICON_SKU_CODE PPI_SEPARATOR PPI_PRODUCT_NAME PPI_SEPARATOR PPI_PRODUCT_VERSION PPI_SEPARATOR

#define VALID 1
#define INVALID 0

/*
 * platform_get_product_name is a stub function to be customized for each platform.
 * It must return a pointer to the product name string.
 * Can use a #define instead of a function in most cases.
 */
char __attribute__((weak)) *platform_get_product_name()
{
    return(PPI_PRODUCT_NAME);
}

/*
 * platform_get_product_UID is a stub function to be customized for each platform.
 * It must return a pointer to the product UID string.
 */
char __attribute__((weak)) *platform_get_product_UID()
{
    static char mac_str[13];
#if defined(CONFIG_ESP_RMAKER_NETWORK_OVER_WIFI)
    uint8_t eth_mac[6];
    esp_err_t err = esp_wifi_get_mac(WIFI_IF_STA, eth_mac);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not fetch MAC address. Please initialise Wi-Fi first");
        return NULL;
    }
    snprintf(mac_str, sizeof(mac_str), "%02X%02X%02X%02X%02X%02X",
            eth_mac[0], eth_mac[1], eth_mac[2], eth_mac[3], eth_mac[4], eth_mac[5]);
#elif defined(CONFIG_ESP_RMAKER_NETWORK_OVER_THREAD) /* CONFIG_ESP_RMAKER_NETWORK_OVER_WIFI */
    uint8_t base_mac[6];
    esp_err_t err = esp_read_mac(base_mac, ESP_MAC_BASE);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not fetch base mac");
        return NULL;
    }
    snprintf(mac_str, sizeof(mac_str), "%02X%02X%02X%02X%02X%02X",
             base_mac[0], base_mac[1], base_mac[2], base_mac[3], base_mac[4], base_mac[5]);
#endif /* CONFIG_ESP_RMAKER_NETWORK_OVER_THREAD */
    return mac_str;
}

/*
 * platform_get_product_version is a stub function to be customized for each platform.
 * It must return a pointer to the product version string.
 * Can use a #define instead of a function in most cases.
 */
char __attribute__((weak)) *platform_get_product_version()
{
    return(PPI_PRODUCT_VERSION);
}

/*
 * platform_get_silicom_sku_code is a stub function to be customized for each platform.
 * It must return a pointer to a valid Silicon SKU code.
 * Can use a #define instead of a function in most cases.
 */
char __attribute__((weak)) *platform_get_silicon_sku_code()
{
    return(PPI_SILICON_SKU_CODE);
}

/*
 * validate_sku_code takes as input the 4 character SKU code and validates it.
 *
 * Returns
 * VALID if valid, else INVALID
 */
int validate_sku_code(char *silicon_sku_code)
{
  int retval = INVALID;

  typedef struct sku_item_t {
      /* entry in sku code lookup table */
      char *skucode; /* code corresponding to sku name */
  } sku_item;

  static sku_item skutable[] =
  {
      /*
       * codes for partner 1
       */
      {  "ES00" }, /* Older Default */
      {  "EX00" }, /* Default */
      {  "EX01" }, /* ESP32-C3, including 8685 */
      {  "EX02" }, /* ESP32-C2, including 8684 */
      {  "EX03" }, /* ESP32-S3 */
      {  "EX04" }, /* ESP32 */
      {  "EX05" }, /* ESP32-C5 */
      {  "EX06" }, /* ESP32-S2 */
      /*
       * add new entries above in any order
       */
      { NULL }  /* this must be the last entry */
  };

  /*
   * linear search for a matching silicon sku code entry
   */
  for (sku_item *p = skutable; p->skucode != NULL; p++)
  {
      if (strcmp(p->skucode, silicon_sku_code) == 0)
      {   /* found match */
          retval = VALID;
          break;
      }
  }
  return (retval);
}

/*
 * validate_platform_inputs
 *
 * Returns
 * VALID if inputs are valid, INVALID otherwise
 */
int validate_platform_inputs(char *product_name, char *product_uid, char *product_version, char *silicon_sku_code)
{
  int retval = INVALID;

  if ( (strlen(product_name) + strlen(product_uid) + strlen(product_version)) <= MAX_LEN_FIELDS_1_2_3)
  {   /* field 1,2,3 length check passed */

      if (validate_sku_code(silicon_sku_code) == VALID)
      {
          retval = VALID;
      }
      else
      { /* invalid sku code */
          ESP_LOGE(TAG, "Error: Invalid sku code");
      }
  }
  else
  {   /* invalid string length */
      ESP_LOGE(TAG, "Error: Invalid field(s) length");
  }
  return(retval);
}

/*
 * create_APN_PPI_string creates the string that must be passed in the UserName field of the MQTT Connect
 * packet as required by the IAP Module Incentive program.  Details are given in the APN3 IoT Acceleration
 * Module Incentive Program Guide-Tech-<partnername> document.
 *
 * Inputs:
 * product_name = pointer to a string holding the product name.
 * product_uid = pointer to a string holding the product UID (unique id)
 * product_version = version number of the product.
 * silicon_sku_code = pointer to a string holding a valid SKU code as specified in the guide.
 *
 * Return values
 * NULL if there is an error and the PPI string could not be generated, otherwise the PPI string is returned.
 */
char * create_APN_PPI_string(char *product_name, char *product_uid, char *product_version, char *silicon_sku_code)
{
    char *retval = NULL;
    static char ppistring[MAX_PPI_STRING_LEN + 1];


    ESP_LOGD(TAG, "PPI string generator v%2.1f", PPI_STRING_GEN_VERSION);

    /*
     * Validate inputs
     */
    if (validate_platform_inputs(product_name, product_uid, product_version, silicon_sku_code))
    { /* valid inputs - build the string */
      snprintf(ppistring, sizeof(ppistring),"%s%s%s%s%s%s%s%s%s%s%s",
              PPI_PREFIX_1, PPI_PREFIX_2, PPI_RESERVED, PPI_PLATFORM, silicon_sku_code,
              PPI_SEPARATOR, product_name, PPI_SEPARATOR, product_version, PPI_SEPARATOR,
              product_uid);
      retval = ppistring;
    }
    else
    { /* error - some inputs are not valid */
      ESP_LOGE(TAG, "Error: Some inputs are not valid");
    }
    return(retval);
}


/*
 * main - setup values for your platform and invoke string generator.
 */
const char __attribute__((weak)) *esp_get_aws_ppi(void)
{
    char *ppistring = NULL;

    ppistring = create_APN_PPI_string( platform_get_product_name(),
                                       platform_get_product_UID(),
                                       platform_get_product_version(),
                                       platform_get_silicon_sku_code() );

    if (ppistring != NULL)
    {
      ESP_LOGD(TAG, "PPI inputs pass all checks. String value is shown below:\n\n%s", ppistring);
    }
    else
    { /* error */
      ESP_LOGE(TAG, "Error creating PPI string");
    }
    return ppistring;
}
