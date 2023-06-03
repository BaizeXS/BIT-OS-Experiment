/* Process Scheduling Algorithms */
#include <iostream>
#include <algorithm>
#include <cstdio>
#include <queue>

using namespace std;

/*
 * Process Control Block Table:
 * 1. Singleton: 
 *      This should be a singleton structure, which is also a global variable that may be accessed by other processes.
 * 2. Signal Handler: (TODO)
 *      Another thing to note is that maybe I should implement a signal handler to make scheduling easier.
 */
struct task_struct {
    int pid;                // Process ID: [0, 65535]
    int status;             // Status(Maybe enum value)

    int t_arr;              // Arrival Time
    int t_run_init;         // Initial Burst Time:          t_run_init      = t_run_exec + t_run_rest
    int t_run_exec;         // Executed Burst Time:         t_run_exec      = t_stop - t_start
    int t_run_rest;         // Rest Burst Time:             t_run_rest      = t_run_init - t_run_exec
    int t_exec_start;       // Execution Start Time:        t_exec_start    = time_now()
    int t_exec_stop;        // Execution Stop Time:         t_exec_stop     = time_now()

    int order;              // Execution Order
    int priority;           // Execution Priority
    int slot;               // Slot
    bool finished;          // Finished Tag
    bool in_queue;          // Flag
} pcb_table[1024];          // Process Control Block Table(Maybe from 0 to 65535)
int pcb_cnt = 0;            // PCB Counter

/*
 * Scheduling Algorithms:
 * 1. FCFS:
 * 2. SJF:
 * 3. SRTF:
 * 4. RR:
 * 5. DPSA:
 */
void FCFS();
void SJF();
void SRTF();
void RR();
void DPSA();

int main() {
    // 0. Initialize
    int algNum = 0;
    // 1. Get the number of scheduling algorithms
    scanf("%d", &algNum);
    // 2. Read parameters from the command line
    while (~scanf("%d/%d/%d/%d/%d", &pcb_table[pcb_cnt].pid, &pcb_table[pcb_cnt].t_arr, &pcb_table[pcb_cnt].t_run_init, &pcb_table[pcb_cnt].priority, &pcb_table[pcb_cnt].slot)) {
        pcb_table[pcb_cnt].t_run_exec = 0;
        pcb_table[pcb_cnt].t_run_rest = pcb_table[pcb_cnt].t_run_init;
        pcb_table[pcb_cnt].t_exec_start = 0;
        pcb_table[pcb_cnt].t_exec_stop = 0;
        pcb_table[pcb_cnt].order = 0;
        pcb_table[pcb_cnt].finished = false;
        pcb_cnt++;
    }
    // 3. Call the algorithm
    switch (algNum) {
        case 1: FCFS(); break;
        case 2: SJF(); break;
        case 3: SRTF(); break;
        case 4: RR(); break;
        case 5: DPSA(); break;
    }
    return 0;
}

void FCFS() {
    // 0. Initialize
    int clock = 0;  // Clock to Record Time
    // 1. Sort the PCB Table
    sort(pcb_table, pcb_table + pcb_cnt, [](task_struct a, task_struct b) {
        if (a.t_arr != b.t_arr) return a.t_arr < b.t_arr;
        else return a.pid < b.pid;
    });
    // 2. Call the FCFS algorithm
    for (int i = 0; i < pcb_cnt; i++) {
        // (1) Set non-preemptive parameters
        pcb_table[i].t_run_exec = pcb_table[i].t_run_init;
        pcb_table[i].t_run_rest = pcb_table[i].t_run_init - pcb_table[i].t_run_exec;
        // (2) Get the start time and the stop time
        clock = max(clock, pcb_table[i].t_arr);
        pcb_table[i].t_exec_start = clock;
        pcb_table[i].t_exec_stop = pcb_table[i].t_exec_start + pcb_table[i].t_run_exec;
        clock = pcb_table[i].t_exec_stop;
        // (3) Set order and finish labels
        pcb_table[i].order = i + 1;
        pcb_table[i].finished = true;
        // (4) Output the results
        printf("%d/%d/%d/%d/%d\n", pcb_table[i].order, pcb_table[i].pid, pcb_table[i].t_exec_start, pcb_table[i].t_exec_stop, pcb_table[i].priority);
    }
}

void SJF() {
    // 0. Initialize
    int clock = 0;                      // Clock to Record Time
    int curr_tag = 0;                   // Current Process Subscript
    int finished_proc_cnt = 0;          // Total Number of Finished Processes
    for (int i = 0; i < pcb_cnt; i++) { // Set Non-preemptive Parameters
        pcb_table[i].t_run_exec = pcb_table[i].t_run_init;
        pcb_table[i].t_run_rest = pcb_table[i].t_run_init - pcb_table[i].t_run_exec;
    }
    // 1. Sort the PCB Table
    sort(pcb_table, pcb_table + pcb_cnt, [](task_struct a, task_struct b) {
        if (a.t_arr != b.t_arr) return a.t_arr < b.t_arr;
        else if (a.t_run_init != b.t_run_init) return a.t_run_init < b.t_run_init;
        else return a.pid < b.pid;
    });
    // 2. Call the SJF algorithm
    while (finished_proc_cnt != pcb_cnt) {
        // (1) Select a process that hasn't been finished(Except the first process)
        if (curr_tag != 0) {
            for (int i = 0; i < pcb_cnt; i++) {
                if (!pcb_table[i].finished) { curr_tag = i; break; }
            }
        }
        // (2) Select a process whose execution burst time is the shortest
        for (int i = 0; i < pcb_cnt; i++) {
            if (!pcb_table[i].finished && pcb_table[i].t_arr <= clock && \
                ((pcb_table[i].t_run_exec < pcb_table[curr_tag].t_run_exec) || (pcb_table[i].t_run_exec == pcb_table[curr_tag].t_run_exec && pcb_table[i].pid < pcb_table[curr_tag].pid))) {
                curr_tag = i;
            }
        }
        // (3) Execute the process
        finished_proc_cnt++;
        // (4) Get the start time
        clock = max(clock, pcb_table[curr_tag].t_arr);
        pcb_table[curr_tag].t_exec_start = clock;
        pcb_table[curr_tag].t_exec_stop = pcb_table[curr_tag].t_exec_start + pcb_table[curr_tag].t_run_exec;
        clock = pcb_table[curr_tag].t_exec_stop;
        // (5) Set order and finish labels
        pcb_table[curr_tag].order = finished_proc_cnt;
        pcb_table[curr_tag].finished = true;
        // (6) Output the results
        printf("%d/%d/%d/%d/%d\n", pcb_table[curr_tag].order, pcb_table[curr_tag].pid, pcb_table[curr_tag].t_exec_start, pcb_table[curr_tag].t_exec_stop, pcb_table[curr_tag].priority);
    }
}

void SRTF() {
    // 0. Initialize
    int clock = 0;                      // Clock to Record Time
    int order = 1;                      // Order
    int curr_tag = 0;                   // Current Process Subscript
    int prev_tag = 0;                   // Previous Process Subscript
    int t_fnshall = 0;                  // Time to finish all the processes
    for (int i = 0; i < pcb_cnt; i++) { // Get the t_fnshall and the arrival timestamps and initialize the rest running time
        t_fnshall += pcb_table[i].t_run_init;
        pcb_table[i].t_run_rest = pcb_table[i].t_run_init;
    }
    // 1. Sort the PCB Table
    sort(pcb_table, pcb_table + pcb_cnt, [](task_struct a, task_struct b) {
        if (a.t_arr != b.t_arr) return a.t_arr < b.t_arr;
        else if (a.t_run_init != b.t_run_init) return a.t_run_init < b.t_run_init;
        else return a.pid < b.pid;
    });
    // 2. Call the SRTF algorithm
    while (clock < t_fnshall) {
        // (1) Select the smallest job
        for (int i = 0; i < pcb_cnt; i++) {
            if (pcb_table[i].finished == false && pcb_table[i].t_arr <= clock && pcb_table[i].t_run_rest < pcb_table[curr_tag].t_run_rest) {
                curr_tag = i;
            }
        }
        // (3) Get the start time
        if (curr_tag != prev_tag) {
            pcb_table[curr_tag].t_exec_start = clock;
        }
        pcb_table[curr_tag].t_run_rest -= 1;
        clock += 1;
        // printf("curr_tag = %d, run_rest: %d\n", curr_tag, pcb_table[curr_tag].t_run_rest);
        if (curr_tag != prev_tag && pcb_table[prev_tag].finished == false) {
            pcb_table[prev_tag].t_exec_stop = clock - 1;
            pcb_table[prev_tag].order = order++;
            // output
            printf("%d/%d/%d/%d/%d\n", pcb_table[prev_tag].order, pcb_table[prev_tag].pid, pcb_table[prev_tag].t_exec_start, pcb_table[prev_tag].t_exec_stop, pcb_table[prev_tag].priority);
        }
        if (pcb_table[curr_tag].t_run_rest <= 0) {
            pcb_table[curr_tag].finished = true;
            pcb_table[curr_tag].t_exec_stop = clock;
            pcb_table[curr_tag].order = order++;
            printf("%d/%d/%d/%d/%d\n", pcb_table[curr_tag].order, pcb_table[curr_tag].pid, pcb_table[curr_tag].t_exec_start, pcb_table[curr_tag].t_exec_stop, pcb_table[curr_tag].priority);
        }
        prev_tag = curr_tag;
        if (pcb_table[curr_tag].finished) {
            for (int i = 0; i < pcb_cnt; i++) {
                if (pcb_table[i].finished == false) { curr_tag = i; break; }
            }
        }
    }
}

void RR() {
    // 0. Initialize
    int clock = 0;
    int order = 1;
    int t_temp = 0;
    int t_comp_tmp = 0;
    int all_finished = 0;
    task_struct tmp_task;
    queue<task_struct> qready;
    // 1. Sort the PCB Table
    sort(pcb_table, pcb_table + pcb_cnt, [](task_struct a, task_struct b) {
        if (a.t_arr != b.t_arr) return a.t_arr < b.t_arr;
        else return a.pid < b.pid;
    });
    // 2. Get the t_fnshall and the arrival timestamps and initialize the rest running time
    for (int i = 0; i < pcb_cnt; i++) {
        pcb_table[i].in_queue = false;
        pcb_table[i].t_run_rest = pcb_table[i].t_run_init;
    }
    // 2. Call the RR algorithm
    qready.push(pcb_table[0]);
    pcb_table[0].in_queue = true;
    while (all_finished != pcb_cnt) {
        tmp_task = qready.front();
        qready.pop();

        t_temp = min(tmp_task.slot, tmp_task.t_run_rest);
        t_comp_tmp = tmp_task.t_run_rest;
        tmp_task.order = order++;
        clock = max(clock, tmp_task.t_arr);
        tmp_task.t_exec_start = clock;
        tmp_task.t_exec_stop = tmp_task.t_exec_start + t_temp;
        tmp_task.t_run_rest -= t_temp;
        printf("%d/%d/%d/%d/%d\n", tmp_task.order, tmp_task.pid, tmp_task.t_exec_start, tmp_task.t_exec_stop, tmp_task.priority);

        clock = tmp_task.t_exec_stop;

        for (int i = 0; i < pcb_cnt; i++) {
            if (pcb_table[i].t_arr <= clock && !pcb_table[i].in_queue) {
                qready.push(pcb_table[i]);
                pcb_table[i].in_queue = true;
            }
        }
        if (tmp_task.slot < t_comp_tmp) { 
            qready.push(tmp_task); 
        } else {
            all_finished++;
        }
    }
}

void DPSA() {
    // 0. Set variables
    int clock = 0;                              // Clock to record current time
    int curr_tag = 0;                           // Subscription for current process
    int order = 1;                              // Sequence number for recording
    int all_finished = 0;                       // Flag to exit when all tasks have been finished
    // 1. Initialize variables
    for (int i = 0; i < pcb_cnt; i++) {         // Initialize original waitTimes
        pcb_table[i].finished = false;
        pcb_table[i].in_queue = false;
        pcb_table[i].t_run_exec = 0;
        pcb_table[i].t_run_rest = pcb_table[i].t_run_init;
    }
    // 2. Sort the PCB Table
    sort(pcb_table, pcb_table + pcb_cnt, [](task_struct a, task_struct b) {
        if (a.t_arr != b.t_arr) return a.t_arr < b.t_arr;
        else return a.pid < b.pid;
    });
    // 2. Call the DPSA algorithm
    while (all_finished < pcb_cnt) {
        // 2.1  Select the next process:
        //          if the ready queue is not empty, select the next process from the queue, 
        //          otherwise select from the rest not int the queue who comes first.
        // 2.1.1    Check if the queue is empty
        int in_queue = 0;
        for (int i = 0; i < pcb_cnt; i++) {
            if (pcb_table[i].in_queue) {
                in_queue++;
            }
        }
        // 2.1.2    Select the next process depending on the check result
        if (in_queue == 0) {
            int tmp_tag;
            for (int i = 0; i < pcb_cnt; i++) {
                if (!pcb_table[i].finished) {
                    tmp_tag = i;
                    break;
                }
            }
            for (int i = 0; i < pcb_cnt; i++) {
                if (!pcb_table[i].finished && pcb_table[i].t_arr < pcb_table[tmp_tag].t_arr) {
                    tmp_tag = i;
                } else if (!pcb_table[i].finished && pcb_table[i].t_arr == pcb_table[tmp_tag].t_arr && pcb_table[i].priority < pcb_table[tmp_tag].priority) {
                    tmp_tag = i;
                }
            }
            curr_tag = tmp_tag;
        } else {
            int tmp_tag;
            for (int i = 0; i < pcb_cnt; i++) {
                if (pcb_table[i].in_queue) {
                    tmp_tag = i;
                    break;
                }
            }
            for (int i = 0; i < pcb_cnt; i++) {
                if (pcb_table[i].in_queue && pcb_table[i].priority < pcb_table[tmp_tag].priority) {
                    tmp_tag = i;
                } else if (pcb_table[i].in_queue && pcb_table[i].priority == pcb_table[tmp_tag].priority && pcb_table[i].t_arr < pcb_table[tmp_tag].t_arr) {
                    tmp_tag = i;
                }
            }
            curr_tag = tmp_tag;
        }
        
        // 2.2  Run the Process
        // 2.2.1    Update the time to know the start time of current process
        clock = max(clock, pcb_table[curr_tag].t_arr);
        // 2.2.2    Execute the process
        pcb_table[curr_tag].t_exec_start = clock;
        pcb_table[curr_tag].t_run_exec = min(pcb_table[curr_tag].slot, pcb_table[curr_tag].t_run_rest);
        pcb_table[curr_tag].t_exec_stop = pcb_table[curr_tag].t_exec_start + pcb_table[curr_tag].t_run_exec;
        pcb_table[curr_tag].t_run_rest -= pcb_table[curr_tag].t_run_exec;
        pcb_table[curr_tag].order = order++;
        // 2.3  Execution is finished
        // 2.3.1    Update the clock again as it is at the stop time of current process
        clock = pcb_table[curr_tag].t_exec_stop;
        // 2.3.2    Update the priority of the current process
        pcb_table[curr_tag].priority += 3;
        // 2.3.3    Update the ready queue: if current process is finished, update its states
        if (pcb_table[curr_tag].t_run_rest > 0) {
            pcb_table[curr_tag].in_queue = true;
        } else {
            // Finished is true now
            pcb_table[curr_tag].finished = true;
            // Remove the process from ready queue
            pcb_table[curr_tag].in_queue = false;
            // Finished tag adds up
            all_finished++;
        }
        // 2.4  Output the result
        printf("%d/%d/%d/%d/%d\n", pcb_table[curr_tag].order, pcb_table[curr_tag].pid, pcb_table[curr_tag].t_exec_start, pcb_table[curr_tag].t_exec_stop, pcb_table[curr_tag].priority);

        // 2.5  Update the ready queue
        for (int i = 0;  i < pcb_cnt; i++) {
            if (!pcb_table[i].finished && !pcb_table[i].in_queue && pcb_table[i].t_arr <= clock) {
                pcb_table[i].in_queue = true;
            }
        }
        // 2.6  Update the priority of the rest processes
        for (int i = 0; i < pcb_cnt; i++) {
            if (i != curr_tag && pcb_table[i].in_queue && pcb_table[i].t_arr < clock) {
                if (pcb_table[i].priority > 0) {
                    pcb_table[i].priority -= 1;
                }
            }
        }
    }  
}
