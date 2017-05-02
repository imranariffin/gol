/*//////////////////////
IMRAN's Version Starts 
//////////////////////*/
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include "par_blk_tiling.h"
#include "util.h"

// #define DEBUG 1

#ifdef DEBUG
void print_board(char *board, int nrows, int ncols);
#endif

/**
 * Swapping the two boards only involves swapping pointers, not
 * copying values.
 */
#define SWAP_BOARDS( b1, b2 )  do { \
  char* temp = b1; \
  b1 = b2; \
  b2 = temp; \
} while(0)

// #define BOARD( __board, LDA, __i, __j )  (__board[(__i) + LDA*(__j)])
#define BOARD( __board, LDA, __i, __j )  (__board[(__i)*(LDA) + (__j)])
// #define BLOCK(__i, __j, __blk_h) ((__i)*(__blk_h) + (__j))
#define NROW_BLKS 2
#define NCOL_BLKS 1
#define N_THREADS ((NROW_BLKS)*(NCOL_BLKS))

typedef struct thr_data_t
{
	char *inboard;
	char *outboard;
	int istart;
	int jstart;
	int blk_h;
	int blk_w;
	int nrows;
	int ncols;
	#ifdef DEBUG
	int block; // for debug
	int curgen; // for debug
	#endif
} thr_data_t;

void* update_block(void *args)
{
	// extract args
	thr_data_t *thr_data = (thr_data_t *)args;
	char *inboard = thr_data->inboard;
	char *outboard = thr_data->outboard;
	int istart = thr_data->istart;
	int jstart = thr_data->jstart;
	int blk_h = thr_data->blk_h;
	int blk_w = thr_data->blk_w;
	int nrows = thr_data->nrows;
	int ncols = thr_data->ncols;
	#ifdef DEBUG
	int block = thr_data->block;
	int curgen = thr_data->curgen;
	#endif

				// int bi = block/NCOL_BLKS;
				// int bj = block%NCOL_BLKS;
				// int istart = bi*blk_h;
				// int jstart = bj*blk_w;

	// debug
	#ifdef DEBUG
	// printf("update_block(bi=%d, bj=%d)\n", iblock, jblock);
	// printf("before update block: inboard:\n");
	// print_board(inboard, nrows, ncols);
	// printf("before update block: outboard:\n");
	// print_board(outboard, nrows, ncols);
	printf("curgen=%d:\n", curgen);
		// printf("\tupdate_board(blk=%d, istrt=%d, jstrt=%d)\n", block, istart, jstart);
		printf("\tupdate_board(thr=%u, blk=%d)\n", pthread_self(), block);
	#endif

	// setup block iteration
	int i, j;
	int maxi = istart + blk_h;
	int maxj = jstart + blk_w;
	for (i = istart; i < maxi; i++)
	{
		for (j = jstart; j < maxj; j++)
		{
			const int inorth = mod (i-1, nrows);
			const int isouth = mod (i+1, nrows);
			const int jwest = mod (j-1, ncols);
			const int jeast = mod (j+1, ncols);

			const char neighbor_count = 
			    (((((((BOARD (inboard, nrows, inorth, jwest) + 
			    BOARD (inboard, nrows, inorth, j)) + 
			    BOARD (inboard, nrows, inorth, jeast)) + 
			    BOARD (inboard, nrows, i, jwest)) +
			    BOARD (inboard, nrows, i, jeast)) + 
			    BOARD (inboard, nrows, isouth, jwest)) +
			    BOARD (inboard, nrows, isouth, j)) + 
			    BOARD (inboard, nrows, isouth, jeast));

			BOARD(outboard, nrows, i, j) = alivep (
				neighbor_count, 
				BOARD (inboard, nrows, i, j));
		}
	}

	// debug
	#ifdef DEBUG
	// printf("after update block: inboard:\n");
	// print_board(inboard, nrows, ncols);
	// printf("after update block: outboard:\n");
	// print_board(outboard, nrows, ncols);
	#endif

	return NULL;
}

char*
game_of_life_par_block (char* outboard, 
        char* inboard,
        const int nrows,
        const int ncols,
        const int gens_max)
{
	/* HINT: in the parallel decomposition, LDA may not be equal to
	   nrows! */

	// init thread and thread data
	pthread_t *thr_arr = (pthread_t *)malloc(
		sizeof(pthread_t)*N_THREADS);
	thr_data_t *thr_data_arr = (thr_data_t *)malloc(
		sizeof(thr_data_t)*N_THREADS);

	int curgen;
	int block;
	int blk_h = nrows/NROW_BLKS;
	int blk_w = ncols/NCOL_BLKS;
	for (curgen = 0; curgen < gens_max; curgen++)
	{
		for (block = 0; block < N_THREADS; block++) 
		{
				thr_data_t * thr_data = &thr_data_arr[block];

				// get block indices
				int bi = block/NCOL_BLKS;
				int bj = block%NCOL_BLKS;
				int i = bi*blk_h;
				int j = bj*blk_w;

				// init thread data
				thr_data->inboard = inboard;
				thr_data->outboard = outboard;
				thr_data->istart = i;
				thr_data->jstart = j;
				thr_data->blk_h = blk_h;
				thr_data->blk_w = blk_w;
				thr_data->nrows = nrows;
				thr_data->ncols = ncols;
				#ifdef DEBUG
				thr_data->block = block;
				thr_data->curgen = curgen;
				#endif

				pthread_create(&thr_arr[block], 
					NULL, 
					update_block, 
					(void *)thr_data);
				// printf("curgen=%d: ", curgen);
				// printf("pthread_created(block=%d)\n", block);
		}
		// 	}
		// }

		// join thread of every block
		for (block = 0; block < N_THREADS; block++)
		{
			pthread_join(thr_arr[block], NULL);
		}
		// debug
		#ifdef DEBUG
		// printf("inboard before swap:\n");
		// print_board(inboard, nrows, ncols);
		// printf("outboard before swap:\n\n");
		// print_board(outboard, nrows, ncols);
		#endif

		SWAP_BOARDS( outboard, inboard );

		// debug
		#ifdef DEBUG
		// printf("inboard after swap:\n");
		// print_board(inboard, nrows, ncols);
		// printf("outboard after swap:\n\n");
		// print_board(outboard, nrows, ncols);
		// return inboard;
		#endif
	}
	/* 
	 * We return the output board, so that we know which one contains
	 * the final result (because we've been swapping boards around).
	 * Just be careful when you free() the two boards, so that you don't
	 * free the same one twice!!! 
	 */
	return inboard;
}

/*//////////////////////
IMRAN's Version 	Ends 
//////////////////////*/

/*****************************************************************************
 * Helper function definitions
 ****************************************************************************/
#ifdef DEBUG
void print_board(char *board, int nrows, int ncols)
{
	int i, j;
	for (i = 0; i < nrows; i++)
	{
		for (j = 0; j < ncols; j++)
		{
			printf("%d ", BOARD(board, nrows, i, j));
		}
		printf("\n");
	}
}
#endif