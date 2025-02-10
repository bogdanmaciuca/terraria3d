CXX=g++
TARGET=t3d
BUILD=./build
SRC=./src
VENDOR=./vendor
CFLAGS=-std=c++20 -g
LINKER=-lGL -lglfw

all: $(BUILD)/main.o $(BUILD)/glw.o $(BUILD)/game.o $(BUILD)/gameobjects.o $(BUILD)/simdjson.o $(BUILD)/glad.o
	$(CXX) -o $(BUILD)/$(TARGET) \
		$(BUILD)/main.o \
		$(BUILD)/glw.o \
		$(BUILD)/game.o \
		$(BUILD)/gameobjects.o \
		$(BUILD)/simdjson.o \
		$(BUILD)/glad.o \
		$(CFLAGS) $(LINKER)

$(BUILD)/main.o: $(SRC)/main.cpp
	$(CXX) -c -o $(BUILD)/main.o $(SRC)/main.cpp $(CFLAGS)

$(BUILD)/glw.o: $(SRC)/glw.cpp $(SRC)/glw.hpp
	$(CXX) -c -o $(BUILD)/glw.o $(SRC)/glw.cpp $(CFLAGS)

$(BUILD)/game.o: $(SRC)/game.cpp $(SRC)/game.hpp
	$(CXX) -c -o $(BUILD)/game.o $(SRC)/game.cpp $(CFLAGS)

$(BUILD)/gameobjects.o: $(SRC)/gameobjects.cpp $(SRC)/gameobjects.hpp
	$(CXX) -c -o $(BUILD)/gameobjects.o $(SRC)/gameobjects.cpp $(CFLAGS)

$(BUILD)/simdjson.o: $(VENDOR)/simdjson/simdjson.cpp $(VENDOR)/simdjson/simdjson.h
	$(CXX) -c -o $(BUILD)/simdjson.o $(VENDOR)/simdjson/simdjson.cpp $(CFLAGS)

$(BUILD)/glad.o: $(VENDOR)/glad/glad.c $(VENDOR)/glad/glad.h $(VENDOR)/glad/khrplatform.h
	$(CXX) -c -o $(BUILD)/glad.o $(VENDOR)/glad/glad.c $(CFLAGS)

clean:
	rm $(BUILD)/*
