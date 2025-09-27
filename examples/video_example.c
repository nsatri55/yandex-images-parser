#include <yandex_parser.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Starting GeckoDriver...\n");
    int gecko_result = yandex_start_geckodriver();
    if (gecko_result != YANDEX_SUCCESS) {
        fprintf(stderr, "Error: Failed to start GeckoDriver (code: %d)\n", gecko_result);
        return 1;
    }
    printf("GeckoDriver started successfully\n");
    
    yandex_session_t* session = yandex_create_session("animals", 2);
    if (!session) {
        fprintf(stderr, "Error: Failed to create session\n");
        yandex_stop_geckodriver();
        return 1;
    }
    
    printf("Searching for videos...\n");
    int result = yandex_search_videos(session);
    if (result != YANDEX_SUCCESS) {
        fprintf(stderr, "Error: %s (code: %d)\n", yandex_get_error_string(result), result);
        yandex_free_session(session);
        yandex_stop_geckodriver();
        return 1;
    }
    printf("Video search completed\n");
    
    int video_count = yandex_get_video_count(session);
    if (video_count > 0) {
        printf("Downloading %d videos...\n", video_count);
        
        result = yandex_download_videos(session, "videos");
        if (result == YANDEX_SUCCESS) {
            printf("Videos downloaded successfully\n");
        } else {
            fprintf(stderr, "Download error: %s\n", yandex_get_error_string(result));
        }
    } else {
        printf("No videos found\n");
    }
    
    yandex_free_session(session);
    yandex_stop_geckodriver();
    return 0;
}