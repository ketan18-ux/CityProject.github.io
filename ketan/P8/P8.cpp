#include <bits/stdc++.h>
using namespace std;

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

// ------------------------- Merge Sort by finishing time ------------------------- //
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

// ------------------------- Greedy schedule for a single bus ------------------------- //
vector<Trip> scheduleSingleBus(const vector<Trip> &sortedTrips) {
    vector<Trip> result;
    if (sortedTrips.empty()) return result;

    int lastEnd = -1;
    for (const auto &t : sortedTrips) {
        if (t.startMin >= lastEnd) {
            result.push_back(t);
            lastEnd = t.endMin;
        }
    }
    return result;
}

// ------------------------- Greedy schedule for multiple buses ------------------------- //
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
    if (busCount <= 0 || sortedTrips.empty()) return allocation;

    priority_queue<BusInfo, vector<BusInfo>, greater<BusInfo>> pq;
    for (int i = 0; i < busCount; i++) {
        pq.push({0, i});
    }

    for (const auto &t : sortedTrips) {
        BusInfo b = pq.top();
        pq.pop();
        if (t.startMin >= b.lastEnd) {
            allocation[b.index].push_back(t);
            b.lastEnd = t.endMin;
        }
        pq.push(b);
    }

    return allocation;
}

// ------------------------- Sample input builder ------------------------- //
void buildSampleTrips(vector<Trip> &trips) {
    trips.clear();
    trips.push_back({1,  toMinutes(6,  0), toMinutes(7,  0)});
    trips.push_back({2,  toMinutes(6, 30), toMinutes(7, 30)});
    trips.push_back({3,  toMinutes(7, 15), toMinutes(8, 15)});
    trips.push_back({4,  toMinutes(8,  0), toMinutes(9,  0)});
    trips.push_back({5,  toMinutes(8, 30), toMinutes(9, 30)});
    trips.push_back({6,  toMinutes(9, 15), toMinutes(10,15)});
    trips.push_back({7,  toMinutes(10, 0), toMinutes(11, 0)});
    trips.push_back({8,  toMinutes(10,45), toMinutes(11,45)});
    trips.push_back({9,  toMinutes(12, 0), toMinutes(13, 0)});
    trips.push_back({10, toMinutes(13,30), toMinutes(14,30)});
}

// ------------------------- Printing helpers ------------------------- //
void printTripList(const vector<Trip> &trips, const string &title) {
    cout << "\n" << title << "\n";
    cout << left << setw(6) << "ID"
         << setw(10) << "Start"
         << setw(10) << "End" << "\n";
    cout << string(26, '-') << "\n";
    for (const auto &t : trips) {
        cout << left << setw(6) << t.id
             << setw(10) << toHHMM(t.startMin)
             << setw(10) << toHHMM(t.endMin) << "\n";
    }
    cout << "\n";
}

void printBusAllocation(const vector<vector<Trip>> &allocation) {
    cout << "\nMulti-Bus Allocation (Greedy):\n";
    for (int b = 0; b < (int)allocation.size(); b++) {
        cout << "Bus " << b + 1 << ":\n";
        if (allocation[b].empty()) {
            cout << "  No trips assigned.\n";
        } else {
            for (const auto &t : allocation[b]) {
                cout << "  Trip " << t.id << " : "
                     << toHHMM(t.startMin) << " - "
                     << toHHMM(t.endMin) << "\n";
            }
        }
        cout << "\n";
    }
}

// ------------------------- Utilization metrics ------------------------- //
int totalMinutesCovered(const vector<Trip> &schedule) {
    int total = 0;
    for (const auto &t : schedule) {
        total += (t.endMin - t.startMin);
    }
    return total;
}

int earliestStart(const vector<Trip> &trips) {
    int ans = INT_MAX;
    for (const auto &t : trips) ans = min(ans, t.startMin);
    return ans == INT_MAX ? 0 : ans;
}

int latestEnd(const vector<Trip> &trips) {
    int ans = 0;
    for (const auto &t : trips) ans = max(ans, t.endMin);
    return ans;
}

// ------------------------- MAIN ------------------------- //
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "Problem 8: Bus Timetable Generation (Sorting + Greedy)\n\n";
    cout << "1. Use sample trips (one day)\n";
    cout << "2. Enter custom trips\n";
    cout << "Choice: ";

    int mode;
    cin >> mode;

    vector<Trip> trips;

    if (mode == 2) {
        int n;
        cout << "\nEnter number of candidate trips: ";
        cin >> n;
        trips.resize(n);
        cout << "Enter trips as: id startHour startMin endHour endMin\n";
        cout << "Example: 1 6 0 7 0  (Trip 1 from 06:00 to 07:00)\n\n";
        for (int i = 0; i < n; i++) {
            int id, sh, sm, eh, em;
            cin >> id >> sh >> sm >> eh >> em;
            trips[i].id = id;
            trips[i].startMin = toMinutes(sh, sm);
            trips[i].endMin   = toMinutes(eh, em);
        }
    } else {
        buildSampleTrips(trips);
        cout << "\nUsing sample set of trips for one day.\n";
    }

    printTripList(trips, "Input Trips (Unsorted):");

    if (trips.empty()) {
        cout << "No trips given.\n";
        return 0;
    }

    mergeSortTrips(trips, 0, (int)trips.size() - 1);
    printTripList(trips, "Trips Sorted by Finishing Time (Merge Sort):");

    vector<Trip> singleBusSchedule = scheduleSingleBus(trips);
    printTripList(singleBusSchedule, "Selected Trips for Bus 1 (Non-overlapping):");

    int cov = totalMinutesCovered(singleBusSchedule);
    int fullStart = earliestStart(trips);
    int fullEnd   = latestEnd(trips);
    int window = max(1, fullEnd - fullStart);

    double utilization = (100.0 * cov) / window;

    cout << "Time Window for all trips : " << toHHMM(fullStart)
         << " - " << toHHMM(fullEnd)
         << "  (" << window << " minutes)\n";
    cout << "Total minutes Bus 1 is running: " << cov << " minutes\n";
    cout << "Approx Utilization of Bus 1   : " << fixed << setprecision(2)
         << utilization << " %\n\n";

    cout << "Now enter how many buses you have for multi-bus allocation (e.g., 2 or 3): ";
    int busCount;
    cin >> busCount;

    if (busCount > 0) {
        auto allocation = scheduleMultipleBuses(trips, busCount);
        printBusAllocation(allocation);
    } else {
        cout << "Invalid bus count. Skipping multi-bus allocation.\n\n";
    }

    cout << "Time Complexity:\n";
    cout << "- Sorting trips (Merge Sort): O(n log n)\n";
    cout << "- Single-bus greedy selection: O(n)\n";
    cout << "- Multi-bus allocation with min-heap: O(n log B)\n";
    cout << "Overall: O(n log n + n log B)\n";

    return 0;
}
