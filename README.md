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
- **File Operations** - Save image URLs to files for later processing

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

// Get a range of images from session
int yandex_get_image_range(yandex_session_t* session, size_t start, size_t end, yandex_image_t** result, size_t* count);

// Free image array memory
void yandex_free_images(yandex_image_t* images, size_t count);
```

#### Image Operations
```c
// Download all images in session
int yandex_download_images(yandex_session_t* session, const char* download_dir);

// Download specific range of images
int yandex_download_image_range(yandex_session_t* session, size_t start, size_t end, const char* download_dir);

// Download single image by URL
int yandex_download_single_image(const char* url, const char* filename);
```

#### File Operations
```c
// Save all image URLs to file
int yandex_save_images(yandex_session_t* session, const char* filename);

// Save range of image URLs to file
int yandex_save_image_range(yandex_session_t* session, size_t start, size_t end, const char* filename);
```

#### Utility Functions
```c
// Get image count
int yandex_get_image_count(yandex_session_t* session);

// Get error description
const char* yandex_get_error_string(int error_code);

// Get download status of specific image
int yandex_get_download_status(yandex_image_t* image);
```

#### GeckoDriver Management
```c
// Start GeckoDriver automatically
int yandex_start_geckodriver(void);

// Stop GeckoDriver
void yandex_stop_geckodriver(void);

// Check if GeckoDriver is running
int yandex_is_geckodriver_running(void);

// Set custom port
int yandex_set_geckodriver_port(int port);

// Get current port
int yandex_get_geckodriver_port(void);
```

### Data Structures

#### Session Structure
```c
typedef struct {
    char* session_id;           // WebDriver session ID
    char* query;                // Search query
    yandex_image_t* images;     // Image array
    size_t image_count;         // Total images found
    int scroll_count;           // Number of scrolls
} yandex_session_t;
```

#### Image Structure
```c
typedef struct {
    char* url;                  // Image URL
    char* title;                // Image title/alt text
    char* source;               // Image source
    size_t index;               // Image index
    char* local_path;           // Local file path after download
    int download_status;        // Download status (1=success, 0=pending, -1=failed)
} yandex_image_t;
```

### Error Codes
```c
#define YANDEX_SUCCESS                   0
#define YANDEX_ERROR_INVALID_PARAM      -1
#define YANDEX_ERROR_MEMORY             -2
#define YANDEX_ERROR_NETWORK            -3
#define YANDEX_ERROR_DRIVER             -4
#define YANDEX_ERROR_NO_IMAGES          -5
#define YANDEX_ERROR_DOWNLOAD           -6
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

### Advanced Usage with Automatic GeckoDriver Management
```c
#include <yandex_parser.h>
#include <stdio.h>

int main() {
    // Set custom GeckoDriver port
    yandex_set_geckodriver_port(4445);
    
    // Start GeckoDriver automatically
    int driver_result = yandex_start_geckodriver();
    if (driver_result != YANDEX_SUCCESS) {
        printf("Error: Failed to start GeckoDriver: %s\n", yandex_get_error_string(driver_result));
        return 1;
    }
    
    printf("GeckoDriver started successfully on port %d\n", yandex_get_geckodriver_port());
    
    // Create session with multiple scrolls
    yandex_session_t* session = yandex_create_session("nature landscapes", 5);
    
    // Search and download
    yandex_search_images(session);
    yandex_download_images(session, "landscapes");
    
    yandex_free_session(session);
    
    // Stop GeckoDriver when done
    yandex_stop_geckodriver();
    return 0;
}
```

### Working with Image Ranges
```c
#include <yandex_parser.h>
#include <stdio.h>

int main() {
    yandex_session_t* session = yandex_create_session("dogs", 3);
    yandex_search_images(session);
    
    // Get a range of images
    yandex_image_t* range_images;
    size_t range_count;
    int result = yandex_get_image_range(session, 0, 10, &range_images, &range_count);
    
    if (result == YANDEX_SUCCESS) {
        printf("Got %zu images in range\n", range_count);
        
        // Process the range
        for (size_t i = 0; i < range_count; i++) {
            printf("Image %zu: %s\n", i, range_images[i].url);
        }
        
        // Free the range
        yandex_free_images(range_images, range_count);
    }
    
    yandex_free_session(session);
    return 0;
}
```

### Saving URLs to File
```c
#include <yandex_parser.h>
#include <stdio.h>

int main() {
    yandex_session_t* session = yandex_create_session("sunset", 2);
    yandex_search_images(session);
    
    // Save all URLs to file
    yandex_save_images(session, "sunset_urls.txt");
    
    // Save range of URLs to file
    yandex_save_image_range(session, 0, 20, "sunset_first_20.txt");
    
    yandex_free_session(session);
    return 0;
}
```

## Quick Start

```c
#include <yandex_parser.h>
#include <stdio.h>

int main() {
    // Start GeckoDriver automatically
    yandex_start_geckodriver();
    
    yandex_session_t* session = yandex_create_session("cats", 2);
    yandex_search_images(session);
    yandex_download_image_range(session, 0, 3, "cats");
    yandex_free_session(session);
    
    // Clean up
    yandex_stop_geckodriver();
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

## Function Reference

### Session Management Functions

| Function | Parameters | Returns | Description |
|----------|------------|---------|-------------|
| `yandex_create_session` | `const char* query, int scroll_count` | `yandex_session_t*` | Create new search session |
| `yandex_free_session` | `yandex_session_t* session` | `void` | Free session memory |
| `yandex_search_images` | `yandex_session_t* session` | `int` | Search for images using WebDriver |
| `yandex_get_image_range` | `yandex_session_t* session, size_t start, size_t end, yandex_image_t** result, size_t* count` | `int` | Get range of images from session |
| `yandex_free_images` | `yandex_image_t* images, size_t count` | `void` | Free image array memory |

### Download Functions

| Function | Parameters | Returns | Description |
|----------|------------|---------|-------------|
| `yandex_download_images` | `yandex_session_t* session, const char* download_dir` | `int` | Download all images in session |
| `yandex_download_image_range` | `yandex_session_t* session, size_t start, size_t end, const char* download_dir` | `int` | Download specific range of images |
| `yandex_download_single_image` | `const char* url, const char* filename` | `int` | Download single image by URL |

### File Operations

| Function | Parameters | Returns | Description |
|----------|------------|---------|-------------|
| `yandex_save_images` | `yandex_session_t* session, const char* filename` | `int` | Save all image URLs to file |
| `yandex_save_image_range` | `yandex_session_t* session, size_t start, size_t end, const char* filename` | `int` | Save range of image URLs to file |

### Utility Functions

| Function | Parameters | Returns | Description |
|----------|------------|---------|-------------|
| `yandex_get_image_count` | `yandex_session_t* session` | `int` | Get total number of images found |
| `yandex_get_error_string` | `int error_code` | `const char*` | Get human-readable error description |
| `yandex_get_download_status` | `yandex_image_t* image` | `int` | Get download status of specific image |

### GeckoDriver Management

| Function | Parameters | Returns | Description |
|----------|------------|---------|-------------|
| `yandex_start_geckodriver` | `void` | `int` | Start GeckoDriver automatically |
| `yandex_stop_geckodriver` | `void` | `void` | Stop GeckoDriver processes |
| `yandex_is_geckodriver_running` | `void` | `int` | Check if GeckoDriver is running |
| `yandex_set_geckodriver_port` | `int port` | `int` | Set GeckoDriver port |
| `yandex_get_geckodriver_port` | `void` | `int` | Get current GeckoDriver port |

## Troubleshooting

- **GeckoDriver not found**: Install from [GitHub releases](https://github.com/mozilla/geckodriver/releases)
- **libcurl not found**: `sudo apt install libcurl4-openssl-dev`
- **Firefox not found**: `sudo apt install firefox-esr`
- **Compilation errors**: Check that all dependencies are installed
- **WebDriver errors**: Ensure GeckoDriver is running on the correct port
- **Memory errors**: Always call `yandex_free_session()` and `yandex_free_images()` to prevent memory leaks

## Performance

- **Search Speed**: ~2-3 seconds per query
- **Download Speed**: ~100-500 KB/s per image
- **Memory Usage**: ~1-2 MB base + ~50KB per image
- **Concurrent Downloads**: Optimized for I/O operations
- **Memory Management**: Dynamic allocation with proper cleanup functions

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- **libcurl** - HTTP client library
- **GeckoDriver** - WebDriver implementation for Firefox
- **Firefox ESR** - Headless browser engine
- **Yandex Images** - Image search service
