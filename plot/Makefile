.PHONY: plot

plot: ../etc/bench-strategy.plot ../var/wall-stats.dat
	gnuplot --persist $< -

../var/wall-stats.dat: ../var/db.sqlite
	sqlite3 -batch $< <../etc/bench-strategy.sql

../var/db.sqlite: ../var/bench-strategy.log ../bin/into-sqlite
	rm -f $@
	<$^ $@
