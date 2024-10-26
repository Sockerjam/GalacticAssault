# Compiler
CXX = g++

# Compiler Flags
CXXFLAGS += -Wall -std=c++17 $(shell sdl2-config --cflags) -I/opt/homebrew/include

# Linker Flags
LDFLAGS += $(shell sdl2-config --libs)

 # Source Files
SOURCES = $(shell find src -name '*.cpp')

# Object Files
OBJECTS = $(SOURCES:.cpp=.o)

# Target Executable
TARGET = GalacticAssault

# Default Rule
all: $(TARGET)

# Build Target
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Compile Source Files into Object Files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -v -c $< -o $@

# Clean Build Files
clean:
	rm -f $(OBJECTS) $(TARGET)

# Run the Game
run: $(TARGET)
	./$(TARGET)
