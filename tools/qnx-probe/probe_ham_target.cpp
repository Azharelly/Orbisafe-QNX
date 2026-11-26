#include <cstdio>
#include <cstring>  
#include <cerrno>   
#include <unistd.h> 

constexpr const char* start_log = "/tmp/ham_starts.txt"; 

int main() {
    FILE* f = fopen(start_log, "a"); 
    if (f == NULL) {
        printf("target: cannot open %s: %s\n", start_log, strerror(errno));
        return 1;
    }
    fprintf(f, "started, pid %d\n", (int)getpid());
    fclose(f);

    printf("target: started, pid %d\n", (int)getpid());
    while (true) {
        sleep(1); 
    }
}