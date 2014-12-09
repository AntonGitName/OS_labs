OUT_DIR=output/
SRC_DIR=src/
OBJ_DIR=$(OUT_DIR)obj/
BIN_DIR=$(OUT_DIR)bin/
CXX=g++
CFLAGS= -Wall -Werror -pthread -std=c++11 -Qunused-arguments

SOURCES=$(wildcard $(SRC_DIR)*.cpp)
OBJECTS=$(addprefix $(OBJ_DIR), $(notdir $(SOURCES:.cpp=.o)))
TARGET=$(addprefix $(BIN_DIR), lab4)

dir_guard=@mkdir -p $(@D)

all: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(dir_guard)
	$(CXX) $(CFLAGS) $^ -o $@	

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	$(dir_guard)
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OUT_DIR)
