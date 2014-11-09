CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=$(wildcard src/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=labDaemon

all: $(SOURCES) $(EXECUTABLE)

onlybin: all
	rm -rf $(OBJECTS)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJECTS)
	rm $(EXECUTABLE)