CC = gcc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Iinclude
COMMON = src/algorithms.c src/scenarios.c src/report.c tests/tests.c
all: ecommerce
ecommerce: src/main.c $(COMMON) include/optimizer.h
	$(CC) $(CFLAGS) src/main.c $(COMMON) -o ecommerce
test: ecommerce
	./ecommerce --test
benchmark: ecommerce
	./ecommerce --benchmark
clean:
	rm -f ecommerce ecommerce.exe
.PHONY: all test benchmark clean
