#include <bits/stdc++.h>
using namespace std;

const int INF = 1e9;

class CityGraph {
public:
    int n;
    vector<vector<int>> adj;

    CityGraph(int nodes = 0) { init(nodes); }

    void init(int nodes) {
        n = nodes;
        adj.assign(n, {});
    }

    void addEdge(int u, int v) {
        if (u < 0 || v < 0 || u >= n || v >= n) return;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
};

// ---------------- DFS (Reachability After Blocking) ---------------- //

void dfs(int u, vector<int> &vis, const vector<vector<int>> &adj) {
    vis[u] = 1;
    for (int v : adj[u])
        if (!vis[v]) dfs(v, vis, adj);
}

// ---------------- BFS Shortest Path ---------------- //

void bfsPath(int src, const CityGraph &g,
             vector<int> &dist, vector<int> &parent) {

    dist.assign(g.n, INF);
    parent.assign(g.n, -1);

    queue<int> q;
    dist[src] = 0;
    q.push(src);

    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (int v : g.adj[u]) {
            if (dist[v] == INF) {
                dist[v] = dist[u] + 1;
                parent[v] = u;
                q.push(v);
            }
        }
    }
}

// ---------------- Reconstruct Path ---------------- //

vector<int> reconstruct(int dest, const vector<int> &parent) {
    vector<int> path;
    for (int cur = dest; cur != -1; cur = parent[cur])
        path.push_back(cur);
    reverse(path.begin(), path.end());
    return path;
}

void printPath(const vector<int> &path, const vector<string> &names) {
    for (size_t i = 0; i < path.size(); i++) {
        cout << names[path[i]];
        if (i + 1 < path.size()) cout << " -> ";
    }
    cout << "\n";
}

// ---------------- CSV Loaders ---------------- //

bool loadLocationsCSV(string file, vector<string> &names) {
    ifstream fin(file);
    if (!fin.is_open()) return false;

    names.clear();
    string line;
    getline(fin, line); // skip header

    while (getline(fin, line)) {
        size_t pos = line.find(',');
        if (pos == string::npos) continue;
        string name = line.substr(pos + 1);
        names.push_back(name);
    }
    return true;
}

bool loadRoadsCSV(string file, vector<pair<int,int>> &edges) {
    ifstream fin(file);
    if (!fin.is_open()) return false;

    edges.clear();
    string line;
    getline(fin, line); // skip header

    while (getline(fin, line)) {
        size_t pos = line.find(',');
        if (pos == string::npos) continue;

        int u = stoi(line.substr(0, pos));
        int v = stoi(line.substr(pos + 1));

        edges.push_back({u, v});
    }
    return true;
}

bool loadBlockedCSV(string file, vector<pair<int,int>> &blocked) {
    ifstream fin(file);
    if (!fin.is_open()) return false;

    blocked.clear();
    string line;
    getline(fin, line); // skip header

    while (getline(fin, line)) {
        size_t pos = line.find(',');
        if (pos == string::npos) continue;

        int u = stoi(line.substr(0, pos));
        int v = stoi(line.substr(pos + 1));

        blocked.push_back({u, v});
    }
    return true;
}

// ---------------- Build Sample (Fallback) ---------------- //

void buildSample(CityGraph &g, vector<pair<int,int>> &blocked, vector<string> &names) {

    g.init(7);
    names = {
        "Hospital(0)", "Square(1)", "Mall(2)",
        "School(3)", "Bridge(4)", "Stadium(5)", "Accident Spot(6)"
    };

    g.addEdge(0,1);
    g.addEdge(1,2);
    g.addEdge(2,3);
    g.addEdge(3,6);
    g.addEdge(1,4);
    g.addEdge(4,5);
    g.addEdge(5,6);

    blocked = { {2,3} };
}

// ---------------- MAIN ---------------- //

int main() {

    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "Problem 4 – Emergency Fastest Route Planner (CSV Enabled)\n\n";

    vector<string> names;
    vector<pair<int,int>> edges, blocked;

    bool ok1 = loadLocationsCSV("locations.csv", names);
    bool ok2 = loadRoadsCSV("roads.csv", edges);
    bool ok3 = loadBlockedCSV("blocked.csv", blocked);

    CityGraph graph;

    if (ok1 && ok2) {
        cout << "Loaded city data from CSV files.\n";

        graph.init(names.size());
        for (auto &e : edges)
            graph.addEdge(e.first, e.second);

    } else {
        cout << "CSV files missing — using sample city.\n";
        buildSample(graph, blocked, names);
    }

    cout << "\nLocations:\n";
    for (int i = 0; i < names.size(); i++)
        cout << i << " → " << names[i] << "\n";

    int station, incident;
    cout << "\nEnter ambulance station index: ";
    cin >> station;
    cout << "Enter accident/incident index: ";
    cin >> incident;

    // Apply blocked roads
    for (auto &b : blocked) {
        int u = b.first, v = b.second;

        graph.adj[u].erase(remove(graph.adj[u].begin(), graph.adj[u].end(), v), graph.adj[u].end());
        graph.adj[v].erase(remove(graph.adj[v].begin(), graph.adj[v].end(), u), graph.adj[v].end());
    }

    // Reachability check
    vector<int> vis(graph.n, 0);
    dfs(station, vis, graph.adj);

    if (!vis[incident]) {
        cout << "\n❌ NO ROUTE POSSIBLE — Blocked roads isolate the incident!\n";
        return 0;
    }

    vector<int> dist, parent;
    bfsPath(station, graph, dist, parent);

    cout << "\nShortest hop-count distance = " << dist[incident] << "\n";
    cout << "Emergency Route:\n";

    auto path = reconstruct(incident, parent);
    printPath(path, names);

    cout << "\nTime Complexity:\n";
    cout << "DFS Reachability: O(V + E)\n";
    cout << "BFS Shortest Path: O(V + E)\n";
    cout << "Overall: Linear time, ideal for real-time emergency routing.\n";

    return 0;
}
