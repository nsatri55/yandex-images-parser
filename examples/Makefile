CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Os -flto -ffunction-sections -fdata-sections -I../include -D_GNU_SOURCE
LDFLAGS = -Wl,--gc-sections -Wl,--strip-all
LIBS = -L.. -lyandex_parser -lcurl

pet_downloader: pet_downloader.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o pet_downloader pet_downloader.c $(LIBS)

pet_downloader_single: pet_downloader_single.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o pet_downloader_single pet_downloader_single.c $(LIBS)

clean:
	rm -f pet_downloader pet_downloader_single *.txt *.jpg
	rm -rf cats/ dogs/ single_cat/ single_dog/

.PHONY: clean
