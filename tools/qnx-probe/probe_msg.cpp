#include <cstdio>
#include <cstring> //comparar texto
#include <cstdint> //tamano del mensaje 16 bits
#include <cerrno> //codigos de error
#include <sys/neutrino.h> //intercambio de mensajes
#include <sys/iofunc.h> //definiciones de mensajes de E/S de QNX    
#include <sys/dispatch.h> //registrar el servidor y que el cliente lo pueda encontrar


constexpr const char* probe_service = "probe";

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
union server_inbox {
    note msg;
    struct _pulse pulse;
};
int run_server(){
    name_attach_t* attach = name_attach(NULL, probe_service, 0);
    if (attach == NULL){
        printf("name_attach failed: %s\n", strerror(errno));
        return 1;
    }
    printf("server: waiting on \"%s\"\n", probe_service);

    server_inbox buf;
        int handled = 0;
        while (handled < notes_to_send){
            rcvid_t rcvid = MsgReceive(attach -> chid, &buf, sizeof(buf), NULL);
            if (rcvid == -1){
                printf("MsgReceive failed: %s\n", strerror(errno));
                break;
            }
            if (rcvid == 0){
                if (buf.pulse.code == _PULSE_CODE_DISCONNECT) {
                ConnectDetach(buf.pulse.scoid);
                }
            continue;
            }
                    
            if (buf.msg.type != note_type) {
                MsgError(rcvid, ENOSYS);
                continue;
            }

            response reply;
            reply.status = 0;
            reply.sequence = buf.msg.sequence;

            MsgReply(rcvid, EOK, &reply, sizeof(reply));

            printf("server received note %u\n", buf.msg.sequence);
            handled++;
            }
        name_detach(attach, 0);
    return 0;

}

int run_client() {
    int coid = name_open(probe_service, 0);
    if (coid == -1) {
        printf("name_open failed: %s\n", strerror(errno));
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

    name_close(coid);
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
