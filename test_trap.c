#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void signal_handler(int sig) {
    printf("Caught signal %d\n", sig);
    exit(1);
}

int main() {
    // Register signal handlers
    signal(SIGTRAP, signal_handler);
    
    printf("Testing SIGTRAP handling on Apple Silicon M1\n");
    printf("About to send SIGTRAP to self...\n");
    fflush(stdout);
    
    // Send SIGTRAP to self
    kill(getpid(), SIGTRAP);
    
    printf("This should not be printed if SIGTRAP causes a trap\n");
    return 0;
}
