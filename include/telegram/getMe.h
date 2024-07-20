typedef struct {
    long id;
    bool is_bot;
    char* first_name;
    char* username;
    bool can_join_groups;
    bool can_read_all_group_messages;
    bool supports_inline_queries;
} tg_bot_info_t;

tg_bot_info_t* tg_get_me(void);
void tg_free_bot_info(tg_bot_info_t* bot_info);
