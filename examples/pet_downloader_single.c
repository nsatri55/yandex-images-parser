#include "../include/yandex_parser.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Single Pet Downloader\n");
    
    yandex_set_geckodriver_port(4445);
    
    if (!yandex_is_geckodriver_running()) {
        printf("Error: GeckoDriver not running on port %d\n", yandex_get_geckodriver_port());
        return 1;
    }
    
    printf("Downloading 1 cat picture...\n");
    yandex_session_t *cats = yandex_create_session("cats", 2);
    if (!cats) {
        printf("Error: Failed to create cat session\n");
        return 1;
    }
    
    int cat_result = yandex_search_images(cats);
    if (cat_result != YANDEX_SUCCESS) {
        printf("Error: %s\n", yandex_get_error_string(cat_result));
        yandex_free_session(cats);
        return 1;
    }
    
    printf("Found %d cat images\n", yandex_get_image_count(cats));
    
    if (yandex_download_image_range(cats, 0, 1, "single_cat") == YANDEX_SUCCESS) {
        printf("Downloaded 1 cat picture\n");
    } else {
        printf("Failed to download cat\n");
    }
    
    yandex_free_session(cats);
    
    printf("\nDownloading 1 dog picture...\n");
    yandex_session_t *dogs = yandex_create_session("dogs", 2);
    if (!dogs) {
        printf("Error: Failed to create dog session\n");
        return 1;
    }
    
    int dog_result = yandex_search_images(dogs);
    if (dog_result != YANDEX_SUCCESS) {
        printf("Error: %s\n", yandex_get_error_string(dog_result));
        yandex_free_session(dogs);
        return 1;
    }
    
    printf("Found %d dog images\n", yandex_get_image_count(dogs));
    
    if (yandex_download_image_range(dogs, 0, 1, "single_dog") == YANDEX_SUCCESS) {
        printf("Downloaded 1 dog picture\n");
    } else {
        printf("Failed to download dog\n");
    }
    
    yandex_free_session(dogs);
    
    printf("\nDownload complete!\n");
    printf("Check the 'single_cat' and 'single_dog' folders for your pictures.\n");
    
    return 0;
}
