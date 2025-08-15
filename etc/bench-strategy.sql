.load ../../sqlean/dist/sqlean.so
.mode tabs
.output ../var/wall-stats.dat
select tool, file_size, avg(wall_micros), stats_stddev_samp(wall_micros) from CopyRun group by tool, file_size;
.output
