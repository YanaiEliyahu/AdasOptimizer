## Build & Run

Run `DEBUG=0 OPT=1 make run`, this implementation requires GCC 7 or higher. (configure/export `CXX` if your default GCC is lower than 7)
Note that it's highly unoptimized (in contrast to highly optimized like tensorflow), profiling shows that the difference between ADAM and ADAS takes less than 20% of the total compute.
