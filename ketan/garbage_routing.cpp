#include <bits/stdc++.h>
using namespace std;

// ------------------------- Dijkstra (single-source shortest paths) ------------------------- //
vector<double> dijkstra(int src, const vector<vector<pair<int,double>>> &adj) {
    int n = adj.size();
    vector<double> dist(n, 1e18);
    vector<int> vis(n, 0);

    priority_queue<pair<double,int>, vector<pair<double,int>>, greater<pair<double,int>>> pq;
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

// ------------------------- Build full distance matrix using Dijkstra ------------------------- //
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

// ------------------------- Greedy route (Nearest-Neighbour) ------------------------- //
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
    route.push_back(start); // return to depot
    return route;
}

double computeRouteCost(const vector<int> &route, const vector<vector<double>> &mat) {
    double cost = 0;
    for (int i = 0; i+1 < (int)route.size(); i++) {
        cost += mat[route[i]][route[i+1]];
    }
    return cost;
}

// ------------------------- Sample map input ------------------------- //
void buildSampleCity(vector<vector<pair<int,double>>> &adj, int &start) {
    /*
         0 (Depot)
         |   \
         4    6
         |     \
        1       2
        | 3   5 |
        3       4
         \     /
           4
           |
           3
    */

    int n = 5;
    adj.assign(n, {});
    start = 0;

    auto add = [&](int u, int v, double w){
        adj[u].push_back({v,w});
        adj[v].push_back({u,w});
    };

    add(0,1,4);
    add(0,2,6);
    add(1,3,3);
    add(2,4,4);
    add(3,4,4);
}

// ------------------------- Pretty printing ------------------------- //
void printMatrix(const vector<vector<double>> &mat) {
    int n = mat.size();
    cout << "\nDistance Matrix (Dijkstra computed):\n";
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
        if (i + 1 < route.size()) cout << " -> ";
    }
    cout << "\n\nTotal Travel Cost: " << cost << "\n";
}

// ------------------------- MAIN ------------------------- //
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "Problem 7: Garbage Truck Route Optimization\n\n";
    cout << "1. Use sample city map\n";
    cout << "2. Enter custom map\n";
    cout << "Choice: ";

    int mode;
    cin >> mode;

    vector<vector<pair<int,double>>> adj;
    int start = 0;

    if (mode == 2) {
        int n, m;
        cout << "\nEnter number of pickup points: ";
        cin >> n;
        adj.assign(n, {});

        cout << "Enter number of connecting roads: ";
        cin >> m;
        cout << "Enter each road as: from to distance\n";
        for (int i = 0; i < m; i++) {
            int u,v;
            double w;
            cin >> u >> v >> w;
            adj[u].push_back({v,w});
            adj[v].push_back({u,w});
        }

        cout << "\nEnter depot index: ";
        cin >> start;
    }
    else {
        buildSampleCity(adj, start);
        cout << "\nUsing sample city map...\n";
    }

    auto mat = buildDistanceMatrix(adj);
    printMatrix(mat);

    auto route = greedyRoute(mat, start);
    double cost = computeRouteCost(route, mat);
    printRoute(route, cost);

    cout << "\nTime Complexity:\n";
    cout << "- Dijkstra for all nodes: O(V * (E log V))\n";
    cout << "- Greedy route building:  O(V^2)\n";
    cout << "Overall: O(V * E log V)\n";

    return 0;
}
