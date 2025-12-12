#include <bits/stdc++.h>
using namespace std;

const long long INF = 1e18;

// -----------------------------------------------------------
// Structures
// -----------------------------------------------------------
struct WeightedEdge { int to; int timeCost; };
struct UnweightedEdge { int to; };

class CityGraph {
public:
    int n;
    vector<vector<WeightedEdge>> timeGraph;
    vector<vector<UnweightedEdge>> simpleGraph;

    CityGraph(int nodes = 0) { init(nodes); }

    void init(int nodes) {
        n = nodes;
        timeGraph.assign(n, {});
        simpleGraph.assign(n, {});
    }

    void addTimeEdge(int u, int v, int t) {
        if (u<0 || v<0 || u>=n || v>=n) return;
        timeGraph[u].push_back({v, t});
        timeGraph[v].push_back({u, t});
    }

    void addSimpleEdge(int u, int v) {
        if (u<0 || v<0 || u>=n || v>=n) return;
        simpleGraph[u].push_back({v});
        simpleGraph[v].push_back({u});
    }
};

// -----------------------------------------------------------
// Dijkstra
// -----------------------------------------------------------
void dijkstraShortestTime(const CityGraph &g, int src,
                          vector<long long> &dist,
                          vector<int> &parent) {
    dist.assign(g.n, INF);
    parent.assign(g.n, -1);

    priority_queue<pair<long long,int>,
        vector<pair<long long,int>>,
        greater<pair<long long,int>>> pq;

    dist[src] = 0;
    pq.push({0, src});

    while (!pq.empty()) {
        auto [curDist, u] = pq.top();
        pq.pop();
        if (curDist > dist[u]) continue;

        for (auto &e : g.timeGraph[u]) {
            int v = e.to;
            long long w = e.timeCost;
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                parent[v] = u;
                pq.push({dist[v], v});
            }
        }
    }
}

// -----------------------------------------------------------
// BFS (unweighted)
// -----------------------------------------------------------
void bfsUnweighted(const CityGraph &g, int src,
                   vector<int> &dist,
                   vector<int> &parent) {
    dist.assign(g.n, -1);
    parent.assign(g.n, -1);

    queue<int> q;
    dist[src] = 0;
    q.push(src);

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        for (auto &e : g.simpleGraph[u]) {
            int v = e.to;
            if (dist[v] == -1) {
                dist[v] = dist[u] + 1;
                parent[v] = u;
                q.push(v);
            }
        }
    }
}

// -----------------------------------------------------------
// Path builder
// -----------------------------------------------------------
vector<int> reconstructPath(int target, const vector<int> &parent) {
    vector<int> path;
    for (int cur = target; cur != -1; cur = parent[cur])
        path.push_back(cur);
    reverse(path.begin(), path.end());
    return path;
}

void printPath(const vector<int> &path, const vector<string> &names) {
    for (size_t i = 0; i < path.size(); i++) {
        cout << names[path[i]];
        if (i + 1 != path.size()) cout << " -> ";
    }
    cout << "\n";
}

// -----------------------------------------------------------
// CSV Loader
// -----------------------------------------------------------
void loadZonesCSV(vector<string> &names) {
    ifstream fin("zones.csv");
    if (!fin.is_open()) {
        cout << "Error: zones.csv not found.\n";
        exit(0);
    }

    names.clear();
    string line;
    getline(fin, line); // skip header

    while (getline(fin, line)) {
        stringstream ss(line);
        string idStr, name;
        getline(ss, idStr, ',');
        getline(ss, name);

        names.push_back(name);
    }
}

void loadRoadsCSV(CityGraph &g) {
    ifstream fin("roads.csv");
    if (!fin.is_open()) {
        cout << "Error: roads.csv not found.\n";
        exit(0);
    }

    string line;
    getline(fin, line); // skip header

    while (getline(fin, line)) {
        stringstream ss(line);
        string uStr, vStr, tStr;

        getline(ss, uStr, ',');
        getline(ss, vStr, ',');
        getline(ss, tStr, ',');

        int u = stoi(uStr);
        int v = stoi(vStr);
        int t = stoi(tStr);

        g.addTimeEdge(u, v, t);
        g.addSimpleEdge(u, v);
    }
}

// -----------------------------------------------------------
// Display tables
// -----------------------------------------------------------
void printTimeTable(const vector<long long> &dist,
                    const vector<int> &parent,
                    const vector<string> &names) {
    cout << "\n=== Fastest Travel-Time Table ===\n\n";
    cout << left << setw(8) << "Zone"
         << setw(30) << "Name"
         << setw(10) << "Time"
         << "Path\n";
    cout << string(70, '-') << "\n";

    for (int i = 0; i < dist.size(); i++) {
        cout << left << setw(8) << i
             << setw(30) << names[i];

        if (dist[i] == INF) {
            cout << setw(10) << "INF" << "unreachable\n";
            continue;
        }

        cout << setw(10) << to_string(dist[i]) + "min";
        printPath(reconstructPath(i, parent), names);
    }
}

void printBFSTable(const vector<int> &dist,
                   const vector<int> &parent,
                   const vector<string> &names) {
    cout << "\n=== BFS Hop Count Table ===\n\n";
    cout << left << setw(8) << "Zone"
         << setw(30) << "Name"
         << setw(10) << "Hops"
         << "Path\n";
    cout << string(70, '-') << "\n";

    for (int i = 0; i < dist.size(); i++) {
        cout << left << setw(8) << i
             << setw(30) << names[i];

        if (dist[i] == -1) {
            cout << setw(10) << "INF" << "unreachable\n";
            continue;
        }

        cout << setw(10) << dist[i];
        printPath(reconstructPath(i, parent), names);
    }
}

// -----------------------------------------------------------
// MAIN
// -----------------------------------------------------------
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<string> names;
    loadZonesCSV(names);

    int n = names.size();
    CityGraph city(n);

    loadRoadsCSV(city);

    cout << "CSV Loaded Successfully.\n";
    cout << "Total Zones: " << n << "\n\n";

    int src;
    cout << "Enter source zone index: ";
    cin >> src;

    if (src < 0 || src >= n) {
        cout << "Invalid source.\n";
        return 0;
    }

    // Dijkstra
    vector<long long> distTime;
    vector<int> parentTime;
    dijkstraShortestTime(city, src, distTime, parentTime);
    printTimeTable(distTime, parentTime, names);

    // BFS
    cout << "\nRun BFS hop model? (1=yes 0=no): ";
    int run; cin >> run;
    if (run == 1) {
        vector<int> dist, parent;
        bfsUnweighted(city, src, dist, parent);
        printBFSTable(dist, parent, names);
    }

    cout << "\nTime Complexity:\n";
    cout << "Dijkstra : O((V + E) log V)\n";
    cout << "BFS      : O(V + E)\n";

    return 0;
}
