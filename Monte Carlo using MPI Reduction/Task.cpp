#include <mpi.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

// function declarations
void reductionMonteCarlo(int myRank, int procCount);

typedef struct {
	double top;
	double left;
	double right;
	double bottom;
} Rectangle;

Rectangle sorroundingShape[8];

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

double estimateArea(Rectangle boundingBox, int sampleCount, int rank) {
	
	int mainRank = rank;
	unsigned int rankSeed = mainRank;

	double estimate = 0.0;
	
	int insidePoints = 0;

	for (int i = 0; i < sampleCount; i++) {
		Point randomPoint = generateRandomPointInsideBox(boundingBox, &rankSeed);
		if (isPointInsideTheCurve(randomPoint)) {
			insidePoints++;
		}
	}
	iPoints += insidePoints;
	printf("Total Points Inside Rank %d: %d\n", rank, insidePoints);

	double boxArea = (boundingBox.right - boundingBox.left) * (boundingBox.top - boundingBox.bottom);
	double curveArea = (boxArea * insidePoints) / sampleCount;

	return curveArea;
}

int main(int argc, char** argv) {

    // Shape 0, 0 to 100, 100
    double bottom = 0;
    double top = 100;
    double range = (100 - 0)/8;
    double left = 0;
    double right = range;
    for (int i = 0; i < 8; i++) {
        sorroundingShape[i].left = left;
        sorroundingShape[i].bottom = bottom;
        sorroundingShape[i].right = right;
        sorroundingShape[i].top = top;
        left += range;
        right += range;
    }
    
    struct timeval start;
    gettimeofday(&start, NULL);
    
    
    MPI_Init(NULL, NULL);

    int procCount;
    MPI_Comm_size(MPI_COMM_WORLD, &procCount);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    reductionMonteCarlo(rank, procCount);
    

    struct timeval end;
    gettimeofday(&end, NULL);
    double runningTime = ((end.tv_sec + end.tv_usec / 1000000.0) - (start.tv_sec + start.tv_usec / 1000000.0));
    printf("Execution Time: %f Seconds\n", runningTime);
    MPI_Finalize();

    return 0;
}

void reductionMonteCarlo(int myRank, int procCount) {

	Rectangle shape = sorroundingShape[myRank];
	int count = 1250000000;
	
	int dataToReduce = estimateArea(shape, count, myRank);
	
	int reductionResult = 0;

	int root = 0; // here the rank zero process is the root of reduction
	MPI_Reduce(&dataToReduce, &reductionResult, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	printf("Rank %d: Total Area: %d\n", myRank, reductionResult);
}

/*

*************************************************************************************
                                      OUTPUT
*************************************************************************************


Total Points Inside Rank 7: 280198728
Rank 7: Total Area: 0
Execution Time: 69.954408 Seconds
Total Points Inside Rank 6: 399994648
Rank 6: Total Area: 0
Execution Time: 73.571998 Seconds
Total Points Inside Rank 5: 478115693
Rank 5: Total Area: 0
Execution Time: 76.630746 Seconds
Total Points Inside Rank 4: 534369555
Rank 4: Total Area: 0
Execution Time: 79.990284 Seconds
Total Points Inside Rank 3: 574998674
Rank 3: Total Area: 0
Execution Time: 80.298790 Seconds
Total Points Inside Rank 2: 604174056
Rank 2: Total Area: 0
Execution Time: 81.827606 Seconds
Total Points Inside Rank 1: 620833666
Rank 1: Total Area: 0
Execution Time: 82.448153 Seconds
Total Points Inside Rank 0: 626041265
Rank 0: Total Area: 3948
Execution Time: 82.591253 Seconds

*/
