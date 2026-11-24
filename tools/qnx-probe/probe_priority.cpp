#include <cstdio>
#include <cstring>        
#include <cstdint>        
#include <cerrno>         
#include <unistd.h>       
#include <sys/neutrino.h> 

constexpr const char* info_file = "/tmp/probe_priority.txt"; 
constexpr uint16_t note_type = 1000;                    
constexpr int notes_to_send = 3;

struct note {
    uint16_t type;     
    uint16_t sequence;
};

struct response {
    uint16_t status;  
    uint16_t sequence;
};

int run_server() {
  
    int chid = ChannelCreate(_NTO_CHF_FIXED_PRIORITY);
    if (chid == -1) {
        printf("ChannelCreate failed: %s\n", strerror(errno));
        return 1;
    }

  
    FILE* f = fopen(info_file, "w");
    if (f == NULL) {
        printf("fopen failed: %s\n", strerror(errno));
        ChannelDestroy(chid);
        return 1;
    }
    fprintf(f, "%d %d\n", (int)getpid(), chid);
    fclose(f);
    printf("server: channel %d created with FIXED_PRIORITY (pid %d)\n", chid, (int)getpid());

    note msg;
    int handled = 0;
    while (handled < notes_to_send) {
        rcvid_t rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
        if (rcvid == -1) {
            printf("MsgReceive failed: %s\n", strerror(errno));
            break;
        }
        if (rcvid == 0) {
            continue; 
        }
        if (msg.type != note_type) {
            MsgError(rcvid, ENOSYS);
            continue;
        }

        response reply;
        reply.status = 0;
        reply.sequence = msg.sequence;
        MsgReply(rcvid, EOK, &reply, sizeof(reply));

        printf("server: received note %u, replied\n", msg.sequence);
        handled++;
    }

    remove(info_file);
    ChannelDestroy(chid);
    return 0;
}

int run_client() {
  
    FILE* f = fopen(info_file, "r");
    if (f == NULL) {
        printf("cannot open %s (is the server running?): %s\n", info_file, strerror(errno));
        return 1;
    }
    int server_pid = 0;
    int server_chid = 0;
    int read_count = fscanf(f, "%d %d", &server_pid, &server_chid);
    fclose(f);
    if (read_count != 2) {
        printf("bad contents in %s\n", info_file);
        return 1;
    }

    int coid = ConnectAttach(0, server_pid, server_chid, _NTO_SIDE_CHANNEL, 0);
    if (coid == -1) {
        printf("ConnectAttach failed: %s\n", strerror(errno));
        return 1;
    }

    int failures = 0;
    for (uint16_t seq = 1; seq <= notes_to_send; seq++) {
        note req;
        req.type = note_type;
        req.sequence = seq;

        response rep;
        if (MsgSend(coid, &req, sizeof(req), &rep, sizeof(rep)) == -1) {
            printf("MsgSend failed: %s\n", strerror(errno));
            failures++;
            break;
        }

        if (rep.status == 0 && rep.sequence == seq) {
            printf("client: note %u -> reply ok\n", seq);
        } else {
            printf("client: note %u -> MISMATCH (status %u, sequence %u)\n",
                   seq, rep.status, rep.sequence);
            failures++;
        }
    }

    ConnectDetach(coid);
    printf(failures == 0 ? "RESULT: WORKS\n" : "RESULT: DOES NOT WORK\n");
    return failures == 0 ? 0 : 1;
}

int main(int argc, char** argv) {
    if (argc == 2 && strcmp(argv[1], "server") == 0) {
        return run_server();
    }
    if (argc == 2 && strcmp(argv[1], "client") == 0) {
        return run_client();
    }
    printf("Usage: %s server | client\n", argv[0]);
    return 1;
}