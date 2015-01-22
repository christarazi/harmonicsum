#include <iostream>
#include <iomanip>
#include <thread>
#include <future>
#include <chrono>
#include <vector>
#include <cstddef>
#include <cmath>

/* H(N) is defined to be the harmonic partial sum to N terms. */

// Set number of threads to hardware capability. 
const std::size_t numThreads = std::thread::hardware_concurrency();

/*
	calculateHarmonic(): function given to threads to compute H(N); returns the individual sum the thread was given to compute.
	unsigned long start: starting point for the loop; different for each thread.
	unsigned long N: amount of terms to compute; different for each thread; divided evenly by # of threads.
*/
double calculateHarmonic(long long start, long long N)
{
	double sum = 0.0;
	for(; start <= N; start++)
	{
		sum += 1.0 / start;
	}
	return sum;
}

/* 
	calculateHarmonicKahan(): function given to threads to compute H(N); returns the individual sum the thread was given to compute.
		computes the sum in much more precise manner. Algorithm can be found here: https://en.wikipedia.org/wiki/Kahan_summation_algorithm
	Parameters used are the same as the above function.
*/
double calculateHarmonicKahan(long long start, long long N)
{
	double sum = 0.0;
	double c = 0.0;
	for(; start <= N; start++)
	{
		double y = (1.0/start) - c;
		double t = sum + y;
		c = (t - sum) - y;
		sum = t;
	}
	return sum;
}

void executeWithThreads(long long N)
{
	std::chrono::time_point<std::chrono::system_clock> start, end;	// variables to count runtime
	std::vector<std::future<double>> futures(numThreads);			// vector storing return value from each thread
	double finalSum = 0.0;
	bool sumMethod;		// indicates whether to use regular sum or Kahan sum
	std::cout << "Enter 0 for Regular sum (faster but less precise)\n \tor \nEnter 1 for Kahan sum (slower but more precise): ";
	std::cin >> sumMethod;
	std::cout << "\n";

	start = std::chrono::system_clock::now();
	if(sumMethod)
	{
		// Assign threads their task.
		for(auto i = 0; i < numThreads; i++)
		{
			// std::async([launch policy], [name of function], [starting point], [N for each thread])
			futures[i] = std::async(std::launch::async, calculateHarmonicKahan, ((N / numThreads) * i) + 1, ((N / numThreads) * (i + 1)));
		}
	}
	else
	{
		// Assign threads their task.
		for(auto i = 0; i < numThreads; i++)
		{
			// std::async([launch policy], [name of function], [starting point], [N for each thread])
			futures[i] = std::async(std::launch::async, calculateHarmonic, ((N / numThreads) * i) + 1, ((N / numThreads) * (i + 1)));
		}
	}

	// Get future value from vector and sum it all up.
	for(auto&& i : futures)
	{
		finalSum += i.get();
	}
	end = std::chrono::system_clock::now();

	if(sumMethod)
	{
		std::cout << "Harmonic Kahan sum of " << N  << " is " 
				  << std::setprecision(std::numeric_limits<double>::digits10 + 1) << finalSum << "\n";
	}
	else
	{
		std::cout << "Harmonic sum of " << N  << " is " 
				  << std::setprecision(std::numeric_limits<double>::digits10 + 1) << finalSum << "\n";
	}
	std::cout << "Calculation took " << std::fixed << std::setprecision(std::numeric_limits<double>::digits10 + 1) 
			  << std::chrono::duration<double> (end-start).count() << " seconds with " << numThreads << " threads.\n";
}

void executeNoThreads(long long N)
{
	std::chrono::time_point<std::chrono::system_clock> start, end;
	double sum = 0.0;
	bool sumMethod;		// indicates whether to use regular sum or Kahan sum
	std::cout << "Enter 0 for Regular sum (faster but less precise)\n \tor \nEnter 1 for Kahan sum (slower but more precise): ";
	std::cin >> sumMethod;
	std::cout << "\n";

	start = std::chrono::system_clock::now();
	if(sumMethod)
	{
		sum = calculateHarmonicKahan(1, N);
	}
	else
	{
		sum = calculateHarmonic(1, N);
	}
	end = std::chrono::system_clock::now();

	if(sumMethod)
	{
		std::cout << "Harmonic Kahan sum of " << N << " is " 
				  << std::setprecision(std::numeric_limits<double>::digits10 + 1) << sum << "\n";
	}
	else
	{
		std::cout << "Harmonic sum of " << N << " is " 
				  << std::setprecision(std::numeric_limits<double>::digits10 + 1) << sum << "\n";
	}
	std::cout << "Calculation took " << std::fixed << std::setprecision(std::numeric_limits<double>::digits10 + 1)
			  << std::chrono::duration<double> (end-start).count() << " seconds.\n";
}

int main()
{
	long long N;

	std::cout << numThreads << " threads usable.\n";
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

	if(N > numThreads && N % numThreads == 0)
	{
		std::cout << "Calculating with threads...\n\n";
		executeWithThreads(N);
	}
	else if(N > numThreads && N % numThreads != 0)
	{
		std::cout << "Calculating without threads...\n\n";
		executeNoThreads(N);
	}
	else
	{
		std::cout << "Calculating without threads due to small N...\n\n";
		executeNoThreads(N);
	}

	return 0;
}
