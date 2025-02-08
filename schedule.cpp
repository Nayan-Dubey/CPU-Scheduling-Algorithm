#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <iomanip>

using namespace std;

struct Process {
    int id, arrivalTime, burstTime, priority, remainingTime;
    int completionTime, turnaroundTime, waitingTime, startTime;
};

// Function to print Gantt Chart
void printGanttChart(const vector<pair<int, int>> &gantt) {
    cout << "\nGantt Chart:\n";
    for (auto &slot : gantt) cout << "| P" << slot.first << " ";
    cout << "|\n";
    for (auto &slot : gantt) cout << slot.second << "   ";
    cout << "\n";
}

// Function to print results
void printResults(const vector<Process> &processes) {
    float totalTAT = 0, totalWT = 0;
    cout << "\nProcess\tAT\tBT\tCT\tTAT\tWT\n";
    for (auto &p : processes) {
        cout << "P" << p.id << "\t" << p.arrivalTime << "\t" << p.burstTime << "\t"
             << p.completionTime << "\t" << p.turnaroundTime << "\t" << p.waitingTime << "\n";
        totalTAT += p.turnaroundTime;
        totalWT += p.waitingTime;
    }
    cout << "\nAvg TAT: " << totalTAT / processes.size() << ", Avg WT: " << totalWT / processes.size() << "\n";
}

// First Come First Serve (FCFS)
void FCFS(vector<Process> processes) {
    sort(processes.begin(), processes.end(), [](Process a, Process b) { return a.arrivalTime < b.arrivalTime; });
    int currentTime = 0;
    vector<pair<int, int>> gantt;
    for (auto &p : processes) {
        currentTime = max(currentTime, p.arrivalTime);
        gantt.push_back({p.id, currentTime});
        p.completionTime = currentTime + p.burstTime;
        p.turnaroundTime = p.completionTime - p.arrivalTime;
        p.waitingTime = p.turnaroundTime - p.burstTime;
        currentTime += p.burstTime;
    }
    printGanttChart(gantt);
    printResults(processes);
}

// Shortest Job First (SJF - Non-Preemptive)
void SJF(vector<Process> processes) {
    int currentTime = 0, completed = 0;
    vector<pair<int, int>> gantt;
    vector<bool> done(processes.size(), false);
    while (completed < processes.size()) {
        int idx = -1, minBurst = 1e9;
        for (int i = 0; i < processes.size(); i++) {
            if (!done[i] && processes[i].arrivalTime <= currentTime && processes[i].burstTime < minBurst) {
                minBurst = processes[i].burstTime;
                idx = i;
            }
        }
        if (idx == -1) { currentTime++; continue; }
        gantt.push_back({processes[idx].id, currentTime});
        processes[idx].completionTime = currentTime + processes[idx].burstTime;
        processes[idx].turnaroundTime = processes[idx].completionTime - processes[idx].arrivalTime;
        processes[idx].waitingTime = processes[idx].turnaroundTime - processes[idx].burstTime;
        currentTime += processes[idx].burstTime;
        done[idx] = true;
        completed++;
    }
    printGanttChart(gantt);
    printResults(processes);
}

// Round Robin (RR)
void RoundRobin(vector<Process> processes, int quantum) {
    queue<int> q;
    int currentTime = 0, i = 0;
    vector<pair<int, int>> gantt;
    for (auto &p : processes) p.remainingTime = p.burstTime;
    while (i < processes.size() && processes[i].arrivalTime == 0) q.push(i++);
    while (!q.empty()) {
        int idx = q.front(); q.pop();
        gantt.push_back({processes[idx].id, currentTime});
        int execTime = min(quantum, processes[idx].remainingTime);
        processes[idx].remainingTime -= execTime;
        currentTime += execTime;
        if (processes[idx].remainingTime == 0) {
            processes[idx].completionTime = currentTime;
            processes[idx].turnaroundTime = processes[idx].completionTime - processes[idx].arrivalTime;
            processes[idx].waitingTime = processes[idx].turnaroundTime - processes[idx].burstTime;
        } else q.push(idx);
        while (i < processes.size() && processes[i].arrivalTime <= currentTime) q.push(i++);
    }
    printGanttChart(gantt);
    printResults(processes);
}

// Priority Scheduling (Non-Preemptive)
void PriorityScheduling(vector<Process> processes) {
    sort(processes.begin(), processes.end(), [](Process a, Process b) {
        return (a.arrivalTime == b.arrivalTime) ? a.priority < b.priority : a.arrivalTime < b.arrivalTime;
    });

    int currentTime = 0;
    vector<pair<int, int>> gantt;
    for (auto &p : processes) {
        currentTime = max(currentTime, p.arrivalTime);
        gantt.push_back({p.id, currentTime});
        p.completionTime = currentTime + p.burstTime;
        p.turnaroundTime = p.completionTime - p.arrivalTime;
        p.waitingTime = p.turnaroundTime - p.burstTime;
        currentTime += p.burstTime;
    }
    printGanttChart(gantt);
    printResults(processes);
}

// Priority Scheduling (Preemptive)
void PrioritySchedulingPreemptive(vector<Process> processes) {
    int currentTime = 0, completed = 0;
    vector<pair<int, int>> gantt;
    vector<bool> done(processes.size(), false);
    for (auto &p : processes) p.remainingTime = p.burstTime;

    while (completed < processes.size()) {
        int idx = -1, highestPriority = 1e9;
        for (int i = 0; i < processes.size(); i++) {
            if (!done[i] && processes[i].arrivalTime <= currentTime && processes[i].priority < highestPriority) {
                highestPriority = processes[i].priority;
                idx = i;
            }
        }
        if (idx == -1) { currentTime++; continue; }
        gantt.push_back({processes[idx].id, currentTime});
        processes[idx].remainingTime--;
        if (processes[idx].remainingTime == 0) {
            processes[idx].completionTime = currentTime + 1;
            processes[idx].turnaroundTime = processes[idx].completionTime - processes[idx].arrivalTime;
            processes[idx].waitingTime = processes[idx].turnaroundTime - processes[idx].burstTime;
            done[idx] = true;
            completed++;
        }
        currentTime++;
    }
    printGanttChart(gantt);
    printResults(processes);
}

int main() {
    int n, choice, quantum;
    cout << "Enter number of processes: ";
    cin >> n;
    vector<Process> processes(n);
    cout << "Enter Arrival Time, Burst Time and Priority for each process:\n";
    for (int i = 0; i < n; i++) {
        cout << "Process " << i + 1 << ": ";
        cin >> processes[i].arrivalTime >> processes[i].burstTime >> processes[i].priority;
        processes[i].id = i + 1;
    }
    cout << "\nChoose scheduling algorithm:\n";
    cout << "1. FCFS\n2. SJF\n3. Round Robin\n4. Priority Scheduling (Non-Preemptive)\n5. Priority Scheduling (Preemptive)\nChoice: ";
    cin >> choice;
    if (choice == 3) {
        cout << "Enter Time Quantum: ";
        cin >> quantum;
    }
    switch (choice) {
        case 1: FCFS(processes); break;
        case 2: SJF(processes); break;
        case 3: RoundRobin(processes, quantum); break;
        case 4: PriorityScheduling(processes); break;
        case 5: PrioritySchedulingPreemptive(processes); break;
        default: cout << "Invalid choice!\n";
    }
    cout << "\nPress Enter to exit...";
    cin.ignore();
    cin.get();
    return 0;
}
