#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cctype>
#include <stdlib.h>
#include <string.h>
#include <deque>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

using namespace std;


typedef struct {
	double top;
	double left;
	double right;
	double bottom;
} Rectangle;

// Rectangle sorroundingShape; // Sequential
Rectangle sorroundingShape[8]; // Parallel


typedef struct {
	int x;
	int y;
} Point;


int isPointInsideTheCurve(Point point) {

	double x_square = point.x * point.x;
	double y_square = point.y * point.y;
	double position = (x_square / 10000) + (y_square / 2500) - 1;
	if (position <= 0.0) {
		return true;
	} else {
		return false;
	}	
}


Point generateRandomPointInsideBox(Rectangle box, unsigned int *seed) {

	
	int horizontal = (box.right - box.left);
	int vertical = (box.top - box.bottom);
	
	int x = (rand_r(seed) % horizontal) + box.left;
    int y = (rand_r(seed) % vertical) + box.bottom;

	Point randomPoint;
	randomPoint.x = x;
	randomPoint.y = y;
	return randomPoint;
}

long int iPoints;

double estimateArea(Rectangle boundingBox, int sampleCount, int threadId) {
	
	int mainThreadId = threadId;
	unsigned int threadIdSeed = mainThreadId;

	double estimate = 0.0;
	
	int insidePoints = 0;

	for (int i = 0; i < sampleCount; i++) {
		Point randomPoint = generateRandomPointInsideBox(boundingBox, &threadIdSeed);
		if (isPointInsideTheCurve(randomPoint)) {
			insidePoints++;
		}
	}
	iPoints += insidePoints;
	std::cout << "Total points inside Thread " << threadId << ": " << insidePoints << "\n";

	double boxArea = (boundingBox.right - boundingBox.left) * (boundingBox.top - boundingBox.bottom);
	double curveArea = (boxArea * insidePoints) / sampleCount;

	return curveArea;
}


int THREAD_COUNT = 8;	 

typedef struct {
	int threadId;
	Rectangle shape;
	int count;

} ThreadArg;

double area[THREAD_COUNT];
void *threadFunction(void *arg) {
	
	ThreadArg *argument = (ThreadArg *) arg;
	int threadId = argument->threadId;
	Rectangle shape = argument->shape;
	int count = argument->count;
	cout << "Thread " << threadId << ": says hello\n";
	area[threadId] = estimateArea(shape, count, threadId);
	//cout << "Thread " << threadId << "done\n";
	return NULL;
}


int main(int argc, char *argv[]) {
	
	if (argc < 6) {
		std::cout << "To run the program you have to provide two things.\n";
		std::cout << "\tFirst the number of samples for the Monte Carlo sampling experiments.\n";
		std::cout << "\tSecond the bounding area within which the samples should be generated.\n";
		std::cout << "The format of using the program:\n";
		std::cout << "\t./program_name sample_count bottom_left_x, bottom_left_y, top_right_x, top_right_y\n";
		std::exit(EXIT_FAILURE);
	}

	long int sampleCount = strtol(argv[1], NULL, 10);
	
	// Parallel
	double bottom = strtod(argv[3], NULL);
	double top = strtod(argv[5], NULL);
	double range = (strtod(argv[4], NULL) - strtod(argv[2], NULL))/THREAD_COUNT;
	double left = strtod(argv[2], NULL);
	double right = range;
	for (int i = 0; i < THREAD_COUNT; i++) {
		sorroundingShape[i].left = left;
		sorroundingShape[i].bottom = bottom;
		sorroundingShape[i].right = right;
		sorroundingShape[i].top = top;
		left += range;
		right += range;
	}
	/*
	// Sequential
	sorroundingShape.left = atoi(argv[2]);
	sorroundingShape.bottom = atoi(argv[3]);
	sorroundingShape.right = atoi(argv[4]);
	sorroundingShape.top = atoi(argv[5]);
	*/
	
	int parallelCount = sampleCount/THREAD_COUNT;
	
	// starting execution timer clock
        struct timeval start;
        gettimeofday(&start, NULL);
	
	// initialize the random number generator;
	srand(time(NULL));


	pthread_t *threads;
	ThreadArg *threadArgs;
	threads = (pthread_t *) malloc(THREAD_COUNT * sizeof(pthread_t));
	threadArgs = (ThreadArg *) malloc(THREAD_COUNT * sizeof(ThreadArg));
        for (int i = 0; i < THREAD_COUNT; i++) {
                threadArgs[i].threadId = i;
                threadArgs[i].shape = sorroundingShape[i];
                threadArgs[i].count = parallelCount;
                pthread_create(&threads[i], NULL, threadFunction, (void *) &threadArgs[i]);
        }

        for (int i = 0; i < THREAD_COUNT; i++) {
                pthread_join(threads[i], NULL);
        }
	
	// Sequential
	//double curveArea = estimateArea(sorroundingShape, sampleCount);
	//std::cout << "The area estimate with " << sampleCount << " samples is: " << curveArea << "\n";
	
	// Parallel
	double totalArea = 0;
	for (int i = 0; i < THREAD_COUNT; i++) {
		totalArea += area[i];
	}
    std::cout << "Total points inside: " << iPoints << "\n";
	std::cout << "The area estimate with " << sampleCount << " samples is: " << totalArea << "\n";
	
	 
	//-------------------------------- calculate running time
	struct timeval end;
	gettimeofday(&end, NULL);
	double runningTime = ((end.tv_sec + end.tv_usec / 1000000.0) - (start.tv_sec + start.tv_usec / 1000000.0));
	std::cout << "Execution Time: " << runningTime << " Seconds" << std::endl;
	
	return 0;
}



/*
*************************************************************************************
                                SEQUENTIAL OUTPUT
*************************************************************************************


Total points inside: 564055197
The area estimate with 10000000000 samples is: 4000.21
Execution Time: 98.6336 Seconds

Total points inside: 563973985
The area estimate with 10000000000 samples is: 3999.63
Execution Time: 106.51 Seconds

Total points inside: 564028957
The area estimate with 10000000000 samples is: 4000.02
Execution Time: 106.485 Seconds

Total points inside: 564017490
The area estimate with 10000000000 samples is: 3999.94
Execution Time: 95.4142 Seconds

Total points inside: 563997672
The area estimate with 10000000000 samples is: 3999.8
Execution Time: 96.1398 Seconds



Average Execution Time: 100.63652 Seconds



*************************************************************************************
                                  PARALLEL OUTPUT
*************************************************************************************


Thread 0: says hello
Thread 1: says hello
Thread 3: says hello
Thread 4: says hello
Thread 2: says hello
Thread 5: says hello
Thread 6: says hello
Thread 7: says hello
Total points inside Thread 7: 235414094
Total points inside Thread 6: 376023832
Total points inside Thread 5: 466662035
Total points inside Thread 4: 527083933
Total points inside Thread 3: 571874361
Total points inside Thread 2: 602079101
Total points inside Thread 1: 620827268
Total points inside Thread 0: 626035321
Total points inside: 4025999945
The area estimate with 10000000000 samples is: 4026
Execution Time: 94.643 Seconds

Thread 0: says hello
Thread 4: says hello
Thread 5: says hello
Thread 2: says hello
Thread 6: says hello
Thread 7: says hello
Thread 3: says hello
Thread 1: says hello
Total points inside Thread 7: 235429022
Total points inside Thread 6: 376032379
Total points inside Thread 5: 466657244
Total points inside Thread 4: 527085596
Total points inside Thread 3: 571877169
Total points inside Thread 2: 602093509
Total points inside Thread 1: 620834192
Total points inside Thread 0: 626041265
Total points inside: 4026050376
The area estimate with 10000000000 samples is: 4026.05
Execution Time: 92.7571 Seconds

Thread 0: says hello
Thread 4: says hello
Thread 2: says hello
Thread 1: says hello
Thread 3: says hello
Thread 5: says hello
Thread 6: says hello
Thread 7: says hello
Total points inside Thread 7: 235415895
Total points inside Thread 6: 376030031
Total points inside Thread 5: 466660712
Total points inside Thread 2: 602091021
Total points inside Thread 4: 527085760
Total points inside Thread 0: 626041265
Total points inside Thread 3: 571876987
Total points inside Thread 1: 620833666
Total points inside: 4026035337
The area estimate with 10000000000 samples is: 4026.04
Execution Time: 90.5372 Seconds

Thread 0: says hello
Thread 1: says hello
Thread 2: says hello
Thread 4: says hello
Thread 6: says hello
Thread 5: says hello
Thread 7: says hello
Thread 3: says hello
Total points inside Thread 7: 235415895
Total points inside Thread 6: 376030031
Total points inside Thread 5: 466660712
Total points inside Thread 3: 571876987
Total points inside Thread 4: 527085760
Total points inside Thread 2: 602091021
Total points inside Thread 1: 620833666
Total points inside Thread 0: 626041265
Total points inside: 4026035337
The area estimate with 10000000000 samples is: 4026.04
Execution Time: 89.8985 Seconds

Thread 0: says hello
Thread 2: says hello
Thread 1: says hello
Thread 4: says hello
Thread 3: says hello
Thread 5: says hello
Thread 6: says hello
Thread 7: says hello
Total points inside Thread 7: 235415895
Total points inside Thread 6: 376030031
Total points inside Thread 5: 466660712
Total points inside Thread 4: 527085760
Total points inside Thread 2: 602091021
Total points inside Thread 1: 620833666
Total points inside Thread 0: 626041265
Total points inside Thread 3: 571876987
Total points inside: 4026035337
The area estimate with 10000000000 samples is: 4026.04
Execution Time: 92.1336 Seconds


Average Execution Time: 91.99388 Seconds


*/
