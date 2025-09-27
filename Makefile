.PHONY: all clean install uninstall help

all:
	@echo "Use CMake: mkdir build && cd build && cmake .. && sudo make && sudo make install"

clean:
	@echo "rm -rf build/"

install:
	@echo "cd build && sudo make install"

uninstall:
	@echo "cd build && sudo make uninstall"

help:
	@echo "Build: mkdir build && cd build && cmake .. && sudo make && sudo make install"
	@echo "Clean: rm -rf build/"
    @echo "Examples: cd build/examples && ./image_example && ./video_example"