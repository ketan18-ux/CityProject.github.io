// File: shortestpath.cpp
// Problem 2 – Shortest Travel‐Time Routing (Dijkstra + BFS)

#include <bits/stdc++.h>
using namespace std;

const long long INF = 1e18;

struct WeightedEdge {
    int to;
    int timeCost;
};

struct UnweightedEdge {
    int to;
};

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
        if (u<0||v<0||u>=n||v>=n) return;
        timeGraph[u].push_back({v, t});
        timeGraph[v].push_back({u, t});
    }

    void addSimpleEdge(int u, int v) {
        if (u<0||v<0||u>=n||v>=n) return;
        simpleGraph[u].push_back({v});
        simpleGraph[v].push_back({u});
    }
};

// ---------------- Dijkstra ---------------- //

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

// ---------------- BFS (Unweighted) ---------------- //

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

// ---------------- Path Builder ---------------- //

vector<int> reconstructPath(int target, const vector<int> &parent) {
    vector<int> path;
    for (int cur = target; cur != -1; cur = parent[cur])
        path.push_back(cur);
    reverse(path.begin(), path.end());
    return path;
}

void printPath(const vector<int> &path, const vector<string> &names) {
    for (size_t i = 0; i < path.size(); i++) {
        int node = path[i];
        cout << names[node];
        if (i + 1 != path.size()) cout << " -> ";
    }
    cout << "\n";
}

// ---------------- Sample Graph ---------------- //

void buildSampleCityGraph(CityGraph &g, vector<string> &names) {
    g.init(6);
    names = {
        "Home Sector (0)",
        "Office District (1)",
        "Metro Station (2)",
        "Ring Road (3)",
        "Shopping Hub (4)",
        "Tech Park (5)"
    };

    g.addTimeEdge(0,2,8);
    g.addTimeEdge(0,3,15);
    g.addTimeEdge(2,3,5);
    g.addTimeEdge(2,4,10);
    g.addTimeEdge(3,4,6);
    g.addTimeEdge(3,5,12);
    g.addTimeEdge(4,1,9);
    g.addTimeEdge(5,1,7);

    g.addSimpleEdge(0,2);
    g.addSimpleEdge(0,3);
    g.addSimpleEdge(2,3);
    g.addSimpleEdge(2,4);
    g.addSimpleEdge(3,4);
    g.addSimpleEdge(3,5);
    g.addSimpleEdge(4,1);
    g.addSimpleEdge(5,1);
}

// ---------------- Display Tables ---------------- //

void printTimeTable(const vector<long long> &dist,
                    const vector<int> &parent,
                    const vector<string> &names) {
    cout << "\n=== Fastest Travel-Time Table ===\n\n";
    cout << left << setw(8) << "Zone"
         << setw(30) << "Name"
         << setw(10) << "Time"
         << "Path\n";
    cout << string(70, '-') << "\n";

    for (int i = 0; i < (int)dist.size(); i++) {
        cout << left << setw(8) << i
             << setw(30) << names[i];
        if (dist[i] == INF) {
            cout << setw(10) << "INF" << "unreachable\n";
            continue;
        }
        cout << setw(10) << (to_string(dist[i]) + " min");
        printPath(reconstructPath(i, parent), names);
    }
    cout << "\n";
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

    for (int i = 0; i < (int)dist.size(); i++) {
        cout << left << setw(8) << i
             << setw(30) << names[i];
        if (dist[i] == -1) {
            cout << setw(10) << "INF" << "unreachable\n";
            continue;
        }
        cout << setw(10) << dist[i];
        printPath(reconstructPath(i, parent), names);
    }
    cout << "\n";
}

// ---------------- Custom Input ---------------- //

void buildCustomGraph(CityGraph &g, vector<string> &names) {
    int n, m;
    cout << "Enter number of zones: ";
    cin >> n;
    g.init(n);
    names.resize(n);

    cout << "Enter names of zones:\n";
    cin.ignore();
    for (int i = 0; i < n; i++)
        getline(cin, names[i]);

    cout << "Enter number of roads: ";
    cin >> m;
    cout << "Enter: u v time\n";
    for (int i = 0; i < m; i++) {
        int u, v, t;
        cin >> u >> v >> t;
        g.addTimeEdge(u, v, t);
        g.addSimpleEdge(u, v);
    }
}

// ---------------- MAIN ---------------- //

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    CityGraph city;
    vector<string> names;

    cout << "Problem 2 – Shortest Travel Time Routing\n\n";
    cout << "1. Sample city\n2. Custom input\nChoice: ";
    int mode;
    cin >> mode;

    if (mode == 2) buildCustomGraph(city, names);
    else {
        buildSampleCityGraph(city, names);
        cout << "Using sample graph.\n";
    }

    int src;
    cout << "\nEnter source zone index: ";
    cin >> src;
    if (src < 0 || src >= city.n) {
        cout << "Invalid source.\n";
        return 0;
    }

    vector<long long> distTime;
    vector<int> parentTime;
    dijkstraShortestTime(city, src, distTime, parentTime);
    printTimeTable(distTime, parentTime, names);

    cout << "Run BFS hop model? (1=yes 0=no): ";
    int run;
    cin >> run;
    if (run == 1) {
        vector<int> dist, parent;
        bfsUnweighted(city, src, dist, parent);
        printBFSTable(dist, parent, names);
    }

    cout << "Dijkstra: O((V+E) log V)\n";
    cout << "BFS:      O(V+E)\n";
    return 0;
}
