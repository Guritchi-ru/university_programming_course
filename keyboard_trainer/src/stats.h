#ifndef STATS_H
#define STATS_H

typedef struct {
    int correct;
    int errors;
    double time_sec;
    int mode;
} SessionStats;

void save_stats(const char *filename, const char *name, SessionStats s);
void show_leaderboard(const char *filename);

#endif