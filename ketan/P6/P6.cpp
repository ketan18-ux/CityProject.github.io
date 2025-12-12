#include <bits/stdc++.h>
using namespace std;

struct AdjEdge { int to; double w; };
struct MSTEdge { int u, v; double w; };

struct PowerGrid {
    int n;
    vector<vector<AdjEdge>> adj;
    vector<string> name;

    PowerGrid(int nodes = 0) { init(nodes); }

    void init(int nodes) {
        n = nodes;
        adj.assign(n, {});
        name.assign(n, "");
    }

    void addEdge(int u, int v, double w) {
        if (u<0 || v<0 || u>=n || v>=n) return;
        adj[u].push_back({v,w});
        adj[v].push_back({u,w});
    }
};

// ---------------- Connectivity Check (DFS) ---------------- //
void dfsConn(int u, const PowerGrid &g, vector<int> &vis) {
    vis[u] = 1;
    for (auto &e : g.adj[u])
        if (!vis[e.to]) dfsConn(e.to, g, vis);
}

bool isConnected(const PowerGrid &g) {
    if (g.n == 0) return true;
    vector<int> vis(g.n, 0);
    dfsConn(0, g, vis);
    for (int i = 0; i < g.n; i++)
        if (!vis[i]) return false;
    return true;
}

// ---------------- Prim's MST with Min-Heap ---------------- //
pair<vector<MSTEdge>, double> primMST(const PowerGrid &g, int start) {
    int n = g.n;
    vector<int> used(n, 0);
    vector<MSTEdge> mst;
    double total = 0;

    using T = pair<double, pair<int,int>>; // (weight,(u,v))
    priority_queue<T, vector<T>, greater<T>> pq;

    used[start] = 1;
    for (auto &e : g.adj[start])
        pq.push({e.w,{start,e.to}});

    while (!pq.empty() && mst.size() < n - 1) {
        auto top = pq.top(); pq.pop();
        double w = top.first;
        int u = top.second.first;
        int v = top.second.second;

        if (used[v]) continue;

        used[v] = 1;
        mst.push_back({u,v,w});
        total += w;

        for (auto &nx : g.adj[v])
            if (!used[nx.to])
                pq.push({nx.w,{v,nx.to}});
    }

    return {mst,total};
}

// ---------------- Print MST ---------------- //
void printMST(const vector<MSTEdge> &mst, double total) {
    cout << "\n--- Minimum Cost Power Backbone (MST) ---\n";
    cout << left << setw(8) << "From"
         << setw(8) << "To"
         << "Cost\n";
    cout << string(30,'-') << "\n";

    for (auto &e : mst) {
        cout << setw(8) << e.u
             << setw(8) << e.v
             << fixed << setprecision(2) << e.w << "\n";
    }

    cout << "\nTotal MST Cost: " << total << "\n\n";
}

// ---------------- CSV Loaders ---------------- //
bool loadNodesCSV(PowerGrid &g) {
    ifstream fin("grid_nodes.csv");
    if (!fin.is_open()) return false;

    vector<string> names;
    string line;
    getline(fin, line); // header

    while (getline(fin, line)) {
        size_t pos = line.find(',');
        if (pos == string::npos) continue;
        string lbl = line.substr(pos + 1);
        names.push_back(lbl);
    }

    g.init(names.size());
    g.name = names;
    return true;
}

bool loadEdgesCSV(PowerGrid &g) {
    ifstream fin("grid_edges.csv");
    if (!fin.is_open()) return false;

    string line;
    getline(fin,line);

    while (getline(fin, line)) {
        stringstream ss(line);
        string a,b,c;
        getline(ss,a,',');
        getline(ss,b,',');
        getline(ss,c,',');

        int u = stoi(a);
        int v = stoi(b);
        double w = stod(c);

        g.addEdge(u,v,w);
    }
    return true;
}

// ---------------- MAIN ---------------- //
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "Problem 6 — Power Grid Load Minimization (CSV Enabled)\n\n";

    PowerGrid g;
    bool ok1 = loadNodesCSV(g);
    bool ok2 = loadEdgesCSV(g);

    int start = 0;

    if (!ok1 || !ok2) {
        cout << "❌ CSV files not found! Please add grid_nodes.csv and grid_edges.csv.\n";
        return 0;
    }

    cout << "Loaded " << g.n << " nodes.\n";

    cout << "Enter starting node index (substation): ";
    cin >> start;

    if (!isConnected(g)) {
        cout << "\nERROR: Graph is not fully connected.\n";
        return 0;
    }

    auto [mst, total] = primMST(g, start);

    printMST(mst, total);

    cout << "Time Complexity: O(E log V)\n";
    return 0;
}
