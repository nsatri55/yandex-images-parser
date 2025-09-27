cmake_minimum_required(VERSION 3.10)
project(yandex_parser_examples LANGUAGES C)

find_library(YANDEX_PARSER_LIBRARY yandex_parser HINTS ${CMAKE_INSTALL_PREFIX}/lib ${CMAKE_BINARY_DIR}/../)
find_path(YANDEX_PARSER_INCLUDE_DIR yandex_parser.h HINTS ${CMAKE_INSTALL_PREFIX}/include ${CMAKE_BINARY_DIR}/../include)

if(NOT YANDEX_PARSER_LIBRARY OR NOT YANDEX_PARSER_INCLUDE_DIR)
    message(FATAL_ERROR "yandex_parser library or headers not found. Please build and install the main library first.")
endif()

include_directories(${YANDEX_PARSER_INCLUDE_DIR})

find_library(CURL_LIBRARY curl)
if(NOT CURL_LIBRARY)
    message(FATAL_ERROR "libcurl not found. Please install libcurl4-openssl-dev")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -std=c11 -Os -flto")

add_executable(image_example image_example.c)
target_link_libraries(image_example yandex_parser ${CURL_LIBRARY} pthread)

add_executable(video_example video_example.c)
target_link_libraries(video_example yandex_parser ${CURL_LIBRARY} pthread)

set_target_properties(image_example video_example PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
)

install(TARGETS image_example video_example DESTINATION bin)