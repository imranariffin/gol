#ifndef _life_h
#define _life_h

/* 	
    Team name:		Team_KI    
	
    Member1:		Kunal Choudhary
    Member1_ID:		999981863
	Member1_Email:	kunal.choudhary@mail.utoronto.ca
	
	Member2:		Imran Ariffin
    Member2_ID:		1000863176
    Member2_Email:	imran.ariffin@mail.utoronto.ca
*/

/**
 * Given the initial board state in inboard and the board dimensions
 * nrows by ncols, evolve the board state gens_max times by alternating
 * ("ping-ponging") between outboard and inboard.  Return a pointer to 
 * the final board; that pointer will be equal either to outboard or to
 * inboard (but you don't know which).
 */
char*
game_of_life (char* outboard, 
	      char* inboard,
	      const int nrows,
	      const int ncols,
	      const int gens_max);

/**
 * Same output as game_of_life() above, except this is not
 * parallelized.  Useful for checking output.
 */
char*
sequential_game_of_life (char* outboard, 
			 char* inboard,
			 const int nrows,
			 const int ncols,
			 const int gens_max);


#endif /* _life_h */
