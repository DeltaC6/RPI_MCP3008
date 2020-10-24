TARGET = main

SRC = src
INC = inc
LIB = lib
BIN = bin

SOURCE = $(wildcard $(SRC)/*.cpp)
OBJECT = $(patsubst %,$(BIN)/%, $(notdir $(SOURCE:.cpp=.o)))

CC = g++
CFLAGS = -Wall -std=c++14 -I$(INC) -lwiringPi -L$(LIB)

$(BIN)/$(TARGET) : $(OBJECT)
	@echo "Linking..."
	$(CC) $(CFLAGS) -o $@ $^
	@echo "Finished..."

$(BIN)/%.o : $(SRC)/%.cpp
	@echo "Compiling..."
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY : help run

run: $(BIN)/$(TARGET)
	@echo "Running..."
	$(BIN)/$(TARGET)

clean:
	@echo "Cleaning..."
	rm -f $(OBJECT) $(BIN)/$(TARGET)

help:
	@ehco "Help..."
	@echo "src: ${SOURCE}"
	@echo "obj: $(OBJECT)"
