#include "mst.hpp"
#include <queue>
#include <algorithm>
#include <limits>

using namespace std;

MST::MST(Graph graph, string algo) : graph(graph)
{
    vector<vector<int>> adjmat = graph.getAdjMat();
    int n = adjmat.size();
    if (n == 0)
    {
        mst = vector<vector<int>>();
    }
    mst = vector<vector<int>>(n, vector<int>(n, 0));

    if (algo == "kruskal")
    {
        kruskal(adjmat);
    }
    else if (algo == "boruvka")
    {
        boruvka(adjmat);
    }
    else
    {
        cout << "Invalid algorithm" << endl;
    }
}

int MST::getWieght()
{
    if (mst.empty())
        return 0;
    int weight = 0;
    for (size_t i = 0; i < mst.size(); i++)
    {
        for (size_t j = i + 1; j < mst[i].size(); j++)
        {
            weight += mst[i][j];
        }
    }
    return weight;
}

void MST::printMST()
{
    for (const auto &row : mst)
    {
        for (int val : row)
        {
            cout << val << " | ";
        }
        cout << endl;
    }
}

vector<vector<int>> MST::getMST()
{
    return mst;
}

void MST::kruskal(vector<vector<int>> &adj)
{
    if (adj.empty())
        return;
    int n = adj.size();
    vector<tuple<int, int, int>> edges;

    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (adj[i][j] > 0)
            {
                edges.emplace_back(adj[i][j], i, j);
            }
        }
    }

    sort(edges.begin(), edges.end());

    vector<int> parent(n);
    for (int i = 0; i < n; i++)
        parent[i] = i;

    function<int(int)> find = [&](int x)
    {
        if (parent[x] != x)
            parent[x] = find(parent[x]);
        return parent[x];
    };

    auto unite = [&](int x, int y)
    {
        x = find(x);
        y = find(y);
        if (x != y)
            parent[y] = x;
    };

    for (const auto &[w, u, v] : edges)
    {
        if (find(u) != find(v))
        {
            unite(u, v);
            mst[u][v] = mst[v][u] = w;
        }
    }
}

void MST::boruvka(vector<vector<int>> &adj)
{
    if (adj.empty())
        return;
    int n = adj.size();
    vector<int> parent(n);
    vector<int> rank(n, 0);
    for (int i = 0; i < n; i++)
        parent[i] = i;

    function<int(int)> find = [&](int x)
    {
        if (parent[x] != x)
            parent[x] = find(parent[x]);
        return parent[x];
    };

    auto unite = [&](int x, int y)
    {
        x = find(x);
        y = find(y);
        if (x == y)
            return false;
        if (rank[x] < rank[y])
            swap(x, y);
        parent[y] = x;
        if (rank[x] == rank[y])
            rank[x]++;
        return true;
    };

    bool change = true;
    while (change)
    {
        change = false;
        vector<pair<int, int>> cheapest(n, {-1, -1});

        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (adj[i][j] > 0)
                {
                    int set1 = find(i), set2 = find(j);
                    if (set1 != set2)
                    {
                        if (cheapest[set1].second == -1 || adj[i][j] < adj[cheapest[set1].first][cheapest[set1].second])
                        {
                            cheapest[set1] = {i, j};
                        }
                        if (cheapest[set2].second == -1 || adj[i][j] < adj[cheapest[set2].first][cheapest[set2].second])
                        {
                            cheapest[set2] = {i, j};
                        }
                    }
                }
            }
        }

        for (int i = 0; i < n; i++)
        {
            if (cheapest[i].second != -1)
            {
                int u = cheapest[i].first, v = cheapest[i].second;
                int set1 = find(u), set2 = find(v);
                if (set1 != set2)
                {
                    mst[u][v] = mst[v][u] = adj[u][v];
                    unite(set1, set2);
                    change = true;
                }
            }
        }
    }
}

vector<int> MST::shortestPath(int start, int end)
{
    if (mst.empty())
        return {};
    
    if (start == end)
        return {start};
    
    int n = mst.size(); // Get the number of nodes from the adjacency matrix
    vector<bool> visited(n, false);
    vector<int> parent(n, -1);
    queue<int> q;

    q.push(start);
    visited[start] = true;

    // BFS to find the shortest path in an unweighted graph
    while (!q.empty())
    {
        int u = q.front();
        q.pop();

        // If we have reached the destination, we can break
        if (u == end)
            break;

        // Traverse neighbors in the adjacency matrix
        for (int v = 0; v < n; v++)
        {
            if (mst[u][v] > 0 && !visited[v])  // There is an edge and `v` is not visited
            {
                visited[v] = true;
                parent[v] = u;
                q.push(v);
            }
        }
    }

    // If we never reached the destination, return an empty path
    if (!visited[end])
        return {};

    // Reconstruct the path from `end` to `start` using the parent array
    vector<int> path;
    for (int v = end; v != -1; v = parent[v])
    {
        path.push_back(v);
    }
    reverse(path.begin(), path.end()); // Reverse to get the path from start to end
    return path;
}


vector<int> MST::longestPath(int start, int end)
{
    if (mst.empty())
        return {};
    int n = mst.size();
    vector<int> dist(n, -1);
    vector<int> parent(n, -1);

    function<void(int, int)> dfs = [&](int u, int p)
    {
        for (int v = 0; v < n; v++)
        {
            if (mst[u][v] > 0 && v != p)
            {
                dist[v] = dist[u] + 1;
                parent[v] = u;
                dfs(v, u);
            }
        }
    };

    dist[start] = 0;
    dfs(start, -1);

    if (dist[end] == -1)
        return {};

    vector<int> path;
    for (int v = end; v != -1; v = parent[v])
    {
        path.push_back(v);
    }
    reverse(path.begin(), path.end());
    return path;
}

int MST::averageDist() {
    if (mst.empty())
        return -1;

    int totalDistance = 0;
    int pathCount = 0;
    size_t n = mst.size();

    // Iterate through all pairs of vertices
    for (size_t start = 0; start < n; ++start) {
        for (size_t end = start + 1; end < n; ++end) {
            if (mst[start][end] > 0) {
                totalDistance += mst[start][end];
                pathCount++;
            }
        }
    }

    if (pathCount == 0)
        return -1;

    // Calculate average and round up
    return (totalDistance + pathCount)  / pathCount;
}