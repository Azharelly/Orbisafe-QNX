#include <cstdio>
#include <cstring>   
#include <cerrno>    
#include <pthread.h> 
#include <sched.h>   

constexpr int wanted_priority = 50; 

const char* policy_name(int policy) {
    if (policy == SCHED_FIFO) {
        return "SCHED_FIFO";
    }
    if (policy == SCHED_RR) {
        return "SCHED_RR";
    }
    if (policy == SCHED_OTHER) {
        return "SCHED_OTHER";
    }
    return "unknown";
}

int show_current(const char* label, int* policy_out, int* priority_out) {
    int policy = 0;
    sched_param param;
    memset(&param, 0, sizeof(param));
    int rc = pthread_getschedparam(pthread_self(), &policy, &param);
    if (rc != EOK) {
        printf("%s: pthread_getschedparam failed: %s\n", label, strerror(rc));
        return 1;
    }
    printf("%s: policy %s, priority %d\n", label, policy_name(policy), param.sched_priority);
    *policy_out = policy;
    *priority_out = param.sched_priority;
    return 0;
}

int main() {
    int policy = 0;
    int priority = 0;
    if (show_current("before", &policy, &priority) != 0) {
        return 1;
    }
    printf("max SCHED_FIFO priority reported by the system: %d\n",
           sched_get_priority_max(SCHED_FIFO));

    sched_param param;
    memset(&param, 0, sizeof(param));
    param.sched_priority = wanted_priority;
    int rc = pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
    if (rc != EOK) {
        printf("pthread_setschedparam failed: %s\n", strerror(rc));
        printf("RESULT: DOES NOT WORK\n");
        return 1;
    }

    if (show_current("after", &policy, &priority) != 0) {
        return 1;
    }
    bool applied = (policy == SCHED_FIFO && priority == wanted_priority);

    int max_priority = sched_get_priority_max(SCHED_FIFO);
    param.sched_priority = max_priority;
    rc = pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
    if (rc == EOK) {
        printf("extra: priority %d (the maximum) was accepted\n", max_priority);
    } else {
        printf("extra: priority %d was refused: %s\n", max_priority, strerror(rc));
    }

    printf(applied ? "RESULT: WORKS\n" : "RESULT: DOES NOT WORK (value not applied)\n");
    return applied ? 0 : 1;
}