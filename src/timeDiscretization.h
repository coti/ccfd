/** \file
 *
 * \author hhh
 * \date Sat 21 Mar 2020 07:48:34 PM CET
 */

#ifndef TIMEDISCRETIZATION_H
#define TIMEDISCRETIZATION_H

#include <stdbool.h>
#include <time.h>

#ifdef _OPENMP
/**
 * \brief Get the CPU time for a parallel program
 */
#	define CPU_TIME() (omp_get_wtime())
#else
/**
 * \brief Get the CPU time for a serial program
 */
#	define CPU_TIME() ((double)clock() / (double)CLOCKS_PER_SEC)
#endif

extern double	cfl;
extern double	dfl;
extern double	t;

extern double	timeOverall;

extern int	timeOrder;
extern bool	isTimeStep1D;

extern bool	isStationary;
extern long	maxIter;
extern double	stopTime;
extern long	iniIterationNumber;
extern double	startTime;
extern double	abortResidual;
extern int	abortVariable;
extern char	abortVariableName[4];
extern double	clAbortResidual;
extern double	cdAbortResidual;
extern bool	doAbortOnClResidual;
extern bool	doAbortOnCdResidual;
extern bool	isRestart;
extern double	restartTime;

extern int	printIter;
extern double	printTime;

extern int	nRKstages;
extern double	RKcoeff[6];
extern bool	isImplicit;

void initTimeDisc(void);
void timeDisc(void);

#endif
