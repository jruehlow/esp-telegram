#include "telegram/helper/_api.h"

#include "sdkconfig.h"
#include "esp_crt_bundle.h"
#include "esp_tls.h"
#include <string.h>
#include <stdlib.h>

#define TELEGRAM_API_URL "https://api.telegram.org/bot"

struct tg_handle_t {
    esp_http_client_handle_t http;
    char* buffer;
    size_t buffer_size;
    size_t buffer_capacity;
};

static esp_err_t http_event_handler(esp_http_client_event_t *evt) {
    tg_handle_t* handle = (tg_handle_t*)evt->user_data;
    
    switch (evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            if (handle->buffer_size + evt->data_len > handle->buffer_capacity) {
                size_t new_capacity = handle->buffer_capacity * 2;
                if (new_capacity < handle->buffer_size + evt->data_len) {
                    new_capacity = handle->buffer_size + evt->data_len;
                }
                char* new_buffer = realloc(handle->buffer, new_capacity);
                if (new_buffer == NULL) {
                    return ESP_ERR_NO_MEM;
                }
                handle->buffer = new_buffer;
                handle->buffer_capacity = new_capacity;
            }
            memcpy(handle->buffer + handle->buffer_size, evt->data, evt->data_len);
            handle->buffer_size += evt->data_len;
            break;
        default:
            break;
    }
    return ESP_OK;
}

tg_handle_t* tg_init(void) {
    tg_handle_t* handle = calloc(1, sizeof(tg_handle_t));
    if (handle == NULL) {
        return NULL;
    }
    handle->buffer_capacity = CONFIG_TELEGRAM_API_BUFFER_SIZE;
    handle->buffer = malloc(handle->buffer_capacity);
    if (handle->buffer == NULL) {
        free(handle);
        return NULL;
    }
    return handle;
}

void tg_deinit(tg_handle_t* handle) {
    if (handle) {
        free(handle->buffer);
        free(handle);
    }
}

tg_api_response_t* tg_make_http_request(tg_handle_t* handle, const tg_api_request_t* params) {
    esp_http_client_config_t config = {
        .url = params->url,
        .method = params->method,
        .timeout_ms = CONFIG_TELEGRAM_API_TIMEOUT_MS,
        .event_handler = http_event_handler,
        .user_data = handle,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    
    handle->http = esp_http_client_init(&config);
    if (handle->http == NULL) {
        return NULL;
    }
    
    for (size_t i = 0; i < params->num_headers; i++) {
        char* header = strdup(params->headers[i]);
        char* value = strchr(header, ':');
        if (value) {
            *value = '\0';
            value++;
            while (*value == ' ') value++;
            esp_http_client_set_header(handle->http, header, value);
        }
        free(header);
    }
    
    if (params->body) {
        esp_http_client_set_post_field(handle->http, params->body, strlen(params->body));
    }
    
    handle->buffer_size = 0;
    esp_err_t err = esp_http_client_perform(handle->http);
    
    tg_api_response_t* response = NULL;
    if (err == ESP_OK) {
        response = malloc(sizeof(tg_api_response_t));
        if (response) {
            response->status_code = esp_http_client_get_status_code(handle->http);
            response->body = malloc(handle->buffer_size + 1);
            if (response->body) {
                memcpy(response->body, handle->buffer, handle->buffer_size);
                response->body[handle->buffer_size] = '\0';
                response->body_length = handle->buffer_size;
            } else {
                free(response);
                response = NULL;
            }
        }
    }
    
    esp_http_client_cleanup(handle->http);
    return response;
}

void tg_free_http_response(tg_api_response_t* response) {
    if (response) {
        free(response->body);
        free(response);
    }
}

tg_api_response_t* tg_api_request(tg_handle_t* handle, const char* method, const char* body_json) {
    char url[256];
    snprintf(url, sizeof(url), "%s%s/%s", TELEGRAM_API_URL, CONFIG_TELEGRAM_BOT_TOKEN, method);
    
    const char* headers[] = {"Content-Type: application/json"};
    
    tg_api_request_t request_params = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .headers = headers,
        .num_headers = 1,
        .body = body_json
    };
    
    return tg_make_http_request(handle, &request_params);
}
