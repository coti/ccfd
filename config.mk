# CCFD build configuration file

# select compiler [gnu, intel]
COMPILER = gnu

# select equation system [euler, navierstokes]
EQNSYS = navierstokes

# multithreading flag [openmp, openacc, off]
PARALLEL = openacc

# enable GPU (requires a multithreading flag) [on, off]
GPU = on

# debugging flag [on, off]
DEBUG = off

# profiling flag [on, off]
PROF = off

# CGNS library setup [3.4.1, 4.1.1]
CGNS_VERSION = 3.4.1
