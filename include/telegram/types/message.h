#include "cJSON.h"
#include <stdint.h>

typedef struct {
    /**
     * @brief Unique message identifier inside this chat
     */
    int32_t message_id;
    /**
    * @brief Date the message was sent in Unix time.
    *
    * It is always a positive number, representing a valid date.
    */
    uint32_t date;

    /*
    * not implemented:
    * every optional field + chat
    */
} tg_message_t;

tg_message_t* parse_message(cJSON* json_message);
