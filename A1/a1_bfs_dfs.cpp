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

    void serialBFS(int start, bool printTraversal) {

        vector<int> visited(V, 0);
        queue<int> q;

        q.push(start);
        visited[start] = 1;

        while (!q.empty()) {

            int node = q.front();
            q.pop();

            if (printTraversal)
                cout << node << " ";

            for (int neighbor : adj[node]) {

                if (!visited[neighbor]) {
                    visited[neighbor] = 1;
                    q.push(neighbor);
                }
            }
        }
    }

    void parallelBFS(int start, bool printTraversal) {

        vector<int> visited(V, 0);
        vector<int> currentLevel;

        currentLevel.push_back(start);
        visited[start] = 1;

        while (!currentLevel.empty()) {

            vector<int> nextLevel;

            if (printTraversal) {

                for (int node : currentLevel)
                    cout << node << " ";
            }

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

    void serialDFSUtil(int node, vector<int>& visited, bool printTraversal) {

        visited[node] = 1;

        if (printTraversal)
            cout << node << " ";

        for (int neighbor : adj[node]) {

            if (!visited[neighbor]) {

                visited[neighbor] = 1;
                serialDFSUtil(neighbor, visited, printTraversal);
            }
        }
    }

    void serialDFS(int start, bool printTraversal) {

        vector<int> visited(V, 0);
        serialDFSUtil(start, visited, printTraversal);
    }

    void parallelDFSUtil(int node, vector<int>& visited, bool printTraversal) {

        if (printTraversal) {

            #pragma omp critical
            {
                cout << node << " ";
            }
        }

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
                    parallelDFSUtil(neighbor, visited, printTraversal);
                }
            }
        }

        #pragma omp taskwait
    }

    void parallelDFS(int start, bool printTraversal) {

        vector<int> visited(V, 0);
        visited[start] = 1;

        #pragma omp parallel
        {
            #pragma omp single
            {
                parallelDFSUtil(start, visited, printTraversal);
            }
        }
    }
};

int main() {

    int V, E, graphChoice;
    Graph* g;

    cout << fixed << setprecision(6);

    cout << "===== GRAPH INPUT MENU =====\n";
    cout << "1. Manual Graph Input\n";
    cout << "2. Random Large Graph\n";

    cout << "Enter your choice: ";
    cin >> graphChoice;

    if (graphChoice == 1) {

        cout << "Enter number of vertices: ";
        cin >> V;

        if (V <= 0) {
            cout << "Invalid number of vertices\n";
            return 0;
        }

        g = new Graph(V);

        cout << "Enter number of edges: ";
        cin >> E;

        if (E < 0) {
            cout << "Invalid number of edges\n";
            return 0;
        }

        cout << "Enter edges (u v):\n";

        for (int i = 0; i < E; i++) {

            int u, v;
            cin >> u >> v;

            g->addEdge(u, v);
        }
    }

    else if (graphChoice == 2) {

        cout << "Enter number of vertices: ";
        cin >> V;

        if (V <= 0) {
            cout << "Invalid number of vertices\n";
            return 0;
        }

        g = new Graph(V);

        int edgesPerVertex;

        cout << "Enter edges per vertex: ";
        cin >> edgesPerVertex;

        srand(time(0));

        for (int i = 0; i < V; i++) {

            for (int j = 0; j < edgesPerVertex; j++) {

                int neighbor = rand() % V;

                if (neighbor != i)
                    g->addEdge(i, neighbor);
            }
        }

        cout << "\nRandom graph generated successfully.\n";
    }

    else {
        cout << "Invalid Choice\n";
        return 0;
    }

    int start;

    cout << "\nEnter starting vertex: ";
    cin >> start;

    if (start < 0 || start >= V) {
        cout << "Invalid starting vertex\n";
        return 0;
    }

    int choice;

    do {

        cout << "\n\n===== MENU =====\n";
        cout << "1. BFS Traversal\n";
        cout << "2. DFS Traversal\n";
        cout << "3. Exit\n";

        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {

            case 1: {

                double startTime, endTime;

                startTime = omp_get_wtime();
                g->serialBFS(start, false);
                endTime = omp_get_wtime();

                cout << "\nSerial BFS Time: "
                     << (endTime - startTime) * 1000
                     << " ms";

                startTime = omp_get_wtime();
                g->parallelBFS(start, false);
                endTime = omp_get_wtime();

                cout << "\nParallel BFS Time: "
                     << (endTime - startTime) * 1000
                     << " ms";

                char printChoice;

                cout << "\n\nDo you want to print traversal? (y/n): ";
                cin >> printChoice;

                if (printChoice == 'y' || printChoice == 'Y') {

                    cout << "\nSerial BFS Traversal: ";
                    g->serialBFS(start, true);

                    cout << "\nParallel BFS Traversal: ";
                    g->parallelBFS(start, true);

                    cout << endl;
                }

                break;
            }

            case 2: {

                double startTime, endTime;

                startTime = omp_get_wtime();
                g->serialDFS(start, false);
                endTime = omp_get_wtime();

                cout << "\nSerial DFS Time: "
                     << (endTime - startTime) * 1000
                     << " ms";

                startTime = omp_get_wtime();
                g->parallelDFS(start, false);
                endTime = omp_get_wtime();

                cout << "\nParallel DFS Time: "
                     << (endTime - startTime) * 1000
                     << " ms";

                char printChoice;

                cout << "\n\nDo you want to print traversal? (y/n): ";
                cin >> printChoice;

                if (printChoice == 'y' || printChoice == 'Y') {

                    cout << "\nSerial DFS Traversal: ";
                    g->serialDFS(start, true);

                    cout << "\nParallel DFS Traversal: ";
                    g->parallelDFS(start, true);

                    cout << endl;
                }

                break;
            }

            case 3:
                cout << "\nExiting Program...\n";
                break;

            default:
                cout << "\nInvalid Choice\n";
        }

    } while (choice != 3);

    delete g;
    return 0;
}