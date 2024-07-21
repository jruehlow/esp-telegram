#include "telegram/helper/_api.h"
#include "telegram/sendMessage.h"
#include "telegram.h"

#include "esp_log.h"
#include "cJSON.h"
#include <string.h>
#include <stdlib.h>

tg_message_t* tg_send_message(tg_send_message_params_t* params) {
    if (params == NULL || params->chat_id == NULL || params->text == NULL) {
        ESP_LOGE(TELEGRAM_LOG_TAG, "Invalid parameters for send_message");
        return NULL;
    }

    tg_handle_t* handle = tg_init();
    if (handle == NULL) {
        ESP_LOGE(TELEGRAM_LOG_TAG, "Failed to initialize tg handle");
        return NULL;
    }

    cJSON *p = cJSON_CreateObject();
    cJSON_AddStringToObject(p, "chat_id", params->chat_id);
    cJSON_AddStringToObject(p, "text", params->text);

    if (params->parse_mode != NULL) {
        cJSON_AddStringToObject(p, "parse_mode", params->parse_mode);
    }
    if (params->disable_notification) {
        cJSON_AddBoolToObject(p, "disable_web_page_preview", true);
    }
    if (params->disable_notification) {
        cJSON_AddBoolToObject(p, "disable_notification", true);
    }

    char* body = cJSON_PrintUnformatted(p);
    cJSON_Delete(p);

    tg_api_response_t* response = tg_api_request(handle, "sendMessage", body);
    free(body);

    if (response == NULL) {
        ESP_LOGE(TELEGRAM_LOG_TAG, "Failed to send message");
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
    cJSON *json_body = cJSON_Parse(response->body);
    if (json_body == NULL) {
        ESP_LOGE(TELEGRAM_LOG_TAG, "Failed to parse JSON response");
        tg_free_http_response(response);
        tg_deinit(handle);
        return NULL;
    }

    cJSON *result = cJSON_GetObjectItem(json_body, "result");
    if (result == NULL) {
        ESP_LOGE(TELEGRAM_LOG_TAG, "No 'result' field in JSON response");
        cJSON_Delete(json_body);
        tg_free_http_response(response);
        tg_deinit(handle);
        return NULL;
    }

    tg_message_t* message = parse_message(result);

    cJSON_Delete(json_body);
    tg_free_http_response(response);
    tg_deinit(handle);

    return message;
}
