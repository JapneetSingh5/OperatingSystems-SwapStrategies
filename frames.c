#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

struct pte {
    int vpn;
    int frame_number;
    int dirty;
    int valid;
    int wildcard;
};

void print_state(int num_mem_access, int misses, int writes, int drops){
    printf("Number of memory accesses:%d\nNumber of misses:%d\nNumber of writes:%d\nNumber of drops:%d\n", num_mem_access, misses, writes, drops);
}

void print_verbose_state(int read, int written, int to_disk, int dropped_dirty, int dropped_not_dirty){
    if(to_disk==1){
        printf("Page 0x%05x was read from disk, page 0x%05x was written to the disk.",read,written);
    }else if(dropped_dirty){
        printf("Page 0x%05x was read from disk, page 0x%05x was dropped (it was not dirty).",read,written);
    }else if(dropped_not_dirty){
        printf("Page 0x%05x was read from disk, page 0x%05x was dropped (it was dirty).",read,written);
    }else{
        printf("Error encountered, exiting....\n");
        exit(1);
    }
}

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
    if(num_frames<0 || num_frames>1000){
        printf("Invalid number of frames \n");
    }
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
    if(strcmp(strategy,OPT)==0){
        printf("Performing OPT.... \n");
    }else if(strcmp(strategy, FIFO)==0){
        printf("Performing FIFO.... \n");
    }else if(strcmp(strategy, CLOCK)==0){
        printf("Performing CLOCK.... \n");
    }else if(strcmp(strategy, LRU)==0){
        printf("Performing LRU.... \n");
    }else if(strcmp(strategy, RANDOM)==0){
        printf("Performing RANDOM.... \n");
    }else{
        printf("Unknown strategy entered, exiting .... \n");
        exit(1);
    }
    // print_state(0,0,0,0);
    print_verbose_state(10,10,1,0,0);
    // char ch;
    // do{
    //     ch = fgetc(file_ptr);
    //     printf("%c", ch);
    // } while (ch != EOF);
    // fclose(file_ptr);

    return 0;
}