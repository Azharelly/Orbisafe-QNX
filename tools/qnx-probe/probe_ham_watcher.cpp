#include <cstdio>
#include <cstring>    
#include <cerrno>      
#include <sys/netmgr.h>
#include <ha/ham.h>    

constexpr const char* entity_name = "probe_ham_target"; 
constexpr const char* target_path = "/tmp/probe_ham_target";

int main() {
    if (ham_connect(0) == -1) {
        printf("ham_connect failed: %s (is ham running?)\n", strerror(errno));
        return 1;
    }

    ham_entity_t* entity = ham_attach(entity_name, 0, -1, target_path, 0);
    if (entity == NULL) {
        printf("ham_attach failed: %s\n", strerror(errno));
        return 1;
    }

    ham_condition_t* condition = ham_condition(entity, CONDDEATH, "death", HREARMAFTERRESTART);
    if (condition == NULL) {
        printf("ham_condition failed: %s\n", strerror(errno));
        return 1;
    }

    ham_action_t* action = ham_action_restart(condition, "restart", target_path, HREARMAFTERRESTART);
    if (action == NULL) {
        printf("ham_action_restart failed: %s\n", strerror(errno));
        return 1;
    }

    printf("watcher: HAM is now watching %s\n", target_path);
    printf("RESULT: ATTACHED (confirm by killing it: slay probe_ham_target)\n");
    return 0;
}