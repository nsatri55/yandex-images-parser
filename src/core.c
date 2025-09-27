#include "../include/yandex_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>

struct ResponseData {
    char *data;
    size_t size;
};

static size_t WriteCallback(char *contents, size_t size, size_t nmemb, void *userp) {
    struct ResponseData *response = (struct ResponseData *)userp;
    size_t total_size = size * nmemb;
    char *ptr = realloc(response->data, response->size + total_size + 1);
    if (!ptr) return 0;
    
    response->data = ptr;
    memcpy(&(response->data[response->size]), contents, total_size);
    response->size += total_size;
    response->data[response->size] = 0;
    return total_size;
}

static int make_request(const char *url, const char *post_data, struct ResponseData *response) {
    CURL *curl = curl_easy_init();
    if (!curl) return -1;
    
    response->data = malloc(1);
    response->size = 0;
    response->data[0] = '\0';
    
    struct curl_slist *headers = curl_slist_append(NULL, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
    
    if (post_data) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
    }
    
    CURLcode res = curl_easy_perform(curl);
    long response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    
    return (res == CURLE_OK) ? (int)response_code : -1;
}

static char* start_geckodriver_session(void) {
    const char *session_data = "{\"capabilities\":{\"alwaysMatch\":{\"browserName\":\"firefox\",\"moz:firefoxOptions\":{\"binary\":\"/usr/bin/firefox-esr\",\"args\":[\"-headless\"]}}}}";
    
    extern int yandex_get_geckodriver_port(void);
    int port = yandex_get_geckodriver_port();
    
    char url[64];
    snprintf(url, sizeof(url), "http://localhost:%d/session", port);
    
    struct ResponseData response;
    int status = make_request(url, session_data, &response);
    
    if (status == 200) {
        char *session_start = strstr(response.data, "\"sessionId\":\"");
        if (session_start) {
            session_start += 13;
            char *session_end = strchr(session_start, '"');
            if (session_end) {
                size_t id_len = session_end - session_start;
                char *session_id = malloc(id_len + 1);
                strncpy(session_id, session_start, id_len);
                session_id[id_len] = '\0';
                free(response.data);
                return session_id;
            }
        }
    }
    free(response.data);
    return NULL;
}

static int navigate_to_yandex_images(const char *session_id, const char *query) {
    char url[512];
    snprintf(url, sizeof(url), "https://yandex.ru/images/search?text=%s&p=1", query);
    
    extern int yandex_get_geckodriver_port(void);
    int port = yandex_get_geckodriver_port();
    
    char request_url[256];
    snprintf(request_url, sizeof(request_url), "http://localhost:%d/session/%s/url", port, session_id);
    
    char nav_data[1024];
    snprintf(nav_data, sizeof(nav_data), "{\"url\":\"%s\"}", url);
    
    struct ResponseData response;
    int status = make_request(request_url, nav_data, &response);
    free(response.data);
    return (status == 200) ? 0 : -1;
}

static int navigate_to_yandex_videos(const char *session_id, const char *query) {
    char url[512];
    snprintf(url, sizeof(url), "https://yandex.ru/video/search?text=%s&p=1", query);
    
    extern int yandex_get_geckodriver_port(void);
    int port = yandex_get_geckodriver_port();
    
    char request_url[256];
    snprintf(request_url, sizeof(request_url), "http://localhost:%d/session/%s/url", port, session_id);
    
    char nav_data[1024];
    snprintf(nav_data, sizeof(nav_data), "{\"url\":\"%s\"}", url);
    
    struct ResponseData response;
    int status = make_request(request_url, nav_data, &response);
    free(response.data);
    return (status == 200) ? 0 : -1;
}


static int scroll_for_more_images(const char *session_id, int count) {
    extern int yandex_get_geckodriver_port(void);
    int port = yandex_get_geckodriver_port();
    
    char request_url[256];
    snprintf(request_url, sizeof(request_url), "http://localhost:%d/session/%s/execute/sync", port, session_id);
    
    for (int i = 0; i < count; i++) {
        const char *script = "{\"script\":\"window.scrollTo(0, document.body.scrollHeight);\",\"args\":[]}";
        struct ResponseData response;
        make_request(request_url, script, &response);
        free(response.data);
        sleep(1);
    }
    return 0;
}

static int extract_images_from_page(const char *session_id, yandex_image_t **images, size_t *count) {
    extern int yandex_get_geckodriver_port(void);
    int port = yandex_get_geckodriver_port();
    
    char request_url[256];
    snprintf(request_url, sizeof(request_url), "http://localhost:%d/session/%s/execute/sync", port, session_id);
    
    const char *script = "{\"script\":\"var images = []; var imgs = document.querySelectorAll('img[src], img[data-src]'); for (var i = 0; i < imgs.length; i++) { var img = imgs[i]; var src = img.src || img.getAttribute('data-src'); if (src && src.startsWith('http') && !src.includes('captcha') && !src.includes('adfstat')) { images.push({url: src, alt: img.alt || '', index: i}); } } return images;\",\"args\":[]}";
    
    struct ResponseData response;
    int status = make_request(request_url, script, &response);
    
    if (status == 200 && response.data) {
        *images = NULL;
        *count = 0;
        
        if (strstr(response.data, "captcha") != NULL || strstr(response.data, "adfstat") != NULL) {
            free(response.data);
            return YANDEX_ERROR_DRIVER;
        }
        
        if (strstr(response.data, "\"url\":") == NULL) {
            free(response.data);
            return YANDEX_ERROR_NO_IMAGES;
        }
        
        char *data = response.data;
        char *url_start;
        size_t image_count = 0;
        
        char *temp_data = data;
        while ((temp_data = strstr(temp_data, "\"url\":")) != NULL) {
            image_count++;
            temp_data += 5;
        }
        
        if (image_count > 0) {
            *images = malloc(sizeof(yandex_image_t) * image_count);
            if (!*images) {
                free(response.data);
                return YANDEX_ERROR_MEMORY;
            }
            *count = image_count;
            
            for (size_t i = 0; i < image_count; i++) {
                (*images)[i].url = NULL;
                (*images)[i].title = NULL;
                (*images)[i].source = NULL;
                (*images)[i].index = i;
                (*images)[i].local_path = NULL;
                (*images)[i].download_status = 0;
            }
            
            size_t current_index = 0;
            char *search_data = data;
            
            while ((url_start = strstr(search_data, "\"url\":")) != NULL && current_index < image_count) {
                char *url_value = strchr(url_start, ':');
                if (url_value) {
                    url_value = strchr(url_value, '"');
                    if (url_value) {
                        url_value++;
                        char *url_end = strchr(url_value, '"');
                        if (url_end) {
                            size_t url_len = url_end - url_value;
                            if (url_len > 0 && url_len < 2048) {
                                (*images)[current_index].url = malloc(url_len + 1);
                                if ((*images)[current_index].url) {
                                    strncpy((*images)[current_index].url, url_value, url_len);
                                    (*images)[current_index].url[url_len] = '\0';
                                    
                                    char *alt_start = strstr(url_end, "\"alt\":");
                                    if (alt_start) {
                                        char *alt_value = strchr(alt_start, ':');
                                        if (alt_value) {
                                            alt_value = strchr(alt_value, '"');
                                            if (alt_value) {
                                                alt_value++;
                                                char *alt_end = strchr(alt_value, '"');
                                                if (alt_end) {
                                                    size_t alt_len = alt_end - alt_value;
                                                    if (alt_len > 0 && alt_len < 512) {
                                                        (*images)[current_index].title = malloc(alt_len + 1);
                                                        if ((*images)[current_index].title) {
                                                            strncpy((*images)[current_index].title, alt_value, alt_len);
                                                            (*images)[current_index].title[alt_len] = '\0';
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    
                                    if (!(*images)[current_index].title) {
                                        (*images)[current_index].title = strdup("");
                                    }
                                    
                                    current_index++;
                                }
                            }
                        }
                    }
                }
                search_data = url_start + 1;
            }
        }
        
        free(response.data);
        return YANDEX_SUCCESS;
    }
    
    if (response.data) free(response.data);
    return YANDEX_ERROR_NETWORK;
}

static int extract_videos_from_page(const char *session_id, yandex_video_t **videos, size_t *count) {
    extern int yandex_get_geckodriver_port(void);
    int port = yandex_get_geckodriver_port();
    
    char request_url[256];
    snprintf(request_url, sizeof(request_url), "http://localhost:%d/session/%s/execute/sync", port, session_id);
    
    const char *script = "{\"script\":\"var videos = []; var videoElements = document.querySelectorAll('video, [data-video-url], .video-item, .serp-item[data-video-url]'); for (var i = 0; i < videoElements.length; i++) { var elem = videoElements[i]; var videoUrl = elem.src || elem.getAttribute('data-video-url') || elem.getAttribute('data-src'); var thumbnailUrl = ''; var title = elem.title || elem.alt || ''; var duration = -1; var format = 'mp4'; if (videoUrl && videoUrl.startsWith('http')) { var thumb = elem.querySelector('img'); if (thumb) thumbnailUrl = thumb.src || thumb.getAttribute('data-src') || ''; var dur = elem.getAttribute('data-duration') || elem.getAttribute('duration'); if (dur) duration = parseInt(dur) || -1; videos.push({url: videoUrl, title: title, thumbnail: thumbnailUrl, duration: duration, format: format, index: i}); } } return videos;\",\"args\":[]}";
    
    struct ResponseData response;
    int status = make_request(request_url, script, &response);
    
    if (status == 200 && response.data) {
        *videos = NULL;
        *count = 0;
        
        if (strstr(response.data, "captcha") != NULL || strstr(response.data, "adfstat") != NULL) {
            free(response.data);
            return YANDEX_ERROR_DRIVER;
        }
        
        if (strstr(response.data, "\"url\":") == NULL) {
            free(response.data);
            return YANDEX_ERROR_NO_VIDEOS;
        }
        
        char *data = response.data;
        char *url_start;
        size_t video_count = 0;
        
        char *temp_data = data;
        while ((temp_data = strstr(temp_data, "\"url\":")) != NULL) {
            video_count++;
            temp_data += 5;
        }
        
        if (video_count > 0) {
            *videos = malloc(sizeof(yandex_video_t) * video_count);
            if (!*videos) {
                free(response.data);
                return YANDEX_ERROR_MEMORY;
            }
            *count = video_count;
            
            for (size_t i = 0; i < video_count; i++) {
                (*videos)[i].url = NULL;
                (*videos)[i].title = NULL;
                (*videos)[i].source = NULL;
                (*videos)[i].thumbnail_url = NULL;
                (*videos)[i].index = i;
                (*videos)[i].local_path = NULL;
                (*videos)[i].download_status = 0;
                (*videos)[i].duration = -1;
                (*videos)[i].format = NULL;
            }
            
            size_t current_index = 0;
            char *search_data = data;
            
            while ((url_start = strstr(search_data, "\"url\":")) != NULL && current_index < video_count) {
                char *url_value = strchr(url_start, ':');
                if (url_value) {
                    url_value = strchr(url_value, '"');
                    if (url_value) {
                        url_value++;
                        char *url_end = strchr(url_value, '"');
                        if (url_end) {
                            size_t url_len = url_end - url_value;
                            if (url_len > 0 && url_len < 2048) {
                                (*videos)[current_index].url = malloc(url_len + 1);
                                if ((*videos)[current_index].url) {
                                    strncpy((*videos)[current_index].url, url_value, url_len);
                                    (*videos)[current_index].url[url_len] = '\0';
                                    
                                    char *title_start = strstr(url_end, "\"title\":");
                                    if (title_start) {
                                        char *title_value = strchr(title_start, ':');
                                        if (title_value) {
                                            title_value = strchr(title_value, '"');
                                            if (title_value) {
                                                title_value++;
                                                char *title_end = strchr(title_value, '"');
                                                if (title_end) {
                                                    size_t title_len = title_end - title_value;
                                                    if (title_len > 0 && title_len < 512) {
                                                        (*videos)[current_index].title = malloc(title_len + 1);
                                                        if ((*videos)[current_index].title) {
                                                            strncpy((*videos)[current_index].title, title_value, title_len);
                                                            (*videos)[current_index].title[title_len] = '\0';
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    
                                    char *thumb_start = strstr(url_end, "\"thumbnail\":");
                                    if (thumb_start) {
                                        char *thumb_value = strchr(thumb_start, ':');
                                        if (thumb_value) {
                                            thumb_value = strchr(thumb_value, '"');
                                            if (thumb_value) {
                                                thumb_value++;
                                                char *thumb_end = strchr(thumb_value, '"');
                                                if (thumb_end) {
                                                    size_t thumb_len = thumb_end - thumb_value;
                                                    if (thumb_len > 0 && thumb_len < 2048) {
                                                        (*videos)[current_index].thumbnail_url = malloc(thumb_len + 1);
                                                        if ((*videos)[current_index].thumbnail_url) {
                                                            strncpy((*videos)[current_index].thumbnail_url, thumb_value, thumb_len);
                                                            (*videos)[current_index].thumbnail_url[thumb_len] = '\0';
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    
                                    char *dur_start = strstr(url_end, "\"duration\":");
                                    if (dur_start) {
                                        char *dur_value = strchr(dur_start, ':');
                                        if (dur_value) {
                                            dur_value++;
                                            char *dur_end = strchr(dur_value, ',');
                                            if (!dur_end) dur_end = strchr(dur_value, '}');
                                            if (dur_end) {
                                                size_t dur_len = dur_end - dur_value;
                                                char dur_str[32];
                                                if (dur_len < sizeof(dur_str)) {
                                                    strncpy(dur_str, dur_value, dur_len);
                                                    dur_str[dur_len] = '\0';
                                                    (*videos)[current_index].duration = atoi(dur_str);
                                                }
                                            }
                                        }
                                    }
                                    
                                    if (!(*videos)[current_index].title) {
                                        (*videos)[current_index].title = strdup("");
                                    }
                                    if (!(*videos)[current_index].thumbnail_url) {
                                        (*videos)[current_index].thumbnail_url = strdup("");
                                    }
                                    (*videos)[current_index].format = strdup("mp4");
                                    
                                    current_index++;
                                }
                            }
                        }
                    }
                }
                search_data = url_start + 1;
            }
        }
        
        free(response.data);
        return YANDEX_SUCCESS;
    }
    
    if (response.data) free(response.data);
    return YANDEX_ERROR_NETWORK;
}


static void close_session(const char *session_id) {
    if (!session_id) return;
    
    extern int yandex_get_geckodriver_port(void);
    int port = yandex_get_geckodriver_port();
    
    char request_url[256];
    snprintf(request_url, sizeof(request_url), "http://localhost:%d/session/%s", port, session_id);
    
    CURL *curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, request_url);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 2L);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
}

yandex_session_t* yandex_create_session(const char *query, int scroll_count) {
    if (!query) return NULL;
    
    yandex_session_t *session = malloc(sizeof(yandex_session_t));
    if (!session) return NULL;
    
    session->query = strdup(query);
    session->scroll_count = scroll_count;
    session->images = NULL;
    session->image_count = 0;
    session->videos = NULL;
    session->video_count = 0;
    session->session_id = NULL;
    
    return session;
}

int yandex_search_images(yandex_session_t *session) {
    if (!session) return YANDEX_ERROR_INVALID_PARAM;
    
    CURLcode curl_result = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (curl_result != CURLE_OK) {
        return YANDEX_ERROR_NETWORK;
    }
    
    session->session_id = start_geckodriver_session();
    if (!session->session_id) {
        curl_global_cleanup();
        return YANDEX_ERROR_DRIVER;
    }
    
    int nav_result = navigate_to_yandex_images(session->session_id, session->query);
    if (nav_result != 0) {
        close_session(session->session_id);
        free(session->session_id);
        session->session_id = NULL;
        curl_global_cleanup();
        return YANDEX_ERROR_NETWORK;
    }
    
    sleep(3);
    scroll_for_more_images(session->session_id, session->scroll_count);
    int result = extract_images_from_page(session->session_id, &session->images, &session->image_count);
    
    close_session(session->session_id);
    free(session->session_id);
    session->session_id = NULL;
    
    curl_global_cleanup();
    return result;
}

int yandex_search_videos(yandex_session_t *session) {
    if (!session) return YANDEX_ERROR_INVALID_PARAM;
    
    CURLcode curl_result = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (curl_result != CURLE_OK) {
        return YANDEX_ERROR_NETWORK;
    }
    
    session->session_id = start_geckodriver_session();
    if (!session->session_id) {
        curl_global_cleanup();
        return YANDEX_ERROR_DRIVER;
    }
    
    int nav_result = navigate_to_yandex_videos(session->session_id, session->query);
    if (nav_result != 0) {
        close_session(session->session_id);
        free(session->session_id);
        session->session_id = NULL;
        curl_global_cleanup();
        return YANDEX_ERROR_NETWORK;
    }
    
    sleep(3);
    scroll_for_more_images(session->session_id, session->scroll_count);
    int result = extract_videos_from_page(session->session_id, &session->videos, &session->video_count);
    
    close_session(session->session_id);
    free(session->session_id);
    session->session_id = NULL;
    
    curl_global_cleanup();
    return result;
}


int yandex_get_image_range(yandex_session_t *session, size_t start, size_t end, yandex_image_t **result, size_t *count) {
    if (!session || !session->images || start >= session->image_count) return YANDEX_ERROR_INVALID_PARAM;
    
    if (end > session->image_count) end = session->image_count;
    if (start >= end) return YANDEX_ERROR_INVALID_PARAM;
    
    size_t range_count = end - start;
    *result = malloc(sizeof(yandex_image_t) * range_count);
    *count = range_count;
    
    for (size_t i = 0; i < range_count; i++) {
        size_t src_index = start + i;
        (*result)[i].url = strdup(session->images[src_index].url);
        (*result)[i].title = session->images[src_index].title ? strdup(session->images[src_index].title) : NULL;
        (*result)[i].source = session->images[src_index].source ? strdup(session->images[src_index].source) : NULL;
        (*result)[i].index = src_index;
        (*result)[i].local_path = NULL;
        (*result)[i].download_status = 0;
    }
    
    return YANDEX_SUCCESS;
}

int yandex_get_video_range(yandex_session_t *session, size_t start, size_t end, yandex_video_t **result, size_t *count) {
    if (!session || !session->videos || start >= session->video_count) return YANDEX_ERROR_INVALID_PARAM;
    
    if (end > session->video_count) end = session->video_count;
    if (start >= end) return YANDEX_ERROR_INVALID_PARAM;
    
    size_t range_count = end - start;
    *result = malloc(sizeof(yandex_video_t) * range_count);
    *count = range_count;
    
    for (size_t i = 0; i < range_count; i++) {
        size_t src_index = start + i;
        (*result)[i].url = strdup(session->videos[src_index].url);
        (*result)[i].title = session->videos[src_index].title ? strdup(session->videos[src_index].title) : NULL;
        (*result)[i].source = session->videos[src_index].source ? strdup(session->videos[src_index].source) : NULL;
        (*result)[i].thumbnail_url = session->videos[src_index].thumbnail_url ? strdup(session->videos[src_index].thumbnail_url) : NULL;
        (*result)[i].index = src_index;
        (*result)[i].local_path = NULL;
        (*result)[i].download_status = 0;
        (*result)[i].duration = session->videos[src_index].duration;
        (*result)[i].format = session->videos[src_index].format ? strdup(session->videos[src_index].format) : NULL;
    }
    
    return YANDEX_SUCCESS;
}


void yandex_free_session(yandex_session_t *session) {
    if (!session) return;
    
    if (session->query) free(session->query);
    if (session->images) yandex_free_images(session->images, session->image_count);
    if (session->videos) yandex_free_videos(session->videos, session->video_count);
    if (session->session_id) free(session->session_id);
    
    free(session);
}

void yandex_free_images(yandex_image_t *images, size_t count) {
    if (!images) return;
    
    for (size_t i = 0; i < count; i++) {
        if (images[i].url) free(images[i].url);
        if (images[i].title) free(images[i].title);
        if (images[i].source) free(images[i].source);
        if (images[i].local_path) free(images[i].local_path);
    }
    
    free(images);
}

void yandex_free_videos(yandex_video_t *videos, size_t count) {
    if (!videos) return;
    
    for (size_t i = 0; i < count; i++) {
        if (videos[i].url) free(videos[i].url);
        if (videos[i].title) free(videos[i].title);
        if (videos[i].source) free(videos[i].source);
        if (videos[i].thumbnail_url) free(videos[i].thumbnail_url);
        if (videos[i].local_path) free(videos[i].local_path);
        if (videos[i].format) free(videos[i].format);
    }
    
    free(videos);
}

