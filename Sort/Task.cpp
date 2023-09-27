#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int compare(const void* numA, const void* numB);
int main(int argc, char *argv[]) {
	clock_t start_time;
    	start_time = clock();
	if (argc < 3) {
		printf("You have not supplied enough command line parameters\n");
		printf("Usage: ./program-name ${input-file-path} ${number-of-integers-to-sort}\n");
		exit(-1);
	}
	FILE *fp;
	if ((fp = fopen(argv[1], "r+")) == NULL) {
        	printf("Could not open the file. probably the file does not exist.\n");
        	exit(-1);
    	}
	int count = atoi(argv[2]);
	int *array = (int *) malloc(count * sizeof(int));
	for (int i = 0; i < count; i++) {
		int status = fscanf(fp, "%d", &array[i]);
		if (status != 1) {
			printf("Some error happened when reading numbers from file. Only read %d items.\n", i);
			exit(-1);
		}	
	}
	clock_t end_time1;
	end_time1 = clock();
	qsort(array, count, sizeof(int), compare);
	
	clock_t end_time2;
	end_time2 = clock();
	
	printf("The sorted array is:\n");
	for (int i = 0; i < count; i++) {
		printf("%d ", array[i]);
	} 
	printf("\n");

	clock_t end_time3;
	end_time3 = clock();

	double time_taken1 =  ((double) (end_time1 - start_time)) / CLOCKS_PER_SEC;
	double time_taken2 =  ((double) (end_time2 - end_time1)) / CLOCKS_PER_SEC;
	double time_taken3 =  ((double) (end_time3 - end_time2)) / CLOCKS_PER_SEC;
	double time_taken4 =  ((double) (end_time3 - start_time)) / CLOCKS_PER_SEC;
	printf("\nTotal time taken to read the input array is %f seconds.\n", time_taken1);
	printf("Total time taken to sort the array is %f seconds.\n", time_taken2);
	printf("Total time taken to print the sorted array is %f seconds.\n", time_taken3);
	printf("Total time taken to run the program is %f seconds.\n", time_taken4);

	fclose(fp);
	free(array);
	return 0;
}

int compare(const void* numA, const void* numB) {
    
	const int* num1 = (const int*)numA;
	const int* num2 = (const int*)numB;

    	if (*num1 > *num2) {
		return 1;
   	}
    	else {
        	if (*num1 == *num2) {
            		return 0;
		}
        	else {
            		return -1;
		}
    	}
}

