set pointsize 1.5

set logscale y 10
set logscale x 2

set format x "2^{%L}"
set xrange [1024:1024*1024*1024*16]

set key outside

set xlabel 'file size (bytes)'
set ylabel 'wall time (milliseconds)'
set title 'Copy: Wall Time Versus File Size'

# plot '../var/wall-stats.dat' using ((strcol(1) eq 'read-write') ? $2 : NaN):($3/1000):($4/1000) with errorbars title 'read-write', \
#      '' using ((strcol(1) eq 'mmap-mmap') ? $2*1.1 : NaN):($3/1000):($4/1000) with errorbars title 'mmap-mmap', \
#      '' using ((strcol(1) eq 'mmap-write') ? $2*1.2 : NaN):($3/1000):($4/1000) with errorbars title 'mmap-write', \
#      '' using ((strcol(1) eq 'read-mmap') ? $2*1.3 : NaN):($3/1000):($4/1000) with errorbars title 'read-mmap', \
#      '' using ((strcol(1) eq 'copy') ? $2*1.4 : NaN):($3/1000):($4/1000) with errorbars title 'copy', \
#      '' using ((strcol(1) eq '/bin/cp') ? $2*1.5 : NaN):($3/1000):($4/1000) with errorbars title '/bin/cp'

plot '../var/wall-stats.dat' using ((strcol(1) eq 'read-write') ? $2 : NaN):($3/1000) title 'read-write', \
     '' using ((strcol(1) eq 'mmap-mmap') ? $2*1.1 : NaN):($3/1000) title 'mmap-mmap', \
     '' using ((strcol(1) eq 'mmap-write') ? $2*1.2 : NaN):($3/1000) title 'mmap-write', \
     '' using ((strcol(1) eq 'read-mmap') ? $2*1.3 : NaN):($3/1000) title 'read-mmap', \
     '' using ((strcol(1) eq 'copy') ? $2*1.4 : NaN):($3/1000) title 'copy', \
     '' using ((strcol(1) eq '/bin/cp') ? $2*1.5 : NaN):($3/1000) title '/bin/cp'