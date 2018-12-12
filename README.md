# ShearsortAlgorithm
Implement parallel Shearsort Algorithm to sort a set of words in lexicographic order using MPI Cartesian Topology, Scatter and Gather

## Requirements:

•	One of the processes reads all words from the file. This process will display the words before and after the sort. It uses Scatter to send words to processes, Gather to collect results. 

•	Use Cartesian Topology for communication between processors during sorting.

•	At the end of the program the points are displayed in descending order.

•	Use Odd Even Sort to sort rows and columns

•	Assume that n is even

•	The input file is organized as following:

Number of words

Word1

Word2

…

WordN

