# Copyright 2020 Grama Nicolae

.PHONY: gitignore clean memory beauty run
.SILENT: beauty clean memory gitignore

# Compilation variables
CC = g++
CFLAGS = -lstdc++fs -Wno-unknown-pragmas -Wno-unused-parameter -Wall -Wextra -pedantic -g -O3 -std=c++17
INCLUDE = src

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)

TEXE = ./test/tester
TST = $(wildcard test/*.cpp)
TOBJ = $(TST:.cpp=.o)

IP = 127.0.0.1
PORT = 8081
USERNAME = Rockyn

# Compiles the programs
build: $(OBJ)
	@echo "Compiling code..."
	@$(CC) -I$(INCLUDE) -o server ./src/Server.o $(CFLAGS) 
	@$(CC) -I$(INCLUDE) -o subscriber ./src/Subscriber.o $(CFLAGS)
	-@rm -f $(OBJ)

# Compiles the server program
build_server: ./src/Server.o
	@echo "Compiling code..."
	@$(CC) -I$(INCLUDE) -o server ./src/Server.o $(CFLAGS) 
	-@rm -f $(OBJ)

# Compiles the client program
build_subscriber: ./src/Subscriber.o
	@echo "Compiling code..."
	@$(CC) -I$(INCLUDE) -o subscriber ./src/Subscriber.o $(CFLAGS)
	-@rm -f $(OBJ)

# Runs the server
run_server: clean build_server
	@echo "Started server"
	@./server $(PORT)
	-@rm -f server subscriber

# Runs the server
run_subscriber: clean build_subscriber
	@echo "Started subscriber"
	@./subscriber $(USERNAME) $(IP) $(PORT)
	-@rm -f server subscriber

# Test the project
test: $(TOBJ)
	@echo "Compiling code..."
	@$(CC) -I$(INCLUDE) -o $(TEXE) $^ $(CFLAGS) ||:
	-@rm -f $(TOBJ) ||:
	@$(TEXE) ||:
	-@rm -f $(TEXE) ||:

%.o: %.cpp
	@$(CC) -I$(INCLUDE) -o $@ -c $< $(CFLAGS) 

# Deletes the binary and object files
clean:
	rm -f server subscriber $(OBJ) TopicServer.zip
	rm -rfd  data/*
	echo "Deleted the binary and object files"

# Automatic coding style, in my personal style
beauty:
	clang-format -i -style=file */*.cpp
	clang-format -i -style=file */*.hpp

# Checks the memory for leaks
MFLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes
memory:clean build
	valgrind $(MFLAGS) ./server $(PORT)

# Adds and updates gitignore rules
gitignore:
	@echo "test/tester" > .gitignore ||:
	@echo "src/*.o" >> .gitignore ||:
	@echo ".vscode*" >> .gitignore ||:
	@echo "server" >> .gitignore ||:
	@echo "subscriber" >> .gitignore ||:	
	@echo "/data" >> .gitignore ||:	
	echo "Updated .gitignore"
	
# Creates an archive of the project
pack: clean
	zip -FSr TopicServer.zip *

# Starts an udp client that will send data on different topics
udp_client:	
	python3 ./checker/udp_client.py --source-port 1234 --input_file ./checker/sample_payloads.json --delay 10 --mode all_once $(IP) $(PORT)

# Git repository statistics (line count)
statistics:
	@git ls-files -x src/* test/* | xargs wc -l | sort -rn

