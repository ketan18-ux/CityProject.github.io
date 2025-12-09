#include <bits/stdc++.h>
using namespace std;

// ---------------- Graph Definition ----------------
class RoadGraph {
public:
    int V;
    vector<vector<pair<int,int>>> adj;   // (neighbor, time)

    RoadGraph(int v = 0) {
        init(v);
    }
    void init(int v) {
        V = v;
        adj.assign(V, {});
    }
    void addEdge(int u, int v, int w) {
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
    }
};

// ---------------- Result Structure ----------------
struct Result {
    vector<int> dist;
    vector<int> parent;
};

// ---------------- Dijkstra (Weighted Shortest Time) ----------------
Result dijkstra(const RoadGraph &g, int src) {
    Result r;
    r.dist.assign(g.V, INT_MAX);
    r.parent.assign(g.V, -1);

    priority_queue<pair<int,int>, vector<pair<int,int>>,
                   greater<pair<int,int>>> pq;

    r.dist[src] = 0;
    pq.push({0, src});

    while (!pq.empty()) {
        auto [t,u] = pq.top(); pq.pop();
        if (t > r.dist[u]) continue;

        for (auto &p : g.adj[u]) {
            int v = p.first, wt = p.second;
            if (r.dist[u] + wt < r.dist[v]) {
                r.dist[v] = r.dist[u] + wt;
                r.parent[v] = u;
                pq.push({r.dist[v], v});
            }
        }
    }
    return r;
}

// ---------------- BFS (Unweighted Zones Only) ----------------
Result bfs_unweighted(const RoadGraph &g, const vector<bool> &allowed, int src) {
    Result r;
    r.dist.assign(g.V, INT_MAX);
    r.parent.assign(g.V, -1);

    queue<int> q;
    r.dist[src] = 0;
    q.push(src);

    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (auto &p : g.adj[u]) {
            int v = p.first;
            if (allowed[v] && r.dist[v] == INT_MAX) {
                r.dist[v] = r.dist[u] + 1;
                r.parent[v] = u;
                q.push(v);
            }
        }
    }
    return r;
}

// ---------------- Path Reconstruction ----------------
vector<int> getPath(int dest, const Result &r) {
    vector<int> path;
    if (r.dist[dest] == INT_MAX) return path;
    for (int v = dest; v != -1; v = r.parent[v])
        path.push_back(v);
    reverse(path.begin(), path.end());
    return path;
}

// ---------------- Output Helpers ----------------
void printPath(const vector<int> &path) {
    for (int i = 0; i < (int)path.size(); i++) {
        cout << path[i];
        if (i+1 < (int)path.size()) cout << " -> ";
    }
}

void printTable(const Result &r, int src) {
    cout << "\nZone  "
         << "Shortest Time (min)  "
         << "Path\n";
    cout << "--------------------------------------------\n";
    for (int i = 0; i < (int)r.dist.size(); i++) {
        cout << i << "     ";
        if (r.dist[i] == INT_MAX) {
            cout << "Unreachable";
        } else {
            cout << r.dist[i] << "                  ";
        }
        if (r.dist[i] != INT_MAX) {
            auto path = getPath(i, r);
            printPath(path);
        }
        cout << "\n";
    }
    cout << endl;
}

// ---------------- File Input ----------------
bool loadFromFile(const string &fname, RoadGraph &g, int &src) {
    ifstream fin(fname);
    if (!fin.is_open()) return false;

    int V, E;
    fin >> V >> E;
    g.init(V);

    for (int i = 0; i < E; i++) {
        int u,v,w;
        fin >> u >> v >> w;
        g.addEdge(u, v, w);
    }
    fin >> src;
    return true;
}

// ---------------- Demo Manual Input ----------------
void loadDemo(RoadGraph &g, int &src) {
    g.init(6);
    g.addEdge(0,1,10);
    g.addEdge(0,2,3);
    g.addEdge(1,3,2);
    g.addEdge(2,1,1);
    g.addEdge(2,3,8);
    g.addEdge(3,4,7);
    g.addEdge(4,5,4);
    src = 0;
}

// ---------------- Main Menu ----------------
int main() {
    RoadGraph g;
    int src;

    cout << "SMART CITY — SHORTEST TRAVEL TIME ROUTING\n";
    cout << "------------------------------------------\n";
    cout << "1. Load input from file (p2_input.txt)\n";
    cout << "2. Use built-in demo graph\n";
    cout << "Choose option: ";
    int ch; cin >> ch;

    if (ch == 1) {
        if (!loadFromFile("p2_input.txt", g, src)) {
            cout << "p2_input.txt not found\n";
            return 0;
        }
    } else {
        loadDemo(g, src);
    }

    cout << "\nGraph Loaded Successfully ✔";
    cout << "\nSource Zone: " << src << "\n";

    cout << "\n--- Algorithm Menu ---\n";
    cout << "1. Dijkstra (Weighted)\n";
    cout << "2. BFS on allowed subset\n";
    cout << "3. Run both\n";
    cout << "Choose: ";
    cin >> ch;

    if (ch == 1 || ch == 3) {
        auto R = dijkstra(g, src);
        cout << "\n================ Dijkstra Result ================\n";
        printTable(R, src);
    }

    if (ch == 2 || ch == 3) {
        vector<bool> allowed(g.V, true);
        cout << "\nEnter number of restricted zones: ";
        int r; cin >> r;
        cout << "Enter restricted zone IDs: ";
        for (int i = 0; i < r; i++) {
            int z; cin >> z;
            allowed[z] = false;
        }

        auto R2 = bfs_unweighted(g, allowed, src);
        cout << "\n================ BFS Result ================\n";
        printTable(R2, src);
    }

    return 0;
}
