#include <stdio.h>
#include <stdlib.h>
#define INFINITY 99999

struct Process {
  int arrival, burst, turnaround, wait, priority, start, remaining_time;
  char name[10];
};

// function declarations
void get_details(struct Process *proc, int num_processes, int choice);
void FCFS(struct Process *proc, int num_processes);
void SJF(struct Process *proc, int num_processes);
void Priority(struct Process *proc, int num_processes);
void RoundRobin(struct Process *proc, int num_processes, int quantum);

int main() {
  int choice;

  while (1) {
    printf("\n\n\t--- SCHEDULING ALGORITHMS ---\n");
    printf("\t1. FCFS\n");
    printf("\t2. SJF\n");
    printf("\t3. Priority Based\n");
    printf("\t4. Round Robin\n");
    printf("\t5. EXIT\n\n");
    printf("\t Enter your choice: ");
    scanf(" %d", &choice); // Added space before %d to consume newline character

    if (choice == 1 || choice == 2 || choice == 3 || choice == 4) {
      int num_processes;
      printf("\tHow many processes: ");
      scanf("%d", &num_processes);

      struct Process proc[num_processes];
      struct Process *sptr = proc;

      get_details(sptr, num_processes, choice);

      if (choice == 1) {
        FCFS(sptr, num_processes);
      } else if (choice == 2) {
        SJF(sptr, num_processes);
      } else if (choice == 3) {
        Priority(sptr, num_processes);
      } else if (choice == 4) {
        int quantum;
        printf("\tEnter time quantum: ");
        scanf("%d", &quantum);
        RoundRobin(sptr, num_processes, quantum);
      }
    } else if (choice == 5) {
      printf("\n\tGoodbye!");
      exit(0);
    } else {
      printf("\n\tInput is wrong, Please choose the corresponding number.");
    }
  }
  return 0;
}

// Method to get arrival, burst time, and priority
void get_details(struct Process *proc, int num_processes, int choice) {
  printf("\tENTER THE VALUES\n\n");
  printf("\tEnter for each process: [Arrival Time] [Burst Time]");

  printf("\n");

  for (int i = 0; i < num_processes; i++) {
    printf("\tEnter for Process %d: ", i);
    scanf("%d %d", &proc[i].arrival, &proc[i].burst);
    proc[i].remaining_time = proc[i].burst;

    if (choice == 3) {
      printf("\tEnter Priority: ");
      scanf("%d", &proc[i].priority);
    }

    sprintf(proc[i].name, "P%d", i);
  }
}

void FCFS(struct Process *proc, int num_processes) {
  printf("\n\t--- First Come First Serve ---\n");

  // Sort processes based on arrival time
  for (int i = 0; i < num_processes - 1; i++) {
    for (int j = i + 1; j < num_processes; j++) {
      if (proc[i].arrival > proc[j].arrival) {
        struct Process temp = proc[i];
        proc[i] = proc[j];
        proc[j] = temp;
      }
    }
  }

  int total_wait = 0;
  int total_turnaround = 0;
  int current_time = 0;

  printf("\n\tGANTT CHART\n");
  for (int i = 0; i < num_processes; i++) {
    printf("\t%s", proc[i].name);
  }
  printf("\n");

  for (int i = 0; i < num_processes; i++) {
    // Calculate waiting time
    if (current_time < proc[i].arrival) {
      current_time = proc[i].arrival;
    }
    proc[i].start = current_time;
    proc[i].wait = current_time - proc[i].arrival;
    total_wait += proc[i].wait;

    // Calculate turnaround time
    current_time += proc[i].burst;
    proc[i].turnaround = current_time - proc[i].arrival;
    total_turnaround += proc[i].turnaround;

    // Print Gantt chart
    printf("\t %d", proc[i].start);
    if (i == num_processes - 1) {
      printf("\t %d", current_time);
    }
  }
  printf("\n");

  // Calculate average turnaround time and average waiting time
  float ave_wait = total_wait / (float)num_processes;
  float ave_turnaround = total_turnaround / (float)num_processes;

  // Display average turnaround time and average waiting time
  printf("\n\t||  Average turnaround time: %.2f  ||\n\t||  Average waiting "
         "time: %.2f  ||\n",
         ave_turnaround, ave_wait);
}

void SJF(struct Process *proc, int num_processes) {
  printf("\n\t--- Shortest Job First (SJF) ---\n");

  // Sort processes based on burst time and arrival time
  for (int i = 0; i < num_processes - 1; i++) {
    for (int j = i + 1; j < num_processes; j++) {
      // Compare burst time and arrival time
      if (proc[i].burst > proc[j].burst ||
          (proc[i].burst == proc[j].burst &&
           proc[i].arrival > proc[j].arrival)) {
        struct Process temp = proc[i];
        proc[i] = proc[j];
        proc[j] = temp;
      }
    }
  }

  // Find each process's waiting time and turnaround time
  int total_wait = 0;
  int total_turnaround = 0;
  int current_time = 0;

  // Array to keep track of whether a process has been executed
  int executed[num_processes];
  for (int i = 0; i < num_processes; i++) {
    executed[i] = 0;
  }

  // Loop until all processes are executed
  while (1) {
    int shortest_index = -1;

    // Find the process with the shortest remaining burst time that has arrived
    for (int i = 0; i < num_processes; i++) {
      if (!executed[i] && proc[i].arrival <= current_time) {
        if (shortest_index == -1 ||
            proc[i].burst < proc[shortest_index].burst) {
          shortest_index = i;
        }
      }
    }

    if (shortest_index == -1) {
      // If no process is ready to execute, move time to the next arrival
      int next_arrival = INFINITY;
      for (int i = 0; i < num_processes; i++) {
        if (!executed[i] && proc[i].arrival < next_arrival) {
          next_arrival = proc[i].arrival;
        }
      }
      current_time = next_arrival;
      continue;
    }

    // Execute the shortest job
    proc[shortest_index].start = current_time;
    proc[shortest_index].wait = current_time - proc[shortest_index].arrival;
    current_time += proc[shortest_index].burst;
    proc[shortest_index].turnaround =
        current_time - proc[shortest_index].arrival;
    total_wait += proc[shortest_index].wait;
    total_turnaround += proc[shortest_index].turnaround;
    executed[shortest_index] = 1;

    // Check if all processes have been executed
    int all_executed = 1;
    for (int i = 0; i < num_processes; i++) {
      if (!executed[i]) {
        all_executed = 0;
        break;
      }
    }

    if (all_executed) {
      break;
    }
  }

  // calculate average wait and average turnaround time
  float ave_wait = total_wait / (float)num_processes;
  float ave_turnaround = total_turnaround / (float)num_processes;

  // Display the Gantt chart with burst times in order
  printf("\n\tGANTT CHART\n");
  int order[num_processes];
  for (int i = 0; i < num_processes; i++) {
    order[i] = i; // Initialize order
  }

  // Sort order based on start time
  for (int i = 0; i < num_processes - 1; i++) {
    for (int j = i + 1; j < num_processes; j++) {
      if (proc[order[i]].start > proc[order[j]].start) {
        int temp = order[i];
        order[i] = order[j];
        order[j] = temp;
      }
    }
  }

  // Print Gantt chart in order
  for (int i = 0; i < num_processes; i++) {
    printf("\t%s", proc[order[i]].name);
  }
  printf("\n");

  for (int i = 0; i < num_processes; i++) {
    printf("\t %d", proc[order[i]].start);
    if (i == num_processes - 1) {
      printf("\t %d", proc[order[i]].start + proc[order[i]].burst);
    }
  }
  printf("\n");

  // Display average turnaround time and average waiting time
  printf("\n\t||  Average turnaround time: %.2f  ||\n\t||  Average waiting "
         "time: %.2f  ||\n",
         ave_turnaround, ave_wait);
}

void Priority(struct Process *proc, int num_processes) {
  printf("\n\t--- Priority-Based Scheduling ---\n");

  int total_wait = 0;
  int total_turnaround = 0;
  int current_time = 0;

  // Array to keep track of whether a process has been executed
  int executed[num_processes];
  for (int i = 0; i < num_processes; i++) {
    executed[i] = 0;
  }

  printf("\n\tGANTT CHART\n");
  int chart_index = 0;
  int chart_times[num_processes + 1];

  // Loop until all processes are executed
  while (1) {
    int highest_priority_index = -1;

    // Find the process with the highest priority that has arrived
    for (int i = 0; i < num_processes; i++) {
      if (!executed[i] && proc[i].arrival <= current_time) {
        if (highest_priority_index == -1 ||
            proc[i].priority < proc[highest_priority_index].priority ||
            (proc[i].priority == proc[highest_priority_index].priority &&
             proc[i].arrival < proc[highest_priority_index].arrival)) {
          highest_priority_index = i;
        }
      }
    }

    // If no process is found that has arrived, move time to the next arrival
    if (highest_priority_index == -1) {
      int next_arrival = INFINITY;
      for (int i = 0; i < num_processes; i++) {
        if (!executed[i] && proc[i].arrival < next_arrival) {
          next_arrival = proc[i].arrival;
        }
      }
      current_time = next_arrival;
      continue;
    }

    // Execute the highest priority process
    struct Process *p = &proc[highest_priority_index];
    if (current_time < p->arrival) {
      current_time = p->arrival;
    }
    chart_times[chart_index++] = current_time;
    p->start = current_time;
    p->wait = current_time - p->arrival;
    total_wait += p->wait;
    current_time += p->burst;
    p->turnaround = current_time - p->arrival;
    total_turnaround += p->turnaround;
    executed[highest_priority_index] = 1;

    // Print Gantt chart
    printf("\t%s", p->name);

    // Check if all processes have been executed
    int all_executed = 1;
    for (int i = 0; i < num_processes; i++) {
      if (!executed[i]) {
        all_executed = 0;
        break;
      }
    }

    if (all_executed) {
      break;
    }
  }
  chart_times[chart_index] = current_time;
  printf("\n");

  // Print the start times in the Gantt chart
  printf("\t%d", chart_times[0]);
  for (int i = 1; i <= num_processes; i++) {
    printf("\t%d", chart_times[i]);
  }
  printf("\n");

  // Calculate average turnaround time and average waiting time
  float ave_wait = total_wait / (float)num_processes;
  float ave_turnaround = total_turnaround / (float)num_processes;

  // Display average turnaround time and average waiting time
  printf("\n\t||  Average turnaround time: %.2f  ||\n\t||  Average waiting "
         "time: %.2f  ||\n",
         ave_turnaround, ave_wait);
}

void RoundRobin(struct Process *proc, int num_processes, int quantum) {
  printf("\n\t--- Round Robin Scheduling ---\n");

  int current_time = 0;
  int total_wait = 0;
  int total_turnaround = 0;

  // Array to keep track of completion status of each process
  int completed[num_processes];
  for (int i = 0; i < num_processes; i++) {
    completed[i] = 0;
  }

  printf("\n\tGANTT CHART\n");
  printf("    | 0 |\n");

  while (1) {
    int all_completed = 1; // Flag to check if all processes are completed
    for (int i = 0; i < num_processes; i++) {
      if (proc[i].remaining_time > 0) {
        all_completed = 0;
        if (proc[i].arrival <= current_time) {
          printf(" %s |", proc[i].name);
          if (proc[i].remaining_time > quantum) {
            printf(" %d |\n", current_time + quantum);
            current_time += quantum;
            proc[i].remaining_time -= quantum;
          } else {
            printf(" %d |\n", current_time + proc[i].remaining_time);
            current_time += proc[i].remaining_time;
            proc[i].wait = current_time - proc[i].burst - proc[i].arrival;
            proc[i].turnaround = current_time - proc[i].arrival;
            total_wait += proc[i].wait;
            total_turnaround += proc[i].turnaround;
            proc[i].remaining_time = 0;
          }
        }
      }
    }
    if (all_completed) {
      break;
    }
  }

  printf("\n");

  // calculate for average wait and average turnaround time
  float ave_wait = total_wait / (float)num_processes;
  float ave_turnaround = total_turnaround / (float)num_processes;

  // Display average turnaround time and average waiting time
  printf("\n\t||  Average turnaround time: %.2f  ||\n\t||  Average waiting "
         "time: %.2f  ||\n",
         ave_turnaround, ave_wait);
}
