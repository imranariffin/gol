/*****************************************************************************
 * life.c
 * Parallelized and optimized implementation of the game of life resides here
 ****************************************************************************/
#include "life.h"
#include "util.h"
#include "stdlib.h"
#include "pthread.h"
#define SWAP_BOARDS( b1, b2 )  do { \
  char* temp = b1; \
  b1 = b2; \
  b2 = temp; \
} while(0)

#define BOARD( __board, __i, __j )  (__board[(__i) + LDA*(__j)])
/*****************************************************************************
 * Helper function definitions
 make clean && make all
 /usr/bin/time -f "%e real" ./gol 10000 inputs/1k.pbm outputs/1k.pbm
 diff outputs/1k.pbm outputs/1k_verify_out.pbm
 ****************************************************************************/
struct thread_data
{
    //t_id
    char* outboard;
    char* inboard;
    int nrows, ncols, start_id, end_id;
};
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
  //return sequential_game_of_life (outboard, inboard, nrows, ncols, gens_max);
  return pthread_game_of_life(outboard, inboard, nrows, ncols, gens_max);
}

char*
pthread_game_of_life (char* outboard, 
        char* inboard,
        const int nrows,
        const int ncols,
        const int gens_max)
{
    
    const int num_threads = 8;//must be a mutliple of 2 otherwise fixing board ptrs after pthread_join fails
    struct thread_data *t_data = (struct thread_data*)malloc(num_threads *sizeof(struct thread_data));
    int i, j, inc = nrows/num_threads;
    for(j = 0, i = 0; i < num_threads; i++, j=j+inc)//i+inc
    {
      t_data[i].outboard = outboard;
      t_data[i].inboard = inboard;
      t_data[i].nrows = nrows;
      t_data[i].ncols = ncols;
      t_data[i].start_id = j;
      t_data[i].end_id = (j+inc);
    }
    
    /* HINT: in the parallel decomposition, LDA may not be equal to
       nrows! */
    pthread_t *t_arr = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
    
    int curgen;
    for (curgen = 0; curgen < gens_max; curgen++)
    {
        /* HINT: you'll be parallelizing these loop(s) by doing a
           geometric decomposition of the output */
           
        //fork        
        for(i = 0; i < num_threads; i++)
        {
          pthread_create(&t_arr[i],NULL,parallel_update,&t_data[i]);
        }
        
        //join
        for(i = 0; i < num_threads; i++)
        {
          pthread_join(t_arr[i], NULL);
        }
        
        //parallel_update(&t_data[0]);
        
        SWAP_BOARDS( outboard, inboard );
        for(i = 0; i < num_threads; i+=2)//fix board ptr b/c they have changed
        {
            t_data[i].outboard = outboard;
            t_data[i].inboard = inboard;
            
            t_data[i+1].outboard = outboard;
            t_data[i+1].inboard = inboard;
        }
    }
    /* 
     * We return the output board, so that we know which one contains
     * the final result (because we've been swapping boards around).
     * Just be careful when you free() the two boards, so that you don't
     * free the same one twice!!! 
     */
    free(t_arr);
    free(t_data);    
    return inboard;
}

void *parallel_update(void* thread_params)
{
    
    struct thread_data *thr = (struct thread_data*)thread_params;
    char* inboard = thr->inboard;
    char* outboard = thr->outboard;
    int nrows = thr->nrows, ncols = thr->ncols;
    int start= thr->start_id, end = thr->end_id;
    
    //int start = 0, end = nrows;    
    
    const int LDA = nrows;
    int i, j;
    int inorth, isouth, jeast;// jwest;
    int north_west, north, north_east, south_west, south, south_east, west, middle, east;
    char neighbor_count;
    
    for (i = start; i < end; i++)
    {
        inorth = mod (i-1, nrows);
        isouth = mod (i+1, nrows);
        
        north_east = BOARD (inboard, inorth, 0);
        east = BOARD (inboard, i, 0);
        south_east = BOARD (inboard, isouth, 0);
        
        north = BOARD (inboard, inorth, mod(-1,ncols));
        middle = BOARD (inboard, i, mod(-1,ncols));
        south = BOARD (inboard, isouth, mod(-1,ncols));
                   
        for (j = 0; j < ncols; j++)
        {                
            //jwest = mod (j-1, ncols);
            jeast = mod (j+1, ncols);
        
            north_west = north;
            north = north_east;
            north_east = BOARD (inboard, inorth, jeast);
            
            west = middle;
            middle = east;
            east = BOARD (inboard, i, jeast);
            
            south_west = south;
            south = south_east;
            south_east = BOARD (inboard, isouth, jeast);
            
            neighbor_count = 
                north_west + 
                north + 
                north_east + 
                west +
                east + 
                south_west +
                south + 
                south_east;
        
            BOARD(outboard, i, j) = (! middle && (neighbor_count == (char) 3)) ||(middle && (neighbor_count >= 2) && (neighbor_count <= 3));//alivep (neighbor_count, middle);
        
        }
    }  
    //SWAP_BOARDS( outboard, inboard );
    
    return NULL;
}