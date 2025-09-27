#include "../include/yandex_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <curl/curl.h>

static size_t download_write_callback(void *contents, size_t size, size_t nmemb, FILE *file) {
    return fwrite(contents, size, nmemb, file);
}

static void* download_worker_thread(void *arg) {
    yandex_download_task_t *task = (yandex_download_task_t *)arg;
    
    FILE *file = fopen(task->filename, "wb");
    if (!file) {
        task->result = YANDEX_ERROR_FILE_WRITE;
        return NULL;
    }
    
    CURL *curl = curl_easy_init();
    if (!curl) {
        fclose(file);
        task->result = YANDEX_ERROR_NETWORK;
        return NULL;
    }
    
    curl_easy_setopt(curl, CURLOPT_URL, task->url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, download_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36");
    
    CURLcode res = curl_easy_perform(curl);
    fclose(file);
    curl_easy_cleanup(curl);
    
    if (res != CURLE_OK) {
        remove(task->filename);
        task->result = YANDEX_ERROR_DOWNLOAD;
    } else {
        task->result = YANDEX_SUCCESS;
        struct stat st;
        if (stat(task->filename, &st) == 0) {
            task->file_size = st.st_size;
        }
    }
    
    return NULL;
}

int yandex_download_multithreaded_pool(yandex_download_pool_t *pool) {
    if (!pool || !pool->tasks) return YANDEX_ERROR_INVALID_PARAM;
    
    pthread_mutex_init(&pool->mutex, NULL);
    pool->completed_count = 0;
    
    for (size_t i = 0; i < pool->task_count; i++) {
        int result = pthread_create(&pool->tasks[i].thread, NULL, download_worker_thread, &pool->tasks[i]);
        if (result != 0) {
            return YANDEX_ERROR_THREAD_CREATE;
        }
    }
    
    for (size_t i = 0; i < pool->task_count; i++) {
        int result = pthread_join(pool->tasks[i].thread, NULL);
        if (result != 0) {
            return YANDEX_ERROR_THREAD_JOIN;
        }
        
        pthread_mutex_lock(&pool->mutex);
        pool->completed_count++;
        pthread_mutex_unlock(&pool->mutex);
    }
    
    pthread_mutex_destroy(&pool->mutex);
    return YANDEX_SUCCESS;
}

int yandex_download_images_multithreaded(yandex_session_t *session, const char *download_dir, int max_threads) {
    if (!session || !session->images || !download_dir) return YANDEX_ERROR_INVALID_PARAM;
    if (max_threads <= 0) max_threads = 4;
    
    yandex_download_pool_t pool;
    pool.task_count = session->image_count;
    pool.tasks = malloc(sizeof(yandex_download_task_t) * pool.task_count);
    if (!pool.tasks) return YANDEX_ERROR_MEMORY;
    
    for (size_t i = 0; i < pool.task_count; i++) {
        char filename[512];
        snprintf(filename, sizeof(filename), "%s/image_%zu.jpg", download_dir, i);
        
        pool.tasks[i].url = session->images[i].url;
        pool.tasks[i].filename = malloc(strlen(filename) + 1);
        if (pool.tasks[i].filename) {
            strcpy(pool.tasks[i].filename, filename);
        }
        pool.tasks[i].result = 0;
        pool.tasks[i].file_size = 0;
    }
    
    int result = yandex_download_multithreaded_pool(&pool);
    
    for (size_t i = 0; i < pool.task_count; i++) {
        if (pool.tasks[i].result == YANDEX_SUCCESS) {
            session->images[i].download_status = 1;
            session->images[i].local_path = strdup(pool.tasks[i].filename);
        } else {
            session->images[i].download_status = -1;
        }
        free(pool.tasks[i].filename);
    }
    
    free(pool.tasks);
    return result;
}

int yandex_download_videos_multithreaded(yandex_session_t *session, const char *download_dir, int max_threads) {
    if (!session || !session->videos || !download_dir) return YANDEX_ERROR_INVALID_PARAM;
    if (max_threads <= 0) max_threads = 4;
    
    yandex_download_pool_t pool;
    pool.task_count = session->video_count;
    pool.tasks = malloc(sizeof(yandex_download_task_t) * pool.task_count);
    if (!pool.tasks) return YANDEX_ERROR_MEMORY;
    
    for (size_t i = 0; i < pool.task_count; i++) {
        char filename[512];
        snprintf(filename, sizeof(filename), "%s/video_%zu.mp4", download_dir, i);
        
        pool.tasks[i].url = session->videos[i].url;
        pool.tasks[i].filename = malloc(strlen(filename) + 1);
        if (pool.tasks[i].filename) {
            strcpy(pool.tasks[i].filename, filename);
        }
        pool.tasks[i].result = 0;
        pool.tasks[i].file_size = 0;
    }
    
    int result = yandex_download_multithreaded_pool(&pool);
    
    for (size_t i = 0; i < pool.task_count; i++) {
        if (pool.tasks[i].result == YANDEX_SUCCESS) {
            session->videos[i].download_status = 1;
            session->videos[i].local_path = strdup(pool.tasks[i].filename);
        } else {
            session->videos[i].download_status = -1;
        }
        free(pool.tasks[i].filename);
    }
    
    free(pool.tasks);
    return result;
}

void yandex_free_download_pool(yandex_download_pool_t *pool) {
    if (!pool) return;
    
    if (pool->tasks) {
        for (size_t i = 0; i < pool->task_count; i++) {
            free(pool->tasks[i].filename);
        }
        free(pool->tasks);
    }
    
    pthread_mutex_destroy(&pool->mutex);
}
