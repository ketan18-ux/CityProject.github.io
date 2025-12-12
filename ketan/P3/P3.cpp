// File: signal_timing.cpp
// Problem 3 – Traffic Signal Timing Optimization
// Updated: loads lanes from lanes.csv (20-input dataset expected)

#include <bits/stdc++.h>
using namespace std;

struct Lane {
    string name;
    int queueLength;        // vehicles currently waiting
    double baseGreen;       // guaranteed minimum green (sec)
    double allocatedGreen;  // final green time (sec) after distribution
};

struct LanePriority {
    int idx;
    int queueLength;
    bool operator<(const LanePriority &other) const {
        return queueLength < other.queueLength; // max-heap
    }
};

// ---------- Utility printing ---------- //

void printLaneTable(const vector<Lane> &lanes, int cycleTime) {
    cout << "\n=== Signal Timing Plan (Cycle " << cycleTime << " sec) ===\n\n";
    cout << left << setw(20) << "Lane"
         << setw(10) << "Queue"
         << setw(18) << "Base Green (s)"
         << "Final Green (s)\n";
    cout << string(70, '-') << "\n";

    cout.setf(ios::fixed);
    cout << setprecision(2);

    for (const auto &ln : lanes) {
        cout << left << setw(20) << ln.name
             << setw(10) << ln.queueLength
             << setw(18) << ln.baseGreen
             << ln.allocatedGreen << "\n";
    }
    cout << "\n";
}

// ---------- Strategy 1: Proportional Allocation ---------- //

void allocateGreenProportional(vector<Lane> &lanes, int cycleTime) {
    int n = (int)lanes.size();
    if (n == 0) return;

    double minSharePercent = 0.20; // 20% of cycle is reserved as equal base
    double basePool = cycleTime * minSharePercent;
    double extraPool = cycleTime - basePool;

    if (extraPool < 0) {
        extraPool = 0;
        basePool = cycleTime;
    }

    double equalBase = basePool / n;
    for (auto &ln : lanes) {
        ln.baseGreen = equalBase;
        ln.allocatedGreen = equalBase;
    }

    long long totalQueue = 0;
    for (auto &ln : lanes) totalQueue += ln.queueLength;

    if (totalQueue == 0) {
        double share = (double)cycleTime / n;
        for (auto &ln : lanes) {
            ln.baseGreen = share;
            ln.allocatedGreen = share;
        }
        return;
    }

    for (auto &ln : lanes) {
        double fraction = (double)ln.queueLength / (double)totalQueue;
        double extra = extraPool * fraction;
        ln.allocatedGreen += extra;
    }
}

// ---------- Strategy 2: Priority-Based Adjustment ---------- //

void adjustUsingPriorityQueue(vector<Lane> &lanes, int cycleTime) {
    int n = (int)lanes.size();
    if (n == 0) return;

    vector<LanePriority> arr;
    arr.reserve(n);
    for (int i = 0; i < n; i++) {
        arr.push_back({i, lanes[i].queueLength});
    }

    priority_queue<LanePriority> pq;
    for (auto &lp : arr) pq.push(lp);

    double totalAllocated = 0.0;
    for (auto &ln : lanes) totalAllocated += ln.allocatedGreen;

    double eps = 1e-6;
    double remaining = (double)cycleTime - totalAllocated;
    if (remaining <= eps) return;

    int steps = 0;
    const double chunk = 2.0; // add 2 seconds per step to heaviest lane

    while (remaining > eps && steps < 1000 && !pq.empty()) {
        LanePriority top = pq.top();
        pq.pop();
        int idx = top.idx;

        if (lanes[idx].queueLength <= 0) break;

        double add = min(chunk, remaining);
        lanes[idx].allocatedGreen += add;
        remaining -= add;
        steps++;

        // reduce the effective queue used for priority decisions
        top.queueLength = max(0, top.queueLength - 1);
        pq.push(top);
    }
}

// ---------- Simple Simulation over Multiple Cycles ---------- //

void simulateMultipleCycles(vector<Lane> lanes,
                            int cycleTime,
                            int dischargeRatePerSec,
                            int cycles) {
    cout << "\n===== Simulation over " << cycles << " cycles =====\n";
    cout << "Assumption: " << dischargeRatePerSec
         << " vehicles / second can pass during green.\n";

    for (int c = 1; c <= cycles; c++) {
        cout << "\n----- Cycle " << c << " -----\n";

        vector<Lane> current = lanes;

        allocateGreenProportional(current, cycleTime);
        adjustUsingPriorityQueue(current, cycleTime);

        printLaneTable(current, cycleTime);

        for (size_t i = 0; i < lanes.size(); i++) {
            double capacity = current[i].allocatedGreen * dischargeRatePerSec;
            int served = (int)capacity;
            lanes[i].queueLength = max(0, lanes[i].queueLength - served);
        }

        cout << "Remaining queues after this cycle:\n";
        cout << left << setw(20) << "Lane" << "Queue Length\n";
        cout << string(40, '-') << "\n";
        for (auto &ln : lanes) {
            cout << left << setw(20) << ln.name << ln.queueLength << "\n";
        }
        cout << "\n";
    }
}

// ---------- CSV Loader (lanes.csv: lane,queue) ---------- //

bool loadLanesCSV(const string &filename, vector<Lane> &lanes) {
    ifstream fin(filename);
    if (!fin.is_open()) return false;

    lanes.clear();
    string line;

    // Read header
    if (!getline(fin, line)) return false;

    while (getline(fin, line)) {
        if (line.size() == 0) continue;
        // parse CSV: lane,queue
        string lane;
        string queueStr;

        // Handle possible commas inside quotes? keep simple: no quotes expected
        size_t commaPos = line.find(',');
        if (commaPos == string::npos) continue;
        lane = line.substr(0, commaPos);
        queueStr = line.substr(commaPos + 1);

        // trim whitespace (both ends)
        auto trim = [](string &s) {
            size_t a = 0;
            while (a < s.size() && isspace((unsigned char)s[a])) a++;
            size_t b = s.size();
            while (b > a && isspace((unsigned char)s[b-1])) b--;
            s = s.substr(a, b - a);
        };
        trim(lane);
        trim(queueStr);
        if (lane.empty() || queueStr.empty()) continue;

        int q = 0;
        try {
            q = stoi(queueStr);
        } catch (...) {
            q = 0;
        }
        Lane ln;
        ln.name = lane;
        ln.queueLength = max(0, q);
        ln.baseGreen = 0.0;
        ln.allocatedGreen = 0.0;
        lanes.push_back(ln);
    }
    return true;
}

// ---------- Sample Data ---------- //

vector<Lane> buildSampleLanes() {
    vector<Lane> lanes;
    lanes.push_back({"North", 35, 0.0, 0.0});
    lanes.push_back({"East",  20, 0.0, 0.0});
    lanes.push_back({"South", 50, 0.0, 0.0});
    lanes.push_back({"West",  10, 0.0, 0.0});
    return lanes;
}

// ---------- Custom Input (interactive fallback) ---------- //

vector<Lane> buildCustomLanesInteractive() {
    int n;
    cout << "Enter number of lanes (e.g., 4): ";
    cin >> n;
    if (n <= 0) {
        cout << "Invalid lane count, using 4.\n";
        n = 4;
    }
    vector<Lane> lanes(n);
    cout << "\nEnter lane name and queue length (vehicles):\n";
    for (int i = 0; i < n; i++) {
        cout << "Lane " << i + 1 << " name: ";
        cin >> ws;
        string name;
        getline(cin, name);
        if (name.empty()) { name = "Lane" + to_string(i+1); }
        lanes[i].name = name;
        cout << "Queue length for " << lanes[i].name << ": ";
        int q; cin >> q;
        lanes[i].queueLength = max(0, q);
        lanes[i].baseGreen = 0.0;
        lanes[i].allocatedGreen = 0.0;
    }
    return lanes;
}

// ---------- MAIN ---------- //

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "Problem 3 – Traffic Signal Timing Optimization (CSV-enabled)\n\n";

    vector<Lane> lanes;
    bool loaded = loadLanesCSV("lanes.csv", lanes);
    if (loaded) {
        cout << "Loaded lanes from lanes.csv (count = " << lanes.size() << ")\n";
    } else {
        cout << "lanes.csv not found or invalid. Using sample 4-way junction.\n";
        lanes = buildSampleLanes();
    }

    // If CSV had too many lanes, we still support them
    int n = (int)lanes.size();
    if (n == 0) {
        cout << "No lanes available. Exiting.\n";
        return 0;
    }

    cout << "\nList of lanes and initial queues:\n";
    for (int i = 0; i < n; ++i) {
        cout << setw(2) << i << " - " << setw(20) << lanes[i].name
             << " queue=" << lanes[i].queueLength << "\n";
    }

    int cycleTime;
    cout << "\nEnter total signal cycle time in seconds (e.g., 120): ";
    cin >> cycleTime;
    if (cycleTime <= 0) {
        cout << "Invalid cycle time, using 120.\n";
        cycleTime = 120;
    }

    vector<Lane> plan = lanes;
    allocateGreenProportional(plan, cycleTime);
    adjustUsingPriorityQueue(plan, cycleTime);

    printLaneTable(plan, cycleTime);

    cout << "Run multi-cycle simulation? (1 = yes, 0 = no): ";
    int simChoice;
    cin >> simChoice;

    if (simChoice == 1) {
        int dischargeRate, cycles;
        cout << "Enter discharge rate (vehicles per second during green, e.g., 1): ";
        cin >> dischargeRate;
        if (dischargeRate <= 0) dischargeRate = 1;

        cout << "Enter number of cycles to simulate: ";
        cin >> cycles;
        if (cycles <= 0) cycles = 3;

        simulateMultipleCycles(lanes, cycleTime, dischargeRate, cycles);
    }

    cout << "\nTime Complexity:\n";
    cout << "- Proportional allocation: O(n)\n";
    cout << "- Priority-based adjustment (heap): O(n + K log n)\n";
    cout << "- Simulation (C cycles): O(C × (n + K log n))\n";
    cout << "Overall: efficient for real-time junction control (n=20 in dataset).\n";

    return 0;
}
