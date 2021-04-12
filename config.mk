# serial compile
CC = cc
MPICC = mpicc

CFLAGS= -O3 -cc=gcc

LFLAGS = -lm

# serial folders
SERSRC_DIR = ser
SEROBJ_DIR = serobj

# parallel folders
PARSRC_DIR = par
PAROBJ_DIR = parobj

# serial source files
SERSRC = $(wildcard $(SERSRC_DIR)/*.c)

# parallel source files
PARSRC = $(wildcard $(PARSRC_DIR)/*.c)

# serial object files
SEROBJ = $(patsubst $(SERSRC_DIR)/%.c, $(SEROBJ_DIR)/%.o, $(SERSRC))

# parallel object files
PAROBJ = $(patsubst $(PARSRC_DIR)/%.c, $(PAROBJ_DIR)/%.o, $(PARSRC))

# serial executable file
SEREXE = percolate_ser

# parallel executable file
PAREXE = percolate
