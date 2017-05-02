make clean
make
/usr/bin/time -f "%e real" ./gol 100 inputs/4.pbm outputs/4_100_par.pbm 
convert outputs/4_100_par.pbm outputs/4_100_par.jpg

