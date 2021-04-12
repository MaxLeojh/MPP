# Message-passing Programming Coursework
---
## 1. Code Structure
The file structure is showed below:
```
|-- /
    |-- org
    |    |-- Makefile
    |    |-- percolate.c
    |    |-- ...
    |    
    |-- par
    |    |-- argInput.c
    |    |-- argInput.h
    |    |-- arralloc.c
    |    |-- arralloc.h
    |    |-- computation.c
    |    |-- initialize.c
    |    |-- mplib.c
    |    |-- mplib.h
    |    |-- percolate.c
    |    |-- percolate.h
    |    |-- percwritedynamic.c
    |    |-- uni.c
    |    |-- uni.h
    |    
    |-- ser
    |    |-- arralloc.c
    |    |-- ...
    |    
    |-- config.mk
    |-- Makefile
    |-- README.md
    |-- test.sh
    |-- percolate.pbs
```
The `org` folder contains the original code, and the `ser` folder contains the serial code that can take arguments and implement boundary periodic, this version of code will used for test. `par` folder contains the source code of **a working message-passing parallel version of the percolation problem using a two-dimensional domain decomposition**.
+ `percolate` files contain the main function of this code.
+ `argInput` files contain the functions used for arguments input, the input arguments are in pair:
    + **The first argument must be the random number seed without flag**.
    + -l [length] : the length of grid.
    + -r [density] : the density of grid.
    + -s [seed] : seed used for random number generator
    + -p [pgmFile] : output file.
+ `initialize` file contains the functions of initialization of the map, scatter map to processors, gather old array from processors to the master, calculate the length of different block on different processors, set halo for old and new arrays.
+ `computation` file contains the main loop function of the map update and the function to judge if the map is percolate.
+ `percwritedynamic` file contains the functions for output file. **This file directly copied from original code**.
+ `mplib` files contain the encapsulation of MPI functions.
+ `arralloc` files contain the functions used for memory allocation for arrays, **these files are copied from *Programming Skills* coursework**
+ `uni` files contain the functions for random number generation. **These files are copied from original code**.

`config.mk` and `Makefile` are build script, through which you can compile, run and test the code. `test.sh` is the script that used for code test, `percolate.pbs` is the script used for running the program on the back end of Cirrus.

## 2. Build and Run on Cirrus
Before you starting build the code, you need to load 2 modules which are:
+ module load mpt
+ module load intel-compilers-18

To build the code, you should cd to the main directory and use command `make` to compile the code, the the default name of executable file is `percolate`. the serial version is `percolate_ser`, you can change the executable file name in `config.mk`.

To run the serial code, just issue the command `./percolate_ser [seed]`, the serial code also take the same arguments as the parallel version.

To run the parallel code on the frontend, use `mpirun -n [numProc] ./percolate [seed]`, the program take arguments as mentioned before, for example, you you can use `mpirun -n 4 ./percolate 1564 -l 300 -r 0.33` to specify the length of grid to 300 and the density to 0.33, the random number seed is 1564, and the code will run on 4 processors.

To run the parallel code on the backend of cirrus, you need to modify the `percolete.pbs`, then use command `qsub percolate.pbs`, submit the job on cirrus. Editing `percolate.pbs` file to change the program behaviour on the backend of cirrus.

This program is able to take any valid p(number of processors), l(length of grid), r(density of map), s(random number seed), as long as the length of map grid is greater than length of processor grid.

To see more *make commands* use `make help`.

## 3. Test
To test the code, modify the variables in test.sh, and then issue command `make test` to test the code.
If you want to run the test.sh script, you may need to use `chmod +x test.sh` to give the authority to the script file.
