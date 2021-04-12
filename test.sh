# Test shell script

#set input arguements
length=288
seed=1564
density=0.411
maxProc=37

# prepare test oracle
./percolate_ser ${seed} -l ${length} -r ${density} -p testOracle/map_${length}_${density}_${seed}.pgm | grep -E "number of changes|The average of|params|actual density" > testOracle/ser_${length}_${density}_${seed}.dat

# testing
for ((i=1; i<=${maxProc}; i=i+1))
do
    echo "Test: Number of processor = $i"
    mpirun -n $i ./percolate ${seed} -p map_$i.pgm -l ${length} -r ${density} | grep -E "number of changes|The average of|params|actual density" > par_$i.dat
    diff -qs map_$i.pgm testOracle/map_${length}_${density}_${seed}.pgm
    diff -qs par_$i.dat testOracle/ser_${length}_${density}_${seed}.dat
done
