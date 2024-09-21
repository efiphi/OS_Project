#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include "graph.hpp"
#include "mst.hpp"

#define PORT 8097          // The port number the server will listen on
#define THREAD_POOL_SIZE 5 // Number of worker threads in the thread pool

/**
 * Struct: Task
 * Represents a task to be processed by a worker thread. Each task consists of
 * a client socket (clientSocket) and a pointer to the graph (graphPtr) the client will interact with.
 */
struct Task
{
    int clientSocket; // The socket descriptor for client connection
    Graph *graphPtr;  // Pointer to the graph the client will modify
};

/**
 * Class: LeaderFollowerThreadPool
 * Manages a pool of worker threads. Each worker thread processes client requests.
 * The leader thread accepts new client connections and distributes them to worker threads.
 */
class LeaderFollowerThreadPool
{
private:
    std::vector<std::thread> workers; // Vector of worker threads
    std::queue<Task> tasks;           // Queue to hold incoming client tasks
    std::mutex queueMutex;            // Mutex to synchronize access to the task queue
    std::condition_variable cv;       // Condition variable to notify worker threads
    bool stopFlag;                    // Flag to indicate when to stop the thread pool

    /**
     * Function: processClient
     * Handles client requests. It sends the menu to the client, reads their input,
     * and performs the requested action (e.g., add an edge, compute MST).
     *
     * @param clientSocket The socket through which the client is connected.
     * @param graphPtr Pointer to the graph that the client will work with.
     */
    void processClient(int clientSocket, Graph *graphPtr)
    {
        char buffer[1024] = {0}; // Buffer for reading client input

        // Menu to display to the client
        std::string menu =
            "Menu:\n"
            "1. Create a new graph (provide adjacency matrix)\n"
            "2. Add an edge (provide: from, to, weight)\n"
            "3. Remove an edge (provide: from, to)\n"
            "4. Get total weight of MST\n"
            "5. Get longest path in MST (provide: start, end)\n"
            "6. Get shortest path in MST (provide: start, end)\n"
            "7. Get average distance between two vertices in MST\n"
            "8. Print MST\n"
            "9. Exit\n";

        while (true)
        {
            // Send the menu to the client
            send(clientSocket, menu.c_str(), menu.size(), 0);

            // Read client input
            ssize_t bytes_read = read(clientSocket, buffer, 1024);
            if (bytes_read <= 0)
            {
                std::cerr << "Client disconnected or error\n";
                close(clientSocket); // Close client socket upon disconnection or error
                return;
            }

            std::istringstream iss(buffer); // Parse the client's input
            int choice;
            iss >> choice;

            // If the input is invalid, notify the client
            if (iss.fail())
            {
                std::string errorMsg = "Invalid choice. Please try again.\n";
                send(clientSocket, errorMsg.c_str(), errorMsg.size(), 0);
                continue;
            }

            // Handle different menu choices
            switch (choice)
            {
            case 1:
            { // Create a new graph
                std::string prompt = "Enter the number of vertices: ";
                send(clientSocket, prompt.c_str(), prompt.size(), 0);

                char matrixBuffer[1024] = {0};
                read(clientSocket, matrixBuffer, 1024); // Read number of vertices from client
                int numVertices = std::stoi(matrixBuffer);

                // Create an empty adjacency matrix
                std::vector<std::vector<int>> adjMat(numVertices, std::vector<int>(numVertices, 0));

                for (int i = 0; i < numVertices; ++i)
                {
                    std::string rowRequest = "Enter row " + std::to_string(i + 1) + " of the adjacency matrix: ";
                    send(clientSocket, rowRequest.c_str(), rowRequest.size(), 0);

                    char rowBuffer[1024] = {0};
                    read(clientSocket, rowBuffer, 1024);

                    std::istringstream rowStream(rowBuffer);
                    for (int j = 0; j < numVertices; ++j)
                    {
                        rowStream >> adjMat[i][j]; // Fill adjacency matrix from client's input
                    }
                }

                *graphPtr = Graph(adjMat); // Assign the new graph to graphPtr
                std::string graphCreatedResponse = "New graph created!\n";
                send(clientSocket, graphCreatedResponse.c_str(), graphCreatedResponse.size(), 0);
                break;
            }
            case 2:
            { // Add an edge to the graph
                std::string prompt = "Provide the edge to add (from, to, weight): ";
                send(clientSocket, prompt.c_str(), prompt.size(), 0);

                char edgeBuffer[1024] = {0};
                read(clientSocket, edgeBuffer, 1024); // Read the edge details (from, to, weight)

                std::istringstream edgeStream(edgeBuffer);
                int from, to, weight;
                edgeStream >> from >> to >> weight;

                graphPtr->addEdge(from, to, weight); // Add the edge to the graph
                std::string response = "Edge added successfully!\n";
                send(clientSocket, response.c_str(), response.size(), 0);
                break;
            }
            case 3:
            { // Remove an edge from the graph
                std::string prompt = "Provide the edge to remove (from, to): ";
                send(clientSocket, prompt.c_str(), prompt.size(), 0);

                char edgeBuffer[1024] = {0};
                read(clientSocket, edgeBuffer, 1024); // Read the edge details (from, to)

                std::istringstream edgeStream(edgeBuffer);
                int from, to;
                edgeStream >> from >> to;

                graphPtr->removeEdge(from, to); // Remove the edge from the graph
                std::string response = "Edge removed successfully!\n";
                send(clientSocket, response.c_str(), response.size(), 0);
                break;
            }
            case 4:
            {                                  // Get the total weight of the MST
                MST mst(*graphPtr, "kruskal"); // Use Kruskal's algorithm to create the MST
                int weight = mst.getWieght();
                std::string response = "Total weight of MST: " + std::to_string(weight) + "\n";
                send(clientSocket, response.c_str(), response.size(), 0);
                break;
            }
            case 5:
            { // Get the longest path in the MST
                std::string prompt = "Provide the start and end vertices for the shortest path: ";
                send(clientSocket, prompt.c_str(), prompt.size(), 0);

                char pathBuffer[1024] = {0};
                read(clientSocket, pathBuffer, 1024); // Read the start and end vertices

                std::istringstream pathStream(pathBuffer);
                int start, end;
                pathStream >> start >> end;
                MST mst(*graphPtr, "kruskal");
                std::vector<int> path = mst.longestPath(start, end);
                std::string response = "Longest path in MST: ";
                for (int v : path)
                {
                    response += std::to_string(v) + " ";
                }
                response += "\n";
                send(clientSocket, response.c_str(), response.size(), 0);
                break;
            }
            case 6:
            { // Get the shortest path in the MST
                std::string prompt = "Provide the start and end vertices for the shortest path: ";
                send(clientSocket, prompt.c_str(), prompt.size(), 0);

                char pathBuffer[1024] = {0};
                read(clientSocket, pathBuffer, 1024); // Read the start and end vertices

                std::istringstream pathStream(pathBuffer);
                int start, end;
                pathStream >> start >> end;

                MST mst(*graphPtr, "kruskal");
                auto path = mst.shortestPath(start, end);

                std::string response = "Shortest path from " + std::to_string(start) + " to " + std::to_string(end) + ": ";
                for (int v : path)
                {
                    response += std::to_string(v) + " ";
                }
                response += "\n";
                send(clientSocket, response.c_str(), response.size(), 0);
                break;
            }
            case 7:
            { // Get the average distance in the MST (as an integer)
                MST mst(*graphPtr, "kruskal");
                int avgDist = static_cast<int>(mst.averageDist());
                std::string response = "Average distance in MST: " + std::to_string(avgDist) + "\n";
                send(clientSocket, response.c_str(), response.size(), 0);
                break;
            }
            case 8:
            { // Print the MST (adjacency matrix format)
                MST mst(*graphPtr, "kruskal");
                std::stringstream mstStream;
                auto mstMatrix = mst.getMST();

                for (const auto &row : mstMatrix)
                {
                    for (const auto &val : row)
                    {
                        mstStream << val << " ";
                    }
                    mstStream << "\n";
                }
                std::string response = "MST Matrix:\n" + mstStream.str();
                send(clientSocket, response.c_str(), response.size(), 0);
                break;
            }
            case 9:
            {                        // Exit the client connection
                close(clientSocket); // Close the connection
                return;
            }
            default:
            { // Invalid choice handling
                std::string errorMsg = "Invalid choice. Please try again.\n";
                send(clientSocket, errorMsg.c_str(), errorMsg.size(), 0);
                break;
            }
            }

            memset(buffer, 0, sizeof(buffer)); // Clear buffer for the next loop
        }
    }

public:
    LeaderFollowerThreadPool() : stopFlag(false)
    {
        // Initialize the worker threads in the thread pool
        for (int i = 0; i < THREAD_POOL_SIZE; ++i)
        {
            workers.emplace_back([this]()
                                 { this->workerLoop(); });
        }
    }

    ~LeaderFollowerThreadPool()
    {
        // Stop the thread pool by signaling all threads to terminate
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stopFlag = true;
        }
        cv.notify_all(); // Notify all threads to wake up and stop
        for (auto &worker : workers)
        {
            worker.join(); // Join each worker thread
        }
    }

    /**
     * Function: workerLoop
     * Each worker thread waits for a task to be assigned. Once assigned, it processes the task.
     */
    void workerLoop()
    {
        while (true)
        {
            Task task;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                cv.wait(lock, [this]()
                        { return !tasks.empty() || stopFlag; }); // Wait for a task or stop signal
                if (stopFlag && tasks.empty())
                    return; // Exit loop if stopFlag is set and no more tasks
                task = tasks.front();
                tasks.pop(); // Get the next task
            }
            processClient(task.clientSocket, task.graphPtr); // Process the task
        }
    }

    /**
     * Function: addTask
     * Adds a new client task to the task queue and notifies a worker thread.
     */
    void addTask(Task task)
    {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            tasks.push(task); // Add the task to the queue
            cv.notify_one();  // Notify one worker thread to process the task
        }
    }
};

/**
 * Main function: Initializes the server and sets up the thread pool for handling clients.
 */
int main()
{
    int serverFd, newSocket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int opt = 1;

    // Create socket file descriptor
    if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        std::cerr << "Socket creation failed\n";
        exit(EXIT_FAILURE);
    }

    // Set socket options to allow port reuse
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        std::cerr << "setsockopt failed\n";
        close(serverFd);
        exit(EXIT_FAILURE);
    }

    // Set up the server address and port
    address.sin_family = AF_INET;         // IPv4
    address.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP
    address.sin_port = htons(PORT);       // Bind to the specified port

    // Bind the socket to the address and port
    if (bind(serverFd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        std::cerr << "Bind failed\n";
        close(serverFd);
        exit(EXIT_FAILURE);
    }

    // Start listening for client connections (with a backlog of 3 clients)
    if (listen(serverFd, 3) < 0)
    {
        std::cerr << "Listen failed\n";
        close(serverFd);
        exit(EXIT_FAILURE);
    }

    // Initialize the Leader-Follower thread pool
    LeaderFollowerThreadPool threadPool;
    Graph graph(std::vector<std::vector<int>>{}); // Create an empty graph

    std::cout << "Server is running. Waiting for clients...\n";

    // Accept new client connections in a loop
    while ((newSocket = accept(serverFd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) >= 0)
    {
        std::cout << "Accepted new client\n";
        Task newTask{newSocket, &graph}; // Create a new task for each client
        threadPool.addTask(newTask);     // Add the task to the thread pool
    }

    // If accept() fails
    if (newSocket < 0)
    {
        std::cerr << "Accept failed\n";
    }

    close(serverFd); // Close the server socket
    return 0;
}