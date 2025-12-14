#include <bits/stdc++.h>
using namespace std;

// ---------------- Edge Structure ---------------- //
struct Edge {
    int u, v, w;
};

// ---------------- Disjoint Set Union ---------------- //
class DSU {
    vector<int> parent, rnk;
public:
    DSU(int n = 0) { init(n); }

    void init(int n) {
        parent.resize(n);
        rnk.assign(n, 0);
        for (int i = 0; i < n; i++)
            parent[i] = i;
    }

    int findSet(int v) {
        if (v == parent[v])
            return v;
        return parent[v] = findSet(parent[v]);
    }

    bool unionSet(int a, int b) {
        a = findSet(a);
        b = findSet(b);
        if (a == b) return false;

        if (rnk[a] < rnk[b]) swap(a, b);
        parent[b] = a;
        if (rnk[a] == rnk[b]) rnk[a]++;
        return true;
    }
};

// ---------------- MST Result ---------------- //
struct MSTResult {
    int totalCost = 0;
    vector<Edge> edges;
};

// ---------------- Kruskal Algorithm ---------------- //
MSTResult kruskalMST(int V, vector<Edge> edges) {
    MSTResult res;

    sort(edges.begin(), edges.end(),
         [](Edge &a, Edge &b) {
             return a.w < b.w;
         });

    DSU dsu(V);

    for (auto &e : edges) {
        if (dsu.unionSet(e.u, e.v)) {
            res.edges.push_back(e);
            res.totalCost += e.w;
            if ((int)res.edges.size() == V - 1)
                break;
        }
    }
    return res;
}

// ---------------- Prim Algorithm ---------------- //
MSTResult primMST(int V, const vector<vector<pair<int,int>>> &adj) {
    MSTResult res;

    vector<bool> inMST(V, false);
    vector<int> key(V, INT_MAX), parent(V, -1);

    priority_queue<pair<int,int>,
        vector<pair<int,int>>,
        greater<pair<int,int>>> pq;

    key[0] = 0;
    pq.push({0, 0});

    while (!pq.empty()) {
        auto [w, u] = pq.top();
        pq.pop();

        if (inMST[u]) continue;

        inMST[u] = true;
        res.totalCost += w;

        if (parent[u] != -1)
            res.edges.push_back({parent[u], u, w});

        for (auto &[v, wt] : adj[u]) {
            if (!inMST[v] && wt < key[v]) {
                key[v] = wt;
                parent[v] = u;
                pq.push({wt, v});
            }
        }
    }
    return res;
}

// ---------------- Main Function ---------------- //
int main() {
    ifstream fin("P1_input.csv");
    if (!fin.is_open()) {
        cout << "P1_input.csv not found\n";
        return 0;
    }

    string line;
    vector<Edge> edges;
    int maxNode = -1;

    // Skip header: u,v,w
    getline(fin, line);

    // Read CSV rows
    while (getline(fin, line)) {
        stringstream ss(line);
        Edge e;
        char comma;

        ss >> e.u >> comma >> e.v >> comma >> e.w;
        edges.push_back(e);

        maxNode = max(maxNode, max(e.u, e.v));
    }
    fin.close();

    int V = maxNode + 1;

    // Build adjacency list
    vector<vector<pair<int,int>>> adj(V);
    for (auto &e : edges) {
        adj[e.u].push_back({e.v, e.w});
        adj[e.v].push_back({e.u, e.w});
    }

    // Run MST Algorithms
    MSTResult K = kruskalMST(V, edges);
    MSTResult P = primMST(V, adj);

    // Output Results
    cout << "Kruskal MST\n";
    cout << "Total Cost: " << K.totalCost << "\n";
    for (auto &e : K.edges)
        cout << e.u << " -- " << e.v << " (" << e.w << ")\n";

    cout << "\nPrim MST\n";
    cout << "Total Cost: " << P.totalCost << "\n";
    for (auto &e : P.edges)
        cout << e.u << " -- " << e.v << " (" << e.w << ")\n";

    cout << "\n";
    if (K.totalCost == P.totalCost)
        cout << "Costs Match ✔\n";
    else
        cout << "Cost Mismatch ⚠\n";

    return 0;
}
