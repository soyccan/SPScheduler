#include "common.h"

int main() {
    // TODO: init

    int P, Q, R;
    int sigQ[12];
    scanf("%d%d%d", &P, &Q, &R);
    for (int i = 0; i < R; ++i) {
        scanf("%d", &sigQ[i]);
    }

    int pdata[2], pmsg[2]; // pipe for data and ACK message
    G(pipe(pdata));
    G(pipe(pmsg));

    int pid;
    G(pid = fork());
    if (pid > 0) {
        // main
        G(close(pdata[1]));
        G(close(pmsg[1]));

        char buf[100];

        for (int i = 0; i < R; ++i) {
            LOG("sigQ[%d]=%d", i,sigQ[i]);
            sleep(FIVE_SEC);

            int sig;
            if (sigQ[i] == 1) {
                sig = SIGUSR1;
            }
            else if (sigQ[i] == 2) {
                sig = SIGUSR2;
            }
            else if (sigQ[i] == 3) {
                sig = SIGUSR3;
            }
            else {
                assert(false);
            }

            G(kill(pid, sig));

//             if (sigQ[i] == 3) {
//                 G(read(pdata[0], buf, sizeof buf));
//                 LOG("main <- hw3: %s", buf);
//                 G(write(STDOUT_FILENO, buf, sizeof buf));
//             }

            G(read(pmsg[0], buf, sizeof buf));
            LOG("main <- hw3 msg: %s", buf);
//             assert();
        }
        G(read(pdata[0], buf, sizeof buf));
        LOG("main <- hw3 data: %s", buf);
        G(write(STDOUT_FILENO, buf, sizeof buf));
//         size_t len = 0;
//         while (len < (sizeof buf) && buf[len] != '\n') ++len;
//         assert(len < (sizeof buf));
//         buf[len] = '\0';
    }
    else {
        // hw3
        G(dup2(pdata[1], STDOUT_FILENO));
        G(close(pdata[0]));
        G(close(pdata[1]));
        G(close(pmsg[0]));

        char _P[5], _Q[5], _pmsgfd[20];
        snprintf(_P, sizeof _P, "%d", P);
        snprintf(_Q, sizeof _Q, "%d", Q);
        snprintf(_pmsgfd, sizeof _pmsgfd, "%d", pmsg[1]);
        execl("./hw3", "./hw3", _P, _Q, "3", _pmsgfd, NULL);
    }
}
