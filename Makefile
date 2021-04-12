# Makefile for MPP coursework

## There are two versions, serial and parallel

# include config file
include config.mk

## (default)       : compile both serial and parallel code.
all : ser par

# compile ***************************************
## ser             : compile serial version
.PHONY : ser
ser : $(SEROBJ)
	$(CC) -o $(SEREXE) $^ $(LFLAGS)

$(SEROBJ_DIR)/%.o : $(SERSRC_DIR)/%.c pre-build
	$(CC) -c $< -o $@

## par             : compile parallel version
.PHONY : par
par : $(PAROBJ)
	$(MPICC) $(CFLAGS) -o $(PAREXE) $^ $(LFLAGS)

$(PAROBJ_DIR)/%.o : $(PARSRC_DIR)/%.c pre-build
	$(MPICC) $(CFLAGS) -c $< -o $@

# execute ***************************************
## srun            : run serial code with default seed
.PHONY :srun
srun : ser
	./$(SEREXE) 1564

## prun            : run parallel code with default seed and 4 node
.PHONY : prun
prun : par
	mpirun -n 16 ./$(PAREXE) 1564

## run             : run both serial and parallel
.PHONY : run
run : srun prun

## test            : check if the two outputs is identical
.PHONY : test
test : clean $(PAREXE) $(SEREXE)
	mkdir -p testOracle
	./test.sh | grep -E "Files|Test"

$(PAREXE) : par

$(SEREXE) : ser

# support ***************************************
# create the folder for object files
.PHONY : pre-build
pre-build :
	mkdir -p $(SEROBJ_DIR)
	mkdir -p $(PAROBJ_DIR)

## clean           : clean generated files.
.PHONY : clean
clean : cleanobj cleanexe cleanout

## cleanobj        : clean the object files
.PHONY : cleanobj
cleanobj :
	rm -rf $(SEROBJ_DIR)
	rm -rf $(PAROBJ_DIR)

## cleanexe        : clean the executable files
.PHONY : cleanexe
cleanexe :
	rm -f $(SEREXE)
	rm -f $(PAREXE)

## cleanout        : clean the output files
.PHONY : cleanout
cleanout :
	rm -f *.pgm
	rm -f *.dat
	rm -rf testOracle

# Help
.PHONY : help
help : Makefile
	@sed -n 's/^##//p' $<

sub :
	qsub percolate.pbs

stat :
	qstat -u s1829767
