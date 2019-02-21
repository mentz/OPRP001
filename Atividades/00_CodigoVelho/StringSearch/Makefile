FILE = src/main.cpp
OUT = stringsearch
FLAGS = -std=c++11
CC = g++

all: compile prep

compile:
	$(CC) $(FILE) -o $(OUT) $(FLAGS)

prep:
	test ! -e big.txt && wget https://norvig.com/big.txt

test:
	bash -c "trap times EXIT; ./stringsearch \"and\" big.txt"

.PHONY: ALL
