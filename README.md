# Yandex Images Parser Library

A high-performance C library for parsing and downloading images from Yandex Images search engine using WebDriver automation.

## Features

- **Smart Image Parsing** - Extract image URLs from Yandex Images search results
- **Batch Download** - Download multiple images with optimized performance
- **WebDriver Integration** - Seamless GeckoDriver/Firefox automation
- **Range Selection** - Download specific image ranges
- **Optimized Performance** - Assembly-optimized memory operations
- **Error Handling** - Comprehensive error codes and status reporting
- **Configurable** - Customizable GeckoDriver ports and settings

## Prerequisites

- **libcurl4-openssl-dev** - HTTP client library
- **firefox-esr** - Firefox browser for headless operation  
- **geckodriver** - WebDriver implementation for Firefox

## Building

```bash
# Build the library
make

# Install globally (requires sudo)
sudo make install

# Or install locally
make install-user
```

## API Reference

### Core Functions

#### Session Management
```c
// Create a new search session
yandex_session_t* yandex_create_session(const char* query, int scroll_count);

// Free session memory
void yandex_free_session(yandex_session_t* session);

// Search for images
int yandex_search_images(yandex_session_t* session);
```

#### Image Operations
```c
// Download all images in session
int yandex_download_images(yandex_session_t* session, const char* folder);

// Download specific range of images
int yandex_download_image_range(yandex_session_t* session, int start, int count, const char* folder);

// Download single image
int yandex_download_single_image(yandex_session_t* session, int index, const char* folder);
```

#### Utility Functions
```c
// Get image count
int yandex_get_image_count(yandex_session_t* session);

// Get error description
const char* yandex_get_error_string(int error_code);

// Get download status
int yandex_get_download_status(yandex_session_t* session, int index);
```

#### GeckoDriver Management
```c
// Start GeckoDriver
int yandex_start_geckodriver(void);

// Stop GeckoDriver
int yandex_stop_geckodriver(void);

// Check if GeckoDriver is running
int yandex_is_geckodriver_running(void);

// Set custom port
void yandex_set_geckodriver_port(int port);

// Get current port
int yandex_get_geckodriver_port(void);
```

### Data Structures

#### Session Structure
```c
typedef struct {
    char query[256];           // Search query
    int scroll_count;          // Number of scrolls
    yandex_image_t* images;    // Image array
    int image_count;           // Total images found
    int session_id;            // WebDriver session ID
} yandex_session_t;
```

#### Image Structure
```c
typedef struct {
    char url[512];             // Image URL
    char title[256];            // Image title
    char local_path[512];       // Local file path
    int download_status;        // Download status
} yandex_image_t;
```

### Error Codes
```c
#define YANDEX_SUCCESS 0
#define YANDEX_ERROR_INVALID_PARAM -1
#define YANDEX_ERROR_MEMORY -2
#define YANDEX_ERROR_NETWORK -3
#define YANDEX_ERROR_DRIVER -4
#define YANDEX_ERROR_NO_IMAGES -5
#define YANDEX_ERROR_DOWNLOAD -6
```

## Usage Examples

### Basic Image Search and Download
```c
#include <yandex_parser.h>
#include <stdio.h>

int main() {
    // Create session
    yandex_session_t* session = yandex_create_session("cats", 2);
    if (!session) {
        printf("Error: Failed to create session\n");
        return 1;
    }
    
    // Search for images
    int result = yandex_search_images(session);
    if (result != YANDEX_SUCCESS) {
        printf("Error: %s\n", yandex_get_error_string(result));
        yandex_free_session(session);
        return 1;
    }
    
    printf("Found %d images\n", yandex_get_image_count(session));
    
    // Download first 5 images
    yandex_download_image_range(session, 0, 5, "cats");
    
    // Cleanup
    yandex_free_session(session);
    return 0;
}
```

### Advanced Usage with Custom Port
```c
#include <yandex_parser.h>
#include <stdio.h>

int main() {
    // Set custom GeckoDriver port
    yandex_set_geckodriver_port(4445);
    
    // Check if GeckoDriver is running
    if (!yandex_is_geckodriver_running()) {
        printf("Starting GeckoDriver...\n");
        yandex_start_geckodriver();
    }
    
    // Create session with multiple scrolls
    yandex_session_t* session = yandex_create_session("nature landscapes", 5);
    
    // Search and download
    yandex_search_images(session);
    yandex_download_images(session, "landscapes");
    
    yandex_free_session(session);
    return 0;
}
```

## Quick Start

```c
#include <yandex_parser.h>
#include <stdio.h>

int main() {
    // Start GeckoDriver: geckodriver --port=4445 &
    
    yandex_session_t* session = yandex_create_session("cats", 2);
    yandex_search_images(session);
    yandex_download_image_range(session, 0, 3, "cats");
    yandex_free_session(session);
    
    return 0;
}
```

Compile and run:
```bash
gcc -o demo demo.c -lyandex_parser -lcurl
./demo
```

## Compilation

### Basic Compilation
```bash
gcc -o program program.c -lyandex_parser -lcurl
```

### With Optimization
```bash
gcc -Wall -Wextra -std=c11 -Os -flto -o program program.c -lyandex_parser -lcurl
```

### Makefile Example
```makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Os -flto
LIBS = -lyandex_parser -lcurl

program: program.c
	$(CC) $(CFLAGS) -o program program.c $(LIBS)

clean:
	rm -f program *.jpg sunset/
```

## Troubleshooting

- **GeckoDriver not found**: Install from [GitHub releases](https://github.com/mozilla/geckodriver/releases)
- **libcurl not found**: `sudo apt install libcurl4-openssl-dev`
- **Firefox not found**: `sudo apt install firefox-esr`
- **Compilation errors**: Check that all dependencies are installed

## Performance

- **Search Speed**: ~2-3 seconds per query
- **Download Speed**: ~100-500 KB/s per image
- **Memory Usage**: ~1-2 MB base + ~50KB per image
- **Concurrent Downloads**: Optimized for I/O operations

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- **libcurl** - HTTP client library
- **GeckoDriver** - WebDriver implementation for Firefox
- **Firefox ESR** - Headless browser engine
- **Yandex Images** - Image search service
