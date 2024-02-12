CC = g++
CFLAGS = -Wall -Wextra -Werror -Wuninitialized
FILES = *.h *.cc
TARGET = postgresql_tcp_proxy_server_logger

all: build

build: clean
	mkdir -p build
	cd build && \
	$(CC) $(CFLAGS) ../*.cc -o ../$(TARGET) && \
	cd ../
	rm -rf build

clean:
	rm -f $(TARGET)

googlestyle:
	clang-format -style=Google -i $(FILES)

cppcheck:
	cppcheck --enable=all --suppress=missingIncludeSystem --language=c++ $(FILES)
