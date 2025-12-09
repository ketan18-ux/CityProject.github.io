#include <bits/stdc++.h>
using namespace std;

struct Edge {
    int to;
    int timeCost; // minutes
};

class EmergencyGraph {
public:
    int V;
    vector<vector<Edge>> adj;
    set<pair<int,int>> blocked;

    EmergencyGraph(int n = 0) {
        init(n);
    }

    void init(int n) {
        V = n;
        adj.assign(V, {});
        blocked.clear();
    }

    void addRoad(int u, int v, int t) {
        adj[u].push_back({v, t});
        adj[v].push_back({u, t});
    }

    void blockRoad(int u, int v) {
        blocked.insert({u, v});
        blocked.insert({v, u});
    }

    bool isBlocked(int u, int v) const {
        return blocked.count({u, v}) > 0;
    }
};

struct BFSResult {
    vector<int> distHop;
    vector<int> parent;
};

void dfsReach(const EmergencyGraph &g, int u,
              vector<bool> &vis) {
    vis[u] = true;
    for (auto &e : g.adj[u]) {
        if (g.isBlocked(u, e.to)) continue;
        if (!vis[e.to]) dfsReach(g, e.to, vis);
    }
}

BFSResult bfsShortest(const EmergencyGraph &g, int src) {
    BFSResult res;
    res.distHop.assign(g.V, -1);
    res.parent.assign(g.V, -1);

    queue<int> q;
    res.distHop[src] = 0;
    q.push(src);

    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (auto &e : g.adj[u]) {
            int v = e.to;
            if (g.isBlocked(u, v)) continue;
            if (res.distHop[v] == -1) {
                res.distHop[v] = res.distHop[u] + 1;
                res.parent[v] = u;
                q.push(v);
            }
        }
    }
    return res;
}

vector<int> reconstructPath(int dest, const BFSResult &res) {
    vector<int> path;
    if (dest < 0 || dest >= (int)res.distHop.size()) return path;
    if (res.distHop[dest] == -1) return path;
    for (int v = dest; v != -1; v = res.parent[v])
        path.push_back(v);
    reverse(path.begin(), path.end());
    return path;
}

int computeTimeAlongPath(const EmergencyGraph &g,
                         const vector<int> &path) {
    if (path.size() < 2) return 0;
    int total = 0;
    for (int i = 0; i + 1 < (int)path.size(); i++) {
        int u = path[i];
        int v = path[i+1];
        bool found = false;
        for (auto &e : g.adj[u]) {
            if (e.to == v) {
                total += e.timeCost;
                found = true;
                break;
            }
        }
        if (!found) return -1;
    }
    return total;
}

void printAdj(const EmergencyGraph &g) {
    cout << "\nRoad Network (unblocked + blocked info):\n";
    for (int u = 0; u < g.V; u++) {
        cout << "Node " << u << " -> ";
        for (auto &e : g.adj[u]) {
            cout << "(" << e.to << ", " << e.timeCost << "min";
            if (g.isBlocked(u, e.to)) cout << ", BLOCKED";
            cout << ") ";
        }
        cout << "\n";
    }
    cout << "\n";
}

bool loadFromFile(const string &fname,
                  EmergencyGraph &g,
                  int &src,
                  int &dst) {
    ifstream fin(fname);
    if (!fin.is_open()) return false;

    int V, E;
    fin >> V >> E;
    g.init(V);

    for (int i = 0; i < E; i++) {
        int u, v, t;
        fin >> u >> v >> t;
        g.addRoad(u, v, t);
    }

    int B;
    fin >> B;
    for (int i = 0; i < B; i++) {
        int u, v;
        fin >> u >> v;
        g.blockRoad(u, v);
    }

    fin >> src >> dst;
    return true;
}

void loadDemo(EmergencyGraph &g, int &src, int &dst) {
    g.init(6);
    g.addRoad(0, 1, 4);
    g.addRoad(1, 2, 5);
    g.addRoad(0, 3, 10);
    g.addRoad(3, 2, 3);
    g.addRoad(2, 4, 4);
    g.addRoad(4, 5, 6);
    g.addRoad(1, 5, 15);

    g.blockRoad(1, 2);
    g.blockRoad(3, 2);

    src = 0;
    dst = 5;
}

void printPathVec(const vector<int> &path) {
    for (int i = 0; i < (int)path.size(); i++) {
        cout << path[i];
        if (i + 1 < (int)path.size()) cout << " -> ";
    }
}

int main() {
    EmergencyGraph g;
    int src, dst;

    cout << "SMART CITY — EMERGENCY ROUTE PLANNER\n";
    cout << "-------------------------------------\n";
    cout << "1. Load from file (p4_emergency_input.txt)\n";
    cout << "2. Use built-in demo network\n";
    cout << "Choose option: ";
    int ch;
    cin >> ch;

    if (ch == 1) {
        if (!loadFromFile("p4_emergency_input.txt", g, src, dst)) {
            cout << "p4_emergency_input.txt not found\n";
            return 0;
        }
    } else {
        loadDemo(g, src, dst);
    }

    cout << "\nStation node: " << src << "\n";
    cout << "Incident node: " << dst << "\n";
    printAdj(g);

    vector<bool> vis(g.V, false);
    dfsReach(g, src, vis);

    if (!vis[dst]) {
        cout << "No feasible route: incident is disconnected due to blocks.\n";
        return 0;
    }

    auto bfsRes = bfsShortest(g, src);
    vector<int> path = reconstructPath(dst, bfsRes);

    if (path.empty()) {
        cout << "No path found by BFS.\n";
        return 0;
    }

    int hops = bfsRes.distHop[dst];
    int timeEst = computeTimeAlongPath(g, path);

    cout << "\n================ EMERGENCY ROUTE RESULT ================\n";
    cout << "Shortest path in terms of hops:\n";
    printPathVec(path);
    cout << "\n";

    cout << "Number of road segments (hops): " << hops << "\n";
    if (timeEst >= 0)
        cout << "Estimated travel time (sum of edge times): "
             << timeEst << " minutes\n";
    else
        cout << "Warning: could not compute time along path.\n";

    cout << "\nNode-wise details:\n";
    cout << "Index  Node\n";
    for (int i = 0; i < (int)path.size(); i++) {
        cout << setw(5) << i << "  " << path[i] << "\n";
    }

    cout << "\nRoute planning complete.\n";
    return 0;
}
