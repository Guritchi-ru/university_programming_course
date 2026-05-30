#include "config_parser.h"
#include "logger.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void trim(char *str) {
    char *p = strchr(str, '#');
    if (p) *p = '\0';
    
    while (*str == ' ' || *str == '\t') str++;
    
    char *end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
        *end = '\0';
        end--;
    }
}

static void set_defaults(Config *cfg) {
    cfg->default_time = 30;
    strcpy(cfg->log_level, "ERROR");
    strcpy(cfg->leaderboard_file, "leaderboard.txt");
}

int load_config(const char *filename, Config *cfg) {
    set_defaults(cfg);
    
    FILE *f = fopen(filename, "r");
    if (!f) {
        log_message(LOG_ERROR, "Cannot open config file %s", filename);
        return 0;
    }
    
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        trim(line);
        if (line[0] == '\0') continue;
        
        char key[64], val[192];
        if (sscanf(line, "%63[^=]=%191s", key, val) == 2) {
            if (strcmp(key, "DEFAULT_TIME") == 0) {
                cfg->default_time = atoi(val);
            } else if (strcmp(key, "LOG_LEVEL") == 0) {
                strncpy(cfg->log_level, val, sizeof(cfg->log_level) - 1);
                cfg->log_level[sizeof(cfg->log_level) - 1] = '\0';
            } else if (strcmp(key, "LEADERBOARD_FILE") == 0) {
                strncpy(cfg->leaderboard_file, val, sizeof(cfg->leaderboard_file) - 1);
                cfg->leaderboard_file[sizeof(cfg->leaderboard_file) - 1] = '\0';
            }
        }
    }
    fclose(f);
    log_message(LOG_INFO, "Config loaded: time=%d log=%s", cfg->default_time, cfg->log_level);
    return 1;
}
