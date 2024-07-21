#include "telegram/helper/_api.h"
#include "telegram/getMe.h"
#include "telegram.h"

#include "esp_log.h"
#include "cJSON.h"
#include <string.h>
#include <stdlib.h>

tg_bot_info_t* tg_get_me(void) {
    tg_handle_t* handle = tg_init();
    if (handle == NULL) {
        ESP_LOGE(TELEGRAM_LOG_TAG, "Failed to initialize tg handle");
        return NULL;
    }

    tg_api_response_t* response = tg_api_request(handle, "getMe", NULL);
    if (response == NULL) {
        ESP_LOGE(TELEGRAM_LOG_TAG, "Failed to get bot info");
        tg_deinit(handle);
        return NULL;
    }

    if (response->status_code != 200) {
        ESP_LOGE(TELEGRAM_LOG_TAG, "API request failed with code %d: %s", response->status_code, response->body);
        tg_free_http_response(response);
        tg_deinit(handle);
        return NULL;
    }

    // Parse JSON response
    cJSON *root = cJSON_Parse(response->body);
    if (root == NULL) {
        ESP_LOGE(TELEGRAM_LOG_TAG, "Failed to parse JSON response");
        tg_free_http_response(response);
        tg_deinit(handle);
        return NULL;
    }

    cJSON *result = cJSON_GetObjectItem(root, "result");
    if (result == NULL) {
        ESP_LOGE(TELEGRAM_LOG_TAG, "No 'result' field in JSON response");
        cJSON_Delete(root);
        tg_free_http_response(response);
        tg_deinit(handle);
        return NULL;
    }

    tg_bot_info_t* bot_info = calloc(1, sizeof(tg_bot_info_t));
    if (bot_info == NULL) {
        ESP_LOGE(TELEGRAM_LOG_TAG, "Failed to allocate memory for bot info");
        cJSON_Delete(root);
        tg_free_http_response(response);
        tg_deinit(handle);
        return NULL;
    }

    cJSON *id = cJSON_GetObjectItem(result, "id");
    cJSON *is_bot = cJSON_GetObjectItem(result, "is_bot");
    cJSON *first_name = cJSON_GetObjectItem(result, "first_name");
    cJSON *username = cJSON_GetObjectItem(result, "username");
    cJSON *can_join_groups = cJSON_GetObjectItem(result, "can_join_groups");
    cJSON *can_read_all_group_messages = cJSON_GetObjectItem(result, "can_read_all_group_messages");
    cJSON *supports_inline_queries = cJSON_GetObjectItem(result, "supports_inline_queries");

    if (id && cJSON_IsNumber(id)) bot_info->id = id->valueint;
    if (is_bot && cJSON_IsBool(is_bot)) bot_info->is_bot = cJSON_IsTrue(is_bot);
    if (first_name && cJSON_IsString(first_name)) bot_info->first_name = strdup(first_name->valuestring);
    if (username && cJSON_IsString(username)) bot_info->username = strdup(username->valuestring);
    if (can_join_groups && cJSON_IsBool(can_join_groups)) bot_info->can_join_groups = cJSON_IsTrue(can_join_groups);
    if (can_read_all_group_messages && cJSON_IsBool(can_read_all_group_messages)) bot_info->can_read_all_group_messages = cJSON_IsTrue(can_read_all_group_messages);
    if (supports_inline_queries && cJSON_IsBool(supports_inline_queries)) bot_info->supports_inline_queries = cJSON_IsTrue(supports_inline_queries);

    cJSON_Delete(root);
    tg_free_http_response(response);
    tg_deinit(handle);
    return bot_info;
}

void tg_free_bot_info(tg_bot_info_t* bot_info) {
    if (bot_info) {
        free(bot_info->first_name);
        free(bot_info->username);
        free(bot_info);
    }
}
