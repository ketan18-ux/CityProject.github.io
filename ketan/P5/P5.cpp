
// Problem 5 – Water Pipeline Pressure Optimization
#include <bits/stdc++.h>
using namespace std;

struct Pipe {
    int u, v;
    int capacity;
    double score;
};

// ------------ DFS to compute downstream demand ------------ //
int dfs(int node, int parent, vector<vector<int>> &adj,
        vector<int> &demand, vector<int> &subtreeDemand) {

    int total = demand[node];
    for (int nxt : adj[node]) {
        if (nxt == parent) continue;
        total += dfs(nxt, node, adj, demand, subtreeDemand);
    }
    subtreeDemand[node] = total;
    return total;
}

// ------------ QuickSort for ranking ------------ //
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

// ------------ CSV Loaders ------------ //

bool loadNodesCSV(vector<int> &demand) {
    ifstream fin("nodes.csv");
    if (!fin.is_open()) return false;

    string line;
    getline(fin, line); // header

    demand.clear();

    while (getline(fin, line)) {
        size_t pos = line.find(',');
        if (pos == string::npos) continue;

        int d = stoi(line.substr(pos + 1));
        demand.push_back(d);
    }
    return true;
}

bool loadPipesCSV(vector<Pipe> &pipes, vector<vector<int>> &adj, int n) {
    ifstream fin("pipes.csv");
    if (!fin.is_open()) return false;

    string line;
    getline(fin, line); // header

    pipes.clear();
    adj.assign(n, {});

    while (getline(fin, line)) {
        vector<string> tokens;
        string temp;
        stringstream ss(line);

        while (getline(ss, temp, ',')) tokens.push_back(temp);
        if (tokens.size() != 3) continue;

        Pipe p;
        p.u = stoi(tokens[0]);
        p.v = stoi(tokens[1]);
        p.capacity = stoi(tokens[2]);

        pipes.push_back(p);
        adj[p.u].push_back(p.v);
        adj[p.v].push_back(p.u);
    }

    return true;
}

// ------------ MAIN ------------ //
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "Problem 5 – Water Pipeline Pressure Optimization (CSV Enabled)\n";

    vector<int> demand;
    if (!loadNodesCSV(demand)) {
        cout << "❌ nodes.csv not found. Exiting.\n";
        return 0;
    }

    int n = demand.size();
    cout << "Loaded " << n << " nodes.\n";

    vector<Pipe> pipes;
    vector<vector<int>> adj;

    if (!loadPipesCSV(pipes, adj, n)) {
        cout << "❌ pipes.csv not found. Exiting.\n";
        return 0;
    }

    int m = pipes.size();
    cout << "Loaded " << m << " pipes.\n";

    int source;
    cout << "\nEnter source node index: ";
    cin >> source;

    vector<int> subtreeDemand(n, 0);
    dfs(source, -1, adj, demand, subtreeDemand);

    for (auto &p : pipes) {
        int nodeDown = (subtreeDemand[p.u] > subtreeDemand[p.v]) ? p.u : p.v;
        p.score = (double)subtreeDemand[nodeDown] / p.capacity;
    }

    quickSort(pipes, 0, m - 1);

    cout << "\nRanked Pipeline Bottlenecks (High score = Critical):\n";
    cout << left << setw(8) << "U"
         << setw(8) << "V"
         << setw(12) << "Capacity"
         << "Score\n";
    cout << string(45, '-') << "\n";

    for (auto &p : pipes) {
        cout << left << setw(8) << p.u
             << setw(8) << p.v
             << setw(12) << p.capacity
             << fixed << setprecision(2) << p.score << "\n";
    }

    cout << "\nTime Complexity:\n";
    cout << "- DFS: O(n + m)\n";
    cout << "- Sorting: O(m log m)\n";
    cout << "Overall: O(n + m log m)\n";

    return 0;
}
