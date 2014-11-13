OUT_DIR=output/
OBJ_DIR=$(OUT_DIR)obj/
BIN_DIR=$(OUT_DIR)bin/
CC=g++
CFLAGS= -Wall -Werror -std=c++11

ALL_SOURCES=$(wildcard *.cpp)
TARGET_SOURCES=$(wildcard v09_*.cpp)
COMMON_SOURCES=$(filter-out $(TARGET_SOURCES), $(wildcard *.cpp))

ALL_OBJECTS=$(addprefix $(OBJ_DIR), $(ALL_SOURCES:.cpp=.o))
TARGET_OBJECTS=$(addprefix $(OBJ_DIR), $(TARGET_SOURCES:.cpp=.o))
COMMON_OBJECTS=$(addprefix $(OBJ_DIR), $(COMMON_SOURCES:.cpp=.o))
ALL_TARGETS=$(addprefix $(BIN_DIR), $(TARGET_SOURCES:%.cpp=%))

dir_guard=@mkdir -p $(@D)

all: $(ALL_TARGETS) $(ALL_OBJECTS)

$(BIN_DIR)%: $(OBJ_DIR)%.o $(COMMON_OBJECTS)
	$(dir_guard)
	$(CC) $(CFLAGS) $^ -o $@	

$(OBJ_DIR)%.o: %.cpp
	$(dir_guard)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OUT_DIR)
