// Hadar Pur 308248533
// Sergei Dvorjin 316859552
#define _CRT_SECURE_NO_WARNINGS
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define DIMENSIONAL 2
#define MASTER 0

const char* FILE_PATH = "<replace with your local path>...\\SortOddEven\\SortOddEven\\words.txt";
const int MAX_LENGTH = 16;

//UP for ascending, DOWN for descending order
enum Direction { UP, DOWN };

//Prints all the words as matrix
void matrixPrintWords(char* words, int N)
{
	printf("print all words as matrix\n"); 
	fflush(stdout);
	for (int i = 0; i < N*N*MAX_LENGTH; i+=MAX_LENGTH)
	{
		if (i%(N*MAX_LENGTH)==0)
		{
			printf("\n");
			fflush(stdout);
		}
		printf("%-16s ", words+i);
		fflush(stdout);
	}
	printf("\n\n"); 
	fflush(stdout);
}

//prints all the words in a snake route.
void snakePrintWords(char* words, int N)
{
	printf("print all words as snake\n");
	fflush(stdout);
	for (int i = 0, isOdd = 0; i < N*N*MAX_LENGTH; i += MAX_LENGTH*N, isOdd++)
	{
		if (isOdd%2 == 0)
		{
			for (int j = i; j < i + (N*MAX_LENGTH) ; j += MAX_LENGTH) {
				printf("%-16s ", words + j);
				fflush(stdout);
			}
		}
		else
		{
			for (int j = i + (N*MAX_LENGTH) - MAX_LENGTH; j >= i; j -= MAX_LENGTH) {
				printf("%-16s ", words + j);
				fflush(stdout);
			}
		}
	}
	printf("\n\n");
	fflush(stdout);
}

void printAllWords(char* words, int numOfWords)
{
	int N = (int)sqrt((double)numOfWords);
	matrixPrintWords(words, N);
	snakePrintWords(words, N);
}

//Ensure that there are not a memory leak.
void checkAllocation(const void *ptr)
{
	if (!ptr)
	{
		printf("Not Enough Memory!");
		fflush(stdout);
		MPI_Finalize();
		exit(1);
	}
}

//Reads words from file and calculate each word's lexicographic value
char* readWordsFile(int* numOfWords) {
	const char* WORDS_FILE = FILE_PATH;
	FILE* file = fopen(WORDS_FILE, "r");

	if (!file)
	{
		printf("could not open the file ");
		fflush(stdout);
		MPI_Finalize();
		exit(1);
	}
	//Get the amount of words in the file
	fscanf(file, "%d", numOfWords);
	//Dynamic allocation of memory to the words array
	char* words = (char*)malloc(sizeof(char)*(*numOfWords)*MAX_LENGTH);
	//Checks if the words allocation is successful.
	checkAllocation(words);

	//read and initalize the words array with the appropriate words from the file
	for (int i = 0; i < *numOfWords; i++)
	{
		fscanf(file, "%s\n", words+ i*MAX_LENGTH);
	}
	fclose(file);
	return words;
}

double log2(double n)
{
	return ((log(n)) / (log(2.0)));
}

//save the word with the greater lexicographic value in the current process
void maxValue(char* rankWord, char* recvWord)
{
	if (strcmp(rankWord, recvWord) <= 0)
	{
		strcpy(rankWord, recvWord);
	}
}

//save the word with the smaller lexicographic value in the current process
void minValue(char* rankWord, char* recvWord)
{
	if (strcmp(rankWord, recvWord) > 0)
	{
		strcpy(rankWord, recvWord);
	}
}

void oddEvenSort(int myId, int N, int firstRank, int secondRank, char* rankWord, enum Direction direct)
{
	MPI_Status status;
	char* recvWord = (char*)malloc(sizeof(char)*MAX_LENGTH);
	for (int step = 0; step < N; step++)
	{
		if (step % 2 != 0)//Odd
		{
			if (((myId%N) != 0) && ((myId%N) != (N - 1)))
			{
				if (myId % 2 != 0)//odd
				{
					MPI_Send(rankWord ,MAX_LENGTH, MPI_CHAR, secondRank, 0, MPI_COMM_WORLD);
					MPI_Recv(recvWord, MAX_LENGTH, MPI_CHAR, secondRank, 0, MPI_COMM_WORLD, &status);
					direct == UP ? minValue(rankWord, recvWord) : maxValue(rankWord, recvWord);
				}
				else //If myId is Even
				{
					MPI_Recv(recvWord, MAX_LENGTH, MPI_CHAR, firstRank, 0, MPI_COMM_WORLD, &status);
					MPI_Send(rankWord, MAX_LENGTH, MPI_CHAR, firstRank, 0, MPI_COMM_WORLD);
					direct == UP ? maxValue(rankWord, recvWord) : minValue(rankWord, recvWord);
				}
			}
		}
		else//If step is Even
		{
			if (myId % 2 != 0)//odd
			{
				MPI_Send(rankWord, MAX_LENGTH, MPI_CHAR, firstRank, 0, MPI_COMM_WORLD);
				MPI_Recv(recvWord, MAX_LENGTH, MPI_CHAR, firstRank, 0, MPI_COMM_WORLD, &status);
				direct == UP ? maxValue(rankWord, recvWord) : minValue(rankWord, recvWord);
			}
			else //myId is Even
			{
				MPI_Recv(recvWord, MAX_LENGTH, MPI_CHAR, secondRank, 0, MPI_COMM_WORLD, &status);
				MPI_Send(rankWord, MAX_LENGTH, MPI_CHAR, secondRank, 0, MPI_COMM_WORLD);
				direct == UP ? minValue(rankWord, recvWord) : maxValue(rankWord, recvWord);
			}
		}
	}
}

//even iteration for row sort, odd Iteration for column sort
void shearSort(int myId, int N, int coord[DIMENSIONAL], MPI_Comm grid_comm, char* word)
{
	enum Direction direct;
	int numOfIterations = 2 * ((int)log2(N)) + 1;
	int firstRank, secondRank;

	for (int i = 0; i < numOfIterations; i++)
	{
		if (i % 2 == 0)//To sort rows
		{
			MPI_Cart_shift(grid_comm, 1, 1, &firstRank, &secondRank);
			coord[0] % 2 == 0 ? direct = UP : direct = DOWN;
			oddEvenSort(myId, N, firstRank, secondRank, word, direct);

		}
		else//To sort columns
		{
			MPI_Cart_shift(grid_comm, 0, 1, &firstRank, &secondRank);
			direct = UP;
			oddEvenSort(coord[0], N, firstRank, secondRank, word, direct);
		}
	}

}

void main(int argc, char *argv[])
{
	int namelen, numprocs, myId, numOfWords;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int dim[DIMENSIONAL], coord[DIMENSIONAL], period[DIMENSIONAL], reorder;
	char word[MAX_LENGTH];
	char* words;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myId);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Get_processor_name(processor_name, &namelen);
	MPI_Status status;
	MPI_Comm grid_comm;

	if (myId == 0)
	{
		//Obtain the words from the file
		words = readWordsFile(&numOfWords);
		//Checks if the number of words is equal to the number of activate process in the program
		if (numOfWords != numprocs)
		{
			printf("the number of words is not equal to the number of processes.\nthe program abort\n"); 
			fflush(stdout);
			MPI_Abort(MPI_COMM_WORLD, 1);
		}
		printf("\nBefore: shear-sort\n------------------\n\n"); 
		fflush(stdout);
		printAllWords(words, numOfWords);
		printf("\n----------------------------------------------------------\n"); 
		fflush(stdout);
	}

	//All the processes have to wait to the root process and the words initalization
	MPI_Barrier(MPI_COMM_WORLD);

	//updating all processes about the numOfWords
	MPI_Bcast(&numOfWords, 1, MPI_INT, MASTER, MPI_COMM_WORLD);

	dim[0] = dim[1] = (int)sqrt((double)numOfWords);
	period[0] = period[1] = 0;
	reorder = 0;

	//Process 0 gives to each process a word
	MPI_Scatter(words, MAX_LENGTH, MPI_CHAR, word, MAX_LENGTH, MPI_CHAR, MASTER, MPI_COMM_WORLD);

	//Creation of grid communicator
	MPI_Cart_create(MPI_COMM_WORLD, DIMENSIONAL, dim, period, reorder, &grid_comm);

	//Each process obtain his lexicographic word value according to the grid communicator
	MPI_Cart_coords(grid_comm, myId, DIMENSIONAL, coord);

	//Each process do a shearSort for to obtain the appropriate word
	shearSort(myId, dim[0], coord, grid_comm, word);

	//Process 0 allocate from each process a word
	MPI_Gather(word, MAX_LENGTH, MPI_CHAR, words, MAX_LENGTH, MPI_CHAR, MASTER, MPI_COMM_WORLD);

	if (myId == 0)
	{
		printf("\nafter shear-sort:\n-----------------\n\n\n"); 
		fflush(stdout);
		printAllWords(words, numOfWords);
		//free memory allocation
		free(words);
	}

	MPI_Finalize();
}
