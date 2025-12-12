#include <bits/stdc++.h>
using namespace std;

// ------------------------- Trip struct ------------------------- //
struct Trip {
    int id;
    int startMin;
    int endMin;
};

// ------------------------- Time utilities ------------------------- //
int toMinutes(int h, int m) {
    return h * 60 + m;
}

string toHHMM(int minutes) {
    int h = minutes / 60;
    int m = minutes % 60;
    ostringstream oss;
    if (h < 10) oss << '0';
    oss << h << ":";
    if (m < 10) oss << '0';
    oss << m;
    return oss.str();
}

// ------------------------- Merge Sort ------------------------- //
void mergeTrips(vector<Trip> &arr, int l, int mid, int r) {
    int n1 = mid - l + 1;
    int n2 = r - mid;

    vector<Trip> L(n1), R(n2);
    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (L[i].endMin < R[j].endMin ||
           (L[i].endMin == R[j].endMin && L[i].startMin <= R[j].startMin)) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void mergeSortTrips(vector<Trip> &arr, int l, int r) {
    if (l >= r) return;
    int mid = l + (r - l) / 2;
    mergeSortTrips(arr, l, mid);
    mergeSortTrips(arr, mid + 1, r);
    mergeTrips(arr, l, mid, r);
}

// ------------------------- Single Bus Greedy ------------------------- //
vector<Trip> scheduleSingleBus(const vector<Trip> &sortedTrips) {
    vector<Trip> result;
    int lastEnd = -1;

    for (const auto &t : sortedTrips) {
        if (t.startMin >= lastEnd) {
            result.push_back(t);
            lastEnd = t.endMin;
        }
    }
    return result;
}

// ------------------------- Multi-Bus Greedy ------------------------- //
struct BusInfo {
    int lastEnd;
    int index;
    bool operator>(const BusInfo &other) const {
        if (lastEnd != other.lastEnd) return lastEnd > other.lastEnd;
        return index > other.index;
    }
};

vector<vector<Trip>> scheduleMultipleBuses(const vector<Trip> &sortedTrips, int busCount) {
    vector<vector<Trip>> allocation(busCount);
    if (busCount <= 0) return allocation;

    priority_queue<BusInfo, vector<BusInfo>, greater<BusInfo>> pq;
    for (int i = 0; i < busCount; i++)
        pq.push({0, i});

    for (const auto &t : sortedTrips) {
        auto b = pq.top();
        pq.pop();

        if (t.startMin >= b.lastEnd) {
            allocation[b.index].push_back(t);
            b.lastEnd = t.endMin;
        }

        pq.push(b);
    }

    return allocation;
}

// ------------------------- Printing ------------------------- //
void printTripList(const vector<Trip> &trips, const string &title) {
    cout << "\n" << title << "\n";
    cout << left << setw(6) << "ID"
         << setw(10) << "Start"
         << setw(10) << "End" << "\n";
    cout << string(26, '-') << "\n";
    for (const auto &t : trips) {
        cout << setw(6) << t.id
             << setw(10) << toHHMM(t.startMin)
             << setw(10) << toHHMM(t.endMin) << "\n";
    }
}

void printBusAllocation(const vector<vector<Trip>> &allocation) {
    cout << "\nMulti-Bus Allocation:\n";
    for (int i = 0; i < allocation.size(); i++) {
        cout << "Bus " << i + 1 << ":\n";
        if (allocation[i].empty())
            cout << "  No trips assigned.\n";
        else {
            for (auto &t : allocation[i])
                cout << "  Trip " << t.id << ": "
                     << toHHMM(t.startMin) << " - "
                     << toHHMM(t.endMin) << "\n";
        }
        cout << "\n";
    }
}

// ------------------------- CSV Loader ------------------------- //
bool loadTripsCSV(vector<Trip> &trips) {
    ifstream fin("trips_20.csv");
    if (!fin.is_open()) return false;

    trips.clear();
    string line;
    getline(fin, line); // skip header

    while (getline(fin, line)) {
        stringstream ss(line);
        string id, sh, sm, eh, em;

        getline(ss, id, ',');
        getline(ss, sh, ',');
        getline(ss, sm, ',');
        getline(ss, eh, ',');
        getline(ss, em, ',');

        Trip t;
        t.id = stoi(id);
        t.startMin = toMinutes(stoi(sh), stoi(sm));
        t.endMin = toMinutes(stoi(eh), stoi(em));

        trips.push_back(t);
    }
    return true;
}

// ------------------------- MAIN ------------------------- //
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "Problem 8 — Bus Timetable Generation (CSV Enabled)\n\n";

    vector<Trip> trips;
    if (!loadTripsCSV(trips)) {
        cout << "❌ Error: trips_20.csv not found.\n";
        return 0;
    }

    printTripList(trips, "Input Trips (Unsorted):");

    mergeSortTrips(trips, 0, trips.size() - 1);

    printTripList(trips, "Trips Sorted by Finishing Time (Merge Sort):");

    auto single = scheduleSingleBus(trips);

    printTripList(single, "Trips Assigned to Bus 1 (Greedy):");

    int busCount;
    cout << "\nEnter number of buses for multi-bus scheduling: ";
    cin >> busCount;

    if (busCount > 0) {
        auto alloc = scheduleMultipleBuses(trips, busCount);
        printBusAllocation(alloc);
    }

    cout << "\nTime Complexity:\n";
    cout << "- Merge Sort: O(n log n)\n";
    cout << "- Greedy (single bus): O(n)\n";
    cout << "- Multi-bus heap scheduling: O(n log B)\n";
    cout << "Overall: O(n log n + n log B)\n";

    return 0;
}
