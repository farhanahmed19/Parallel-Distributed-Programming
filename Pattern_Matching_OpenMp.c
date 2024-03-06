#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <omp.h>

#define MAX_TEXT_SIZE 1000000

void bruteForce(char *text, char *pattern, int textSize, int patternSize) 
{
    int localCount = 0;

    double compStartTime = omp_get_wtime();

    #pragma omp parallel for reduction(+:localCount)
    for (int i = 0; i <= textSize - patternSize; ++i) 
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
            printf("Pattern found at index %d by thread %d\n", i, omp_get_thread_num());
            localCount++;
        }
    }

    double compEndTime = omp_get_wtime();

    int globalCount;

    double commStartTime = omp_get_wtime();

    globalCount = localCount;

    double commEndTime = omp_get_wtime();

    #pragma omp master
    {
        printf("Total occurrences: %d\n", globalCount);
        printf("Computation time: %f seconds\n", compEndTime - compStartTime);
        printf("Communication time: %f seconds\n", commEndTime - commStartTime);
    }
}

int main(int argc, char *argv[]) 
{
    if (argc != 3) 
	{
        printf("Usage: %s <text_file> <pattern>\n", argv[0]);
        return 1;
    }

    char *fileName = argv[1];
    char *pattern = argv[2];

    FILE *file = fopen(fileName, "r");
    if (file == NULL) 
	{
        printf("Error opening file %s\n", fileName);
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

    printf("Total characters in the text file: %d\n", printableCharCount);

    int patternSize = strlen(pattern);

    omp_set_num_threads(8);

    double startTime = omp_get_wtime();

    bruteForce(text, pattern, printableCharCount, patternSize);

    double endTime = omp_get_wtime();

    printf("Total time taken: %f seconds\n", endTime - startTime);

    return 0;
}