#pragma once

#define SRMODEL_STRING_LENGTH 32
#ifdef ESP_PLATFORM
#include "esp_partition.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int num;                 // the number of files
    char **files;            // the model files, like wn9_index, wn9_data
    char **data;             // the pointer of file data
    int *sizes;              // the size of different file
} srmodel_data_t;

typedef struct {
    char **model_name;                        // the name of models, like "wn9_hilexin"(wakenet9, hilexin), "mn5_en"(multinet5, english)
    char **model_info;                        // the information of models, like "wakeNet9_v1h24_Hi,ESP_3_0.63_0.635", the format is as follows:
                                              // (model type)_(version)_(word1)_(detection window)_(threshold1)_(threshold2)_(word2 ...)_...
#ifdef ESP_PLATFORM
    esp_partition_t *partition;               // partition label used to save the files of model
#endif
    void * mmap_handle;                       // mmap_handle if using esp_partition_mmap else NULL; 
    int num;                                  // the number of models
    srmodel_data_t **model_data;              // the model data , NULL if spiffs format
} srmodel_list_t;


#define MODEL_NAME_MAX_LENGTH 64

/**
 * @brief Return all avaliable models in flash.
 *
 * @param partition_label    The spiffs label defined in your partition file used to save models.
 *
 * @return all avaliable models,save as srmodel_list_t.
 */
srmodel_list_t *esp_srmodel_init(const char *partition_label);

/**
 * @brief Free srmodel_list_t and unregister SPIFFS filesystem if open SPIFFS filesystem.
 *
 * @param models    The srmodel_list_t point allocated by esp_srmodel_init function.
 *
 * @return all avaliable models in spiffs,save as srmodel_list_t.
 */
void esp_srmodel_deinit(srmodel_list_t *models);

/**
 * @brief Return the first model name containing the specified keywords
 *        If keyword is NULL, we will ignore the keyword.
 *
 * @param models      The srmodel_list_t point allocated by esp_srmodel_init function.
 * @param keyword1    The specified keyword1 , like ESP_WN_PREDIX(the prefix of wakenet),
 *                                                  ESP_MN_PREFIX(the prefix of multinet),
 *
 * @param keyword2    The specified keyword2, like ESP_MN_ENGLISH(the english multinet)
 *                                                 ESP_MN_CHINESE(the chinese multinet)
 *                                                "alexa" (the "alexa" wakenet)
 * @return return model name if can find one model name containing the keywords otherwise return NULL.
 */
char *esp_srmodel_filter(srmodel_list_t *models, const char *keyword1, const char *keyword2);


/**
 * @brief Check whether the specified model name exists or not.
 *
 * @param models      The srmodel_list_t point allocated by esp_srmodel_init function.
 * @param model_name  The specified model name
 * @return return index in models if model name exists otherwise return -1
 */
int esp_srmodel_exists(srmodel_list_t *models, char *model_name);

/**
 * @brief Get wake words from model_name. 
 *        If there are multiple wake words in one model, all wake words will be joined by ";". 
 *
 * @param models       The srmodel_list_t point allocated by srmodel_spiffs_init function.
 * @param model_name   The specified model name
 * @return The string of wake words.
 */
char *esp_srmodel_get_wake_words(srmodel_list_t *models, char *model_name);

/**
 * @brief Initialize and mount SPIFFS filesystem, return all avaliable models in spiffs.
 *
 * @param part    The spiffs partition.
 *
 * @return all avaliable models in spiffs,save as srmodel_list_t.
 */
#ifdef ESP_PLATFORM
srmodel_list_t *srmodel_spiffs_init(const esp_partition_t *part);
#endif

/**
 * @brief unregister SPIFFS filesystem and free srmodel_list_t.
 *
 * @param models    The srmodel_list_t point allocated by srmodel_spiffs_init function.
 *
 * @return all avaliable models in spiffs,save as srmodel_list_t.
 */
void srmodel_spiffs_deinit(srmodel_list_t *models);


/**
 * @brief Return base path of srmodel spiffs
 *
 * @return the base path od srmodel spiffs
 */
char *get_model_base_path(void);

/**
 * @brief Return static srmodel pointer.
 *        static srmodel pointer will be set after esp_srmodel_init
 *
 * @return the pointer of srmodel_list_t
 */
srmodel_list_t *get_static_srmodels(void);

/**
 * @brief Load models from binary data
 *
 * @param root   The binary model data.
 *
 * @return all avaliable models,save as srmodel_list_t.
 */
srmodel_list_t *srmodel_load(const void *root);



#ifdef ESP_PLATFORM
#include "dl_lib_coefgetter_if.h"
/**
 * @brief Return model_coeff_getter_t pointer base on model_name
 *
 * @warning Just support ESP32 to load old wakenet
 *
 * @param model_name    The model name
 *
 * @return model_coeff_getter_t pointer or NULL
 */
model_coeff_getter_t *srmodel_get_model_coeff(char *model_name);
#endif


#ifdef __cplusplus
}
#endif