/***********************************************************************
/
/  GRID CLASS: Phases
/
/  written by: Wolfram Schmidt
/  date:       October 2005
/  modified1: Sep, 2014: modified to support Enzo 2.4 // P. Grete
/
/  PURPOSE: sets initial phase factors and phase multiplicators
/           for the inverse FT of the global forcing spectrum
/           onto the local grid
/
************************************************************************/

#include "preincludes.h"
#include "macros_and_parameters.h"
#include "typedefs.h"
#include "global_data.h"
#include "Fluxes.h"
#include "GridList.h"
#include "ExternalBoundary.h"
#include "Grid.h"
#include "phys_constants.h"

void grid::Phases()
{
    int dim, i, j, k, m, n;
    int i1 = Forcing.get_LeftBoundary(1), i2 =  Forcing.get_RightBoundary(1);
    int j1 = Forcing.get_LeftBoundary(2), j2 =  Forcing.get_RightBoundary(2);
    int k1 = Forcing.get_LeftBoundary(3), k2 =  Forcing.get_RightBoundary(3);
    int size = Forcing.get_NumModes();
    int mask[size];
    
    float delta[MAX_DIMENSION];
    float wave_number[MAX_DIMENSION];
    float phase_start[MAX_DIMENSION];
    float incr[MAX_DIMENSION];

    if ((PhaseFctInitEven != NULL) && (PhaseFctInitOdd != NULL)) return;

    if (debug) printf("Computing initial phase factors\n");

    PhaseFctInitEven = new float[size];
    PhaseFctInitOdd  = new float[size];

    Forcing.copy_mask(mask);

    for (dim = 0; dim < GridRank; dim++) {

    delta[dim]       = (GridRightEdge[dim]- GridLeftEdge[dim]) / (GridEndIndex[dim] - GridStartIndex[dim] + 1);
    wave_number[dim] = 2. * pi / (DomainRightEdge[dim] - DomainLeftEdge[dim]);
    phase_start[dim] = wave_number[dim] * (GridLeftEdge[dim] - GridStartIndex[dim] * delta[dim]); 
    incr[dim]        = wave_number[dim] * delta[dim];

    PhaseFctMultEven[dim] = new float[size];
    PhaseFctMultOdd[dim]  = new float[size];

    for (m = 0; m < size; m++) {
        PhaseFctMultEven[dim][m] = 1.0; 
        PhaseFctMultOdd[dim][m]  = 0.0; 
    }
    }

    if (debug) printf("Computing phase factor multiplicators\n");

    /* calculate initial phase factors corresponding to the origin of the grid 
       and multiplicators for single cell shifts */

    m = 0;
    for (i = 1; i <= i2; i++)
    if (mask[i-1]) {

        PhaseFctInitEven[m] = cos(i*phase_start[0]);
        PhaseFctInitOdd [m] = sin(i*phase_start[0]);

        PhaseFctMultEven[0][m] = cos(i*incr[0]);
        PhaseFctMultOdd [0][m] = sin(i*incr[0]);

        ++m;
    }
    
    if (GridRank > 1) {
    
    n = i2;
    for (j = 1; j <= j2; j++) 
        for (i = i1; i <= i2; i++)
        if (mask[n++]) {

            PhaseFctInitEven[m] = 
            cos(i*phase_start[0]) * cos(j*phase_start[1]) - 
            sin(i*phase_start[0]) * sin(j*phase_start[1]);
            PhaseFctInitOdd[m] = 
            cos(i*phase_start[0]) * sin(j*phase_start[1]) + 
            sin(i*phase_start[0]) * cos(j*phase_start[1]);

            PhaseFctMultEven[0][m] = cos(i*incr[0]);
            PhaseFctMultOdd [0][m] = sin(i*incr[0]);
            
            PhaseFctMultEven[1][m] = cos(j*incr[1]);
            PhaseFctMultOdd [1][m] = sin(j*incr[1]);
            
            ++m;
        }

    if (GridRank > 2) {

        for (k = 1; k <= k2; k++)
        for (j = j1; j <= j2; j++)
            for (i = i1; i <= i2; i++)
                if (mask[n++]) {

                PhaseFctInitEven[m] = 
                    (cos(i*phase_start[0]) * cos(j*phase_start[1]) - 
                     sin(i*phase_start[0]) * sin(j*phase_start[1])) * cos(k*phase_start[2]) -
                    (cos(i*phase_start[0]) * sin(j*phase_start[1]) + 
                     sin(i*phase_start[0]) * cos(j*phase_start[1])) * sin(k*phase_start[2]);

                PhaseFctInitOdd[m] = 
                    (cos(i*phase_start[0]) * cos(j*phase_start[1]) - 
                     sin(i*phase_start[0]) * sin(j*phase_start[1])) * sin(k*phase_start[2]) +
                    (cos(i*phase_start[0]) * sin(j*phase_start[1]) + 
                     sin(i*phase_start[0]) * cos(j*phase_start[1])) * cos(k*phase_start[2]);

                PhaseFctMultEven[0][m] = cos(i*incr[0]);
                PhaseFctMultOdd [0][m] = sin(i*incr[0]);

                PhaseFctMultEven[1][m] = cos(j*incr[1]);
                PhaseFctMultOdd [1][m] = sin(j*incr[1]);

                PhaseFctMultEven[2][m] = cos(k*incr[2]);
                PhaseFctMultOdd [2][m] = sin(k*incr[2]);

                ++m;
                }
    }
    }
}
