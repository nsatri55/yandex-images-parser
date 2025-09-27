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
        case YANDEX_ERROR_NO_VIDEOS:
            return "No videos found";
        case YANDEX_ERROR_DRIVER_NOT_FOUND:
            return "GeckoDriver not found";
        case YANDEX_ERROR_DRIVER_START_FAILED:
            return "Failed to start GeckoDriver";
        case YANDEX_ERROR_DRIVER_CONNECTION:
            return "GeckoDriver connection failed";
        case YANDEX_ERROR_FIREFOX_NOT_FOUND:
            return "Firefox not found";
        case YANDEX_ERROR_SESSION_TIMEOUT:
            return "WebDriver session timeout";
        case YANDEX_ERROR_HTTP_ERROR:
            return "HTTP error";
        case YANDEX_ERROR_FILE_WRITE:
            return "File write error";
        case YANDEX_ERROR_STREAM_INIT:
            return "Stream initialization error";
        case YANDEX_ERROR_THREAD_CREATE:
            return "Thread creation error";
        case YANDEX_ERROR_THREAD_JOIN:
            return "Thread join error";
        default:
            return "Unknown error";
    }
}

int yandex_get_image_count(yandex_session_t *session) {
    if (!session) return -1;
    return (int)session->image_count;
}

int yandex_get_video_count(yandex_session_t *session) {
    if (!session) return -1;
    return (int)session->video_count;
}

int yandex_get_download_status(yandex_image_t *image) {
    if (!image) return -1;
    return image->download_status;
}

int yandex_get_video_download_status(yandex_video_t *video) {
    if (!video) return -1;
    return video->download_status;
}

