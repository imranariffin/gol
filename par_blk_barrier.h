#ifndef _par_blk_barrier_h
#define _par_blk_barrier_h

/*
split the board into n_thread=n_col_blks*n_row_blks blocks.
each block will be processed by the function update_block on
one pthread.

calculation of neigh_count is done in parenthesis to allow ILP.

instead of fork and join, use barrier
*/

char*
gol_par_blk_barrier (char* outboard, 
        char* inboard,
        const int nrows,
        const int ncols,
        const int gens_max);

#endif /* _par_blk_barrier_h */
