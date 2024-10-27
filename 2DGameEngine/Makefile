# Compiler
CXX = g++

# Compiler Flags
CXXFLAGS += -Wall -std=c++17 $(shell sdl2-config --cflags) -I/opt/homebrew/include

# Linker Flags
LDFLAGS += $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf -lSDL2_mixer 

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
	$(CXX) $(CXXFLAGS) $(OBJECTS) -v -o $(TARGET) $(LDFLAGS)

# Compile Source Files into Object Files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build Object Files to Check for Compiler Errors
build: $(OBJECTS)
	@echo "All source files have been compiled into object files."

# Clean Build Files
clean:
	rm -f $(OBJECTS) $(TARGET)

# Run the Game
run: $(TARGET)
	./$(TARGET)
