package main

import (
	"fmt"
	"io"
	"sort"
)

type (
	Process struct {
		ProcessID     string
		ArrivalTime   int64
		BurstDuration int64
		Priority      int64
	}
	TimeSlice struct {
		PID   string
		Start int64
		Stop  int64
	}
)

//region Schedulers

// FCFSSchedule outputs a schedule of processes in a GANTT chart and a table of timing given:
// • an output writer
// • a title for the chart
// • a slice of processes
func FCFSSchedule(w io.Writer, title string, processes []Process) {
	var (
		serviceTime     int64
		totalWait       float64
		totalTurnaround float64
		lastCompletion  float64
		waitingTime     int64
		schedule        = make([][]string, len(processes))
		gantt           = make([]TimeSlice, 0)
	)
	for i := range processes {
		if processes[i].ArrivalTime > 0 {
			waitingTime = serviceTime - processes[i].ArrivalTime
		}
		totalWait += float64(waitingTime)

		start := waitingTime + processes[i].ArrivalTime

		turnaround := processes[i].BurstDuration + waitingTime
		totalTurnaround += float64(turnaround)

		completion := processes[i].BurstDuration + processes[i].ArrivalTime + waitingTime
		lastCompletion = float64(completion)

		schedule[i] = []string{
			fmt.Sprint(processes[i].ProcessID),
			fmt.Sprint(processes[i].Priority),
			fmt.Sprint(processes[i].BurstDuration),
			fmt.Sprint(processes[i].ArrivalTime),
			fmt.Sprint(waitingTime),
			fmt.Sprint(turnaround),
			fmt.Sprint(completion),
		}
		serviceTime += processes[i].BurstDuration

		gantt = append(gantt, TimeSlice{
			PID:   processes[i].ProcessID,
			Start: start,
			Stop:  serviceTime,
		})
	}

	count := float64(len(processes))
	aveWait := totalWait / count
	aveTurnaround := totalTurnaround / count
	aveThroughput := count / lastCompletion

	outputTitle(w, title)
	outputGantt(w, gantt)
	outputSchedule(w, schedule, aveWait, aveTurnaround, aveThroughput)
}

func SJFSchedule(w io.Writer, title string, processes []Process) {
	var currentTime int64
	var totalWaitTime int64
	var totalTurnaroundTime int64
	var gantt []TimeSlice
	var schedule [][]string

	processQueue := make([]Process, 0)
	completedProcesses := make([]Process, 0)

	// Make a copy of processes to track remaining burst times without altering original slice
	processesCopy := append([]Process(nil), processes...)

	// Continue looping until all processes are completed
	for len(completedProcesses) < len(processes) {
		// Add processes that have arrived to the queue
		for i, proc := range processesCopy {
			if proc.ArrivalTime <= currentTime && proc.BurstDuration > 0 {
				processQueue = append(processQueue, proc)
				// Mark as added by setting BurstDuration to -1 in the copy
				processesCopy[i].BurstDuration = -1
			}
		}

		// Sort the processQueue by BurstDuration (for those with BurstDuration > 0)
		sort.SliceStable(processQueue, func(i, j int) bool {
			return processQueue[i].BurstDuration < processQueue[j].BurstDuration
		})

		if len(processQueue) > 0 {
			currentProcess := processQueue[0]
			processQueue = processQueue[1:] // Dequeue the selected process

			// Calculate wait time (currentTime - arrivalTime)
			waitTime := currentTime - currentProcess.ArrivalTime
			totalWaitTime += waitTime

			// Execute the process
			executionStart := currentTime
			currentTime += currentProcess.BurstDuration // Advance currentTime by BurstDuration
			executionEnd := currentTime

			// Calculate turnaround time (currentTime - arrivalTime)
			turnaroundTime := currentTime - currentProcess.ArrivalTime
			totalTurnaroundTime += turnaroundTime

			// Record execution for Gantt chart
			gantt = append(gantt, TimeSlice{PID: currentProcess.ProcessID, Start: executionStart, Stop: executionEnd})

			// Add to completed processes
			completedProcesses = append(completedProcesses, currentProcess)

			// Add to schedule for detailed output
			schedule = append(schedule, []string{
				currentProcess.ProcessID,
				fmt.Sprint(currentProcess.Priority),
				fmt.Sprint(currentProcess.BurstDuration),
				fmt.Sprint(currentProcess.ArrivalTime),
				fmt.Sprint(waitTime),
				fmt.Sprint(turnaroundTime),
				fmt.Sprint(executionEnd),
			})
		} else {
			// If no process is ready to execute, advance time
			currentTime++
		}
	}

	// Calculate and display metrics
	averageWaitTime := float64(totalWaitTime) / float64(len(processes))
	averageTurnaroundTime := float64(totalTurnaroundTime) / float64(len(processes))
	throughput := float64(len(processes)) / float64(currentTime)

	outputTitle(w, title)
	outputGantt(w, gantt)
	outputSchedule(w, schedule, averageWaitTime, averageTurnaroundTime, throughput)
}

// Helper function to check if a process is in a slice
func contains(slice []Process, proc Process) bool {
	for _, item := range slice {
		if item.ProcessID == proc.ProcessID {
			return true
		}
	}
	return false
}

func SJFPrioritySchedule(w io.Writer, title string, processes []Process) {
	var currentTime int64
	var totalWaitTime int64
	var totalTurnaroundTime int64
	var gantt []TimeSlice
	var schedule [][]string

	processQueue := make([]Process, 0)
	completedProcesses := make([]Process, 0)

	// Continuously process until all processes are completed
	for len(completedProcesses) < len(processes) {
		// Add eligible processes to the queue
		for i := range processes {
			proc := &processes[i]
			if proc.ArrivalTime <= currentTime && !contains(completedProcesses, *proc) && !contains(processQueue, *proc) {
				processQueue = append(processQueue, *proc)
			}
		}

		// Sort the queue by priority, then by burst duration for same priority
		sort.SliceStable(processQueue, func(i, j int) bool {
			if processQueue[i].Priority == processQueue[j].Priority {
				return processQueue[i].BurstDuration < processQueue[j].BurstDuration
			}
			return processQueue[i].Priority < processQueue[j].Priority
		})

		if len(processQueue) > 0 {
			currentProcess := processQueue[0]
			processQueue = processQueue[1:] // Dequeue the selected process

			// Execute the process
			waitTime := currentTime - currentProcess.ArrivalTime
			totalWaitTime += waitTime

			executionStart := currentTime
			currentTime += currentProcess.BurstDuration
			executionEnd := currentTime

			turnaroundTime := currentTime - currentProcess.ArrivalTime
			totalTurnaroundTime += turnaroundTime

			gantt = append(gantt, TimeSlice{PID: currentProcess.ProcessID, Start: executionStart, Stop: executionEnd})
			completedProcesses = append(completedProcesses, currentProcess)

			schedule = append(schedule, []string{
				currentProcess.ProcessID,
				fmt.Sprint(currentProcess.Priority),
				fmt.Sprint(currentProcess.BurstDuration),
				fmt.Sprint(currentProcess.ArrivalTime),
				fmt.Sprint(waitTime),
				fmt.Sprint(turnaroundTime),
				fmt.Sprint(executionEnd),
			})
		} else {
			// If no process is ready, advance time
			currentTime++
		}
	}

	// Calculate and display metrics
	averageWaitTime := float64(totalWaitTime) / float64(len(processes))
	averageTurnaroundTime := float64(totalTurnaroundTime) / float64(len(processes))
	throughput := float64(len(completedProcesses)) / float64(currentTime)

	outputTitle(w, title)
	outputGantt(w, gantt)
	outputSchedule(w, schedule, averageWaitTime, averageTurnaroundTime, throughput)
}

func RRSchedule(w io.Writer, title string, processes []Process) {
	const timeQuantum int64 = 4 // Time quantum for RR scheduling
	var currentTime int64 = 0
	var totalWaitTime int64 = 0
	var totalTurnaroundTime int64 = 0
	processQueue := make([]Process, len(processes)) // Use a slice to simulate a queue
	copy(processQueue, processes)

	// Sort processes by their arrival time to simulate arrival order
	sort.Slice(processQueue, func(i, j int) bool {
		return processQueue[i].ArrivalTime < processQueue[j].ArrivalTime
	})

	// Create a slice to hold the execution timeline for each process (for Gantt chart)
	var executionTimeline []TimeSlice

	for len(processQueue) > 0 {
		process := processQueue[0]      // Get the first process in the queue
		processQueue = processQueue[1:] // Dequeue

		executionStart := currentTime
		executionEnd := executionStart + min(process.BurstDuration, timeQuantum)

		// Update process burst duration or mark as completed
		process.BurstDuration -= min(process.BurstDuration, timeQuantum)
		if process.BurstDuration > 0 {
			// If process is not completed, enqueue it again
			processQueue = append(processQueue, process)
		} else {
			// Process completed, calculate wait and turnaround time
			totalTurnaroundTime += currentTime + (executionEnd - executionStart) - process.ArrivalTime
			totalWaitTime += currentTime - process.ArrivalTime
		}

		// Update current time
		currentTime += executionEnd - executionStart

		// Add execution time slice for Gantt chart
		executionTimeline = append(executionTimeline, TimeSlice{
			PID:   process.ProcessID,
			Start: executionStart,
			Stop:  executionEnd,
		})
	}

	// Calculate and display metrics
	averageWaitTime := float64(totalWaitTime) / float64(len(processes))
	averageTurnaroundTime := float64(totalTurnaroundTime) / float64(len(processes))
	throughput := float64(len(processes)) / float64(currentTime)

	outputTitle(w, title)
	outputGantt(w, executionTimeline) // You might need to implement or adjust this function based on your setup
	fmt.Fprintf(w, "Average Wait Time: %.2f\n", averageWaitTime)
	fmt.Fprintf(w, "Average Turnaround Time: %.2f\n", averageTurnaroundTime)
	fmt.Fprintf(w, "Throughput: %.2f processes/unit time\n", throughput)
}

// Helper function to find the minimum of two int64 values
func min(a, b int64) int64 {
	if a < b {
		return a
	}
	return b
}

//endregion
