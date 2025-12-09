#include <bits/stdc++.h>
using namespace std;

struct Edge {
    int u, v, w;
};

class DSU {
    vector<int> parent, rnk;
public:
    DSU(int n = 0) { init(n); }
    void init(int n) {
        parent.resize(n);
        rnk.assign(n, 0);
        for (int i = 0; i < n; i++) parent[i] = i;
    }
    int findSet(int v) {
        if (v == parent[v]) return v;
        return parent[v] = findSet(parent[v]);
    }
    bool unionSet(int a, int b) {
        a = findSet(a); b = findSet(b);
        if (a == b) return false;
        if (rnk[a] < rnk[b]) swap(a, b);
        parent[b] = a;
        if (rnk[a] == rnk[b]) rnk[a]++;
        return true;
    }
};

struct MSTResult {
    int totalCost = 0;
    vector<Edge> edges;
};

MSTResult kruskalMST(int V, vector<Edge> edges) {
    MSTResult res;
    sort(edges.begin(), edges.end(), [](auto &a, auto &b) { return a.w < b.w; });
    DSU dsu(V);

    for (auto &e : edges) {
        if (dsu.unionSet(e.u, e.v)) {
            res.edges.push_back(e);
            res.totalCost += e.w;
            if ((int)res.edges.size() == V - 1) break;
        }
    }
    return res;
}

MSTResult primMST(int V, const vector<vector<pair<int,int>>> &adj) {
    MSTResult res;
    vector<bool> inMST(V, false);
    vector<int> key(V, INT_MAX), parent(V, -1);
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;

    key[0] = 0;
    pq.push({0, 0});

    while (!pq.empty()) {
        auto [curW, u] = pq.top(); pq.pop();
        if (inMST[u]) continue;
        inMST[u] = true;
        res.totalCost += curW;

        if (parent[u] != -1)
            res.edges.push_back({parent[u], u, curW});

        for (auto &nbr : adj[u]) {
            int v = nbr.first, w = nbr.second;
            if (!inMST[v] && w < key[v]) {
                key[v] = w;
                parent[v] = u;
                pq.push({w, v});
            }
        }
    }
    return res;
}

void printMST(string title, MSTResult &mst) {
    cout << "\n" << title << ":\n";
    cout << "Total Cost: " << mst.totalCost << "\n";
    for (auto &e : mst.edges)
        cout << e.u << " -- " << e.v << "  (Cost " << e.w << ")\n";
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    ifstream fin("road_network.txt");
    if (!fin.is_open()) {
        cout << "Input file road_network.txt not found!\n";
        return 0;
    }

    int V, E;
    fin >> V >> E;
    vector<Edge> edges(E);
    for (int i = 0; i < E; i++) fin >> edges[i].u >> edges[i].v >> edges[i].w;
    fin.close();

    vector<vector<pair<int,int>>> adj(V);
    for (auto &e : edges) {
        adj[e.u].push_back({e.v, e.w});
        adj[e.v].push_back({e.u, e.w});
    }

    MSTResult K = kruskalMST(V, edges);
    MSTResult P = primMST(V, adj);

    printMST("Kruskal MST", K);
    printMST("Prim MST", P);

    cout << "\nComparison Result:\n";
    if (K.totalCost == P.totalCost)
        cout << "✔ Both MST costs match. Implementation correct.\n";
    else
        cout << "⚠ Cost mismatch. Check graph data.\n";

    return 0;
}
