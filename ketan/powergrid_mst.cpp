#include <bits/stdc++.h>
using namespace std;

struct AdjEdge {
    int to;
    double w;
};

struct MSTEdge {
    int u, v;
    double w;
};

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
        if (u < 0 || v < 0 || u >= n || v >= n) return;
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
    }

    void labelNode(int i, const string &s) {
        if (i >= 0 && i < n) name[i] = s;
    }
};

// DFS to check connectivity (MST needs connected graph)
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

// Prim's MST with priority queue (min-heap)
pair<vector<MSTEdge>, double> primMST(const PowerGrid &g, int start) {
    int n = g.n;
    vector<int> used(n, 0);
    vector<MSTEdge> mstEdges;
    double total = 0.0;

    using T = pair<double, pair<int,int>>; // (weight,(u,v))
    priority_queue<T, vector<T>, greater<T>> pq;

    used[start] = 1;
    for (auto &e : g.adj[start])
        pq.push({e.w, {start, e.to}});

    while (!pq.empty() && mstEdges.size() < n - 1) {
        auto top = pq.top(); pq.pop();
        double w = top.first;
        int u = top.second.first;
        int v = top.second.second;

        if (used[v]) continue;
        used[v] = 1;
        mstEdges.push_back({u, v, w});
        total += w;

        for (auto &nx : g.adj[v])
            if (!used[nx.to])
                pq.push({nx.w, {v, nx.to}});
    }
    return {mstEdges, total};
}

// Print MST result neatly
void printMST(const vector<MSTEdge> &mst, double total,
              const PowerGrid &g) {
    cout << "\n--- Recommended Power Backbone (MST) ---\n";
    cout << left << setw(8) << "From"
         << setw(8) << "To"
         << setw(12) << "Cost/Loss\n";
    cout << string(30,'-') << "\n";

    for (auto &e : mst) {
        cout << setw(8) << e.u;
        cout << setw(8) << e.v;
        cout << setw(12) << fixed << setprecision(2) << e.w;
        cout << "\n";
    }
    cout << "\nTotal wiring cost + loss: " 
         << total << "\n\n";
}

// Example sample grid
void buildSampleGrid(PowerGrid &g, int &start) {
    /*
         0 (Main Substation)
        /  \
     10    12
      /      \
     1        2
    / \      / \
   4   6    7   5
    */

    g.init(6);
    g.labelNode(0,"Main Substation");
    g.labelNode(1,"New Colony A");
    g.labelNode(2,"New Colony B");
    g.labelNode(3,"Transformer A1");
    g.labelNode(4,"Transformer A2");
    g.labelNode(5,"Transformer B1");

    g.addEdge(0,1,10.0);
    g.addEdge(0,2,12.0);
    g.addEdge(1,3,4.0);
    g.addEdge(1,4,6.0);
    g.addEdge(2,5,7.5);

    start = 0;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    PowerGrid g;
    int start = 0;

    cout << "Problem 6: Power Grid Load Minimization\n\n";
    cout << "1. Use sample power grid\n";
    cout << "2. Enter custom grid\n";
    cout << "Choice: ";
    int mode;
    cin >> mode;

    if (mode == 2) {
        int nodes, edges;
        cout << "\nEnter number of nodes: ";
        cin >> nodes;
        g.init(nodes);

        cout << "Enter label for each node:\n";
        for (int i = 0; i < nodes; i++) {
            string s;
            cout << "Node " << i << ": ";
            cin.ignore();
            getline(cin, s);
            g.labelNode(i, s);
        }

        cout << "\nEnter number of cables: ";
        cin >> edges;
        cout << "Enter: u v cost\n";
        for (int i = 0; i < edges; i++) {
            int u,v; double w;
            cin >> u >> v >> w;
            g.addEdge(u,v,w);
        }

        cout << "\nEnter start node index: ";
        cin >> start;

    } else {
        buildSampleGrid(g, start);
        cout << "\nUsing sample power grid...\n\n";
    }

    if (!isConnected(g)) {
        cout << "\nERROR: Graph is not fully connected.\n";
        cout << "Cannot compute MST until network connections are complete.\n";
        return 0;
    }

    auto [mst, total] = primMST(g, start);

    printMST(mst, total, g);

    cout << "Time Complexity: O(E log V)\n";
    cout << "All colonies are connected using minimum wiring cost.\n";

    return 0;
}
