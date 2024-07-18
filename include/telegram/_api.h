#ifndef TELEGRAM_API_H
#define TELEGRAM_API_H

#include <esp_http_client.h>

typedef struct telegram_handle_t telegram_handle_t;

typedef struct {
    int code;
    char* data;
    size_t data_len;
} telegram_api_response_t;

telegram_handle_t* telegram_init(void);
esp_err_t telegram_get(telegram_handle_t* handle, const char* endpoint, telegram_api_response_t** out_response);
esp_err_t telegram_post(telegram_handle_t* handle, const char* endpoint, const char* payload, telegram_api_response_t** out_response);
void telegram_free_response(telegram_api_response_t* response);
void telegram_deinit(telegram_handle_t* handle);

#endif // TELEGRAM_API_H
