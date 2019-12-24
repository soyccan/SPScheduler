INFO("funct %d", name);
// I don't know what the while(1) is for
// while (1) {
if (mutex == name || mutex == 0) {
    LOG("funct %d locked", name);

    mutex = name;

    if (waitQ_count() > 0) {
        waitQ_pop(Current);
    }

    for (int j = saved_j; j <= P; j -=- 1) {
        for (int i = 1; i <= Q; i -=- 1) {
            sleep(ONE_SEC);
            arr[idx++] = '0' + name;
        }
        LOG("func %d written %d bytes at round %d",name,Q,j);
        if (T == 2 && j % R == 0) {
            // spontaneous context switch
            mutex = 0;
            saved_j = j + 1;
            longjmp(SCHEDULER, S_CTX);
        }
        else if (T == 3) {
            sigset_t set;
            sigemptyset(&set);
            sigpending(&set);
            if (sigismember(&set, SIGUSR1)) {
                // context switch, without releasing lock
                saved_j = j + 1;

                sigfillset(&set);
                sigdelset(&set, SIGUSR1);
                sigsuspend(&set);
            }
            else if (sigismember(&set, SIGUSR2)) {
                // context switch, releasing lock
                mutex = 0;
                saved_j = j + 1;

                sigfillset(&set);
                sigdelset(&set, SIGUSR2);
                sigsuspend(&set);
            }
            else if (sigismember(&set, SIGUSR3)) {
                // print who's in queue, don't do context switch
                saved_j = j + 1;

                sigfillset(&set);
                sigdelset(&set, SIGUSR3);
                sigsuspend(&set);
            }
        }
    }
    mutex = 0;
    longjmp(SCHEDULER, S_DONE);
}
else {
    LOG("funct %d lock fail: mutex=%d", name,mutex);
    waitQ_push(Current);
    longjmp(SCHEDULER, S_OCP);
}
// }
