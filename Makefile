# Makefile

CXX = g++
COVFLAGS = --coverage # gcov -b -c *.cpp
CXXFLAGS = -Wall -std=c++17 -g
OBJECTS = graph.o prim_mst_solver.o kruskal_mst_solver.o mst_solver.o main.o server.o task.o responseStage.o threadPool.o ActiveObject.o
# Source files
SRCS = $(wildcard *.cpp)
LEADEROBJ = leaderFollowerServer.o graph.o prim_mst_solver.o kruskal_mst_solver.o mst_solver.o task.o responseStage.o ActiveObject.o

# All Target
all: mst_solver leaderFollower

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

ActiveObject.o: ActiveObject.cpp ActiveObject.hpp
	$(CXX) $(CXXFLAGS) -c ActiveObject.cpp -o ActiveObject.o

leaderFollowerServer.o: leaderFollowerServer.cpp leaderFollowerServer.hpp
	$(CXX) $(CXXFLAGS) -c leaderFollowerServer.cpp -o leaderFollowerServer.o

leaderFollower: $(LEADEROBJ)
	$(CXX) $(CXXFLAGS) -o leaderFollower $(LEADEROBJ)

# Generate code coverage report
coverageLF: leaderFollower
	./leaderFollower -v 6 -e 10
	@for file in $(SRCS); do \
		gcov -o build $$file; \
	done
	mkdir -p coverage
	mv *.gcov coverage/

memcheckLF: leaderFollower
	valgrind --leak-check=full --track-origins=yes ./leaderFollower

helgrindLF: leaderFollower
	valgrind --tool=helgrind ./leaderFollower

cachegrindLF: leaderFollower
	valgrind --tool=cachegrind ./leaderFollower


# Valgrind Targets
memcheck: mst_solver 
	valgrind --leak-check=full --track-origins=yes ./mst_solver 

helgrind: mst_solver
	valgrind --tool=helgrind ./mst_solver 

cachegrind: mst_solver 
	valgrind --tool=cachegrind ./mst_solver 

# Clean
clean:
	rm -f *.o *.gcov *.gcda *.gcno mst_solver leaderFollower
