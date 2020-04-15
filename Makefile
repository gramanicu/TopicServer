# Copyright 2020 Grama Nicolae

.PHONY: gitignore clean memory beauty run
.SILENT: beauty clean memory gitignore

# Compilation variables
CC = g++
CFLAGS = -lstdc++fs -Wno-unused-parameter -Wall -Wextra -pedantic -g -O3 -std=c++17
INCLUDE = src

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)

TEXE = ./test/tester
TST = $(wildcard test/*.cpp)
TOBJ = $(TST:.cpp=.o)

IP = 127.0.0.1
PORT = 8080
USERNAME = Rockyn

# Compiles the programs
build: $(OBJ)
	$(CC) -I$(INCLUDE) -o server ./src/Server.o $(CFLAGS) 
	$(CC) -I$(INCLUDE) -o subscriber ./src/Subscriber.o $(CFLAGS)
	-@rm -f $(OBJ)

# Runs the server
run_server: clean build
	./server $(PORT)
	-@rm -f server subscriber

# Runs the server
run_subscriber: clean build
	./subscriber $(USERNAME) $(IP) $(PORT)
	-@rm -f server subscriber

# Test the project
test: $(TOBJ)
	@$(CC) -I$(INCLUDE) -o $(TEXE) $^ $(CFLAGS) ||:
	-@rm -f $(TOBJ) ||:
	$(TEXE) ||:
	-@rm -f $(TEXE) ||:

%.o: %.cpp
	$(CC) -I$(INCLUDE) -o $@ -c $< $(CFLAGS) 

# Deletes the binary and object files
clean:
	rm -f server subscriber $(OBJ) TopicServer.zip
	echo "Deleted the binary and object files"

# Automatic coding style, in my personal style
beauty:
	clang-format -i -style=file */*.cpp
	clang-format -i -style=file */*.hpp

# Checks the memory for leaks
MFLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes
memory:clean build
	valgrind $(MFLAGS) ./$(EXE)

# Adds and updates gitignore rules
gitignore:
	@echo "test/tester" > .gitignore ||:
	@echo "src/*.o" >> .gitignore ||:
	@echo ".vscode*" >> .gitignore ||:	
	echo "Updated .gitignore"
	
# Creates an archive of the project
pack: clean
	zip -FSr TopicServer.zip *

udp_client:
	python3 ./checker/udp_client.py --source-port 1234 --input_file ./checker/sample_payloads.json --delay 10 --mode all_once 127.0.0.1 8080

