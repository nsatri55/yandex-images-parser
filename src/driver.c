#include "../include/yandex_parser.h"
#include <stdlib.h>
#include <curl/curl.h>

static int geckodriver_port = 4444;

int yandex_start_geckodriver(void) {
    return YANDEX_SUCCESS;
}

void yandex_stop_geckodriver(void) {
    int result1 = system("pkill -f geckodriver 2>/dev/null");
    int result2 = system("pkill -f firefox 2>/dev/null");
    (void)result1;
    (void)result2;
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
