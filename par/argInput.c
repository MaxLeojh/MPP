#include "argInput.h"
/*
 * This file contains the functions used for inputing arguements
 */

 // deal with the input argeuments from commond line
int input(int argc, char *args[], struct inputArgs *inputVal)
{
    int oc;
    int flag;
    //set default input values
    inputVal->length = 288;
    inputVal->density = 0.411;
    inputVal->seed = 1564;
    strcpy(inputVal->pgmFile, "map.pgm");

    inputVal->seed = atoi(args[1]);
    //optind:set the optarg index to 2
    optind = 2;

    //read arguments from commond line
    while((oc = getopt(argc, args, "l:r:s:p:")) != -1){
        switch(oc){
            case 'l': //set length
                inputVal->length = atoi(optarg);
                break;
            case 'r': //set density
                inputVal->density = atof(optarg);
                break;
            case 's': // set seed
                inputVal->seed = atoi(optarg);
                break;
            case 'p': // set PGM file path
                strcpy(inputVal->pgmFile,optarg);
                break;
        }
    }

    //check if the input is valid.
    if (checkInput(inputVal) == 0){
        printf("Input is not valid\n");
        return 3;
    }
    return 0;
}

//check if the input is valid.
int checkInput(struct inputArgs *inputVal)
{
    int flag = 1;
    if (inputVal->length < 1){
        flag *= 0;
    }
    if (inputVal->density > 1 || inputVal->density < 0){
        flag *= 0;
    }
    if (inputVal->seed > 900000000 || inputVal->seed < 0){
        flag *= 0;
    }
    return flag;
}

//print out the arguments, just for debug.
void printArgs(struct inputArgs *inputVal)
{
    printf("length = %d\n", inputVal->length);
    printf("density = %f\n", inputVal->density);
    printf("seed = %d\n", inputVal->seed);
    printf("PGM file path = %s\n", inputVal->pgmFile);
}
