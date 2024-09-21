#include <iostream>           // For standard I/O operations
#include <thread>             // For creating and managing threads
#include <queue>              // For task queues used in ActiveObject
#include <mutex>              // For synchronizing access to shared resources
#include <condition_variable> // For thread synchronization
#include <sys/socket.h>       // For socket programming
#include <netinet/in.h>       // For structures and constants related to internet addresses
#include <unistd.h>           // For close() and other POSIX APIs
#include <cstring>            // For string manipulation and error handling (e.g., strerror)
#include <sstream>            // For string stream manipulation (menu building, parsing)
#include <vector>             // For handling dynamic arrays (adjacency matrices)
#include "graph.hpp"          // Includes custom Graph class
#include "mst.hpp"            // Includes custom MST class

#define PORT 8094 // Defines the port number on which the server will listen for client connections

/**
 * Class: ActiveObject
 * Implements the Active Object design pattern. This class encapsulates an asynchronous task execution model,
 * where tasks (functions) are posted to an internal queue, and a dedicated worker thread processes each task
 * in sequence. This enables asynchronous processing.
 */
class ActiveObject
{
private:
    std::thread worker;                      // Worker thread that processes the tasks
    std::queue<std::function<void()>> tasks; // Queue of tasks to be executed
    std::mutex mutex;                        // Mutex to protect access to the task queue
    std::condition_variable cv;              // Condition variable to signal the worker thread when tasks are available
    bool running = true;                     // Indicates whether the worker thread should continue running

public:
    /**
     * Constructor: Starts the worker thread.
     * The worker thread runs in an infinite loop, waiting for tasks to be posted in the queue.
     */
    ActiveObject()
    {
        worker = std::thread([this]()
                             {
                try {
                    while (running) {
                        std::function<void()> task;
                        {
                            std::unique_lock<std::mutex> lock(mutex);
                            cv.wait(lock, [this]() { return !tasks.empty() || !running; });
                            if (!running) return;
                            task = std::move(tasks.front());
                            tasks.pop();
                        }
                        task();  // Execute the task
                    }
                } catch (const std::exception &e) {
                    std::cerr << "Exception in ActiveObject worker thread: " << e.what() << std::endl;
                } });
    }

    /**
     * Function: post
     * Adds a new task to the queue and notifies the worker thread to process it.
     *
     * @param task A function (lambda or otherwise) to be executed by the ActiveObject.
     */
    void post(std::function<void()> task)
    {
        {
            std::unique_lock<std::mutex> lock(mutex);
            tasks.push(task);
            cv.notify_one();
        }
    }

    /**
     * Function: stop
     * Stops the worker thread by setting running to false and waking up the thread if it's waiting.
     */
    void stop()
    {
        {
            std::unique_lock<std::mutex> lock(mutex);
            running = false;
            cv.notify_all();
        }
        if (worker.joinable())
        {
            worker.join();
        }
    }
    ~ActiveObject()
    {
        stop();
    }
};

/**
 * Function: menu
 * Generates and returns the menu string that will be displayed to the client.
 * The menu presents options to perform different actions on the graph.
 *
 * @returns A string containing the menu options.
 */
std::string menu()
{
    std::stringstream ss;
    ss << "Menu:\n";                                               // Adds the title "Menu:" to the stringstream
    ss << "1. Create a new graph (provide adjacency matrix)\n";    // Option 1: Create a new graph
    ss << "2. Add an edge (provide: from, to, weight)\n";          // Option 2: Add an edge to the graph
    ss << "3. Remove an edge (provide: from, to)\n";               // Option 3: Remove an edge from the graph
    ss << "4. Get total weight of MST\n";                          // Option 4: Calculate the weight of the MST
    ss << "5. Get longest path in MST (provide: start, end)\n";    // Option 5: Calculate the longest path in the MST
    ss << "6. Get shortest path in MST (provide: start, end)\n";   // Option 6: Calculate the shortest path in the MST
    ss << "7. Get average distance between two vertices in MST\n"; // Option 7: Calculate average distance in the MST
    ss << "8. Print MST\n";                                        // Option 8: Print the MST matrix
    ss << "9. Exit\n";                                             // Option 9: Exit the program
    return ss.str();                                               // Converts the stringstream to a string and returns it
}

/**
 * Function: handleClientPipeline
 * Handles client interaction and requests through a socket. The function interacts with the client
 * by receiving commands, processing them, and sending back responses.
 * This is where the pipeline pattern is implemented using multiple Active Objects for different operations.
 *
 * @param newSocket The socket descriptor for communicating with the client.
 * @param graphPtr A pointer to the Graph object to perform operations on.
 */
void handleClientPipeline(int newSocket, Graph *graphPtr)
{
    ActiveObject stage1, stage2, stage3;               // ActiveObject instances to handle stages of the pipeline
    static MST mst(*graphPtr, "kruskal");              // MST object initialized with the current graph
    static bool mstCreated = false;                    // Tracks whether the MST has been created
    bool graphExists = graphPtr->getNumVertices() > 0; // Checks if the graph exists

    while (true)
    {
        // Send the menu to the client
        std::string welcome = menu();
        send(newSocket, welcome.c_str(), welcome.size(), 0);

        // Read client input
        char buffer[1024] = {0};
        ssize_t bytes_read = read(newSocket, buffer, 1024);
        if (bytes_read <= 0)
        {
            std::cerr << "Client disconnected or error: " << strerror(errno) << std::endl;
            close(newSocket);
            return;
        }

        std::string input(buffer);
        std::istringstream iss(input);
        int choice;
        iss >> choice;

        if (choice != 1 && !graphExists)
        {
            std::string errorMsg = "Please create a graph first using option 1.\n";
            send(newSocket, errorMsg.c_str(), errorMsg.size(), 0);
            continue;
        }

        switch (choice)
        {
        case 1:
        { // Create a new graph
            std::string response = "Enter the number of vertices: ";
            send(newSocket, response.c_str(), response.size(), 0);

            char matrixBuffer[1024] = {0};
            read(newSocket, matrixBuffer, 1024);
            int numVertices = std::stoi(matrixBuffer);

            std::vector<std::vector<int>> adjMat(numVertices, std::vector<int>(numVertices, 0)); // Empty adjacency matrix

            for (int i = 0; i < numVertices; ++i)
            {
                std::string rowRequest = "Enter row " + std::to_string(i + 1) + " of the adjacency matrix: ";
                send(newSocket, rowRequest.c_str(), rowRequest.size(), 0);

                char rowBuffer[1024] = {0};
                read(newSocket, rowBuffer, 1024);

                std::istringstream rowStream(rowBuffer);
                for (int j = 0; j < numVertices; ++j)
                {
                    rowStream >> adjMat[i][j]; // Fill in the adjacency matrix
                }
            }

            *graphPtr = Graph(adjMat); // Create a new graph
            mstCreated = false;        // Reset the MST flag
            graphExists = true;        // Mark that the graph exists
            std::string graphCreatedResponse = "New graph created!\n";
            send(newSocket, graphCreatedResponse.c_str(), graphCreatedResponse.size(), 0);
            break;
        }
        case 2:
        { // Add an edge
            std::string edgeRequest = "Provide the edge to add (from, to, weight): ";
            send(newSocket, edgeRequest.c_str(), edgeRequest.size(), 0);

            char edgeBuffer[1024] = {0};
            read(newSocket, edgeBuffer, 1024);
            std::istringstream edgeStream(edgeBuffer);
            int from, to, weight;
            edgeStream >> from >> to >> weight;

            stage1.post([&]()
                        {
                        graphPtr->addEdge(from, to, weight);
                        mstCreated = false; // Reset MST flag
                        std::string response = "Edge added successfully!\n";
                        send(newSocket, response.c_str(), response.size(), 0); });
            break;
        }
        case 3:
        { // Remove an edge
            std::string edgeRequest = "Provide the edge to remove (from, to): ";
            send(newSocket, edgeRequest.c_str(), edgeRequest.size(), 0);

            char edgeBuffer[1024] = {0};
            read(newSocket, edgeBuffer, 1024);
            std::istringstream edgeStream(edgeBuffer);
            int from, to;
            edgeStream >> from >> to;

            stage1.post([&]()
                        {
                        graphPtr->removeEdge(from, to);
                        mstCreated = false; // Reset MST flag
                        std::string response = "Edge removed successfully!\n";
                        send(newSocket, response.c_str(), response.size(), 0); });
            break;
        }
        case 4:
        { // Get MST weight
            stage2.post([&]()
                        {
                        if (!mstCreated) {
                            mst = MST(*graphPtr, "kruskal"); // Create the MST
                            mstCreated = true;
                        }
                        int weight = mst.getWieght();
                        std::string response = "Total weight of MST: " + std::to_string(weight) + "\n";
                        send(newSocket, response.c_str(), response.size(), 0); });
            break;
        }
        case 5:
        { // Get the longest path in the MST
            std::string pathRequest = "Provide the start and end vertices for the shortest path: ";
            send(newSocket, pathRequest.c_str(), pathRequest.size(), 0);
            char pathBuffer[1024] = {0};
            read(newSocket, pathBuffer, 1024); // Read the start and end vertices
            std::istringstream pathStream(pathBuffer);
            int start, end;
            stage3.post([&]()
                        {
                    pathStream >> start >> end;
                        if (!mstCreated) {
                            mst = MST(*graphPtr, "kruskal"); // Create the MST
                            mstCreated = true;
                        }
                     std::vector<int> path = mst.longestPath(start, end);
                    std::string response = "Longest path from "+ std::to_string(start) + " to " + std::to_string(end) + ": ";
                    for (int v : path) {
                        response += std::to_string(v) + " ";
                    }
                    response += "\n";
                        send(newSocket, response.c_str(), response.size(), 0); });
            break;
        }
        case 6:
        { // Get the shortest path in the MST
            std::string pathRequest = "Provide the start and end vertices for the shortest path: ";
            send(newSocket, pathRequest.c_str(), pathRequest.size(), 0);
            char pathBuffer[1024] = {0};
            read(newSocket, pathBuffer, 1024);
            std::istringstream pathStream(pathBuffer);
            int start, end;
            pathStream >> start >> end;

            stage3.post([&]()
                        {
                        if (!mstCreated) {
                            mst = MST(*graphPtr, "kruskal"); // Create the MST
                            mstCreated = true;
                        }
                        auto path = mst.shortestPath(start, end);
                        std::string response = "Shortest path from " + std::to_string(start) + " to " + std::to_string(end) + ": ";
                        for (auto& p : path) {
                            response += std::to_string(p) + " "; // Build path response
                        }
                        response += "\n";
                        send(newSocket, response.c_str(), response.size(), 0); });
            break;
        }
        case 7:
        { // Get the average distance in the MST
            stage3.post([&]()
                        {
                        if (!mstCreated) {
                            mst = MST(*graphPtr, "kruskal"); // Create the MST
                            mstCreated = true;
                        }
                        int avg = mst.averageDist();
                        std::string response = "Average distance in MST: " + std::to_string(avg) + "\n";
                        send(newSocket, response.c_str(), response.size(), 0); });
            break;
        }
        case 8:
        { // Print the MST matrix
            stage3.post([&]()
                        {
                            if (!mstCreated)
                            {
                                mst = MST(*graphPtr, "kruskal"); // Create the MST
                                mstCreated = true;
                            }
                            std::stringstream mstStream;   // To build the MST output
                            auto mstMatrix = mst.getMST(); // Get the MST matrix
                            for (const auto &row : mstMatrix)
                            {
                                for (const auto &val : row)
                                {
                                    mstStream << val << " "; // Append each value to the stringstream
                                }
                                mstStream << "\n"; // New line after each row
                            }
                            std::string mstOutput = "MST Matrix:\n" + mstStream.str(); // Build the final output string
                            send(newSocket, mstOutput.c_str(), mstOutput.size(), 0);   // Send the MST to the client
                        });
            break;
        }
        case 9: // Exit the program
            close(newSocket);
            return;
        default: // Invalid choice
            std::string response = "Invalid choice. Please try again.\n";
            send(newSocket, response.c_str(), response.size(), 0);
            break;
        }
    }
}

/**
 * Main Function: Initializes the server and listens for client connections.
 * The server uses a socket to accept connections and handle multiple clients in parallel.
 */
int main()
{
    int serverFd, newSocket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int opt = 1;

    // Create socket
    if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        std::cerr << "Socket creation failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Allow port reuse
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        std::cerr << "setsockopt failed" << std::endl;
        close(serverFd);
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket
    if (bind(serverFd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        close(serverFd);
        exit(EXIT_FAILURE);
    }

    // Start listening
    if (listen(serverFd, 3) < 0)
    {
        std::cerr << "Listen failed: " << strerror(errno) << std::endl;
        close(serverFd);
        exit(EXIT_FAILURE);
    }

    // Create an empty graph with an empty adjacency matrix
    Graph graph{std::vector<std::vector<int>>{}}; // Empty graph

    std::cout << "Server is running. Waiting for clients..." << std::endl;

    // Accept clients and handle them
    while ((newSocket = accept(serverFd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) >= 0)
    {
        std::cout << "Accepted new client" << std::endl;
        handleClientPipeline(newSocket, &graph); // Handle client requests
    }

    if (newSocket < 0)
    {
        std::cerr << "Accept failed: " << strerror(errno) << std::endl;
    }

    close(serverFd);
    return 0;
}