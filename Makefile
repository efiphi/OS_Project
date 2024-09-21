CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -g
INCLUDES = -I.
LIBS =

# Source files for Pipeline server
PIPELINE_SOURCES = graph.cpp mst.cpp pipelineServer.cpp
PIPELINE_OBJECTS = $(PIPELINE_SOURCES:.cpp=.o)

# Source files for Leader-Follower server
LEADER_FOLLOWER_SOURCES = graph.cpp mst.cpp leaderFollowerServer.cpp
LEADER_FOLLOWER_OBJECTS = $(LEADER_FOLLOWER_SOURCES:.cpp=.o)

# Executables
PIPELINE_EXEC = pipeline_server
LEADER_FOLLOWER_EXEC = leaderfollower_server

.PHONY: all clean run_pipeline run_leaderfollower valgrind_pipeline valgrind_leaderfollower helgrind_pipeline helgrind_leaderfollower cg_pipeline cg_leaderfollower

# Default build compiles both servers
all: $(PIPELINE_EXEC) $(LEADER_FOLLOWER_EXEC)

# Compile the Pipeline server
$(PIPELINE_EXEC): $(PIPELINE_OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

# Compile the Leader-Follower server
$(LEADER_FOLLOWER_EXEC): $(LEADER_FOLLOWER_OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

# Run the Pipeline server
run_pipeline: $(PIPELINE_EXEC)
	./$(PIPELINE_EXEC)

# Run the Leader-Follower server
run_leaderfollower: $(LEADER_FOLLOWER_EXEC)
	./$(LEADER_FOLLOWER_EXEC)

# Valgrind memcheck for Pipeline server
valgrind_pipeline: $(PIPELINE_EXEC)
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./$(PIPELINE_EXEC)

# Valgrind memcheck for Leader-Follower server
valgrind_leaderfollower: $(LEADER_FOLLOWER_EXEC)
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./$(LEADER_FOLLOWER_EXEC)

# Valgrind helgrind for Pipeline server (detecting race conditions)
helgrind_pipeline: $(PIPELINE_EXEC)
	valgrind --tool=helgrind ./$(PIPELINE_EXEC)

# Valgrind helgrind for Leader-Follower server (detecting race conditions)
helgrind_leaderfollower: $(LEADER_FOLLOWER_EXEC)
	valgrind --tool=helgrind ./$(LEADER_FOLLOWER_EXEC)

# Valgrind cachegrind for Pipeline server (cache performance analysis)
cg_pipeline: $(PIPELINE_EXEC)
	valgrind --tool=cachegrind ./$(PIPELINE_EXEC)

# Valgrind cachegrind for Leader-Follower server (cache performance analysis)
cg_leaderfollower: $(LEADER_FOLLOWER_EXEC)
	valgrind --tool=cachegrind ./$(LEADER_FOLLOWER_EXEC)

clean:
	rm -f $(PIPELINE_OBJECTS) $(LEADER_FOLLOWER_OBJECTS) $(PIPELINE_EXEC) $(LEADER_FOLLOWER_EXEC)
