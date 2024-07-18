typedef struct {
    long id;
    bool is_bot;
    char* first_name;
    char* username;
    bool can_join_groups;
    bool can_read_all_group_messages;
    bool supports_inline_queries;
} telegram_bot_info_t;

telegram_bot_info_t* telegram_get_me(void);
void telegram_free_bot_info(telegram_bot_info_t* bot_info);
