#include <stdbool.h>
#include <stdint.h>

#include "telegram/types/message.h"

typedef struct {
    char* chat_id;
    char* text;
    char* parse_mode;
    bool disable_notification;
    bool protect_content;

    /*
    * not implemented:
    * business_connection_id,
    * message_thread_id,
    * entities,
    * link_preview_options,
    * message_effect_id,
    * reply parameters,
    * reply_markup
    */
} tg_send_message_params_t;

tg_message_t* tg_send_message(tg_send_message_params_t* params);
