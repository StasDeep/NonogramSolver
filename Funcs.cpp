#include "stdafx.h"
#include "Funcs.h"

/* Initialize dynamic 2D array of integer numbers. */
int **CreateArr(int m, int n)
{
	int **arr = new int *[m];
	for (int i = 0; i < m; i++)
		arr[i] = new int[n];
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			arr[i][j] = 0;
	return arr;
}

/* Number to string function. */
std::string N2S(int a)
{
	std::stringstream ss;
	ss << a;
	std::string str;
	ss >> str;
	return str;
}
