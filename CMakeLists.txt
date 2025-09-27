cmake_minimum_required(VERSION 3.10)
project(yandex_parser VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O3 -flto -ffunction-sections -fdata-sections -D_GNU_SOURCE -march=native -mtune=native -funroll-loops -finline-functions -fomit-frame-pointer")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNDEBUG")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections -Wl,--strip-all")

find_library(CURL_LIBRARY curl)
if(NOT CURL_LIBRARY)
    message(FATAL_ERROR "libcurl not found. Please install libcurl4-openssl-dev")
endif()

include_directories(include)

set(CORE_SOURCES
    src/core.c
    src/files.c
    src/download.c
    src/driver.c
    src/utils.c
    src/multithread.c
)

add_library(yandex_parser STATIC ${CORE_SOURCES})

target_link_libraries(yandex_parser ${CURL_LIBRARY} pthread)

set_target_properties(yandex_parser PROPERTIES
    OUTPUT_NAME yandex_parser
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

install(TARGETS yandex_parser
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES include/yandex_parser.h
    DESTINATION include
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/yandex_parser.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/yandex_parser.pc"
    @ONLY
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/yandex_parser.pc"
    DESTINATION lib/pkgconfig
)

add_subdirectory(examples)
