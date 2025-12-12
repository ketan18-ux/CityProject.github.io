#include <bits/stdc++.h>
using namespace std;

const int INF = 1e9;

// -------------------------- Sample Graph Builder -------------------------- //
void buildSampleCity(
    vector<vector<int>> &adj,
    vector<string> &names,
    vector<int> &floodSources,
    vector<int> &shelters,
    int &safeThreshold
) {
    int n = 8;
    adj.assign(n, {});
    names = {
        "River Bank",        // 0 (flood source)
        "Low Lying Colony",  // 1
        "Market Area",       // 2
        "City Park",         // 3
        "Main Junction",     // 4
        "Hill Side",         // 5 (safe high ground)
        "School Shelter",    // 6 (shelter)
        "Stadium Shelter"    // 7 (shelter)
    };

    auto addEdge = [&](int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    };

    addEdge(0, 1);
    addEdge(1, 2);
    addEdge(2, 3);
    addEdge(3, 4);
    addEdge(4, 5);
    addEdge(4, 6);
    addEdge(3, 7);

    floodSources = {0};
    shelters = {6, 7};
    safeThreshold = 3;
}

// -------------------------- Multi-Source BFS -------------------------- //
vector<int> multiSourceBFS(const vector<vector<int>> &adj, const vector<int> &sources) {
    int n = (int)adj.size();
    vector<int> dist(n, INF);
    queue<int> q;

    for (int src : sources) {
        if (src < 0 || src >= n) continue;
        dist[src] = 0;
        q.push(src);
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

// -------------------------- Input Helpers -------------------------- //
void printCity(const vector<vector<int>> &adj, const vector<string> &names) {
    cout << "\nCity Graph (Adjacency List Representation):\n";
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

void readCustomCity(
    vector<vector<int>> &adj,
    vector<string> &names,
    vector<int> &floodSources,
    vector<int> &shelters,
    int &safeThreshold
) {
    int n, m;
    cout << "Enter number of localities (nodes): ";
    cin >> n;
    cout << "Enter number of roads (edges): ";
    cin >> m;

    adj.assign(n, {});
    names.assign(n, "");

    cout << "\nDo you want to give custom names to localities? (1 = yes, 0 = no): ";
    int choice;
    cin >> choice;
    if (choice == 1) {
        cout << "Enter " << n << " locality names (one per line):\n";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        for (int i = 0; i < n; i++) {
            string s;
            getline(cin, s);
            if (s.empty()) s = "Locality-" + to_string(i);
            names[i] = s;
        }
    } else {
        for (int i = 0; i < n; i++) {
            names[i] = "Locality-" + to_string(i);
        }
    }

    cout << "\nEnter " << m << " undirected roads as: u v (0-based indices)\n";
    cout << "Example: 0 1 means road between 0 and 1.\n";
    for (int i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;
        if (u < 0 || v < 0 || u >= n || v >= n || u == v) {
            cout << "Invalid edge (" << u << ", " << v << "), skipping.\n";
            continue;
        }
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    int k;
    cout << "\nEnter number of initial flood source localities: ";
    cin >> k;
    floodSources.clear();
    cout << "Enter " << k << " indices for flood sources: ";
    for (int i = 0; i < k; i++) {
        int x;
        cin >> x;
        if (x < 0 || x >= n) {
            cout << "Invalid source " << x << ", skipping.\n";
            continue;
        }
        floodSources.push_back(x);
    }

    int s;
    cout << "\nEnter number of shelter localities: ";
    cin >> s;
    shelters.clear();
    cout << "Enter " << s << " indices for shelters: ";
    for (int i = 0; i < s; i++) {
        int x;
        cin >> x;
        if (x < 0 || x >= n) {
            cout << "Invalid shelter " << x << ", skipping.\n";
            continue;
        }
        shelters.push_back(x);
    }

    cout << "\nEnter safe time threshold (max BFS level before roads are considered cut off): ";
    cin >> safeThreshold;
}

// -------------------------- Reporting Functions -------------------------- //
void reportFloodTimes(
    const vector<int> &dist,
    const vector<string> &names
) {
    cout << "\nFlood Arrival Time for Each Locality (in BFS time-steps):\n";
    cout << left << setw(8) << "Index"
         << setw(25) << "Locality"
         << "Flood Time\n";
    cout << string(50, '-') << "\n";

    for (int i = 0; i < (int)dist.size(); i++) {
        cout << left << setw(8) << i;
        string name = (i < (int)names.size()) ? names[i] : ("Locality-" + to_string(i));
        cout << setw(25) << name;

        if (dist[i] == INF) {
            cout << "Not Reached\n";
        } else {
            cout << dist[i] << "\n";
        }
    }
    cout << "\n";
}

void reportShelterSafety(
    const vector<int> &dist,
    const vector<int> &shelters,
    const vector<string> &names,
    int safeThreshold
) {
    cout << "Shelter Safety Status (threshold = " << safeThreshold << "):\n";
    cout << left << setw(8) << "Index"
         << setw(25) << "Shelter"
         << setw(15) << "Flood Time"
         << "Status\n";
    cout << string(70, '-') << "\n";

    for (int shelter : shelters) {
        string name = (shelter < (int)names.size()) ? names[shelter] : ("Locality-" + to_string(shelter));
        cout << left << setw(8) << shelter
             << setw(25) << name;

        string floodTimeStr;
        string status;

        if (shelter < 0 || shelter >= (int)dist.size()) {
            floodTimeStr = "Invalid";
            status = "Invalid Index";
        } else if (dist[shelter] == INF) {
            floodTimeStr = "Not Reached";
            status = "SAFE (never flooded)";
        } else {
            floodTimeStr = to_string(dist[shelter]);
            if (dist[shelter] <= safeThreshold) {
                status = "UNSAFE (flooded too early)";
            } else {
                status = "SAFE (flood after threshold)";
            }
        }

        cout << setw(15) << floodTimeStr
             << status << "\n";
    }
    cout << "\n";
}

void listEvacuationOrderCandidates(
    const vector<int> &dist,
    const vector<int> &shelters,
    const vector<string> &names,
    int safeThreshold
) {
    struct EvacInfo {
        int idx;
        int time;
        string name;
    };

    vector<EvacInfo> needEvac;
    for (int s : shelters) {
        if (s < 0 || s >= (int)dist.size()) continue;
        if (dist[s] != INF && dist[s] <= safeThreshold) {
            EvacInfo e;
            e.idx = s;
            e.time = dist[s];
            e.name = (s < (int)names.size()) ? names[s] : ("Locality-" + to_string(s));
            needEvac.push_back(e);
        }
    }

    if (needEvac.empty()) {
        cout << "No shelters are flooded before threshold. No urgent evacuation needed.\n\n";
        return;
    }

    sort(needEvac.begin(), needEvac.end(), [](const EvacInfo &a, const EvacInfo &b) {
        if (a.time != b.time) return a.time < b.time;
        return a.idx < b.idx;
    });

    cout << "Suggested Evacuation Priority (shelters where flood comes early):\n";
    cout << left << setw(8) << "Index"
         << setw(25) << "Shelter"
         << "Flood Time\n";
    cout << string(55, '-') << "\n";

    for (const auto &e : needEvac) {
        cout << left << setw(8) << e.idx
             << setw(25) << e.name
             << e.time << "\n";
    }
    cout << "\n";
}

// -------------------------- MAIN -------------------------- //
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "Problem 10: Flood Evacuation Simulation (Multi-source BFS)\n\n";
    cout << "1. Use sample city\n";
    cout << "2. Enter custom city\n";
    cout << "Choice: ";

    int choice;
    cin >> choice;

    vector<vector<int>> adj;
    vector<string> names;
    vector<int> floodSources;
    vector<int> shelters;
    int safeThreshold = 0;

    if (choice == 2) {
        readCustomCity(adj, names, floodSources, shelters, safeThreshold);
    } else {
        buildSampleCity(adj, names, floodSources, shelters, safeThreshold);
        cout << "\nUsing sample city network with predefined flood sources and shelters.\n";
    }

    if (adj.empty()) {
        cout << "Graph is empty. Exiting.\n";
        return 0;
    }

    if (floodSources.empty()) {
        cout << "\nNo flood sources specified. Nothing to simulate.\n";
        return 0;
    }

    printCity(adj, names);

    vector<int> dist = multiSourceBFS(adj, floodSources);

    reportFloodTimes(dist, names);
    if (!shelters.empty()) {
        reportShelterSafety(dist, shelters, names, safeThreshold);
        listEvacuationOrderCandidates(dist, shelters, names, safeThreshold);
    } else {
        cout << "No shelters were specified. Only flood arrival times are shown.\n\n";
    }

    cout << "Time Complexity:\n";
    cout << "- Multi-source BFS: O(V + E)\n";
    cout << "- Shelter reporting & sorting (for evacuation order): O(S log S)\n";
    cout << "Total: O(V + E + S log S), dominated by O(V + E) for sparse city graphs.\n";

    return 0;
}
