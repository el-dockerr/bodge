# Makefile for Bodge Build System
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2
TARGET = bodge
SRCDIR = src
SOURCES = $(SRCDIR)/core.cpp $(SRCDIR)/main.cpp $(SRCDIR)/ProjectConfig.cpp $(SRCDIR)/StringUtils.cpp $(SRCDIR)/ConfigParser.cpp $(SRCDIR)/BuildSystem.cpp $(SRCDIR)/git.cpp $(SRCDIR)/FileSystemUtils.cpp $(SRCDIR)/Architecture.cpp $(SRCDIR)/FileWatcher.cpp $(SRCDIR)/BuildLogger.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

# Compile source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)

# Install (copy to system path - modify as needed)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# Uninstall
uninstall:
	rm -f /usr/local/bin/$(TARGET)

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

# Help
help:
	@echo "Available targets:"
	@echo "  all      - Build the project (default)"
	@echo "  clean    - Remove build artifacts"
	@echo "  debug    - Build with debug symbols"
	@echo "  install  - Install to system path"
	@echo "  uninstall- Remove from system path"
	@echo "  help     - Show this help message"

.PHONY: all clean install uninstall debug help