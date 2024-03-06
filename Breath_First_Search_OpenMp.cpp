#include <iostream>
#include <vector>
#include <queue>
#include <omp.h>

using namespace std;

int main() {
    int V;
    cout << "Enter the number of vertices: ";
    cin >> V;

    vector<vector<int>> adj(V);

    for (int i = 0; i < V; ++i) {
        int leftChild = 2 * i + 1;
        int rightChild = 2 * i + 2;

        if (leftChild < V) {
            adj[i].push_back(leftChild);
        }

        if (rightChild < V) {
            adj[i].push_back(rightChild);
        }
    }
    
    double computation_start = omp_get_wtime();

    bool* visited = new bool[V];
    for (int i = 0; i < V; ++i) {
        visited[i] = false;
    }

    queue<int> q;

    visited[0] = true;
    q.push(0);

    double communication_start = omp_get_wtime();

    while (!q.empty()) {
        int queueSize = q.size();

        #pragma omp parallel
        {
            #pragma omp for
            for (int i = 0; i < queueSize; ++i) {
                int s;
                #pragma omp critical
                {
                    s = q.front();
                    q.pop();
                    cout << s << " Visited " << " by thread " << omp_get_thread_num() << endl;
                }


                #pragma omp critical
                {
                    for (int j = 0; j < adj[s].size(); ++j) {

                        if (!visited[adj[s][j]]) {
                            visited[adj[s][j]] = true;
                            q.push(adj[s][j]);
                        }
                    }
                }
            }
        }
    }

    double communication_end = omp_get_wtime();
    double communication_time = communication_end - communication_start;
    double computation_end = omp_get_wtime();
    double computation_time = computation_end - computation_start - communication_time;

    delete[] visited;

    cout << "Communication time: " << communication_time << " seconds" << endl;
    cout << "Computation time: " << computation_time << " seconds" << endl;

    return 0;
}

