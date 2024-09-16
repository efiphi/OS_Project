# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++17

# Target executable
TARGET = mst_solver

# Source files
SOURCES = graph.cpp \
          prim_mst_solver.cpp \
          kruskal_mst_solver.cpp \
          mst_solver.cpp \
          main.cpp \
          server.cpp \
          task.cpp \
          graphUpdateStage.cpp \
          mstComputationStage.cpp \
          responseStage.cpp \
          threadPool.cpp \

# Object files (derived from source files)
OBJECTS = $(SOURCES:.cpp=.o)

# Default rule
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

# Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up the build (removes object files and the executable)
clean:
	rm -f $(OBJECTS) $(TARGET)

# Run the project
run: $(TARGET)
	./$(TARGET)
