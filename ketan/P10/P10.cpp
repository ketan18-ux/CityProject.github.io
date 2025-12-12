#include <bits/stdc++.h>
using namespace std;

const int INF = 1e9;

// ---------------- CSV Loaders ---------------- //
bool loadNodesCSV(vector<string> &names, vector<int> &floodSources,
                  vector<int> &shelters)
{
    ifstream fin("flood_nodes.csv");
    if (!fin.is_open()) return false;

    names.clear();
    floodSources.clear();
    shelters.clear();

    string line;
    getline(fin, line); // header

    while (getline(fin, line)) {
        string idStr, name, type;
        stringstream ss(line);
        getline(ss, idStr, ',');
        getline(ss, name, ',');
        getline(ss, type, ',');

        int id = stoi(idStr);
        if (id >= names.size()) names.resize(id + 1);
        names[id] = name;

        if (type == "FLOOD") floodSources.push_back(id);
        if (type == "SHELTER") shelters.push_back(id);
    }

    return true;
}

bool loadEdgesCSV(vector<vector<int>> &adj, int n)
{
    ifstream fin("flood_edges.csv");
    if (!fin.is_open()) return false;

    adj.assign(n, {});

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
    }
    return true;
}

bool loadThreshold(int &threshold)
{
    ifstream fin("threshold.csv");
    if (!fin.is_open()) return false;

    string line;
    getline(fin, line); // header
    getline(fin, line);

    threshold = stoi(line);
    return true;
}

// ---------------- Multi-Source BFS ---------------- //
vector<int> multiSourceBFS(const vector<vector<int>> &adj,
                           const vector<int> &sources)
{
    int n = adj.size();
    vector<int> dist(n, INF);
    queue<int> q;

    for (int s : sources) {
        if (s >= 0 && s < n) {
            dist[s] = 0;
            q.push(s);
        }
    }

    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (int v : adj[u]) {
            if (dist[v] == INF) {
                dist[v] = dist[u] + 1;
                q.push(v);
            }
        }
    }
    return dist;
}

// ---------------- Printing ---------------- //
void printCity(const vector<vector<int>> &adj, const vector<string> &names)
{
    cout << "\nCity Graph:\n";
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

void reportFlood(const vector<int> &dist, const vector<string> &names)
{
    cout << "\nFlood Arrival Times:\n";
    for (int i = 0; i < dist.size(); i++) {
        cout << i << " (" << names[i] << ") : ";
        if (dist[i] == INF) cout << "Not Reached\n";
        else cout << dist[i] << "\n";
    }
    cout << "\n";
}

void reportShelters(const vector<int> &dist, const vector<int> &shelters,
                    const vector<string> &names, int threshold)
{
    cout << "Shelter Status (threshold = " << threshold << "):\n";
    for (int s : shelters) {
        cout << s << " (" << names[s] << ") : ";
        if (dist[s] == INF) {
            cout << "SAFE (never flooded)\n";
        } else if (dist[s] <= threshold) {
            cout << "UNSAFE (flood comes at " << dist[s] << ")\n";
        } else {
            cout << "SAFE (flood comes late at " << dist[s] << ")\n";
        }
    }
    cout << "\n";
}

void evacuationOrder(const vector<int> &dist, const vector<int> &shelters,
                     const vector<string> &names, int threshold)
{
    struct Info { int idx; int t; };
    vector<Info> bad;

    for (int s : shelters)
        if (dist[s] <= threshold)
            bad.push_back({s, dist[s]});

    sort(bad.begin(), bad.end(), [](auto &a, auto &b){ return a.t < b.t; });

    cout << "Evacuation Priority:\n";
    if (bad.empty()) {
        cout << "All shelters safe.\n\n";
        return;
    }

    for (auto &x : bad)
        cout << x.idx << " (" << names[x.idx] << ") → flood at " << x.t << "\n";

    cout << "\n";
}

// ---------------- MAIN ---------------- //
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "Problem 10 — Flood Evacuation Simulation (CSV Enabled)\n\n";

    vector<string> names;
    vector<int> floodSources, shelters;
    int threshold;

    if (!loadNodesCSV(names, floodSources, shelters)) {
        cout << "Missing flood_nodes.csv\n";
        return 0;
    }
    if (!loadThreshold(threshold)) {
        cout << "Missing threshold.csv\n";
        return 0;
    }

    vector<vector<int>> adj;
    if (!loadEdgesCSV(adj, names.size())) {
        cout << "Missing flood_edges.csv\n";
        return 0;
    }

    printCity(adj, names);

    auto dist = multiSourceBFS(adj, floodSources);

    reportFlood(dist, names);
    reportShelters(dist, shelters, names, threshold);
    evacuationOrder(dist, shelters, names, threshold);

    cout << "Time Complexity: O(V + E)\n";
    return 0;
}
