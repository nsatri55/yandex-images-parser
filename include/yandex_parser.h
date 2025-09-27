#ifndef YANDEX_PARSER_H
#define YANDEX_PARSER_H

#include <stddef.h>
#include <stdint.h>
#include <pthread.h>

#define YANDEX_SUCCESS 0
#define YANDEX_ERROR_INVALID_PARAM -1
#define YANDEX_ERROR_MEMORY -2
#define YANDEX_ERROR_NETWORK -3
#define YANDEX_ERROR_DRIVER -4
#define YANDEX_ERROR_NO_IMAGES -5
#define YANDEX_ERROR_DOWNLOAD -6
#define YANDEX_ERROR_NO_VIDEOS -7
#define YANDEX_ERROR_DRIVER_NOT_FOUND -8
#define YANDEX_ERROR_DRIVER_START_FAILED -9
#define YANDEX_ERROR_DRIVER_CONNECTION -10
#define YANDEX_ERROR_FIREFOX_NOT_FOUND -11
#define YANDEX_ERROR_SESSION_TIMEOUT -12
#define YANDEX_ERROR_HTTP_ERROR -13
#define YANDEX_ERROR_FILE_WRITE -14
#define YANDEX_ERROR_STREAM_INIT -15
#define YANDEX_ERROR_THREAD_CREATE -16
#define YANDEX_ERROR_THREAD_JOIN -17

typedef struct {
    char *url;
    char *title;
    char *source;
    size_t index;
    char *local_path;
    int download_status;
} yandex_image_t;

typedef struct {
    char *url;
    char *title;
    char *source;
    char *thumbnail_url;
    size_t index;
    char *local_path;
    int download_status;
    int duration;
    char *format;
} yandex_video_t;


typedef struct {
    char *session_id;
    char *query;
    yandex_image_t *images;
    size_t image_count;
    yandex_video_t *videos;
    size_t video_count;
    int scroll_count;
} yandex_session_t;

typedef struct {
    char *url;
    char *filename;
    int result;
    size_t file_size;
    pthread_t thread;
} yandex_download_task_t;

typedef struct {
    yandex_download_task_t *tasks;
    size_t task_count;
    size_t completed_count;
    pthread_mutex_t mutex;
    int max_threads;
} yandex_download_pool_t;

yandex_session_t* yandex_create_session(const char *query, int scroll_count);
int yandex_search_images(yandex_session_t *session);
int yandex_search_videos(yandex_session_t *session);
int yandex_get_image_range(yandex_session_t *session, size_t start, size_t end, yandex_image_t **result, size_t *count);
int yandex_get_video_range(yandex_session_t *session, size_t start, size_t end, yandex_video_t **result, size_t *count);
void yandex_free_session(yandex_session_t *session);
void yandex_free_images(yandex_image_t *images, size_t count);
void yandex_free_videos(yandex_video_t *videos, size_t count);

int yandex_save_images(yandex_session_t *session, const char *filename);
int yandex_save_image_range(yandex_session_t *session, size_t start, size_t end, const char *filename);
int yandex_save_videos(yandex_session_t *session, const char *filename);
int yandex_save_video_range(yandex_session_t *session, size_t start, size_t end, const char *filename);

int yandex_download_images(yandex_session_t *session, const char *download_dir);
int yandex_download_image_range(yandex_session_t *session, size_t start, size_t end, const char *download_dir);
int yandex_download_single_image(const char *url, const char *filename);
int yandex_download_videos(yandex_session_t *session, const char *download_dir);
int yandex_download_video_range(yandex_session_t *session, size_t start, size_t end, const char *download_dir);
int yandex_download_single_video(const char *url, const char *filename);
int yandex_download_single_video_stream(const char *url, const char *filename, size_t chunk_size);

int yandex_download_images_multithreaded(yandex_session_t *session, const char *download_dir, int max_threads);
int yandex_download_videos_multithreaded(yandex_session_t *session, const char *download_dir, int max_threads);
int yandex_download_multithreaded_pool(yandex_download_pool_t *pool);
void yandex_free_download_pool(yandex_download_pool_t *pool);

int yandex_start_geckodriver(void);
void yandex_stop_geckodriver(void);
int yandex_is_geckodriver_running(void);
int yandex_set_geckodriver_port(int port);
int yandex_get_geckodriver_port(void);

const char* yandex_get_error_string(int error_code);
int yandex_get_image_count(yandex_session_t *session);
int yandex_get_video_count(yandex_session_t *session);
int yandex_get_download_status(yandex_image_t *image);
int yandex_get_video_download_status(yandex_video_t *video);

#endif // YANDEX_PARSER_H
