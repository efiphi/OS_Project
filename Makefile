# Makefile

CXX = g++
CXXFLAGS = -Wall -std=c++17 -g
OBJECTS = graph.o prim_mst_solver.o kruskal_mst_solver.o mst_solver.o main.o server.o task.o responseStage.o threadPool.o

# All Target
all: mst_solver

# Link
mst_solver: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o mst_solver $(OBJECTS)

# Compile
graph.o: graph.cpp graph.hpp
	$(CXX) $(CXXFLAGS) -c graph.cpp -o graph.o

prim_mst_solver.o: prim_mst_solver.cpp prim_mst_solver.hpp
	$(CXX) $(CXXFLAGS) -c prim_mst_solver.cpp -o prim_mst_solver.o

kruskal_mst_solver.o: kruskal_mst_solver.cpp kruskal_mst_solver.hpp
	$(CXX) $(CXXFLAGS) -c kruskal_mst_solver.cpp -o kruskal_mst_solver.o

mst_solver.o: mst_solver.cpp mst_solver.hpp
	$(CXX) $(CXXFLAGS) -c mst_solver.cpp -o mst_solver.o

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

server.o: server.cpp server.hpp
	$(CXX) $(CXXFLAGS) -c server.cpp -o server.o

task.o: task.cpp task.hpp
	$(CXX) $(CXXFLAGS) -c task.cpp -o task.o

responseStage.o: responseStage.cpp responseStage.hpp
	$(CXX) $(CXXFLAGS) -c responseStage.cpp -o responseStage.o

threadPool.o: threadPool.cpp threadPool.hpp
	$(CXX) $(CXXFLAGS) -c threadPool.cpp -o threadPool.o

# Valgrind Targets
memcheck: mst_solver
	valgrind --leak-check=full --track-origins=yes ./mst_solver

helgrind: mst_solver
	valgrind --tool=helgrind ./mst_solver

cachegrind: mst_solver
	valgrind --tool=cachegrind ./mst_solver

# Clean
clean:
	rm -f *.o mst_solver
