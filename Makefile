CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Os -flto -ffunction-sections -fdata-sections -I./include -D_GNU_SOURCE
LDFLAGS = -Wl,--gc-sections -Wl,--strip-all
LIBS = -lcurl
LIBRARY = libyandex_parser.a

CORE_SOURCES = src/core.c src/files.c src/download.c src/driver.c src/utils.c
OBJECTS = $(CORE_SOURCES:.c=.o)

all: $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	ar rcs $@ $^

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(LIBRARY) *.txt

install: $(LIBRARY)
	@echo "Installing library..."
	@mkdir -p /usr/local/lib
	@mkdir -p /usr/local/include
	@cp $(LIBRARY) /usr/local/lib/
	@cp include/yandex_parser.h /usr/local/include/
	@echo "Library installed successfully"

uninstall:
	@echo "Uninstalling library..."
	@rm -f /usr/local/lib/$(LIBRARY)
	@rm -f /usr/local/include/yandex_parser.h
	@echo "Library uninstalled successfully"

.PHONY: all clean install uninstall
