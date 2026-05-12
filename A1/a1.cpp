#include <iostream>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <omp.h>
#include <iomanip>

using namespace std;

class Graph {

    int V;
    vector<vector<int>> adj;

public:

    Graph(int V) {
        this->V = V;
        adj.resize(V);
    }

    void addEdge(int u, int v) {

        if (u < 0 || v < 0 || u >= V || v >= V)
            return;

        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    // ---------------- SERIAL BFS ----------------

    void serialBFS(int start) {

        vector<int> visited(V, 0);
        queue<int> q;

        q.push(start);
        visited[start] = 1;

        while (!q.empty()) {

            int node = q.front();
            q.pop();

            for (int neighbor : adj[node]) {

                if (!visited[neighbor]) {
                    visited[neighbor] = 1;
                    q.push(neighbor);
                }
            }
        }
    }

    // ---------------- PARALLEL BFS ----------------

    void parallelBFS(int start) {

        vector<int> visited(V, 0);
        vector<int> currentLevel;

        currentLevel.push_back(start);
        visited[start] = 1;

        while (!currentLevel.empty()) {

            vector<int> nextLevel;

            #pragma omp parallel for
            for (int i = 0; i < currentLevel.size(); i++) {

                int node = currentLevel[i];

                for (int neighbor : adj[node]) {

                    if (!visited[neighbor]) {

                        #pragma omp critical
                        {
                            if (!visited[neighbor]) {

                                visited[neighbor] = 1;
                                nextLevel.push_back(neighbor);
                            }
                        }
                    }
                }
            }

            currentLevel = nextLevel;
        }
    }

    // ---------------- SERIAL DFS ----------------

    void serialDFSUtil(int node, vector<int>& visited) {

        visited[node] = 1;

        for (int neighbor : adj[node]) {

            if (!visited[neighbor]) {

                visited[neighbor] = 1;
                serialDFSUtil(neighbor, visited);
            }
        }
    }

    void serialDFS(int start) {

        vector<int> visited(V, 0);
        serialDFSUtil(start, visited);
    }

    // ---------------- PARALLEL DFS ----------------

    void parallelDFSUtil(int node, vector<int>& visited) {

        for (int neighbor : adj[node]) {

            if (!visited[neighbor]) {

                bool createTask = false;

                #pragma omp critical
                {
                    if (!visited[neighbor]) {

                        visited[neighbor] = 1;
                        createTask = true;
                    }
                }

                if (createTask) {

                    #pragma omp task shared(visited)
                    parallelDFSUtil(neighbor, visited);
                }
            }
        }

        #pragma omp taskwait
    }

    void parallelDFS(int start) {

        vector<int> visited(V, 0);
        visited[start] = 1;

        #pragma omp parallel
        {
            #pragma omp single
            {
                parallelDFSUtil(start, visited);
            }
        }
    }
};

int main() {

    cout << fixed << setprecision(6);

    int V, edgesPerVertex;

    cout << "Enter number of vertices: ";
    cin >> V;

    if (V <= 0) {
        cout << "Invalid number of vertices\n";
        return 0;
    }

    cout << "Enter edges per vertex: ";
    cin >> edgesPerVertex;

    Graph g(V);

    srand(time(0));

    for (int i = 0; i < V; i++) {

        for (int j = 0; j < edgesPerVertex; j++) {

            int neighbor = rand() % V;

            if (neighbor != i)
                g.addEdge(i, neighbor);
        }
    }

    cout << "\nRandom graph generated successfully.\n";

    int start = 0;

    double startTime, endTime;

    // Serial BFS
    startTime = omp_get_wtime();
    g.serialBFS(start);
    endTime = omp_get_wtime();

    cout << "\nSerial BFS Time: "
         << (endTime - startTime) * 1000
         << " ms";

    // Parallel BFS
    startTime = omp_get_wtime();
    g.parallelBFS(start);
    endTime = omp_get_wtime();

    cout << "\nParallel BFS Time: "
         << (endTime - startTime) * 1000
         << " ms";

    // Serial DFS
    startTime = omp_get_wtime();
    g.serialDFS(start);
    endTime = omp_get_wtime();

    cout << "\nSerial DFS Time: "
         << (endTime - startTime) * 1000
         << " ms";

    // Parallel DFS
    startTime = omp_get_wtime();
    g.parallelDFS(start);
    endTime = omp_get_wtime();

    cout << "\nParallel DFS Time: "
         << (endTime - startTime) * 1000
         << " ms\n";

    return 0;
