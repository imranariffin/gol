#
# Include platform-dependent settings, and definitions.
#
include Makefile.include

LDFLAGS += -lm 
CPPFLAGS += "-DCOMPILER=\"$(CC)\"" "-DFLAGS=\"$(COPTFLAGS)\""
CPPFLAGSPG += "-DCOMPILER=\"$(CC)\"" "-DFLAGS=\"$(COPTFLAGSPG)\""

#
# Include the realtime clock library, if required.
#
ifeq ($(TIMER_TYPE),USE_CLOCK_GETTIME_REALTIME)
	LDFLAGS += -lrt
else
	ifeq ($(TIMER_TYPE),USE_CLOCK_GETTIME_MONOTONIC)
		LDFLAGS += -lrt
	endif
endif

GOL_EXE = gol
GOL_VERIFY_EXE = gol_verify
GOL_EXE_PG = gol_pg
# GOL_OBJS = gol.o life.o lifeseq.o load.o save.o 
GOL_OBJS = gol.o par_block.o par_block_2.o par_blk_barrier.o par_blk_margin.o life.o lifeseq.o load.o save.o 
GOL_OBJS_PG = gol.pg.o par_block.pg.o par_block_2.pg.o par_blk_barrier.o par_blk_margin.o life.pg.o lifeseq.pg.o load.pg.o save.pg.o 
GOL_VERIFY_OBJS = gol.verify.o life.o lifeseq.o load.o save.o 
BITBOARD_EXE = initboard
BITBOARD_OBJS = bitboard.o random_bit.o
EXES = $(GOL_EXE) $(BITBOARD_EXE)
OBJS = $(GOL_OBJS) $(BITBOARD_OBJS)


all: $(GOL_EXE) $(BITBOARD_EXE)

all_pg: $(GOL_EXE_PG) $(BITBOARD_EXE)

$(GOL_EXE): $(GOL_OBJS) 
	$(LINKER) $(CFLAGS) $(LDFLAGS) $(GOL_OBJS) -o $@

$(GOL_EXE_PG): $(GOL_OBJS_PG) 
	$(LINKER) $(CFLAGS_PG) $(LDFLAGS) $(GOL_OBJS_PG) -o $@ 

$(GOL_VERIFY_EXE): $(GOL_VERIFY_OBJS) 
	$(LINKER) $(CFLAGS) $(LDFLAGS) $(GOL_VERIFY_OBJS) -o $@ 

$(BITBOARD_EXE): $(BITBOARD_OBJS)
	$(LINKER) $(CFLAGS) $(LDFLAGS) $(BITBOARD_OBJS) -o $@ 

%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

%.pg.o: %.c
	$(CC) -c $(CFLAGS_PG) $(CPPFLAGSPG) $< -o $@

%.verify.o: %.c
	$(CC) -c $(CFLAGS) -DVERIFY_FLAG $(CPPFLAGS) $< -o $@

lifeseq.o: lifeseq.c life.h util.h

life.o: life.c life.h util.h

load.o: load.c load.h

save.o: save.c save.h

gol.o: gol.c life.h load.h save.h 

bitboard.o: bitboard.c random_bit.h

random_bit.o: random_bit.c random_bit.h

# new files
par_block.o: par_block.c par_block.h

par_block_2.o: par_block_2.c par_block_2.h

par_blk_barrier.o: par_blk_barrier.c par_blk_barrier.h

par_blk_margin.o: par_blk_margin.c par_blk_margin.h
 
clean:
	rm -f $(GOL_OBJS) $(GOL_VERIFY_OBJS) $(GOL_EXE) $(GOL_VERIFY_EXE) $(BITBOARD_OBJS) $(BITBOARD_EXE) $(GOL_OBJS_PG) $(GOL_EXE_PG)
