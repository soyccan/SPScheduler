#include "common.h"

int main() {
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

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
            sleep(FIVE_SEC);

            LOG("sigQ[%d]=%d", i,sigQ[i]);
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

            if (sigQ[i] == 3) {
                G(read(pdata[0], buf, sizeof buf));
                LOG("main <- hw3 data: %s", buf);
                strtok(buf, "\n");
                puts(buf);
            }

            G(read(pmsg[0], buf, sizeof buf));
            LOG("main <- hw3 msg: %s", buf);
            assert(strcmp(buf, ACK) == 0);
        }
        G(read(pdata[0], buf, sizeof buf));
        LOG("main <- hw3 data: %s", buf);
        strtok(buf, "\n");
        puts(buf);
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
