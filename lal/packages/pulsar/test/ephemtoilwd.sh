#!/bin/sh

# SYNOPSIS:
#
# This script reads Earth and Sun ephemeris files and produces ILWD files.

# USAGE:
#
# ilwdem ephemeris.dat > ephemeris.ilwd

# ASSUMPTIONS:
#
# 1. The file format is as detailed below (in brief, the first line is header
#    info, and then every four lines is an entry consisting of ten numbers).
# 2. The first number in each entry is the GPS time, which is an integer
#    (though it is represented as a float), and the TIME SPACING IS CONSTANT.
# 3. The units are SI units.

# FORMAT:
#
# - the first line contains three integers, the GPS start time, the time step,
#   and the number of enteries
# - each entry is in the format " %f %f %f\n %f %e %e\n %e %e %e\n %e\n"
#   where the first number in the entry is the GPS time and the remaining
#   nine numbers are positions, velocities, and accelerations (three
#   components of each) respectively.
#   NOTE: each entry spans 4 lines


infile=$1
tmpfile=$infile.tmp
rm -f $tmpfile

# First line contains useful info
nlines=`sed -n '1s/[^0-9]*\([0-9][0-9]*\)[^0-9][^0-9]*\([0-9][0-9]*\)\.[0-9]*[^0-9][^0-9]*\([0-9][0-9]*\)/\3/p' $infile`
tstart=`sed -n '1s/[^0-9]*\([0-9][0-9]*\)[^0-9][^0-9]*\([0-9][0-9]*\)\.[0-9]*[^0-9][^0-9]*\([0-9][0-9]*\)/\1/p' $infile`
tstep=`sed -n '1s/[^0-9]*\([0-9][0-9]*\)[^0-9][^0-9]*\([0-9][0-9]*\)\.[0-9]*[^0-9][^0-9]*\([0-9][0-9]*\)/\2/p' $infile`

# Rest of the file has spurious newlines... correct this!
# Also get rid of multiple spaces and initial spaces.
# Store results in temporary file.
sed -n '2,${
N
s/\n/ /
N
s/\n/ /
N
s/\n/ /
s/  */ /g
s/^  *//
p
}' $infile > $tmpfile

# Re-read start and stop times
tstart=`sed -n '1s/[^0-9].*$//p' $tmpfile`
tstop=`sed -n '$s/[^0-9].*$//p' $tmpfile`

# Sanity check
dt1=`expr $nlines \* $tstep - $tstep`
dt2=`expr $tstop - $tstart`
if test $dt1 -ne $dt2
then
  echo "sanity check failed: time step seems wrong"
  echo "nlines =" $nlines
  echo "tstart =" $tstart
  echo "tstop =" $tstop
  echo "tstep =" $tstep
  exit 1
fi

# Print ILWD file
printf "<?ilwd?>\n"
printf "  <ilwd name='$infile::sequence' size='7'>\n"
printf "    <lstring name='real:domain' size='4'>TIME</lstring>\n"
printf "    <int_4u name='gps_sec:start_time' units='sec'>$tstart</int_4u>\n"
printf "    <int_4u name='gps_sec:start_time' units='nanosec'>0</int_4u>\n"
printf "    <int_4u name='gps_sec:stop_time' units='sec'>$tstop</int_4u>\n"
printf "    <int_4u name='gps_sec:stop_time' units='nanosec'>0</int_4u>\n"
printf "    <real_4 name='time:step_size' units='sec'>$tstep</real_4>\n"
printf "    <real_4 dims='9,$nlines' name='data' ndim='2' units='s,s,s,s/s,s/s,s/s,1/s,1/s,1/s'>"
# The data: delete GPS time from data file and remove newlines.
sed 's/[^ ]* //' $tmpfile | tr '\n' ' '
printf "</real_4>\n"
printf "  </ilwd>\n"

rm -f $tmpfile
