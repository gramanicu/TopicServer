# Copyright 2020 Grama Nicolae

.PHONY: gitignore clean memory beauty run
.SILENT: beauty clean memory gitignore

# Compilation variables
CC = g++
CFLAGS = -lstdc++fs -Wno-unused-parameter -Wall -Wextra -pedantic -g -O3 -std=c++17
EXE = TopicServer
SRC = $(wildcard src/*.cpp)
TST = $(wildcard test/*.cpp)
OBJ = $(SRC:.cpp=.o)
TOBJ = $(TST:.cpp=.o)
INCLUDE = src

# Compiles the program
build: $(OBJ)
	$(info Compiling code...)
	@$(CC) -I$(INCLUDE) -o $(EXE) $^ $(CFLAGS) ||:
	$(info Compilation successfull)
	-@rm -f *.o ||:
	@$(MAKE) -s gitignore ||:

%.o: %.cpp
	$(CC) -I$(INCLUDE) -o $@ -c $< $(CFLAGS) 

# Executes the binary
run: clean build
	./$(EXE)

# Test the project
test: $(TOBJ)
	@$(CC) -I$(INCLUDE) -o ./test/$(EXE) $^ $(CFLAGS) ||:
	-@rm -f $(TOBJ) ||:
	./test/$(EXE)

# Deletes the binary and object files
clean:
	rm -f $(EXE) $(OBJ) TopicServer.zip
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
	@echo "$(EXE)" > .gitignore ||:
	@echo "src/*.o" >> .gitignore ||:
	@echo ".vscode*" >> .gitignore ||:	
	echo "Updated .gitignore"
	
# Creates an archive of the project
pack: clean
	zip -FSr TopicServer.zip *

