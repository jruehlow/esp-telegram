#include "telegram/types/message.h"
#include "telegram.h"

#include "esp_log.h"
#include <stdlib.h>

tg_message_t* parse_message(cJSON* json_message) {
    tg_message_t* message = calloc(1, sizeof(tg_message_t));
    if (message == NULL) {
        ESP_LOGE(TELEGRAM_LOG_TAG, "Failed to allocate memory for message");
        return NULL;
    }

    cJSON* message_id = cJSON_GetObjectItem(json_message, "message_id");
    cJSON* date = cJSON_GetObjectItem(json_message, "date");

    if (message_id && cJSON_IsNumber(message_id)) {
        message->message_id = message_id->valueint;
    }

    if (date && cJSON_IsNumber(date)) {
        message->date = date->valueint;
    }

    return message;
}
