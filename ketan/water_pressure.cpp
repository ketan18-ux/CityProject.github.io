#include <bits/stdc++.h>
using namespace std;

/*
 Efficient ranking of water pipelines based on:
 - Downstream demand served by each pipe
 - Pipe capacity
 Pipes with high (demand / capacity) ratio are bottlenecks.
*/

// ------------------- Edge Structure ------------------- //
struct Pipe {
    int u, v;
    int capacity;
    double score;
};

// ------------------- DFS to compute downstream demand ------------------- //
int dfs(int node, int parent, vector<vector<int>> &adj, vector<int> &demand, vector<int> &subtreeDemand) {
    int total = demand[node];
    for (int nxt : adj[node]) {
        if (nxt == parent) continue;
        total += dfs(nxt, node, adj, demand, subtreeDemand);
    }
    subtreeDemand[node] = total;
    return total;
}

// ------------------- Quick Sort for Ranking ------------------- //
int partition(vector<Pipe> &pipes, int low, int high) {
    double pivot = pipes[high].score;
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (pipes[j].score > pivot) { // Descending
            i++;
            swap(pipes[i], pipes[j]);
        }
    }
    swap(pipes[i + 1], pipes[high]);
    return i + 1;
}

void quickSort(vector<Pipe> &pipes, int low, int high) {
    if (low < high) {
        int pi = partition(pipes, low, high);
        quickSort(pipes, low, pi - 1);
        quickSort(pipes, pi + 1, high);
    }
}

// ------------------- Main ------------------- //
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "WATER PIPE PRESSURE OPTIMIZATION\n";
    cout << "--------------------------------\n";

    int n, m;
    cout << "Enter number of nodes (junctions): ";
    cin >> n;
    cout << "Enter number of pipes (edges): ";
    cin >> m;

    vector<vector<int>> adj(n);
    vector<int> demand(n);
    vector<Pipe> pipes;
    pipes.reserve(m);

    cout << "\nEnter household demand for each node:\n";
    for (int i = 0; i < n; i++) cin >> demand[i];

    cout << "\nEnter each pipe as: u v capacity\n";
    for (int i = 0; i < m; i++) {
        Pipe p;
        cin >> p.u >> p.v >> p.capacity;
        adj[p.u].push_back(p.v);
        adj[p.v].push_back(p.u);
        pipes.push_back(p);
    }

    int source;
    cout << "\nEnter the source node (where water originates): ";
    cin >> source;

    vector<int> subtreeDemand(n, 0);
    dfs(source, -1, adj, demand, subtreeDemand);

    for (auto &p : pipes) {
        int nodeDown = (subtreeDemand[p.u] > subtreeDemand[p.v]) ? p.u : p.v;
        p.score = (double)subtreeDemand[nodeDown] / p.capacity;
    }

    quickSort(pipes, 0, m - 1);

    cout << "\nRanked Pipeline Bottlenecks (higher score = more critical):\n";
    cout << left << setw(8) << "U"
         << setw(8) << "V"
         << setw(12) << "Capacity"
         << "Score (Demand/Cap)\n";
    cout << string(45, '-') << "\n";

    for (auto &p : pipes) {
        cout << left << setw(8) << p.u
             << setw(8) << p.v
             << setw(12) << p.capacity
             << fixed << setprecision(2) << p.score << "\n";
    }

    cout << "\nTime Complexity:\n";
    cout << "- DFS: O(n)\n";
    cout << "- Quick Sort: O(m log m)\n";
    cout << "Overall: O(n + m log m) — Efficient for smart city networks.\n";

    return 0;
}
