#include "../include/yandex_parser.h"
#include <stdio.h>

const char* yandex_get_error_string(int error_code) {
    switch (error_code) {
        case YANDEX_SUCCESS:
            return "Success";
        case YANDEX_ERROR_INVALID_PARAM:
            return "Invalid parameter";
        case YANDEX_ERROR_MEMORY:
            return "Memory allocation error";
        case YANDEX_ERROR_NETWORK:
            return "Network error";
        case YANDEX_ERROR_DRIVER:
            return "WebDriver error";
        case YANDEX_ERROR_NO_IMAGES:
            return "No images found";
        case YANDEX_ERROR_DOWNLOAD:
            return "Download error";
        default:
            return "Unknown error";
    }
}

int yandex_get_image_count(yandex_session_t *session) {
    if (!session) return -1;
    return (int)session->image_count;
}

int yandex_get_download_status(yandex_image_t *image) {
    if (!image) return -1;
    return image->download_status;
}
