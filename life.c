/*****************************************************************************
 * life.c
 * Parallelized and optimized implementation of the game of life resides here
 ****************************************************************************/
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include "life.h"
#include "util.h"
#include "par_block.h"
#include "par_block_2.h"
#include "par_blk_barrier.h"
#include "par_blk_margin.h"

/*****************************************************************************
 * Game of life implementation
 ****************************************************************************/
char*
game_of_life (char* outboard, 
	      char* inboard,
	      const int nrows,
	      const int ncols,
	      const int gens_max)
{
	// return sequential_game_of_life (outboard, inboard, nrows, ncols, gens_max);
	// return game_of_life_par_block (outboard, inboard, nrows, ncols, gens_max);
	return gol_par_block_2 (outboard, inboard, nrows, ncols, gens_max);
	// return gol_par_blk_barrier (outboard, inboard, nrows, ncols, gens_max);
	// return gol_par_blk_margin (outboard, inboard, nrows, ncols, gens_max);
}
