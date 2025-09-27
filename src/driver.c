#include "../include/yandex_parser.h"
#include <stdlib.h>
#include <curl/curl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

static int geckodriver_port = 4444;

int yandex_start_geckodriver(void) {
    yandex_stop_geckodriver();
    sleep(1);
    
    if (yandex_is_geckodriver_running()) {
        return YANDEX_SUCCESS;
    }
    
    const char* geckodriver_paths[] = {
        "/usr/local/bin/geckodriver",
        "/usr/bin/geckodriver",
        "./geckodriver",
        "geckodriver"
    };
    
    const char* geckodriver_path = NULL;
    for (size_t i = 0; i < sizeof(geckodriver_paths) / sizeof(geckodriver_paths[0]); i++) {
        if (access(geckodriver_paths[i], X_OK) == 0) {
            geckodriver_path = geckodriver_paths[i];
            break;
        }
    }
    
    if (!geckodriver_path) {
        return YANDEX_ERROR_DRIVER;
    }
    
    char command[512];
    snprintf(command, sizeof(command), "%s --port=%d --log=error > /dev/null 2>&1 &", 
             geckodriver_path, geckodriver_port);
    
    int result = system(command);
    if (result != 0) {
        return YANDEX_ERROR_DRIVER;
    }
    
    for (int i = 0; i < 5; i++) {
        sleep(1);
        if (yandex_is_geckodriver_running()) {
            return YANDEX_SUCCESS;
        }
    }
    
    return YANDEX_ERROR_DRIVER;
}

void yandex_stop_geckodriver(void) {
    int result1 = system("pkill -f geckodriver 2>/dev/null");
    int result2 = system("pkill -f firefox 2>/dev/null");
    int result3 = system("pkill -f firefox-esr 2>/dev/null");
    (void)result1; (void)result2; (void)result3;
    
    sleep(1);
    
    int result4 = system("pkill -9 -f geckodriver 2>/dev/null");
    int result5 = system("pkill -9 -f firefox 2>/dev/null");
    int result6 = system("pkill -9 -f firefox-esr 2>/dev/null");
    (void)result4; (void)result5; (void)result6;
}

int yandex_is_geckodriver_running(void) {
    CURL *curl = curl_easy_init();
    if (!curl) return 0;
    
    char url[64];
    snprintf(url, sizeof(url), "http://localhost:%d/status", geckodriver_port);
    
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2L);
    
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    
    return (res == CURLE_OK) ? 1 : 0;
}

int yandex_set_geckodriver_port(int port) {
    if (port < 1 || port > 65535) return YANDEX_ERROR_INVALID_PARAM;
    geckodriver_port = port;
    return YANDEX_SUCCESS;
}

int yandex_get_geckodriver_port(void) {
    return geckodriver_port;
}
