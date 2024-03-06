#include <iostream>
#include <vector>
#include <queue>
#include <mpi.h>

using namespace std;

void addEdge(vector<int> adj[], int u, int v) {
    adj[u].push_back(v);
}

void createBinaryTree(vector<int> adj[], int root, int V) {
    int leftChild, rightChild;

    for (int i = 0; i < V; ++i) {
        leftChild = 2 * i + 1;
        rightChild = 2 * i + 2;

        if (leftChild < V) {
            addEdge(adj, i, leftChild);
        }

        if (rightChild < V) {
            addEdge(adj, i, rightChild);
        }
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int numNodes;
    if (rank == 0) {
        cout << "Enter the number of nodes: ";
        cin >> numNodes;
    }

    MPI_Bcast(&numNodes, 1, MPI_INT, 0, MPI_COMM_WORLD);

    vector<int> adj[numNodes];

    if (rank == 0) {
        createBinaryTree(adj, 0, numNodes);
    }

    for (int i = 0; i < numNodes; ++i) {
        int size = adj[i].size();
        MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);

        if (rank == 0) {
            MPI_Bcast(adj[i].data(), size, MPI_INT, 0, MPI_COMM_WORLD);
        } else {
            adj[i].resize(size);
            MPI_Bcast(adj[i].data(), size, MPI_INT, 0, MPI_COMM_WORLD);
        }
    }

    int startVertex = rand() % numNodes;

    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

    // BFS function
    cout << rank << " ";
    bool *visited = new bool[numNodes];
    for (int i = 0; i < numNodes; ++i) {
        visited[i] = false;
    }

    queue<int> q;

    int verticesPerProcess = numNodes / size;
    int start = rank * verticesPerProcess;
    int end = (rank + 1) * verticesPerProcess;

    visited[start] = true;
    q.push(start);

    double communication_start = MPI_Wtime();

    while (!q.empty()) {
        int currentVertex = q.front();
        q.pop();

        for (int i = 0; i < adj[currentVertex].size(); i++) {
            int neighbor = adj[currentVertex][i];
            if (!visited[neighbor - start]) {
                visited[neighbor - start] = true;
                q.push(neighbor - start);
            }
        }
    }

    double communication_end = MPI_Wtime();
    double communication_time = communication_end - communication_start;

    delete[] visited;

    double end_time = MPI_Wtime();
    double total_time = end_time - start_time - communication_time;

    if (rank == 0) {
        cout << "Communication time: " << communication_time << " seconds" << endl;
        cout << "Computation time: " << total_time << " seconds" << endl;
    }

    MPI_Finalize();

    return 0;
}

