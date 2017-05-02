make clean
make
/usr/bin/time -f "%e real" ./gol 100 inputs/4.pbm outputs/4.pbm 
convert outputs/4.pbm outputs/4.jpg

