set xlabel "Top"
set ylabel "Acerto"
set xrange [0:29]
set yrange[0:1]
set xtics "1/29"
set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 pi -1 ps 1.5
set terminal postscript eps enhanced color font 'Helvetica,16'
set output 'roc.eps'
plot "input.dat" title "" with linespoints ls 1
