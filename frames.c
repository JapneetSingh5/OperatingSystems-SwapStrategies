#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

// comments and definitions of strategies used are sourced from OSTEP #22
struct pte {
    int vpn;
    int frame_number;
    int dirty;
    int valid;
    int wildcard;
};

struct accessed_page {
    int address;
    int read;
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

void execute_opt(char* trace_file_name, int num_frames, int is_verbose){
    printf("Performing OPT.... \n");
// The optimal replacement policy
// leads to the fewest number of misses overall. 
// Belady showed that a simple (but, unfortunately, difficult to implement!) 
// approach that *replaces the page that will be 
// accessed furthest in the future is the optimal policy*,
// resulting in the fewest-possible cache misses.
}

void execute_fifo(char* trace_file_name, int num_frames, int is_verbose){
    printf("Performing FIFO.... \n");
// FIFO (first-in, first-out) replacement, where pages
// were simply placed in a queue when they enter the system; when a replacement occurs, the page on the tail of the queue (the “first-in” page) is
// evicted. FIFO has one great strength: it is quite simple to implement.
}

void execute_clock(char* trace_file_name, int num_frames, int is_verbose){
    printf("Performing CLOCK.... \n");
//     How does the OS employ the use bit to approximate LRU? Well, there
// could be a lot of ways, but with the clock algorithm [C69], one simple
// approach was suggested. Imagine all the pages of the system arranged in
// a circular list. A clock hand points to some particular page to begin with
// (it doesn’t really matter which). When a replacement must occur, the OS
// checks if the currently-pointed to page P has a use bit of 1 or 0. If 1, this
// implies that page P was recently used and thus is not a good candidate
// for replacement. Thus, the use bit for P is set to 0 (cleared), and the clock
// hand is incremented to the next page (P + 1). The algorithm continues
// until it finds a use bit that is set to 0, implying this page has not been
// recently used (or, in the worst case, that all pages have been and that we
// have now searched through the entire set of pages, clearing all the bits).
}

void execute_lru(char* trace_file_name, int num_frames, int is_verbose){
    printf("Performing LRU.... \n");
//  Similarly, the Least-RecentlyUsed (LRU) policy replaces the least-recently-used page.
}

void execute_random(char* trace_file_name, int num_frames, int is_verbose){
    printf("Performing RANDOM.... \n");
//     simply picks a
// random page to replace under memory pressure
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
    struct pte pages[num_frames];
    // pages[99].frame_number = 10;
    printf("%d %d", pages[99].vpn, pages[99].frame_number);
    if(strcmp(strategy,OPT)==0){
        execute_opt(trace_file_name, num_frames, is_verbose);
    }else if(strcmp(strategy, FIFO)==0){
        execute_fifo(trace_file_name, num_frames, is_verbose);
    }else if(strcmp(strategy, CLOCK)==0){
        execute_clock(trace_file_name, num_frames, is_verbose);
    }else if(strcmp(strategy, LRU)==0){
        execute_lru(trace_file_name, num_frames, is_verbose);
    }else if(strcmp(strategy, RANDOM)==0){
        execute_random(trace_file_name, num_frames, is_verbose);
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