// File: emergency_route.cpp
// Problem 4 – Fastest Emergency Route Planner (BFS + DFS)

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
        if (u<0||v<0||u>=n||v>=n) return;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
};

// ---------- DFS: Check if source and incident are connected ignoring blocked roads ---------- //

void dfs(int u, vector<int> &vis, const vector<vector<int>> &adj) {
    vis[u] = 1;
    for (int v : adj[u])
        if (!vis[v]) dfs(v, vis, adj);
}

// ---------- BFS: Find shortest hop path ---------- //

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

// ---------- Reconstruct and Print Path ---------- //

vector<int> reconstruct(int dest, const vector<int> &parent) {
    vector<int> path;
    for (int cur = dest; cur != -1; cur = parent[cur])
        path.push_back(cur);
    reverse(path.begin(), path.end());
    return path;
}

void printPath(const vector<int> &path, const vector<string> &names) {
    for (size_t i=0; i<path.size(); i++) {
        cout << names[path[i]];
        if (i+1 < path.size()) cout << " -> ";
    }
    cout << "\n";
}

// ---------- Build Sample Road Network with Blocked Edges ---------- //

void buildSample(CityGraph &g,
                 vector<pair<int,int>> &blocked,
                 vector<string> &names) {
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

    blocked = { {2,3} }; // road blocked here due to crowd/repair
}

// ---------- MAIN ---------- //

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "Problem 4 – Emergency Fastest Route Planner\n\n";
    cout << "1. Use sample city map\n2. Enter custom graph\nChoice: ";
    
    int mode; cin >> mode;

    CityGraph graph;
    vector<string> names;
    vector<pair<int,int>> blocked;

    if (mode == 2) {
        int n, m;
        cout << "\nEnter number of locations: ";
        cin >> n;
        graph.init(n);
        names.resize(n);

        cin.ignore();
        cout << "Enter location names:\n";
        for (int i=0;i<n;i++) getline(cin, names[i]);

        cout << "Enter number of roads: ";
        cin >> m;
        cout << "Enter roads: u v\n";
        for (int i=0;i<m;i++) {
            int u,v; cin>>u>>v;
            graph.addEdge(u,v);
        }

        int b;
        cout << "Enter number of blocked roads: ";
        cin >> b;
        cout << "Enter blocked: u v\n";
        for (int i=0;i<b;i++){
            int u,v; cin>>u>>v;
            blocked.push_back({u,v});
        }
    } 
    else {
        buildSample(graph, blocked, names);
        cout << "\nUsing sample emergency network.\n";
    }

    int station, incident;
    cout << "\nEnter ambulance station index: ";
    cin >> station;
    cout << "Enter accident/incident index: ";
    cin >> incident;

    // Remove blocked edges
    for (auto &p : blocked) {
        int u=p.first, v=p.second;
        graph.adj[u].erase(remove(graph.adj[u].begin(), graph.adj[u].end(), v), graph.adj[u].end());
        graph.adj[v].erase(remove(graph.adj[v].begin(), graph.adj[v].end(), u), graph.adj[v].end());
    }

    // Check connectivity after removing blocked roads
    vector<int> vis(graph.n,0);
    dfs(station, vis, graph.adj);

    if (!vis[incident]) {
        cout << "\nNo valid route available! Road blocked → area unreachable!\n";
        return 0;
    }

    // Find hop-distance shortest path
    vector<int> dist, parent;
    bfsPath(station, graph, dist, parent);

    cout << "\nShortest hop-count distance: " << dist[incident] << "\n";
    cout << "Route for ambulance:\n";
    auto path = reconstruct(incident, parent);
    printPath(path, names);

    cout << "\nTime Complexity:\n";
    cout << "DFS (reachability): O(V + E)\n";
    cout << "BFS (shortest hops): O(V + E)\n";
    cout << "Efficient for real emergency systems.\n";

    return 0;
}
