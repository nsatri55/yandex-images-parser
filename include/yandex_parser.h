#ifndef YANDEX_PARSER_H
#define YANDEX_PARSER_H

#include <stddef.h>
#include <stdint.h>

#define YANDEX_SUCCESS 0
#define YANDEX_ERROR_INVALID_PARAM -1
#define YANDEX_ERROR_MEMORY -2
#define YANDEX_ERROR_NETWORK -3
#define YANDEX_ERROR_DRIVER -4
#define YANDEX_ERROR_NO_IMAGES -5
#define YANDEX_ERROR_DOWNLOAD -6

typedef struct {
    char *url;
    char *title;
    char *source;
    size_t index;
    char *local_path;
    int download_status;
} yandex_image_t;

typedef struct {
    char *session_id;
    char *query;
    yandex_image_t *images;
    size_t image_count;
    int scroll_count;
} yandex_session_t;

yandex_session_t* yandex_create_session(const char *query, int scroll_count);
int yandex_search_images(yandex_session_t *session);
int yandex_get_image_range(yandex_session_t *session, size_t start, size_t end, yandex_image_t **result, size_t *count);
void yandex_free_session(yandex_session_t *session);
void yandex_free_images(yandex_image_t *images, size_t count);

int yandex_save_images(yandex_session_t *session, const char *filename);
int yandex_save_image_range(yandex_session_t *session, size_t start, size_t end, const char *filename);

int yandex_download_images(yandex_session_t *session, const char *download_dir);
int yandex_download_image_range(yandex_session_t *session, size_t start, size_t end, const char *download_dir);
int yandex_download_single_image(const char *url, const char *filename);

int yandex_start_geckodriver(void);
void yandex_stop_geckodriver(void);
int yandex_is_geckodriver_running(void);
int yandex_set_geckodriver_port(int port);
int yandex_get_geckodriver_port(void);

const char* yandex_get_error_string(int error_code);
int yandex_get_image_count(yandex_session_t *session);
int yandex_get_download_status(yandex_image_t *image);

#endif // YANDEX_PARSER_H
