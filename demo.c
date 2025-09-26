#include <yandex_parser.h>
#include <stdio.h>

int main() {
    printf("Yandex Images Parser Demo\n");
    
    yandex_session_t* session = yandex_create_session("cats", 2);
    yandex_search_images(session);
    yandex_download_image_range(session, 0, 3, "cats");
    yandex_free_session(session);
    
    printf("Demo completed!\n");
    return 0;
}
