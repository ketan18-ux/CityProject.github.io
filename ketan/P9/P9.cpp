#include <bits/stdc++.h>
using namespace std;

struct Edge { int u, v; };
struct BridgeInfo { int u, v, rank; };

// ---------------- CSV Loaders ---------------- //
bool loadNodesCSV(vector<string> &names) {
    ifstream fin("bridge_nodes.csv");
    if (!fin.is_open()) return false;

    names.clear();
    string line;
    getline(fin, line); // header
    while (getline(fin, line)) {
        size_t pos = line.find(',');
        if (pos == string::npos) continue;
        names.push_back(line.substr(pos + 1));
    }
    return true;
}

bool loadEdgesCSV(vector<vector<int>> &adj, vector<Edge> &edges, int n) {
    ifstream fin("bridge_edges.csv");
    if (!fin.is_open()) return false;

    adj.assign(n, {});
    edges.clear();

    string line;
    getline(fin, line); // header

    while (getline(fin, line)) {
        string a, b;
        stringstream ss(line);
        getline(ss, a, ',');
        getline(ss, b, ',');

        int u = stoi(a);
        int v = stoi(b);

        adj[u].push_back(v);
        adj[v].push_back(u);
        edges.push_back({u, v});
    }
    return true;
}

// ---------------- DFS Bridge Detection ---------------- //
void dfsBridge(int u, int parent, const vector<vector<int>> &adj,
               vector<int> &disc, vector<int> &low, int &timer,
               vector<Edge> &bridges)
{
    disc[u] = low[u] = ++timer;

    for (int v : adj[u]) {
        if (v == parent) continue;

        if (disc[v] == 0) {
            dfsBridge(v, u, adj, disc, low, timer, bridges);
            low[u] = min(low[u], low[v]);

            if (low[v] > disc[u])
                bridges.push_back({u, v});
        }
        else {
            low[u] = min(low[u], disc[v]);
        }
    }
}

vector<Edge> findBridges(const vector<vector<int>> &adj) {
    int n = adj.size();
    vector<int> disc(n, 0), low(n, 0);
    vector<Edge> bridges;
    int timer = 0;

    for (int i = 0; i < n; i++)
        if (disc[i] == 0)
            dfsBridge(i, -1, adj, disc, low, timer, bridges);

    return bridges;
}

// ---------------- Component Size Computation ---------------- //
void bfsComponent(int start, const vector<vector<int>> &adj,
                  vector<int> &compId, int compIndex, vector<int> &compSize)
{
    queue<int> q;
    q.push(start);
    compId[start] = compIndex;
    int count = 0;

    while (!q.empty()) {
        int u = q.front(); q.pop();
        count++;
        for (int v : adj[u]) {
            if (compId[v] == -1) {
                compId[v] = compIndex;
                q.push(v);
            }
        }
    }
    if (compIndex >= compSize.size()) compSize.resize(compIndex + 1);
    compSize[compIndex] = count;
}

vector<int> computeComponentSizes(const vector<vector<int>> &adj) {
    int n = adj.size();
    vector<int> compId(n, -1);
    vector<int> compSize;
    int compIndex = 0;

    for (int i = 0; i < n; i++)
        if (compId[i] == -1)
            bfsComponent(i, adj, compId, compIndex++, compSize);

    vector<int> nodeSize(n);
    for (int i = 0; i < n; i++)
        nodeSize[i] = compSize[compId[i]];

    return nodeSize;
}

// ---------------- Scoring ---------------- //
vector<BridgeInfo> scoreBridges(const vector<Edge> &bridges,
                                const vector<int> &compSize) {

    vector<BridgeInfo> scored;
    for (auto &e : bridges) {
        int score = compSize[e.u] + compSize[e.v];
        scored.push_back({e.u, e.v, score});
    }

    sort(scored.begin(), scored.end(),
        [](auto &a, auto &b){ return a.rank > b.rank; });

    return scored;
}

// ---------------- Printing ---------------- //
void printAdj(const vector<vector<int>> &adj, const vector<string> &names) {
    cout << "\nAdjacency List:\n";
    for (int i = 0; i < adj.size(); i++) {
        cout << i << " (" << names[i] << ") -> ";
        for (int j = 0; j < adj[i].size(); j++) {
            cout << adj[i][j];
            if (j + 1 < adj[i].size()) cout << ", ";
        }
        cout << "\n";
    }
    cout << "\n";
}

void printBridges(const vector<Edge> &bridges, const vector<string> &names) {
    cout << "\nCritical Bridges Found:\n";
    for (auto &e : bridges)
        cout << e.u << "-" << e.v << "  [" << names[e.u] << " <-> " << names[e.v] << "]\n";
    cout << "\n";
}

void printScores(const vector<BridgeInfo> &S, const vector<string> &names) {
    cout << "Ranked Bridge Priority List:\n";
    int r = 1;
    for (auto &b : S) {
        cout << r++ << ") " << names[b.u] << " <-> " << names[b.v]
             << "  | score = " << b.rank << "\n";
    }
    cout << "\n";
}

// ---------------- MAIN ---------------- //
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "Problem 9 — Bridge Priority Model (CSV Enabled)\n\n";

    vector<string> names;
    if (!loadNodesCSV(names)) {
        cout << "❌ Missing bridge_nodes.csv\n";
        return 0;
    }

    vector<vector<int>> adj;
    vector<Edge> edges;

    if (!loadEdgesCSV(adj, edges, names.size())) {
        cout << "❌ Missing bridge_edges.csv\n";
        return 0;
    }

    printAdj(adj, names);

    auto bridges = findBridges(adj);
    if (bridges.empty()) {
        cout << "No bridges found — network is 2-edge-connected.\n";
        return 0;
    }

    printBridges(bridges, names);

    auto compSizes = computeComponentSizes(adj);
    auto ranked = scoreBridges(bridges, compSizes);

    printScores(ranked, names);

    cout << "Time Complexity: O(V + E + B log B)\n";
    return 0;
}
