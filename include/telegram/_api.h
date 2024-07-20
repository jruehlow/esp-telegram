#ifndef TELEGRAM_API_H
#define TELEGRAM_API_H

#include "esp_http_client.h"
#include <stddef.h>

typedef struct tg_handle_t tg_handle_t;

typedef struct {
    const char* url;
    esp_http_client_method_t method;
    const char** headers;
    size_t num_headers;
    const char* body;
} tg_api_request_t;

typedef struct {
    int status_code;
    char* body;
    size_t body_length;
} tg_api_response_t;

tg_handle_t* tg_init(void);
void tg_deinit(tg_handle_t* handle);

tg_api_response_t* tg_make_http_request(tg_handle_t* handle, const tg_api_request_t* params);
void tg_free_http_response(tg_api_response_t* response);

tg_api_response_t* tg_api_request(tg_handle_t* handle, const char* bot_token, const char* method, const char* params_json);

#endif // TELEGRAM_API_H
