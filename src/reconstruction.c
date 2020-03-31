/*
 * reconstruction.c
 *
 * Created: Sat 28 Mar 2020 10:17:02 AM CET
 * Author : hhh
 */

#include <math.h>

#include "main.h"
#include "reconstruction.h"
#include "finiteVolume.h"
#include "mesh.h"

/* extern variables */
int limiter;
double venk_k;
int nVarGrad;

/*
 * limiter after Barth & Jespersen
 * 2D, unstructured limiter
 */
void limiterBarthJespersen(elem_t *aElem)
{
	/* determine uMin and uMax */
	double uMin[NVAR], uMax[NVAR];
	uMin[RHO] = uMax[RHO] = aElem->pVar[RHO];
	uMin[VX]  = uMax[VX]  = aElem->pVar[VX];
	uMin[VY]  = uMax[VY]  = aElem->pVar[VY];
	uMin[P]   = uMax[P]   = aElem->pVar[P];

	side_t *aSide = aElem->firstSide;
	while (aSide) {
		uMin[RHO] = fmin(uMin[RHO], aSide->connection->elem->pVar[RHO]);
		uMin[VX]  = fmin(uMin[VX],  aSide->connection->elem->pVar[VX]);
		uMin[VY]  = fmin(uMin[VY],  aSide->connection->elem->pVar[VY]);
		uMin[P]   = fmin(uMin[P],   aSide->connection->elem->pVar[P]);

		uMax[RHO] = fmax(uMax[RHO], aSide->connection->elem->pVar[RHO]);
		uMax[VX]  = fmax(uMax[VX],  aSide->connection->elem->pVar[VX]);
		uMax[VY]  = fmax(uMax[VY],  aSide->connection->elem->pVar[VY]);
		uMax[P]   = fmax(uMax[P],   aSide->connection->elem->pVar[P]);

		aSide = aSide->nextElemSide;
	}

	double minDiff[NVAR], maxDiff[NVAR];
	minDiff[RHO] = uMin[RHO] - aElem->pVar[RHO];
	minDiff[VX]  = uMin[VX]  - aElem->pVar[VX];
	minDiff[VY]  = uMin[VY]  - aElem->pVar[VY];
	minDiff[P]   = uMin[P]   - aElem->pVar[P];

	maxDiff[RHO] = uMax[RHO] - aElem->pVar[RHO];
	maxDiff[VX]  = uMax[VX]  - aElem->pVar[VX];
	maxDiff[VY]  = uMax[VY]  - aElem->pVar[VY];
	maxDiff[P]   = uMax[P]   - aElem->pVar[P];

	/* loop over all edges: determine phi */
	double phi[NVAR] = {1.0}, phiLoc[NVAR], uDiff[NVAR];
	aSide = aElem->firstSide;
	while (aSide) {
		phiLoc[RHO] = 1.0;
		phiLoc[VX]  = 1.0;
		phiLoc[VY]  = 1.0;
		phiLoc[P]   = 1.0;
		for (int iVar = 0; iVar < NVAR; ++iVar) {
			uDiff[iVar] = aElem->u_x[iVar] * aSide->GP[X]
				    + aElem->u_y[iVar] * aSide->GP[Y];
			if (uDiff[iVar] > 0.0) {
				phiLoc[iVar] = fmin(1.0, maxDiff[iVar] / uDiff[iVar]);
			} else if (uDiff[iVar] < 0.0) {
				phiLoc[iVar] = fmin(1.0, minDiff[iVar] / uDiff[iVar]);
			}
		}
		phi[RHO] = fmin(phi[RHO], phiLoc[RHO]);
		phi[VX]  = fmin(phi[VX],  phiLoc[VX]);
		phi[VY]  = fmin(phi[VY],  phiLoc[VY]);
		phi[P]   = fmin(phi[P],   phiLoc[P]);

		aSide = aSide->nextElemSide;
	}

	/* compute limited gradients */
	aElem->u_x[RHO] *= phi[RHO];
	aElem->u_x[VX]  *= phi[VX];
	aElem->u_x[VY]  *= phi[VY];
	aElem->u_x[P]   *= phi[P];

	aElem->u_y[RHO] *= phi[RHO];
	aElem->u_y[VX]  *= phi[VX];
	aElem->u_y[VY]  *= phi[VY];
	aElem->u_y[P]   *= phi[P];
}

/*
 * limiter Venkatakrishnan, with additional limiting parameter k
 * 2D, unstructured limiter
 */
void limiterVenkatakrishnan(elem_t *aElem)
{

}

/*
 * compute the gradients of dU/dx
 */
void spatialReconstruction(double time)
{
	if (spatialOrder == 1) {
		/* set side states to be equal to mean value */
		#pragma omp parallel for
		for (long iElem = 0; iElem < nElems; ++iElem) {
			elem_t *aElem = elem[iElem];
			side_t *aSide = aElem->firstSide;
			aElem->u_x[RHO] = 0.0;
			aElem->u_x[VX]  = 0.0;
			aElem->u_x[VY]  = 0.0;
			aElem->u_x[P]   = 0.0;

			aElem->u_y[RHO] = 0.0;
			aElem->u_y[VX]  = 0.0;
			aElem->u_y[VY]  = 0.0;
			aElem->u_y[P]   = 0.0;

			aElem->u_t[RHO] = 0.0;
			aElem->u_t[VX]  = 0.0;
			aElem->u_t[VY]  = 0.0;
			aElem->u_t[P]   = 0.0;

			while (aSide) {
				aSide->pVar[RHO] = aElem->pVar[RHO];
				aSide->pVar[VX]  = aElem->pVar[VX];
				aSide->pVar[VY]  = aElem->pVar[VY];
				aSide->pVar[P]   = aElem->pVar[P];
				aSide = aSide->nextElemSide;
			}
		}
	} else {
		/* reconstruction of values at side GPs */
		#pragma omp parallel for
		for (long iElem = 0; iElem < nElems; ++iElem) {
			elem_t *aElem = elem[iElem];
			aElem->u_x[RHO] = 0.0;
			aElem->u_x[VX]  = 0.0;
			aElem->u_x[VY]  = 0.0;
			aElem->u_x[P]   = 0.0;

			aElem->u_y[RHO] = 0.0;
			aElem->u_y[VX]  = 0.0;
			aElem->u_y[VY]  = 0.0;
			aElem->u_y[P]   = 0.0;

			aElem->u_t[RHO] = 0.0;
			aElem->u_t[VX]  = 0.0;
			aElem->u_t[VY]  = 0.0;
			aElem->u_t[P]   = 0.0;
		}

		setBCatBarys(time);

		for (long iSide = 0; iSide < nSides; ++iSide) {
			side_t *aSide = side[iSide];

			/* difference of primitive variables over the side */
			double pDiff[NVAR];
			pDiff[RHO] = aSide->connection->elem->pVar[RHO] - aSide->elem->pVar[RHO];
			pDiff[VX]  = aSide->connection->elem->pVar[VX]  - aSide->elem->pVar[VX];
			pDiff[VY]  = aSide->connection->elem->pVar[VY]  - aSide->elem->pVar[VY];
			pDiff[P]   = aSide->connection->elem->pVar[P]   - aSide->elem->pVar[P];

			/* least squares reconstruction */
			elem_t *aElem = aSide->elem;
			/* main element */
			aElem->u_x[RHO] += aSide->w[X] * pDiff[RHO];
			aElem->u_x[VX]  += aSide->w[X] * pDiff[VX];
			aElem->u_x[VY]  += aSide->w[X] * pDiff[VY];
			aElem->u_x[P]   += aSide->w[X] * pDiff[P];

			aElem->u_y[RHO] += aSide->w[Y] * pDiff[RHO];
			aElem->u_y[VX]  += aSide->w[Y] * pDiff[VX];
			aElem->u_y[VY]  += aSide->w[Y] * pDiff[VY];
			aElem->u_y[P]   += aSide->w[Y] * pDiff[P];

			/* connection element */
			aElem = aSide->connection->elem;
			aElem->u_x[RHO] -= aSide->connection->w[X] * pDiff[RHO];
			aElem->u_x[VX]  -= aSide->connection->w[X] * pDiff[VX];
			aElem->u_x[VY]  -= aSide->connection->w[X] * pDiff[VY];
			aElem->u_x[P]   -= aSide->connection->w[X] * pDiff[P];

			aElem->u_y[RHO] -= aSide->connection->w[Y] * pDiff[RHO];
			aElem->u_y[VX]  -= aSide->connection->w[Y] * pDiff[VX];
			aElem->u_y[VY]  -= aSide->connection->w[Y] * pDiff[VY];
			aElem->u_y[P]   -= aSide->connection->w[Y] * pDiff[P];
		}

		/* TODO: check parallel version's speed
		#pragma omp parallel for
		for (long iElem = 0; iElem < nElems; ++iElem) {
			elem_t *aElem = elem[iElem];
			side_t *aSide = aElem->firstSide;
			while (aSide) {
				double pDiff[NVAR];
				pDiff[RHO] = aSide->connection->elem->pVar[RHO] - aSide->elem->pVar[RHO];
				pDiff[VX]  = aSide->connection->elem->pVar[VX]  - aSide->elem->pVar[VX];
				pDiff[VY]  = aSide->connection->elem->pVar[VY]  - aSide->elem->pVar[VY];
				pDiff[P]   = aSide->connection->elem->pVar[P]   - aSide->elem->pVar[P];

				aElem->u_x[RHO] += aSide->w[X] * pDiff[RHO];
				aElem->u_x[VX]  += aSide->w[X] * pDiff[VX];
				aElem->u_x[VY]  += aSide->w[X] * pDiff[VY];
				aElem->u_x[P]   += aSide->w[X] * pDiff[P];

				aElem->u_y[RHO] += aSide->w[Y] * pDiff[RHO];
				aElem->u_y[VX]  += aSide->w[Y] * pDiff[VX];
				aElem->u_y[VY]  += aSide->w[Y] * pDiff[VY];
				aElem->u_y[P]   += aSide->w[Y] * pDiff[P];

				aSide = aSide->nextElemSide;
			}
		}
		*/

		/* limit gradients and reconstruct values at side GPs */
		#pragma omp parallel for
		for (long iElem = 0; iElem < nElems; ++iElem) {
			elem_t *aElem = elem[iElem];

			/* limit gradient */
			switch (limiter) {
			case BARTHJESPERSEN:
				limiterBarthJespersen(aElem);
				break;
			case VENKATAKRISHNAN:
				limiterVenkatakrishnan(aElem);
				break;
			}

			/* reconstruct values at side GPs */
			side_t *aSide = aElem->firstSide;
			while (aSide) {
				double dx = aSide->GP[X];
				double dy = aSide->GP[Y];
				aSide->pVar[RHO] += dx * aElem->u_x[RHO] + dy * aElem->u_y[RHO];
				aSide->pVar[VX]  += dx * aElem->u_x[VX]  + dy * aElem->u_y[VX];
				aSide->pVar[VY]  += dx * aElem->u_x[VY]  + dy * aElem->u_y[VY];
				aSide->pVar[P]   += dx * aElem->u_x[P]   + dy * aElem->u_y[P];

				aSide = aSide->nextElemSide;
			}
		}
	}
}
