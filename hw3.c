#include "common.h"
#include "scheduler.h"

#ifdef DEBUG
# define INTERLEAVE()
#else
# define INTERLEAVE() sleep(1)
#endif

int P, Q, R;
jmp_buf* MAIN;
// Note: declare as global variable doesn't work
// jmp_buf won't store any data after setjmp()
// reason unknown
int mutex;
// lock for multiplexing

void funct_1(int name) {
    LOG("funct %d init", name);
    // Note
    Current = Head = malloc(sizeof(FCB));
    Current->Name = name;
    Current->Previous = Current->Next = Current;

    int saved_j = 1;

    if (setjmp(Current->Environment) == 0) {
        LOG("funct %d after setjmp", name);
        funct_5(name+1);
    }
    else {
        LOG("funct %d", name);
        if (mutex == name || mutex == 0) {
            LOG("funct %d locked", name);
            mutex = name;
            for (int j = saved_j; j <= P; j -=- 1) {
                for (int i = 1; i <= Q; i -=- 1) {
                    INTERLEAVE();
                    arr[idx++] = '0' + name;
                }
                if (j % R == 0) {
                    mutex = 0;
                    saved_j = j + 1;
                    longjmp(SCHEDULER, 1); // 1: undone
                }
            }
            mutex = 0;
            longjmp(SCHEDULER, -2); // -2: done
        }
        LOG("funct %d lock fail: mutex=%d", name,mutex);
        longjmp(SCHEDULER, 1); // 1: undone
    }
}


void funct_2(int name) {
    LOG("funct %d init", name);
    FCB_ptr new = malloc(sizeof(FCB));
    new->Name = name;
    new->Previous = Current;
    new->Next = Head;
    Current->Next = new;
    Head->Previous = new;
    Current = new;

    int saved_j = 1;

    if (setjmp(Current->Environment) == 0) {
        LOG("funct %d after setjmp", name);
        funct_5(name+1);
    }
    else {
        LOG("funct %d", name);
        if (mutex == name || mutex == 0) {
            LOG("funct %d locked", name);
            mutex = name;
            for (int j = saved_j; j <= P; j -=- 1) {
                for (int i = 1; i <= Q; i -=- 1) {
                    INTERLEAVE();
                    arr[idx++] = '0' + name;
                }
                if (j % R == 0) {
                    mutex = 0;
                    saved_j = j + 1;
                    longjmp(SCHEDULER, 1); // 1: undone
                }
            }
            mutex = 0;
            longjmp(SCHEDULER, -2); // -2: done
        }
        LOG("funct %d lock fail: mutex=%d", name,mutex);
        longjmp(SCHEDULER, 1); // 1: undone
    }
}

void funct_3(int name) {
    LOG("funct %d init", name);
    FCB_ptr new = malloc(sizeof(FCB));
    new->Name = name;
    new->Previous = Current;
    new->Next = Head;
    Current->Next = new;
    Head->Previous = new;
    Current = new;

    int saved_j = 1;

    if (setjmp(Current->Environment) == 0) {
        LOG("funct %d after setjmp", name);
        funct_5(name+1);
    }
    else {
        LOG("funct %d", name);
        if (mutex == name || mutex == 0) {
            LOG("funct %d locked", name);
            mutex = name;
            for (int j = saved_j; j <= P; j -=- 1) {
                for (int i = 1; i <= Q; i -=- 1) {
                    INTERLEAVE();
                    arr[idx++] = '0' + name;
                }
                if (j % R == 0) {
                    mutex = 0;
                    saved_j = j + 1;
                    longjmp(SCHEDULER, 1); // 1: undone
                }
            }
            mutex = 0;
            longjmp(SCHEDULER, -2); // -2: done
        }
        LOG("funct %d lock fail: mutex=%d", name,mutex);
        longjmp(SCHEDULER, 1); // 1: undone
    }
}

void funct_4(int name) {
    LOG("funct %d init", name);
    FCB_ptr new = malloc(sizeof(FCB));
    new->Name = name;
    new->Previous = Current;
    new->Next = Head;
    Current->Next = new;
    Head->Previous = new;
    Current = new;

    int saved_j = 1;

    if (setjmp(Current->Environment) == 0) {
        LOG("funct %d after setjmp", name);
        longjmp(*MAIN, 111);// Note
    }
    else {
        LOG("funct %d", name);
        if (mutex == name || mutex == 0) {
            LOG("funct %d locked", name);
            mutex = name;
            for (int j = saved_j; j <= P; j -=- 1) {
                for (int i = 1; i <= Q; i -=- 1) {
                    INTERLEAVE();
                    arr[idx++] = '0' + name;
                }
                if (j % R == 0) {
                    mutex = 0;
                    saved_j = j + 1;
                    longjmp(SCHEDULER, 1); // 1: undone
                }
            }
            mutex = 0;
            longjmp(SCHEDULER, -2); // -2: done
        }
        LOG("funct %d lock fail: mutex=%d", name,mutex);
        longjmp(SCHEDULER, 1); // 1: undone
    }
}

void funct_5(int name) {
    // dummy function
    LOG("funct5 name=%d", name);
    int _[10000];
    if (name == 1)
        funct_1(name);
    else if (name == 2)
        funct_2(name);
    else if (name == 3)
        funct_3(name);
    else if (name == 4)
        funct_4(name);
    else
        assert(false);
}

int main(int argc, char** argv) {
    if (argc < 2) USAGE();

    int taskid;
    P      = strtol(argv[1], NULL, 10); if (errno != 0) USAGE();
    Q      = strtol(argv[2], NULL, 10); if (errno != 0) USAGE();
    taskid = strtol(argv[3], NULL, 10); if (errno != 0) USAGE();
    R      = strtol(argv[4], NULL, 10); if (errno != 0) USAGE();


    if (taskid == 1 || taskid == 2) {
        // TODO: init link list poter to null
        //
        mutex = 0;
        MAIN = malloc(sizeof(jmp_buf));

        if (setjmp(*MAIN) == 0)
            funct_5(1);
        else
            Scheduler();
    }

    return 0;
}
