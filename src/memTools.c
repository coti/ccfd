/*
 * memTools.c
 *
 * Created: Fri 27 Mar 2020 02:38:03 PM CET
 * Author : hhh
 *
 * Manual Memory Management:
 *
 * " The manual type involves malloc and free, and is where most of your segfaults
 * happen. This memory model is why Jesus weeps when he has to code in C. "
 *	- Ben Klemens
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgnslib.h"

/*
 * allocate a dynamic 2D array of integers
 */
long **dyn2DintArray(long I, long J)
{
	long **arr = calloc(1, sizeof(long *) * I + sizeof(long) * I * J);
	if (!arr) {
		printf("| ERROR: could not allocate arr\n");
		exit(1);
	}

	long *ptr = (long *)(arr + I);
	for (long i = 0; i < I; ++i) {
		arr[i] = ptr + J * i;
	}
	return arr;
}

/*
 * allocate a dynamic 2D array of integers
 */
cgsize_t **dyn2DcgsizeArray(long I, long J)
{
	cgsize_t **arr = calloc(1, sizeof(cgsize_t *) * I + sizeof(cgsize_t) * I * J);
	if (!arr) {
		printf("| ERROR: could not allocate arr\n");
		exit(1);
	}

	cgsize_t *ptr = (cgsize_t *)(arr + I);
	for (long i = 0; i < I; ++i) {
		arr[i] = ptr + J * i;
	}
	return arr;
}

/*
 * allocate a dynamic 2D array of doubles
 */
double **dyn2DdblArray(long I, long J)
{
	double **arr = calloc(1, sizeof(double *) * I + sizeof(double) * I * J);
	if (!arr) {
		printf("| ERROR: could not allocate arr\n");
		exit(1);
	}

	double *ptr = (double *)(arr + I);
	for (long i = 0; i < I; ++i) {
		arr[i] = ptr + J * i;
	}
	return arr;
}

/*
 * allocate a dynamic 3D array of integers
 */
long ***dyn3DintArray(long I, long J, long K)
{
	long ***arr = calloc(1, sizeof(long *) * I + sizeof(long **) * I * J + sizeof(long) * I * J * K);
	if (!arr) {
		printf("| ERROR: could not allocate arr\n");
		exit(1);
	}

	long **ptrI = (long **)(arr + I);
	long *ptrJ = (long *)(arr + I + I * J);
	for (long i = 0; i < I; ++i) {
		arr[i] = ptrI + J * i;
		for (long j = 0; j < J; ++j) {
			arr[i][j] = ptrJ + J * K * i + K * j;
		}
	}
	return arr;
}

/*
 * allocate a dynamic 3D array of doubles
 */
double ***dyn3DdblArray(long I, long J, long K)
{
	double ***arr = calloc(1, sizeof(double *) * I + sizeof(double **) * I * J + sizeof(double) * I * J * K);
	if (!arr) {
		printf("| ERROR: could not allocate arr\n");
		exit(1);
	}

	double **ptrI = (double **)(arr + I);
	double *ptrJ = (double *)(arr + I + I * J);
	for (long i = 0; i < I; ++i) {
		arr[i] = ptrI + J * i;
		for (long j = 0; j < J; ++j) {
			arr[i][j] = ptrJ + J * K * i + K * j;
		}
	}
	return arr;
}

/*
 * allocate a dynamic 4D array of doubles
 */
double ****dyn4DdblArray(long I, long J, long K, long L)
{
	double ****arr = calloc(1, sizeof(double *) * I + sizeof(double **) * I * J + sizeof(double ***) * I * J * K + sizeof(double) * I * J * K * L);
	if (!arr) {
		printf("| ERROR: could not allocate arr\n");
		exit(1);
	}

	double ***ptrI = (double ***)(arr + I);
	double **ptrJ = (double **)(arr + I + I * J);
	double *ptrK = (double *)(arr + I + I * J + I * J * K);
	for (long i = 0; i < I; ++i) {
		arr[i] = ptrI + J * i;
		for (long j = 0; j < J; ++j) {
			arr[i][j] = ptrJ + J * K * i + K * j;
			for (long k = 0; k < K; ++k) {
				arr[i][j][k] = ptrK + J * K * L * i + K * L * j + L * k;
			}
		}
	}
	return arr;
}

/*
 * allocate a dynamic array of strings
 */
char **dynStringArray(long I, long J)
{
	char **arr = calloc(1, sizeof(char *) * I + sizeof(char) * I * J);
	if (!arr) {
		printf("| ERROR: could not allocate arr\n");
		exit(1);
	}

	char *ptr = (char *)(arr + I);
	for (long i = 0; i < I; ++i) {
		arr[i] = ptr + J * i;
	}
	return arr;
}
