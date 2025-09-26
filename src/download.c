#include "../include/yandex_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <curl/curl.h>

static inline const char* get_file_extension(const char *url) {
    const char *ext = strrchr(url, '.');
    if (!ext) return ".jpg";
    
    if (strcasecmp(ext, ".jpg") == 0 || strcasecmp(ext, ".jpeg") == 0) return ".jpg";
    if (strcasecmp(ext, ".png") == 0) return ".png";
    if (strcasecmp(ext, ".gif") == 0) return ".gif";
    if (strcasecmp(ext, ".webp") == 0) return ".webp";
    
    return ".jpg";
}

static size_t download_write_callback(void *contents, size_t size, size_t nmemb, FILE *file) {
    return fwrite(contents, size, nmemb, file);
}

int yandex_download_single_image(const char *url, const char *filename) {
    if (!url || !filename) return YANDEX_ERROR_INVALID_PARAM;
    
    FILE *file = fopen(filename, "wb");
    if (!file) return YANDEX_ERROR_MEMORY;
    
    CURL *curl = curl_easy_init();
    if (!curl) {
        fclose(file);
        return YANDEX_ERROR_NETWORK;
    }
    
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, download_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    
    CURLcode res = curl_easy_perform(curl);
    long response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    
    curl_easy_cleanup(curl);
    fclose(file);
    
    return (res == CURLE_OK && response_code == 200) ? YANDEX_SUCCESS : YANDEX_ERROR_DOWNLOAD;
}

int yandex_download_images(yandex_session_t *session, const char *download_dir) {
    if (!session || !session->images || !download_dir) return YANDEX_ERROR_INVALID_PARAM;
    
    if (mkdir(download_dir, 0755) != 0 && errno != EEXIST) {
        return YANDEX_ERROR_MEMORY;
    }
    
    for (size_t i = 0; i < session->image_count; i++) {
        session->images[i].download_status = 0;
        session->images[i].local_path = NULL;
    }
    
    int success_count = 0;
    
    for (size_t i = 0; i < session->image_count; i++) {
        char filename[512];
        const char *ext = get_file_extension(session->images[i].url);
        snprintf(filename, sizeof(filename), "%s/image_%zu%s", 
                 download_dir, session->images[i].index, ext);
        
        int result = yandex_download_single_image(session->images[i].url, filename);
        
        if (result == YANDEX_SUCCESS) {
            session->images[i].download_status = 1;
            session->images[i].local_path = strdup(filename);
            success_count++;
        } else {
            session->images[i].download_status = -1;
        }
    }
    
    return YANDEX_SUCCESS;
}

int yandex_download_image_range(yandex_session_t *session, size_t start, size_t end, const char *download_dir) {
    if (!session || !session->images || !download_dir) return YANDEX_ERROR_INVALID_PARAM;
    
    if (start >= session->image_count) return YANDEX_ERROR_INVALID_PARAM;
    if (end > session->image_count) end = session->image_count;
    if (start >= end) return YANDEX_ERROR_INVALID_PARAM;
    
    if (mkdir(download_dir, 0755) != 0 && errno != EEXIST) {
        return YANDEX_ERROR_MEMORY;
    }
    
    for (size_t i = start; i < end; i++) {
        session->images[i].download_status = 0;
        session->images[i].local_path = NULL;
    }
    
    int success_count = 0;
    
    for (size_t i = start; i < end; i++) {
        char filename[512];
        const char *ext = get_file_extension(session->images[i].url);
        snprintf(filename, sizeof(filename), "%s/image_%zu%s", 
                 download_dir, session->images[i].index, ext);
        
        int result = yandex_download_single_image(session->images[i].url, filename);
        
        if (result == YANDEX_SUCCESS) {
            session->images[i].download_status = 1;
            session->images[i].local_path = strdup(filename);
            success_count++;
        } else {
            session->images[i].download_status = -1;
        }
    }
    
    return YANDEX_SUCCESS;
}