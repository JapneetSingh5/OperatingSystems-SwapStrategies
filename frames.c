#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
    char* OPT = "OPT";
    char* FIFO = "FIFO";
    char* CLOCK = "CLOCK";
    char* LRU = "LRU";
    char* RANDOM = "RANDOM";
    char* verbose = "-verbose";
    printf("You have entered %d arguments \n",argc);
    for (int i = 0; i < argc; ++i)
        printf("%s \n", argv[i]);
    char* trace_file_name = argv[1];
    printf("%s\n", trace_file_name);
    int num_frames = atoi(argv[2]);
    printf("Number of frames: %d \n", num_frames);
    char* strategy = argv[3];
    printf("%s \n",strategy);
    int is_verbose = 0;
    if(argc==5){
        if(strcmp(verbose, argv[4])==0){
            is_verbose = 1;
        }
    }
    printf("Is verbose %d \n", is_verbose);
    FILE* file_ptr = fopen(trace_file_name, "r");
    if(file_ptr==NULL){
        printf("File not found, exiting.. \n");
        exit(1);
    }
    char ch;
    do{
        ch = fgetc(file_ptr);
        printf("%c", ch);
    } while (ch != EOF);
    fclose(file_ptr);

    return 0;
}