#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// comments and definitions of strategies used are sourced from OSTEP #22
int debug = 0;
// 32 bit address space used
int addr_size = 32;
// offset stored using 12 bits in the address
int page_frame_size = 12;
// virtual page number hence, will be stored in the remaining 20 bits
int page_num_size = 20;
// for clock strategy
int clock_pointer = 0;

struct pte *frames;

struct pte {
    int vpn;
    int frame_number;
    int dirty;
    int valid;
    int use;
    int first_read;
    int last_read;
    int first_write;
    int last_write;
    int brought_in_at;
    int last_used;
};

struct accessed_page {
    int address;
    int read;
};

void print_state(int num_mem_access, int misses, int writes, int drops){
    printf("Number of memory accesses:%d\nNumber of misses:%d\nNumber of writes:%d\nNumber of drops:%d\n",num_mem_access,misses,writes,drops);
}

void print_verbose_state(int read, int written, int was_dirty){
    if(was_dirty==1){
        printf("Page 0x%05x was read from disk, page 0x%05x was written to the disk. \n",read,written);
    }else if(was_dirty==0){
        printf("Page 0x%05x was read from disk, page 0x%05x was dropped (it was not dirty). \n",read,written);
    }else{
        printf("Error: inaccessible code reached, exiting.. \n");
        exit(1);
    }
}

int execute_opt(char* trace_file_name, int num_frames, int is_verbose){
    if(debug==1) printf("Performing OPT.... \n");
// The optimal replacement policy
// leads to the fewest number of misses overall. 
// Belady showed that a simple (but, unfortunately, difficult to implement!) 
// approach that replaces the page that will be 
// accessed furthest in the future is the optimal policy,
// resulting in the fewest-possible cache misses.
return -1;
}

int execute_fifo(struct pte frames[], int num_frames){
    if(debug==1) printf("Performing FIFO.... \n");
    // FIFO (first-in, first-out) replacement, where pages
    // were simply placed in a queue when they enter the system; when a replacement occurs, the page on the tail of the queue (the “first-in” page) is
    // evicted. FIFO has one great strength: it is quite simple to implement.
    int evict_idx = 0;
    int first_in = frames[0].brought_in_at;
    assert(first_in != -1 && "brought in at should not be -1, as frame is in the memory hence must be brought in at some point");
    for(int i=1; i<num_frames; i++){
        assert(frames[i].brought_in_at != -1 && "brought in at should not be -1, as frame is in the memory hence must be brought in at some point");
        if(frames[i].brought_in_at < first_in){
            first_in = frames[i].brought_in_at;
            evict_idx = i;
        }
    }
    return evict_idx;
}

// where to begin?
// how to update begin after evict inde return?
// piazza doubt?
int execute_clock(int num_frames, int last_evicted_index){
    if(debug==1) printf("Performing CLOCK.... \n");
    int start_pointer = clock_pointer;
    int evict_idx_temp = -1;
    do {
        if(frames[clock_pointer].use == 1) {
            frames[clock_pointer].use = 0;
            clock_pointer = (clock_pointer + 1) % num_frames;
            continue;
        }
        evict_idx_temp = clock_pointer;
        clock_pointer = (clock_pointer + 1) % num_frames;
        return evict_idx_temp % num_frames;
    } while (start_pointer != clock_pointer);

    evict_idx_temp = clock_pointer;
    clock_pointer = (clock_pointer + 1) % num_frames;
    return (evict_idx_temp)%num_frames;
// How does the OS employ the use bit to approximate LRU? Well, there
// could be a lot of ways, but with the clock algorithm, one simple
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

int execute_lru(int num_frames){
    //  printf("Performing LRU.... \n");
    //  Similarly, the Least-Recently Used (LRU) policy replaces the least-recently-used page.
    int evict_idx = 0;
    int last_used = frames[0].last_used;
    if(frames[0].last_used == -1){
        last_used = frames[0].last_used;
    }
    if(frames[0].last_used != -1 && frames[0].last_used < last_used){
        last_used = frames[0].last_used;
    }
    assert(last_used != -1 && "last used should not be -1, as frame is in the memory hence must be accessed at some point");
    for(int i=1; i<num_frames; i++){
        if(frames[i].last_used != -1 && frames[i].last_used < last_used){
            last_used = frames[i].last_used;
            evict_idx = i;
        }
    }
    return evict_idx;
}

int execute_random(int num_frames){
    // printf("Performing RANDOM.... \n");
    // simply picks a random page to replace under memory pressure
    int idx = rand() % num_frames;
    return idx;
}

int main(int argc, char** argv)
{
    int seed = 5635;
    srand(seed);
    char* OPT = "OPT";
    char* FIFO = "FIFO";
    char* CLOCK = "CLOCK";
    char* LRU = "LRU";
    char* RANDOM = "RANDOM";
    char* verbose = "-verbose";
    // 2nd argument will be the name of the trace file 
    char* trace_file_name = argv[1];
    int num_frames = atoi(argv[2]);
    if(num_frames<0 || num_frames>1000){
        printf("Invalid number of frames \n");
    }
    char* strategy = argv[3];
    int is_verbose = 0;
    // verbose will be the 5th argument, if it is present 
    // sanity check to check if it is indeed verbose, set verbose flag to true
    if(argc==5){
        if(strcmp(verbose, argv[4])==0){
            is_verbose = 1;
        }
    }
    // printf("Is verbose %d \n", is_verbose);
    FILE* file_ptr = fopen(trace_file_name, "r");
    if(file_ptr==NULL){
        printf("File not found, exiting.. \n");
        exit(1);
    }
    frames = (struct pte*)malloc(num_frames * sizeof(struct pte));
    for(int i=0; i<num_frames; i++){
        frames[i].vpn = -1;
        frames[i].frame_number = i;
        frames[i].dirty = 0;
        frames[i].valid = 0;
        frames[i].use = -1;
        frames[i].first_read = -1;
        frames[i].last_read = -1;
        frames[i].first_write = -1;
        frames[i].last_write = -1;
        frames[i].brought_in_at = -1;
        frames[i].last_used = -1;
    }
    // read line and update these 3 parameters at each iteration
    char read_or_write;
    unsigned virt_mem_addr;
    int loop_exit = 0;
    // meme accesses counts the number of lines basically
    int mem_accesses = 0;
    // writes to disk incremented at each dirty drop
    int writes_to_disk = 0;
    int drops_non_dirty = 0;
    // evictions incremented at every eviction / frames found full
    int evictions = 0;
    // missess incremented whenever frame not found in memory
    int misses = 0;
    int last_evicted_index = -1;
    while(loop_exit == 0){
        if(EOF == fscanf(file_ptr, "%x %c", &virt_mem_addr, &read_or_write)){
            loop_exit=1;
        }else{
            mem_accesses++;
        }
        int page_num_acc = virt_mem_addr >> page_frame_size;
        if(read_or_write == 'R'){
            // READ case
            int page_found_in_mem = 0;
            int empty_frame_idx = -1;
            for(int i=0; i<num_frames; i++){
                if(frames[i].vpn == page_num_acc){
                    // Page found in memory
                    page_found_in_mem = 1;
                    assert(frames[i].valid == 1 && "page in mem, valid bit should be 1");
                    if(frames[i].first_read==-1){
                        frames[i].first_read = mem_accesses - 1;
                        frames[i].last_read = mem_accesses - 1;
                    }else{
                        frames[i].last_read = mem_accesses - 1;
                    }
                    frames[i].last_used = mem_accesses - 1;
                    break;
                }else if(frames[i].vpn==-1 && empty_frame_idx == -1){
                    empty_frame_idx = i;
                }
            }
            if(page_found_in_mem == 0){
                if(debug==1) printf("READ - Missed page %d in memory at access %d \n", page_num_acc, mem_accesses);
                misses++;
                if(empty_frame_idx == -1){
                    evictions++;
                    // no empty frame left, will have to evict some frame
                    // find index of frame to evict according to whatever strategy is being used
                    int evict_idx = -1;
                    if(strcmp(strategy,OPT)==0){
                        evict_idx = execute_opt(trace_file_name, num_frames, is_verbose);
                    }else if(strcmp(strategy, FIFO)==0){
                        evict_idx = execute_fifo(frames, num_frames);
                    }else if(strcmp(strategy, CLOCK)==0){
                        evict_idx = execute_clock(num_frames, last_evicted_index);
                    }else if(strcmp(strategy, LRU)==0){
                        evict_idx = execute_lru(num_frames);
                    }else if(strcmp(strategy, RANDOM)==0){
                        evict_idx = execute_random(num_frames);
                    }else{
                        printf("Unknown strategy entered, exiting .... \n");
                        exit(1);
                    }
                    assert(evict_idx!=-1);
                    last_evicted_index = evict_idx;
                    if(debug==1) printf("Evicting %d, is  dirty %d \n",frames[evict_idx].vpn,frames[evict_idx].dirty);
                    if(frames[evict_idx].dirty==1){
                        if(debug==1) printf("dirty drop page %d\n", frames[evict_idx].vpn);
                        writes_to_disk++;
                        // evicting dirty page, print state accordingly
                        // evicting frames[evict_idx].vpn, bringing in page_num_acc
                        if(is_verbose==1){
                            print_verbose_state(page_num_acc, frames[evict_idx].vpn, 1);
                        }
                    }else{
                        if(debug==1) printf("Non dirty drop page %d\n", frames[evict_idx].vpn);
                        drops_non_dirty++;
                        // evicted page was not dirty
                        // evicting frames[evict_idx].vpn, bringing in page_num_acc
                        if(is_verbose==1){
                            print_verbose_state(page_num_acc, frames[evict_idx].vpn, 0);
                        }
                    }
                    // simulating bringing the page in from memory ...
                    if(debug==1) printf("page number %d is at frame no. %d \n", page_num_acc, evict_idx);
                    frames[evict_idx].vpn = page_num_acc;
                    frames[evict_idx].brought_in_at = mem_accesses - 1;
                    frames[evict_idx].use = 1;
                    frames[evict_idx].frame_number = evict_idx;
                    frames[evict_idx].valid = 1;
                    frames[evict_idx].first_read = mem_accesses - 1;
                    frames[evict_idx].last_read = mem_accesses - 1;
                    frames[evict_idx].first_write = -1;
                    frames[evict_idx].last_write = -1;
                    frames[evict_idx].dirty = 0;
                    frames[evict_idx].last_used = mem_accesses - 1;
                }else{
                    // simulating bringing the page in from memory ...
                    // set page num to empty idx page frame
                    assert(empty_frame_idx != -1 && "empty frame index can't be -1");
                    if(debug==1) printf("page number %d is at frame no. %d \n", page_num_acc, empty_frame_idx);
                    frames[empty_frame_idx].vpn = page_num_acc;
                    frames[empty_frame_idx].frame_number = empty_frame_idx;
                    frames[empty_frame_idx].brought_in_at = mem_accesses - 1;
                    frames[empty_frame_idx].use = 1;
                    frames[empty_frame_idx].valid = 1;
                    frames[empty_frame_idx].first_read = mem_accesses - 1;
                    frames[empty_frame_idx].last_read = mem_accesses - 1;
                    frames[empty_frame_idx].first_write = -1;
                    frames[empty_frame_idx].last_write = -1;
                    frames[empty_frame_idx].dirty = 0;
                    frames[empty_frame_idx].last_used = mem_accesses - 1;
                }
            }
        }else{
            // WRITE case
            int page_found_in_mem = 0;
            int empty_frame_idx = -1;
            for(int i=0; i<num_frames; i++){
                if(frames[i].vpn == page_num_acc){
                    // Page found in memory, make it dirty
                    page_found_in_mem = 1;
                    assert(frames[i].valid == 1 && "page in mem, valid bit should be 1");
                    if(debug==1) printf("Writing %d, now DIRTY \n", frames[i].vpn);
                    frames[i].dirty = 1;
                    if(frames[i].first_write == -1){
                        frames[i].first_write = mem_accesses - 1;
                        frames[i].last_write = mem_accesses - 1;
                    }else{
                        frames[i].last_write = mem_accesses - 1;
                    }
                    frames[i].last_used = mem_accesses - 1;
                    break;
                }else if(frames[i].vpn==-1 && empty_frame_idx == -1){
                    empty_frame_idx = i;
                }
            }
            // page not found in memory
            if(page_found_in_mem == 0){
                if(debug==1) printf("WRITE - Missed page %d in memory at access %d \n", page_num_acc, mem_accesses);
                misses++;
                // if all frames are full
                if(empty_frame_idx == -1){
                    evictions++;
                    // no empty frame left, will have to evict some frame
                    // find index of frame to evict according to whatever strategy is being used
                    int evict_idx = -1;
                    if(strcmp(strategy,OPT)==0){
                        evict_idx = execute_opt(trace_file_name, num_frames, is_verbose);
                    }else if(strcmp(strategy, FIFO)==0){
                        evict_idx = execute_fifo(frames, num_frames);
                    }else if(strcmp(strategy, CLOCK)==0){
                        evict_idx = execute_clock(num_frames, last_evicted_index);
                    }else if(strcmp(strategy, LRU)==0){
                        evict_idx = execute_lru(num_frames);
                    }else if(strcmp(strategy, RANDOM)==0){
                        evict_idx = execute_random(num_frames);
                    }else{
                        printf("Unknown strategy entered, exiting .... \n");
                        exit(1);
                    }
                    last_evicted_index = evict_idx;
                    if(debug==1) printf("Evicting %d, is  dirty %d \n",frames[evict_idx].vpn,frames[evict_idx].dirty);
                    if(frames[evict_idx].dirty==1){
                        writes_to_disk++;
                        if(debug==1) printf("dirty drop page %d\n", frames[evict_idx].vpn);
                        // evicting dirty page, print state accordingly
                        // evicting frames[evict_idx].vpn, bringing in page_num_acc
                        if(is_verbose==1){
                            print_verbose_state(page_num_acc, frames[evict_idx].vpn, 1);
                        }
                    }else{
                        drops_non_dirty++;
                        if(debug==1) printf("Non dirty drop page %d\n", frames[evict_idx].vpn);
                        // evicted page was not dirty
                        // evicting frames[evict_idx].vpn, bringing in page_num_acc
                        if(is_verbose==1){
                            print_verbose_state(page_num_acc, frames[evict_idx].vpn, 0);
                        }
                    }
                    // simulating bringing the page in from memory ...
                    if(debug==1) printf("page number %d is at frame no. %d \n", page_num_acc, evict_idx);
                    frames[evict_idx].vpn = page_num_acc;
                    frames[evict_idx].frame_number = evict_idx;
                    frames[evict_idx].brought_in_at = mem_accesses - 1;
                    frames[evict_idx].use = 1;
                    frames[evict_idx].valid = 1;
                    frames[evict_idx].first_write = mem_accesses - 1;
                    frames[evict_idx].last_write = mem_accesses - 1;
                    frames[evict_idx].first_read = -1;
                    frames[evict_idx].last_read = -1;
                    frames[evict_idx].dirty = 1;
                    frames[evict_idx].last_used = mem_accesses - 1;
                }else{
                    // simulating bringing the page in from memory ...
                    // set page num to empty idx page frame
                    assert(empty_frame_idx != -1 && "empty frame index can't be -1");
                    if(debug==1) printf("page number %d is at frame no. %d \n", page_num_acc, empty_frame_idx);
                    frames[empty_frame_idx].vpn = page_num_acc;
                    frames[empty_frame_idx].frame_number = empty_frame_idx;
                    frames[empty_frame_idx].brought_in_at = mem_accesses - 1;
                    frames[empty_frame_idx].use = 1;
                    frames[empty_frame_idx].valid = 1;
                    frames[empty_frame_idx].first_write = mem_accesses - 1;
                    frames[empty_frame_idx].last_write = mem_accesses - 1;
                    frames[empty_frame_idx].first_read = -1;
                    frames[empty_frame_idx].last_read = -1;
                    frames[empty_frame_idx].dirty = 1;
                    frames[empty_frame_idx].last_used = mem_accesses - 1;
                }
            }            
        }
    }
    print_state(mem_accesses, misses, writes_to_disk, drops_non_dirty);
    // printf("Evictions %d \n", evictions);
    return 0;
}