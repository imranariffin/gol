#ifndef _par_blk_tiling_h
#define _par_blk_tiling_h

char*
gol_par_blk_tiling (char* outboard, 
        char* inboard,
        const int nrows,
        const int ncols,
        const int gens_max);

#endif /* _par_blk_tiling_h */