#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

typedef struct {
    int default_time;
    char log_level[10];
    char leaderboard_file[256];
} Config;

int load_config(const char *filename, Config *cfg);

#endif