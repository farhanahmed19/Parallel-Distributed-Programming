#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <ctype.h>

#define MAX_TEXT_SIZE 1000000

void bruteForce(char *text, char *pattern, int textSize, int patternSize, int rank, int size) 
{
    int chunkSize = textSize / size;
    int startIndex = rank * chunkSize;
    int endIndex = (rank == size - 1) ? textSize : startIndex + chunkSize;

    int localCount = 0;

    double compStartTime = MPI_Wtime();

    for (int i = startIndex; i <= endIndex - patternSize; ++i) 
	{
        int j;
        for (j = 0; j < patternSize; ++j) 
		{
            if (text[i + j] != pattern[j])
            {
                break;
            }
        }

        if (j == patternSize) 
		{
            printf("Pattern found at index %d by process %d\n", i, rank);
            localCount++;
        }
    }

    double compEndTime = MPI_Wtime();

    int globalCount;
    double commStartTime = MPI_Wtime();

    MPI_Reduce(&localCount, &globalCount, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    double commEndTime = MPI_Wtime();

    if (rank == 0) 
	{
        printf("Total occurrences: %d\n", globalCount);
        printf("Computation time: %f seconds\n", compEndTime - compStartTime);
        printf("Communication time: %f seconds\n", commEndTime - commStartTime);
    }
}

int main(int argc, char *argv[]) 
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 3) 
	{
        if (rank == 0) 
		{
            printf("Usage: %s <text_file> <pattern>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    char *fileName = argv[1];
    char *pattern = argv[2];

    FILE *file = fopen(fileName, "r");
    if (file == NULL) 
	{
        if (rank == 0) 
		{
            printf("Error opening file %s\n", fileName);
        }
        MPI_Finalize();
        return 1;
    }

    char text[MAX_TEXT_SIZE];
    int printableCharCount = 0;

    int c;
    while ((c = fgetc(file)) != EOF) 
	{
        if (isprint(c)) 
		{
            text[printableCharCount++] = c;
        }
    }

    fclose(file);

    MPI_Bcast(&printableCharCount, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(text, printableCharCount, MPI_CHAR, 0, MPI_COMM_WORLD);

    if (rank == 0) 
	{
        printf("Total characters in the text file: %d\n", printableCharCount);
    }

    int patternSize = strlen(pattern);

    MPI_Barrier(MPI_COMM_WORLD);

    double startTime = MPI_Wtime();

    bruteForce(text, pattern, printableCharCount, patternSize, rank, size);

    MPI_Barrier(MPI_COMM_WORLD);

    double endTime = MPI_Wtime();

    if (rank == 0) 
	{
        printf("Total time taken: %f seconds\n", endTime - startTime);
    }

    MPI_Finalize();

    return 0;
}