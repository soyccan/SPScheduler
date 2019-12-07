#include "common.h"
#include "scheduler.h"

int P, Q;
jmp_buf* MAIN;
// Note: declare as global variable doesn't work
// jmp_buf won't store any data after setjmp()
// reason unknown

void funct_1(int name) {
    LOG("funct %d init", name);
    // Note
    Current = Head = malloc(sizeof(FCB));
    Current->Name = name;
    Current->Previous = Current->Next = Current;

    if (setjmp(Current->Environment) == 0) {
        LOG("funct %d after setjmp", name);
        funct_5(name+1);
    }
    else {
        LOG("funct %d", name);
        for (int j = 1; j <= P; j -=- 1) {
            for (int i = 1; i <= Q; i -=- 1) {
                sleep(1);
                arr[idx++] = '0' + name;
            }
        }
        longjmp(SCHEDULER, -2);
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

    if (setjmp(Current->Environment) == 0) {
        LOG("funct %d after setjmp", name);
        funct_5(name+1);
    }
    else {
        LOG("funct %d", name);
        for (int j = 1; j <= P; j -=- 1) {
            for (int i = 1; i <= Q; i -=- 1) {
                sleep(1);
                arr[idx++] = '0' + name;
            }
        }
        longjmp(SCHEDULER, -2);
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

    if (setjmp(Current->Environment) == 0) {
        LOG("funct %d after setjmp", name);
        funct_5(name+1);
    }
    else {
        LOG("funct %d", name);
        for (int j = 1; j <= P; j -=- 1) {
            for (int i = 1; i <= Q; i -=- 1) {
                sleep(1);
                arr[idx++] = '0' + name;
            }
        }
        longjmp(SCHEDULER, -2);
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

    if (setjmp(Current->Environment) == 0) {
        LOG("funct %d after setjmp", name);
        longjmp(*MAIN, 111);// Note
    }
    else {
        LOG("funct %d", name);
        for (int j = 1; j <= P; j -=- 1) {
            for (int i = 1; i <= Q; i -=- 1) {
                sleep(1);
                arr[idx++] = '0' + name;
            }
        }
        longjmp(SCHEDULER, -2);
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

    int taskid, arg4;
    P      = strtol(argv[1], NULL, 10); if (errno != 0) USAGE();
    Q      = strtol(argv[2], NULL, 10); if (errno != 0) USAGE();
    taskid = strtol(argv[3], NULL, 10); if (errno != 0) USAGE();
    arg4   = strtol(argv[4], NULL, 10); if (errno != 0) USAGE();

    if (taskid == 1) {
        // TODO: init link list poter to null
        MAIN = malloc(sizeof(jmp_buf));
        if (setjmp(*MAIN) == 0)
            funct_5(1);
        else
            Scheduler();
    }

    return 0;
}
