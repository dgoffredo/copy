
set title 'Wall Time of Copy Versus File Size and Buffer Size'
set xlabel 'file size (bytes)'
set ylabel 'buffer size (bytes)'
set zlabel 'wall time (microseconds)'
set logscale x
set logscale y
set logscale z

set key outside

splot 'bench-buffer-size.dat' using 1:2:3

