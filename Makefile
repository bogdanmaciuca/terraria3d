CXX=g++
TARGET=t3d
BUILD=./build
SRC=./src
VENDOR=./vendor
CFLAGS=-std=c++20
LINKER=

all: $(BUILD)/main.o $(BUILD)/game.o $(BUILD)/gameobjects.o $(BUILD)/simdjson.o
	$(CXX) -o $(BUILD)/$(TARGET) \
		$(BUILD)/main.o \
		$(BUILD)/game.o \
		$(BUILD)/gameobjects.o \
		$(BUILD)/simdjson.o \
		$(CFLAGS) $(LINKER)

$(BUILD)/main.o: $(SRC)/main.cpp
	$(CXX) -c -o $(BUILD)/main.o $(SRC)/main.cpp $(CFLAGS)

$(BUILD)/game.o: $(SRC)/game.cpp $(SRC)/game.hpp
	$(CXX) -c -o $(BUILD)/game.o $(SRC)/game.cpp $(CFLAGS)

$(BUILD)/gameobjects.o: $(SRC)/gameobjects.cpp $(SRC)/gameobjects.hpp
	$(CXX) -c -o $(BUILD)/gameobjects.o $(SRC)/gameobjects.cpp $(CFLAGS)

$(BUILD)/simdjson.o: $(VENDOR)/simdjson.cpp $(VENDOR)/simdjson.h
	$(CXX) -c -o $(BUILD)/simdjson.o $(VENDOR)/simdjson.cpp $(CFLAGS)

