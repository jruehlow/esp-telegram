#include "telegram/_api.h"
#include "telegram.h"

#include "sdkconfig.h"
#include "esp_crt_bundle.h"
#include "esp_tls.h"
#include "esp_log.h"

#include <string.h>
#include <stdlib.h>


struct telegram_handle_t {
    esp_http_client_handle_t http;
    char* buffer;
    size_t buffer_size;
};

static esp_err_t http_event_handler(esp_http_client_event_t *evt) {
    telegram_handle_t* handle = (telegram_handle_t*)evt->user_data;
    
    if (evt->event_id == HTTP_EVENT_ON_DATA) {
        if (handle->buffer_size + evt->data_len < CONFIG_TELEGRAM_API_BUFFER_SIZE) {
            memcpy(handle->buffer + handle->buffer_size, evt->data, evt->data_len);
            handle->buffer_size += evt->data_len;
        }
    }
    return ESP_OK;
}

telegram_handle_t* telegram_init(void) {
    telegram_handle_t* handle = calloc(1, sizeof(telegram_handle_t));
    handle->buffer = malloc(CONFIG_TELEGRAM_API_BUFFER_SIZE);
    return handle;
}

static esp_err_t telegram_request(telegram_handle_t* handle, esp_http_client_method_t method, const char* endpoint, const char* payload, telegram_api_response_t** out_response) {
    char url[256];
    snprintf(url, sizeof(url), "%s%s%s", TELEGRAM_API_URL, CONFIG_TELEGRAM_BOT_TOKEN, endpoint);

    esp_http_client_config_t config = {
        .url = url,
        .method = method,
        .timeout_ms = CONFIG_TELEGRAM_API_TIMEOUT_MS,
        .event_handler = http_event_handler,
        .user_data = handle,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    handle->http = esp_http_client_init(&config);
    if (handle->http == NULL) {
        return ESP_FAIL;
    }

    if (payload) {
        esp_http_client_set_post_field(handle->http, payload, strlen(payload));
    }

    esp_err_t err = esp_http_client_perform(handle->http);
    if (err == ESP_OK) {
        telegram_api_response_t* response = malloc(sizeof(telegram_api_response_t));
        response->code = esp_http_client_get_status_code(handle->http);
        response->data = malloc(handle->buffer_size + 1);
        memcpy(response->data, handle->buffer, handle->buffer_size);
        response->data[handle->buffer_size] = '\0';
        response->data_len = handle->buffer_size;
        *out_response = response;
    }

    esp_http_client_cleanup(handle->http);
    handle->buffer_size = 0;
    return err;
}

esp_err_t telegram_get(telegram_handle_t* handle, const char* endpoint, telegram_api_response_t** out_response) {
    return telegram_request(handle, HTTP_METHOD_GET, endpoint, NULL, out_response);
}

esp_err_t telegram_post(telegram_handle_t* handle, const char* endpoint, const char* payload, telegram_api_response_t** out_response) {
    return telegram_request(handle, HTTP_METHOD_POST, endpoint, payload, out_response);
}

void telegram_free_response(telegram_api_response_t* response) {
    if (response) {
        free(response->data);
        free(response);
    }
}

void telegram_deinit(telegram_handle_t* handle) {
    if (handle) {
        free(handle->buffer);
        free(handle);
    }
}
