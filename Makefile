.PHONY: all
all: format test build

.PHONY: format
format:
	clang-format src/* include/* -i

.PHONY: build
build:
	mkdir -p build
	cd build && \
	cmake .. && \
	make

.PHONY: debug
debug:
	mkdir -p build
	cd build && \
	cmake -DCMAKE_BUILD_TYPE=debug .. && \
	make

.PHONY: clean
clean:
	rm -rf build

.PHONY: mina-ryad-debug
mina-ryad-debug:
	make format && make clean && make build && make debug && gdb build/monitor

.PHONY: mina-ryad-run
mina-ryad-run:
	make format && make clean && make build && make debug && ./build/monitor
