#include "../include/yandex_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yandex_save_images(yandex_session_t *session, const char *filename) {
    if (!session || !session->images || !filename) return YANDEX_ERROR_INVALID_PARAM;
    
    FILE *file = fopen(filename, "w");
    if (!file) return YANDEX_ERROR_MEMORY;
    
    for (size_t i = 0; i < session->image_count; i++) {
        fprintf(file, "%s\n", session->images[i].url);
    }
    
    fclose(file);
    return YANDEX_SUCCESS;
}

int yandex_save_image_range(yandex_session_t *session, size_t start, size_t end, const char *filename) {
    if (!session || !session->images || !filename) return YANDEX_ERROR_INVALID_PARAM;
    
    if (start >= session->image_count) return YANDEX_ERROR_INVALID_PARAM;
    if (end > session->image_count) end = session->image_count;
    if (start >= end) return YANDEX_ERROR_INVALID_PARAM;
    
    FILE *file = fopen(filename, "w");
    if (!file) return YANDEX_ERROR_MEMORY;
    
    for (size_t i = start; i < end; i++) {
        fprintf(file, "%s\n", session->images[i].url);
    }
    
    fclose(file);
    return YANDEX_SUCCESS;
}

int yandex_save_videos(yandex_session_t *session, const char *filename) {
    if (!session || !session->videos || !filename) return YANDEX_ERROR_INVALID_PARAM;
    
    FILE *file = fopen(filename, "w");
    if (!file) return YANDEX_ERROR_MEMORY;
    
    for (size_t i = 0; i < session->video_count; i++) {
        fprintf(file, "%s\n", session->videos[i].url);
    }
    
    fclose(file);
    return YANDEX_SUCCESS;
}

int yandex_save_video_range(yandex_session_t *session, size_t start, size_t end, const char *filename) {
    if (!session || !session->videos || !filename) return YANDEX_ERROR_INVALID_PARAM;
    
    if (start >= session->video_count) return YANDEX_ERROR_INVALID_PARAM;
    if (end > session->video_count) end = session->video_count;
    if (start >= end) return YANDEX_ERROR_INVALID_PARAM;
    
    FILE *file = fopen(filename, "w");
    if (!file) return YANDEX_ERROR_MEMORY;
    
    for (size_t i = start; i < end; i++) {
        fprintf(file, "%s\n", session->videos[i].url);
    }
    
    fclose(file);
    return YANDEX_SUCCESS;
}

