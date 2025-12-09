#include <bits/stdc++.h>
using namespace std;

struct Edge {
    int u;
    int v;
};

// ---------------------- Input & Sample Graph ---------------------- //
void buildSampleGraph(vector<vector<int>> &adj, vector<string> &names, vector<Edge> &edges) {
    int n = 7;
    adj.assign(n, {});
    names.clear();
    names = {
        "Central Hub",      // 0
        "River Side",       // 1
        "Old Town",         // 2
        "Tech Park",        // 3
        "Industrial Zone",  // 4
        "Airport Link",     // 5
        "New Suburb"        // 6
    };

    auto addEdge = [&](int a, int b) {
        adj[a].push_back(b);
        adj[b].push_back(a);
        edges.push_back({a, b});
    };

    addEdge(0, 1);
    addEdge(1, 2);
    addEdge(2, 0);    // 0-1-2 forms a cycle (not bridges)

    addEdge(1, 3);    // this might be a bridge
    addEdge(3, 4);
    addEdge(4, 5);
    addEdge(5, 3);    // 3-4-5 forms a cycle (internal)

    addEdge(4, 6);    // critical link to New Suburb (likely bridge)
}

// ---------------------- DFS-based Bridge Detection ---------------------- //
struct BridgeInfo {
    int u, v;
    int rank;
};

void dfsBridge(
    int u,
    int parent,
    const vector<vector<int>> &adj,
    vector<int> &disc,
    vector<int> &low,
    int &timer,
    vector<Edge> &bridges
) {
    disc[u] = low[u] = ++timer;

    for (int v : adj[u]) {
        if (v == parent) continue;

        if (disc[v] == 0) {
            dfsBridge(v, u, adj, disc, low, timer, bridges);
            low[u] = min(low[u], low[v]);

            if (low[v] > disc[u]) {
                bridges.push_back({u, v});
            }
        } else {
            low[u] = min(low[u], disc[v]);
        }
    }
}

vector<Edge> findBridges(const vector<vector<int>> &adj) {
    int n = (int)adj.size();
    vector<int> disc(n, 0), low(n, 0);
    int timer = 0;
    vector<Edge> bridges;

    for (int i = 0; i < n; i++) {
        if (disc[i] == 0) {
            dfsBridge(i, -1, adj, disc, low, timer, bridges);
        }
    }
    return bridges;
}

// ---------------------- Criticality Scoring ---------------------- //
void buildComponent(
    int start,
    const vector<vector<int>> &adj,
    vector<int> &compId,
    int compIndex,
    vector<int> &compSize
) {
    int n = (int)adj.size();
    queue<int> q;
    q.push(start);
    compId[start] = compIndex;
    int countNodes = 0;

    while (!q.empty()) {
        int u = q.front(); q.pop();
        countNodes++;
        for (int v : adj[u]) {
            if (compId[v] == -1) {
                compId[v] = compIndex;
                q.push(v);
            }
        }
    }
    if ((int)compSize.size() <= compIndex) compSize.resize(compIndex + 1, 0);
    compSize[compIndex] = countNodes;
}

vector<int> computeComponentSizes(const vector<vector<int>> &adj) {
    int n = (int)adj.size();
    vector<int> compId(n, -1);
    vector<int> compSize;
    int compIndex = 0;

    for (int i = 0; i < n; i++) {
        if (compId[i] == -1) {
            buildComponent(i, adj, compId, compIndex, compSize);
            compIndex++;
        }
    }

    vector<int> nodeSize(n, 0);
    for (int i = 0; i < n; i++) {
        if (compId[i] >= 0 && compId[i] < (int)compSize.size()) {
            nodeSize[i] = compSize[compId[i]];
        }
    }
    return nodeSize;
}

vector<BridgeInfo> scoreBridges(
    const vector<Edge> &bridges,
    const vector<int> &componentSize,
    const vector<string> &names
) {
    vector<BridgeInfo> scored;
    for (const auto &e : bridges) {
        int u = e.u;
        int v = e.v;
        int score = componentSize[u] + componentSize[v];
        scored.push_back({u, v, score});
    }
    sort(scored.begin(), scored.end(), [](const BridgeInfo &a, const BridgeInfo &b) {
        if (a.rank != b.rank) return a.rank > b.rank;
        if (a.u != b.u) return a.u < b.u;
        return a.v < b.v;
    });
    return scored;
}

// ---------------------- Formatting Helpers ---------------------- //
void printAdjacency(const vector<vector<int>> &adj, const vector<string> &names) {
    cout << "\nAdjacency List of Road Network:\n";
    for (int i = 0; i < (int)adj.size(); i++) {
        cout << "Node " << i;
        if (i < (int)names.size()) cout << " (" << names[i] << ")";
        cout << " -> ";
        for (int j = 0; j < (int)adj[i].size(); j++) {
            int v = adj[i][j];
            cout << v;
            if (v < (int)names.size()) cout << "(" << names[v] << ")";
            if (j + 1 < (int)adj[i].size()) cout << ", ";
        }
        cout << "\n";
    }
    cout << "\n";
}

void printBridgesRaw(const vector<Edge> &bridges, const vector<string> &names) {
    if (bridges.empty()) {
        cout << "No critical bridges found in this network (graph is 2-edge-connected).\n";
        return;
    }

    cout << "Bridges (Critical Links):\n";
    for (const auto &e : bridges) {
        cout << "Edge " << e.u << " - " << e.v;
        if (e.u < (int)names.size() && e.v < (int)names.size()) {
            cout << "  [" << names[e.u] << " <-> " << names[e.v] << "]";
        }
        cout << "\n";
    }
    cout << "\n";
}

void printScoredBridges(const vector<BridgeInfo> &scored, const vector<string> &names) {
    if (scored.empty()) return;

    cout << "Ranked Bridge Priority List:\n";
    cout << left << setw(6) << "Rank"
         << setw(12) << "Nodes"
         << "Description\n";
    cout << string(60, '-') << "\n";

    for (int i = 0; i < (int)scored.size(); i++) {
        const auto &b = scored[i];
        string n1 = (b.u < (int)names.size()) ? names[b.u] : ("Node" + to_string(b.u));
        string n2 = (b.v < (int)names.size()) ? names[b.v] : ("Node" + to_string(b.v));
        cout << left << setw(6) << (i + 1)
             << setw(12) << (to_string(b.u) + "-" + to_string(b.v))
             << n1 << "  <-->  " << n2
             << "   | priority score ≈ " << b.rank << "\n";
    }
    cout << "\n";
}

// ---------------------- Custom Input ---------------------- //
void readCustomGraph(
    vector<vector<int>> &adj,
    vector<string> &names,
    vector<Edge> &edges
) {
    int n, m;
    cout << "\nEnter number of junctions (nodes): ";
    cin >> n;
    cout << "Enter number of roads (edges): ";
    cin >> m;

    adj.assign(n, {});
    edges.clear();

    cout << "\nDo you want to give custom names for each junction? (1 = yes, 0 = no): ";
    int nameChoice;
    cin >> nameChoice;

    names.assign(n, "");
    if (nameChoice == 1) {
        cout << "Enter " << n << " names, one per line:\n";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        for (int i = 0; i < n; i++) {
            string s;
            getline(cin, s);
            if (s.empty()) {
                s = "Node-" + to_string(i);
            }
            names[i] = s;
        }
    } else {
        for (int i = 0; i < n; i++) {
            names[i] = "Node-" + to_string(i);
        }
    }

    cout << "\nEnter " << m << " undirected edges as: u v (0-based indices)\n";
    cout << "Example: 0 1   means junction 0 is connected to junction 1.\n\n";

    for (int i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;
        if (u < 0 || v < 0 || u >= n || v >= n || u == v) {
            cout << "Invalid edge (" << u << ", " << v << "), skipping.\n";
            continue;
        }
        adj[u].push_back(v);
        adj[v].push_back(u);
        edges.push_back({u, v});
    }
}

// ---------------------- MAIN ---------------------- //
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "Problem 9: Bridge Construction Priority Model (DFS Bridges)\n\n";
    cout << "1. Use sample city network\n";
    cout << "2. Enter custom road network\n";
    cout << "Choice: ";

    int mode;
    cin >> mode;

    vector<vector<int>> adj;
    vector<string> names;
    vector<Edge> edges;

    if (mode == 2) {
        readCustomGraph(adj, names, edges);
    } else {
        buildSampleGraph(adj, names, edges);
        cout << "\nUsing sample city network with 7 junctions.\n";
    }

    printAdjacency(adj, names);

    vector<Edge> bridges = findBridges(adj);
    printBridgesRaw(bridges, names);

    if (bridges.empty()) {
        cout << "\nNo bridges, so all links have alternatives.\n";
        cout << "Time Complexity: O(V + E)\n";
        return 0;
    }

    vector<int> compSizes = computeComponentSizes(adj);
    vector<BridgeInfo> scored = scoreBridges(bridges, compSizes, names);
    printScoredBridges(scored, names);

    cout << "Time Complexity Details:\n";
    cout << "- DFS bridge detection : O(V + E)\n";
    cout << "- Component size computation (BFS/DFS) : O(V + E)\n";
    cout << "- Sorting bridges by priority : O(B log B), B = number of bridges\n";
    cout << "Overall : O(V + E + B log B)  (dominated by O(V + E) for sparse graphs)\n";

    return 0;
}
