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
    
    yandex_session_t* session = yandex_create_session("nature", 3);
    if (!session) {
        fprintf(stderr, "Error: Failed to create session\n");
        yandex_stop_geckodriver();
        return 1;
    }
    
    printf("Searching for images...\n");
    int result = yandex_search_images(session);
    if (result != YANDEX_SUCCESS) {
        fprintf(stderr, "Error: %s (code: %d)\n", yandex_get_error_string(result), result);
        yandex_free_session(session);
        yandex_stop_geckodriver();
        return 1;
    }
    printf("Image search completed\n");
    
    int image_count = yandex_get_image_count(session);
    if (image_count > 0) {
        printf("Found %d images\n", image_count);
        printf("Downloading first 5 images...\n");
        
        result = yandex_download_image_range(session, 0, 5, "images");
        if (result == YANDEX_SUCCESS) {
            printf("Images downloaded successfully\n");
        } else {
            fprintf(stderr, "Download error: %s\n", yandex_get_error_string(result));
        }
    } else {
        printf("No images found\n");
    }
    
    yandex_free_session(session);
    yandex_stop_geckodriver();
    return 0;
}
