oddEvenSort(numOfProc, *value, myRank, rightProc, leftProc, sortingOrientation) {

	// if(sortingOrientation == accending) {
	// 	keepValue = max ();
	// 	giveValue = min();
	// } else {
	// 	keepValue = min ();
	// 	giveValue = max();
	// }

	otherValue; // value from neighbor process

	for (step = 0; step < numOfProc; step++) {

		if (myRank % 2 == 0) 
		{
			if(step % 2 == 0) 
			{
				if(myRank != numOfProc-1) 
				{	// if there are even amount of processes
					send(myValue, rightProc/*myRank + 1*/)
					recv (otherValue, rightProc/*myRank + 1*/)
					value = (sortingOrientation == accending) ? min(myValue, otherValue) : max(myValue, otherValue)
				}
			}

		else 
		{ //odd step
			if( myRank != 0) 
			{ 
				send(myValue, leftProc/*myRank – 1*/)
				recv(otherValue, leftProc/*myRank -1*/)
				value == (sortingOrientation == accending) ?  max(myValue, otherValue) 	: min(myValue, otherValue)
			}
		}
	}

	else 
	{ // odd rank – check for first or last rank
		if(step % 2 == 0) 
		{
			send(myValue, leftProc/*myRand - 1*/)
			recv (otherValue, leftProc/*myRank - 1*/)
			value == (sortingOrientation == accending) ?  max(myValue, otherValue) : min(myValue, otherValue)
		}

		else 
		{ //odd rank
			if(myRank != numOfProc-1) 
			{
				send(myValue, rightProc/*myRank +1*/)
				recv(otherValue, rightProc/*myRank +1*/)
				value = (sortingOrientation == accending) ? min(myValue, otherValue) : max(myValue, otherValue)
			}

		}
	}
}
}

====================================================================================================================



 shearSort( myId, N, coord[DIMENSIONS], MPI_Comm grid_comm, char* word)
{
	 Direction direct;
	 numOfIterations = 2 * ((int)log2(N)) + 1;
	 firstRank, secondRank;

	for ( i = 0; i < numOfIterations; i++)
	{
		if (i % 2 == 0)//To sort rows
		{
			Cart_shift(grid_comm, 1, 1, &firstRank, &secondRank);
			coord[0] % 2 == 0 ? direct = UP : direct = DOWN;
			oddEvenSort(myId, N, firstRank, secondRank, word, direct);

		}
		else//To sort columns
		{
			Cart_shift(grid_comm, 0, 1, &firstRank, &secondRank);
			direct = UP;
			oddEvenSort(coord[0], N, firstRank, secondRank, word, direct);
		}
	}

}