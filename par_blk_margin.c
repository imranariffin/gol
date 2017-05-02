/*//////////////////////
IMRAN's Version Starts 
//////////////////////*/
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include "par_blk_margin.h"
#include "util.h"

// #define DEBUG 1

#ifdef DEBUG
void print_board(char *board, int nrows, int ncols);
#endif

/**
 * Swapping the two boards only involves swapping pointers, not
 * copying values.
 */
// #define SWAP_BOARDS( b1, b2 )  do { \
//   char* temp = b1; \
//   b1 = b2; \
//   b2 = temp; \
// } while(0)

// static inline void
// swap_boards(char *b1, char *b2)
// {
// 	char * temp = b1;
// 	b1 = b2;
// 	b2 = temp;
// }

// #define BOARD( __board, LDA, __i, __j )  (__board[(__i) + LDA*(__j)])
#define BOARD( __board, LDA, __i, __j )  (__board[(__i)*(LDA) + (__j)])
// #define BLOCK(__i, __j, __blk_h) ((__i)*(__blk_h) + (__j))
#define NROW_BLKS 8
#define NCOL_BLKS 1
#define N_THREADS (((NROW_BLKS)*(NCOL_BLKS))+(1))

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
} thr_data_t;

void* update_blk_main(void *args)
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

	// setup block iteration
	int i, j;
	int maxi = istart + blk_h;
	int maxj = jstart + blk_w;
	int iw = ncols*istart; int iwj = 0;
	for (i = istart; i < maxi; i++)
	{
		for (j = jstart; j < maxj; j++)
		{
			iwj = iw + j;

			const int inorth = i-1;
			const int isouth = i+1;
			const int jwest = j-1;
			const int jeast = j+1;

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
		iw += ncols;
	}	

	return NULL;
}

/*
calculate for margin of whole board, desginate one extra thread
for this function.
*/
void* update_blk_margin(void *args)
{
	// extract args
	thr_data_t *thr_data = (thr_data_t *)args;
	char *inboard = thr_data->inboard;
	char *outboard = thr_data->outboard;
	int nrows = thr_data->nrows;
	int ncols = thr_data->ncols;

	int i, j;
	// top panel
	i = 0;
	for (j = 0; j < ncols; j++)
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

	// bottom
	i = nrows - 1;
	for (j = 0; j < ncols; j++)
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

	// left panel
	j = 0;
	for (i = 1; i < nrows - 1; i++)
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

	// right panel
	j = ncols - 1;
	for (i = 1; i < nrows - 1; i++)
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

	return NULL;
}

char*
gol_par_blk_margin (char* outboard, 
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
		for (block = 0; block < N_THREADS - 1; block++) 
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
				thr_data->istart = i + ((bi==0)&1); // create top  margin
				thr_data->jstart = j + ((bj==0)&1); // create left margin
				thr_data->blk_h = blk_h - ((bi==(NROW_BLKS - 1))&1); // create bottom margin
				thr_data->blk_w = blk_w - ((bj==(NCOL_BLKS - 1))&1); // create right margin
				thr_data->nrows = nrows;
				thr_data->ncols = ncols;

				pthread_create(&thr_arr[block], 
					NULL, 
					update_blk_main, 
					(void *)thr_data);
		}
		// margin
		thr_data_t * thr_data = &thr_data_arr[N_THREADS - 1];
		thr_data->inboard = inboard;
		thr_data->outboard = outboard;
		thr_data->ncols = ncols;
		thr_data->nrows = nrows;
		pthread_create(&thr_arr[N_THREADS - 1], 
			NULL, 
			update_blk_margin, 
			(void *)thr_data);

		// join thread of every block
		for (block = 0; block < N_THREADS; block++)
		{
			pthread_join(thr_arr[block], NULL);
		}

		// SWAP_BOARDS( outboard, inboard );
		// swap_boards(outboard, inboard);
		char * temp = outboard;
		outboard = inboard;
		inboard = temp;
	}
	/* 
	 * We return the output board, so that we know which one contains
	 * the final result (because we've been swapping boards around).
	 * Just be careful when you free() the two boards, so that you don't
	 * free the same one twice!!! 
	 */
	return inboard;
}

/*****************************************************************************
 * Helper function definitions
 ****************************************************************************/