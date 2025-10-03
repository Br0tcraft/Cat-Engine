#include <3ds.h>
#include "debugg.hpp"
#include <cstdio>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#define debug false
#define inConsole false
void createDirs(const char* path) {
    char tmp[256];
    snprintf(tmp, sizeof(tmp), "%s", path);

    for (char* p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            mkdir(tmp, 0777);
            *p = '/';
        }
    }
    mkdir(tmp, 0777);
}

void log(const char* msg) {
    if (!debug) {
        return;
    }
    if (inConsole) {
        printf("%s\n", msg);
    }
    else
    {
        //write in Logfile append mode
        FILE *logFile = fopen("log.txt", "a");
        if (logFile) {
            fprintf(logFile, "%s\n", msg);
            fclose(logFile);
        }
    }
};