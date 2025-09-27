# Yandex Images Parser Library

A high-performance C library for parsing and downloading images and videos from Yandex Images search engine using WebDriver automation.

## Features

- **Image & Video Search** - Extract media URLs from Yandex Images search results
- **Batch Downloads** - Download multiple images and videos with optimized performance
- **Multi-threaded Downloads** - Concurrent downloads with significant speed improvement
- **Streaming Downloads** - Chunked I/O for faster video downloads
- **WebDriver Integration** - Seamless GeckoDriver/Firefox automation
- **Range Selection** - Download specific image and video ranges
- **File Operations** - Save image and video URLs to files for later processing
- **Optimized Performance** - Assembly-optimized with maximum compiler optimizations
- **Error Handling** - Comprehensive error codes and status reporting

## Prerequisites

- **libcurl4-openssl-dev** - HTTP client library
- **firefox-esr** - Firefox browser for headless operation  
- **geckodriver** - WebDriver implementation for Firefox
- **cmake** - Build system
- **pthread** - Multi-threading support

## Building

The library uses CMake with maximum optimization settings:

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build with optimizations
make

# Install globally (requires sudo)
sudo make install
```

## Performance

### Memory Usage
- **Resident Memory**: ~12MB (RSS)
- **Virtual Memory**: ~93MB (VSZ)

### Optimization Features
- **Compiler Flags**: `-O3 -flto -march=native -mtune=native -funroll-loops -finline-functions -fomit-frame-pointer`
- **Linker Flags**: `-Wl,--gc-sections -Wl,--strip-all`
- **Assembly Optimization**: Native CPU optimizations applied
- **Size Optimization**: Function and data sections optimization

## API Reference

### Session Management
```c
// Create a new search session
yandex_session_t* yandex_create_session(const char* query, int scroll_count);

// Free session memory
void yandex_free_session(yandex_session_t* session);

// Search for images
int yandex_search_images(yandex_session_t* session);

// Search for videos
int yandex_search_videos(yandex_session_t* session);

// Get image range
int yandex_get_image_range(yandex_session_t* session, size_t start, size_t end, yandex_image_t** result, size_t* count);

// Get video range
int yandex_get_video_range(yandex_session_t* session, size_t start, size_t end, yandex_video_t** result, size_t* count);

// Free images
void yandex_free_images(yandex_image_t* images, size_t count);

// Free videos
void yandex_free_videos(yandex_video_t* videos, size_t count);
```

### Download Functions
```c
// Download single image
int yandex_download_single_image(const char* url, const char* filename);

// Download single video
int yandex_download_single_video(const char* url, const char* filename);

// Download single video with streaming
int yandex_download_single_video_stream(const char* url, const char* filename, size_t chunk_size);

// Download all images
int yandex_download_images(yandex_session_t* session, const char* download_dir);

// Download all videos
int yandex_download_videos(yandex_session_t* session, const char* download_dir);

// Download image range
int yandex_download_image_range(yandex_session_t* session, size_t start, size_t end, const char* download_dir);

// Download video range
int yandex_download_video_range(yandex_session_t* session, size_t start, size_t end, const char* download_dir);
```

### Multi-threaded Downloads
```c
// Download images with multiple threads
int yandex_download_images_multithreaded(yandex_session_t *session, const char *download_dir, int max_threads);

// Download videos with multiple threads  
int yandex_download_videos_multithreaded(yandex_session_t *session, const char *download_dir, int max_threads);

// Download with custom thread pool
int yandex_download_multithreaded_pool(yandex_download_pool_t *pool);

// Free download pool
void yandex_free_download_pool(yandex_download_pool_t *pool);
```

### File Operations
```c
// Save image URLs to file
int yandex_save_images(yandex_session_t* session, const char* filename);

// Save video URLs to file
int yandex_save_videos(yandex_session_t* session, const char* filename);

// Save image range to file
int yandex_save_image_range(yandex_session_t* session, size_t start, size_t end, const char* filename);

// Save video range to file
int yandex_save_video_range(yandex_session_t* session, size_t start, size_t end, const char* filename);
```

### Utility Functions
```c
// Get error string
const char* yandex_get_error_string(int error_code);

// Get image count
int yandex_get_image_count(yandex_session_t* session);

// Get video count
int yandex_get_video_count(yandex_session_t* session);

// Get download status
int yandex_get_download_status(yandex_image_t* image);
int yandex_get_video_download_status(yandex_video_t* video);
```

### GeckoDriver Management
```c
// Start GeckoDriver
int yandex_start_geckodriver(void);

// Stop GeckoDriver
void yandex_stop_geckodriver(void);

// Check if GeckoDriver is running
int yandex_is_geckodriver_running(void);

// Set GeckoDriver port
int yandex_set_geckodriver_port(int port);

// Get GeckoDriver port
int yandex_get_geckodriver_port(void);
```

## Data Structures

### Session Structure
```c
typedef struct {
    char *session_id;
    char *query;
    yandex_image_t *images;
    size_t image_count;
    yandex_video_t *videos;
    size_t video_count;
    int scroll_count;
} yandex_session_t;
```

### Image Structure
```c
typedef struct {
    char *url;
    char *title;
    char *source;
    size_t index;
    char *local_path;
    int download_status;
} yandex_image_t;
```

### Video Structure
```c
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
```

## Error Codes

```c
#define YANDEX_SUCCESS                   0
#define YANDEX_ERROR_INVALID_PARAM      -1
#define YANDEX_ERROR_MEMORY             -2
#define YANDEX_ERROR_NETWORK            -3
#define YANDEX_ERROR_DRIVER             -4
#define YANDEX_ERROR_NO_IMAGES          -5
#define YANDEX_ERROR_DOWNLOAD           -6
#define YANDEX_ERROR_NO_VIDEOS          -7
#define YANDEX_ERROR_DRIVER_NOT_FOUND   -8
#define YANDEX_ERROR_DRIVER_START_FAILED -9
#define YANDEX_ERROR_DRIVER_CONNECTION -10
#define YANDEX_ERROR_FIREFOX_NOT_FOUND  -11
#define YANDEX_ERROR_SESSION_TIMEOUT   -12
#define YANDEX_ERROR_HTTP_ERROR         -13
#define YANDEX_ERROR_FILE_WRITE         -14
#define YANDEX_ERROR_STREAM_INIT        -15
#define YANDEX_ERROR_THREAD_CREATE      -16
#define YANDEX_ERROR_THREAD_JOIN        -17
```

## Usage Examples

### Basic Image Download
```c
#include <yandex_parser.h>
#include <stdio.h>

int main() {
    printf("Starting GeckoDriver...\n");
    if (yandex_start_geckodriver() != YANDEX_SUCCESS) {
        fprintf(stderr, "Error: Failed to start GeckoDriver\n");
        return 1;
    }
    
    yandex_session_t* session = yandex_create_session("nature", 3);
    if (!session) {
        fprintf(stderr, "Error: Failed to create session\n");
        yandex_stop_geckodriver();
        return 1;
    }
    
    int result = yandex_search_images(session);
    if (result != YANDEX_SUCCESS) {
        fprintf(stderr, "Error: %s\n", yandex_get_error_string(result));
        yandex_free_session(session);
        yandex_stop_geckodriver();
        return 1;
    }
    
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
```

### Video Download
```c
#include <yandex_parser.h>
#include <stdio.h>

int main() {
    printf("Starting GeckoDriver...\n");
    if (yandex_start_geckodriver() != YANDEX_SUCCESS) {
        fprintf(stderr, "Error: Failed to start GeckoDriver\n");
        return 1;
    }
    
    yandex_session_t* session = yandex_create_session("animals", 2);
    if (!session) {
        fprintf(stderr, "Error: Failed to create session\n");
        yandex_stop_geckodriver();
        return 1;
    }
    
    int result = yandex_search_videos(session);
    if (result != YANDEX_SUCCESS) {
        fprintf(stderr, "Error: %s\n", yandex_get_error_string(result));
        yandex_free_session(session);
        yandex_stop_geckodriver();
        return 1;
    }
    
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
```

## Compilation

### CMake Build System (Recommended)
```bash
# Create build directory
mkdir build
cd build

# Configure and build
cmake ..
make
sudo make install
```

### Manual Compilation
```bash
# Basic compilation
gcc -o my_program my_program.c -lyandex_parser -lcurl -lpthread

# With optimization
gcc -O3 -flto -march=native -o my_program my_program.c -lyandex_parser -lcurl -lpthread
```

## Troubleshooting

- **GeckoDriver not found**: Install geckodriver and ensure it's in PATH
- **Firefox not found**: `sudo apt install firefox-esr`
- **Compilation errors**: Check that all dependencies are installed
- **WebDriver errors**: Ensure GeckoDriver is running on the correct port
- **Memory errors**: Always call `yandex_free_session()` to prevent memory leaks

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- libcurl for HTTP operations
- Firefox ESR for WebDriver automation
- GeckoDriver for WebDriver implementation
- pthread for multi-threading support