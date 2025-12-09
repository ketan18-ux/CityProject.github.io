#include <bits/stdc++.h>
using namespace std;

struct Lane {
    string name;
    int id;
    int queueNow;        // vehicles currently waiting
    double arrivalRate;  // vehicles arriving per minute
    double departRate;   // vehicles that can pass per second when green
    double greenShare;   // seconds in current cycle
    int passedThisCycle;
};

struct CycleStats {
    int cycleNo;
    vector<int> qStart;
    vector<int> qEnd;
    vector<int> passed;
    vector<double> greenTime;
};

void printLaneTable(const vector<Lane> &lanes) {
    cout << "\nLane Status:\n";
    cout << "-------------------------------------------------------------\n";
    cout << left << setw(8) << "ID"
         << setw(12) << "Name"
         << setw(10) << "Queue"
         << setw(14) << "Arr/min"
         << setw(14) << "Dep/sec" << "\n";
    cout << "-------------------------------------------------------------\n";
    for (const auto &L : lanes) {
        cout << left << setw(8) << L.id
             << setw(12) << L.name
             << setw(10) << L.queueNow
             << setw(14) << L.arrivalRate
             << setw(14) << L.departRate << "\n";
    }
    cout << "-------------------------------------------------------------\n";
}

void computeGreenTimes(vector<Lane> &lanes, double cycleTime, double minGreen) {
    int n = lanes.size();
    int totalQueue = 0;
    for (auto &L : lanes) totalQueue += max(L.queueNow, 0);

    for (auto &L : lanes) {
        L.greenShare = 0.0;
        L.passedThisCycle = 0;
    }

    if (totalQueue == 0) {
        double equalShare = cycleTime / n;
        for (auto &L : lanes) L.greenShare = equalShare;
        return;
    }

    double baseTotal = minGreen * n;
    double remaining = max(0.0, cycleTime - baseTotal);

    for (auto &L : lanes) {
        double weight = (double)max(L.queueNow, 0) / (double)totalQueue;
        L.greenShare = minGreen + remaining * weight;
    }
}

void simulateCycle(vector<Lane> &lanes, double cycleTime, CycleStats &stats) {
    int n = lanes.size();

    stats.qStart.resize(n);
    stats.qEnd.resize(n);
    stats.passed.resize(n);
    stats.greenTime.resize(n);

    for (int i = 0; i < n; i++) {
        stats.qStart[i] = lanes[i].queueNow;
    }

    double minutes = cycleTime / 60.0;
    for (auto &L : lanes) {
        int arrivals = (int)round(L.arrivalRate * minutes);
        L.queueNow += arrivals;
    }

    for (int i = 0; i < n; i++) {
        double maxOut = lanes[i].greenShare * lanes[i].departRate;
        int canPass = (int)floor(maxOut);
        int passed = min(canPass, max(lanes[i].queueNow, 0));
        lanes[i].queueNow -= passed;
        lanes[i].passedThisCycle = passed;
    }

    for (int i = 0; i < n; i++) {
        stats.qEnd[i] = lanes[i].queueNow;
        stats.passed[i] = lanes[i].passedThisCycle;
        stats.greenTime[i] = lanes[i].greenShare;
    }
}

void printCycleReport(const vector<Lane> &lanes, const CycleStats &stats) {
    int n = lanes.size();
    cout << "\n================ CYCLE " << stats.cycleNo << " REPORT ================\n";
    cout << left << setw(8) << "Lane"
         << setw(12) << "Name"
         << setw(10) << "Q(start)"
         << setw(10) << "Q(end"
         << ")"
         << setw(10) << "Passed"
         << setw(14) << "Green(s)" << "\n";
    cout << "-----------------------------------------------------------------\n";
    for (int i = 0; i < n; i++) {
        cout << left << setw(8) << lanes[i].id
             << setw(12) << lanes[i].name
             << setw(10) << stats.qStart[i]
             << setw(10) << stats.qEnd[i]
             << setw(10) << stats.passed[i]
             << setw(14) << fixed << setprecision(2) << stats.greenTime[i] << "\n";
    }
    cout << "-----------------------------------------------------------------\n";
}

bool loadFromFile(const string &fname, vector<Lane> &lanes, double &cycleTime, double &minGreen) {
    ifstream fin(fname);
    if (!fin.is_open()) return false;

    int n;
    fin >> n;
    lanes.clear();
    lanes.resize(n);

    for (int i = 0; i < n; i++) {
        lanes[i].id = i;
        fin >> lanes[i].name
            >> lanes[i].queueNow
            >> lanes[i].arrivalRate
            >> lanes[i].departRate;
        lanes[i].greenShare = 0;
        lanes[i].passedThisCycle = 0;
    }
    fin >> cycleTime >> minGreen;
    return true;
}

void loadDemo(vector<Lane> &lanes, double &cycleTime, double &minGreen) {
    lanes.clear();
    Lane n, e, s, w;

    n.id = 0; n.name = "North"; n.queueNow = 25; n.arrivalRate = 18.0; n.departRate = 1.2;
    e.id = 1; e.name = "East";  e.queueNow = 15; e.arrivalRate = 10.0; e.departRate = 1.1;
    s.id = 2; s.name = "South"; s.queueNow = 40; s.arrivalRate = 22.0; s.departRate = 1.3;
    w.id = 3; w.name = "West";  w.queueNow = 20; w.arrivalRate = 14.0; w.departRate = 1.0;

    lanes.push_back(n);
    lanes.push_back(e);
    lanes.push_back(s);
    lanes.push_back(w);

    cycleTime = 120.0;
    minGreen = 10.0;
}

int main() {
    vector<Lane> lanes;
    double cycleTime = 120.0;
    double minGreen = 8.0;

    cout << "SMART CITY — TRAFFIC SIGNAL TIMING OPTIMISER\n";
    cout << "--------------------------------------------\n";
    cout << "1. Load from file (p3_signal_input.txt)\n";
    cout << "2. Use built-in demo junction\n";
    cout << "Choose option: ";
    int ch;
    cin >> ch;

    if (ch == 1) {
        if (!loadFromFile("p3_signal_input.txt", lanes, cycleTime, minGreen)) {
            cout << "p3_signal_input.txt not found\n";
            return 0;
        }
    } else {
        loadDemo(lanes, cycleTime, minGreen);
    }

    cout << "\nInitial Junction Setup:\n";
    printLaneTable(lanes);
    cout << "\nCycle time (seconds): " << cycleTime << "\n";
    cout << "Minimum green per lane (seconds): " << minGreen << "\n";

    cout << "\nEnter number of cycles to simulate: ";
    int cycles;
    cin >> cycles;

    vector<CycleStats> history;
    history.reserve(cycles);

    for (int c = 1; c <= cycles; c++) {
        computeGreenTimes(lanes, cycleTime, minGreen);

        CycleStats cs;
        cs.cycleNo = c;
        simulateCycle(lanes, cycleTime, cs);

        printCycleReport(lanes, cs);
        history.push_back(cs);
    }

    cout << "\n================ SUMMARY ================\n";
    int n = lanes.size();
    vector<long long> totalPassed(n, 0);
    vector<long long> totalQStart(n, 0);
    vector<long long> totalQEnd(n, 0);
    vector<double> totalGreen(n, 0.0);

    for (auto &cs : history) {
        for (int i = 0; i < n; i++) {
            totalQStart[i] += cs.qStart[i];
            totalQEnd[i] += cs.qEnd[i];
            totalPassed[i] += cs.passed[i];
            totalGreen[i] += cs.greenTime[i];
        }
    }

    cout << left << setw(8) << "Lane"
         << setw(12) << "Name"
         << setw(14) << "AvgQ(start)"
         << setw(14) << "AvgQ(end)"
         << setw(14) << "TotalPass"
         << setw(14) << "AvgGreen(s)" << "\n";
    cout << "-----------------------------------------------------------------------\n";

    for (int i = 0; i < n; i++) {
        double avgQStart = (double)totalQStart[i] / cycles;
        double avgQEnd = (double)totalQEnd[i] / cycles;
        double avgGreen = totalGreen[i] / cycles;

        cout << left << setw(8) << lanes[i].id
             << setw(12) << lanes[i].name
             << setw(14) << fixed << setprecision(2) << avgQStart
             << setw(14) << fixed << setprecision(2) << avgQEnd
             << setw(14) << totalPassed[i]
             << setw(14) << fixed << setprecision(2) << avgGreen << "\n";
    }
    cout << "-----------------------------------------------------------------------\n";

    cout << "\nSimulation complete.\n";
    return 0;
}
