#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

const int NUM_THREADS = 4;

// H(N) is defined to be the harmonic partial sum to N terms.
// calculateHarmonic(): function given to threads to compute H(N).
// unsigned long start: starting point for the loop; different for each thread.
// unsigned long N: amount of terms to compute; different for each thread; divided evenly by # of threads.
// double *sum: pointer to a double which holds a thread's individual sum for H(N).
void calculateHarmonic(long long start, long long N, double *sum)
{
	for(; start <= N; start++)
	{
		*sum += 1.0/start;
	}
}

void executeWithThreads(long long N)
{
	std::thread t[NUM_THREADS];			// using four threads
	std::chrono::time_point<std::chrono::system_clock> start, end;
	double sum[NUM_THREADS] = {0.0};	// array of doubles for each sum calculated by each thread
	double finalSum = 0.0;	
	start = std::chrono::system_clock::now();

	// Assign threads their task.
	for(int i = 0; i < NUM_THREADS; i++)
	{
		// thread([name of function], [starting point], [N], [address of individual element in sum array])
		t[i] = std::thread(calculateHarmonic, (((N/NUM_THREADS)*i)+1), ((N/NUM_THREADS)*(i+1)), &(sum[i]));
	}

	// Join threads to terminate and sum up all individual sums into finalSum.
	for(int i = 0; i < NUM_THREADS; i++)
	{
		t[i].join();
		finalSum += sum[i];
	}

	end = std::chrono::system_clock::now();
	std::cout << "Harmonic sum of " << N  << " is " << std::setprecision(std::numeric_limits<double>::digits10 + 1) << finalSum << "\n";
	std::cout << "Calculation took " << std::setprecision(std::numeric_limits<double>::digits10 + 1) 
			  << std::chrono::duration<double> (end-start).count() << " seconds.\n";
}

void executeNoThreads(long long N)
{
	std::chrono::time_point<std::chrono::system_clock> start, end;
	double sum = 0.0;
	start = std::chrono::system_clock::now();

	for(int i = 1; i <= N; i++)
	{
		sum += 1.0/i;
	}

	end = std::chrono::system_clock::now();
	std::cout << "Harmonic sum of " << N << " is " << std::setprecision(std::numeric_limits<double>::digits10 + 1) << sum << "\n";
	std::cout << "Calculation took " << std::setprecision(std::numeric_limits<double>::digits10 + 1)
			  << std::chrono::duration<double> (end-start).count() << " seconds.\n";
}

int main()
{
	long long N;

	std::cout << "Enter the amount of terms to calculate the Harmonic sum to: ";
	std::cin >> N;
	if(N <= 0)
	{
		// Using std::endl here to force a buffer flush.
		std::cerr << "N must be a nonzero positive integer." << std::endl;
		std::cerr << "Returning " << EXIT_FAILURE << " from main()..." << std::endl;
		return EXIT_FAILURE;
	}
	std::cout << "\n";

	if(N > NUM_THREADS && N % NUM_THREADS == 0)
	{
		std::cout << "Calculating with threads...\n";
		executeWithThreads(N);
	}
	else if(N > NUM_THREADS && N % NUM_THREADS != 0)
	{
		std::cout << "Calculating without threads...\n";
		executeNoThreads(N);
	}
	else
	{
		std::cout << "Calculating without threads due to small N...\n";
		executeNoThreads(N);
	}

	return 0;
}
