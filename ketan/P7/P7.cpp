#include <bits/stdc++.h>
using namespace std;

// ---------------- Dijkstra ---------------- //
vector<double> dijkstra(int src, const vector<vector<pair<int,double>>> &adj) {
    int n = adj.size();
    vector<double> dist(n, 1e18);
    vector<int> vis(n, 0);

    priority_queue<pair<double,int>,
                   vector<pair<double,int>>,
                   greater<pair<double,int>>> pq;

    dist[src] = 0;
    pq.push({0, src});

    while (!pq.empty()) {
        auto [cost, u] = pq.top();
        pq.pop();
        if (vis[u]) continue;
        vis[u] = 1;

        for (auto &nx : adj[u]) {
            int v = nx.first;
            double w = nx.second;
            if (dist[v] > cost + w) {
                dist[v] = cost + w;
                pq.push({dist[v], v});
            }
        }
    }
    return dist;
}

// ------------- Build Distance Matrix ------------- //
vector<vector<double>> buildDistanceMatrix(const vector<vector<pair<int,double>>> &adj) {
    int n = adj.size();
    vector<vector<double>> mat(n, vector<double>(n, 1e18));

    for (int i = 0; i < n; i++) {
        auto dist = dijkstra(i, adj);
        for (int j = 0; j < n; j++)
            mat[i][j] = dist[j];
    }
    return mat;
}

// ------------- Greedy Nearest Neighbour TSP ------------- //
vector<int> greedyRoute(const vector<vector<double>> &mat, int start) {
    int n = mat.size();
    vector<int> visited(n, 0);
    vector<int> route;
    route.reserve(n);

    int cur = start;
    visited[cur] = 1;
    route.push_back(cur);

    for (int step = 1; step < n; step++) {
        double best = 1e18;
        int nxt = -1;
        for (int i = 0; i < n; i++) {
            if (!visited[i] && mat[cur][i] < best) {
                best = mat[cur][i];
                nxt = i;
            }
        }
        if (nxt == -1) break;
        visited[nxt] = 1;
        route.push_back(nxt);
        cur = nxt;
    }

    route.push_back(start);
    return route;
}

double computeRouteCost(const vector<int> &route, const vector<vector<double>> &mat) {
    double cost = 0;
    for (int i = 0; i + 1 < (int)route.size(); i++) {
        cost += mat[route[i]][route[i+1]];
    }
    return cost;
}

// ---------------- CSV Loaders ---------------- //
bool loadNodesCSV(vector<string> &name) {
    ifstream fin("garbage_nodes.csv");
    if (!fin.is_open()) return false;

    string line;
    getline(fin, line); // skip header
    name.clear();

    while (getline(fin, line)) {
        size_t pos = line.find(',');
        if (pos == string::npos) continue;
        name.push_back(line.substr(pos+1));
    }
    return true;
}

bool loadEdgesCSV(vector<vector<pair<int,double>>> &adj, int n) {
    ifstream fin("garbage_edges.csv");
    if (!fin.is_open()) return false;

    adj.assign(n, {});
    string line;
    getline(fin, line); // skip header

    while (getline(fin, line)) {
        string u,v,d;
        stringstream ss(line);
        getline(ss, u, ',');
        getline(ss, v, ',');
        getline(ss, d, ',');

        int a = stoi(u);
        int b = stoi(v);
        double w = stod(d);

        adj[a].push_back({b,w});
        adj[b].push_back({a,w});
    }
    return true;
}

// ---------------- Printing ---------------- //
void printMatrix(const vector<vector<double>> &mat) {
    int n = mat.size();
    cout << "\nDistance Matrix:\n";
    cout << "      ";
    for (int j = 0; j < n; j++) cout << setw(6) << j;
    cout << "\n";

    for (int i = 0; i < n; i++) {
        cout << setw(4) << i << " ";
        for (int j = 0; j < n; j++) {
            if (mat[i][j] > 1e17) cout << setw(6) << "INF";
            else cout << setw(6) << fixed << setprecision(1) << mat[i][j];
        }
        cout << "\n";
    }
    cout << "\n";
}

void printRoute(const vector<int> &route, double cost) {
    cout << "--- Garbage Truck Route ---\n";
    for (int i = 0; i < route.size(); i++) {
        cout << route[i];
        if (i+1 < route.size()) cout << " -> ";
    }
    cout << "\n\nTotal Travel Cost: " << cost << "\n";
}

// ---------------- MAIN ---------------- //
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "Problem 7 – Garbage Truck Route Optimization (CSV Enabled)\n\n";

    vector<string> names;
    if (!loadNodesCSV(names)) {
        cout << "❌ Error: garbage_nodes.csv missing.\n";
        return 0;
    }

    vector<vector<pair<int,double>>> adj;
    if (!loadEdgesCSV(adj, names.size())) {
        cout << "❌ Error: garbage_edges.csv missing.\n";
        return 0;
    }

    cout << "Loaded " << names.size() << " pickup points.\n";
    cout << "Enter depot index: ";

    int start;
    cin >> start;

    auto mat = buildDistanceMatrix(adj);
    printMatrix(mat);

    auto route = greedyRoute(mat, start);
    double cost = computeRouteCost(route, mat);

    printRoute(route, cost);

    cout << "\nTime Complexity:\n";
    cout << "- Dijkstra for all nodes: O(V * (E log V))\n";
    cout << "- Route construction:     O(V^2)\n";
    cout << "Overall: O(V * E log V)\n";

    return 0;
}
