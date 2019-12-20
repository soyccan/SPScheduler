#include "common.h"
#include "scheduler.h"

#ifdef DEBUG
# define INTERLEAVE()
#else
# define INTERLEAVE() sleep(1)
#endif

#define SIGUSR3 SIGWINCH
#define ACK 'A'

// parameter for longjmp to scheduler
#define S_DONE (-2) // task is done
#define S_CTX  (1)  // context switch
#define S_OCP  (2)  // lock failed, occupied
#define S_CONT (3)  // back to current function


jmp_buf MAIN;
// Note: declare as global variable doesn't work
// jmp_buf won't store any data after setjmp()
// reason unknown
// Update: the problem somehow fixed.

int P, Q, R;
int T; // which task

int mutex; // lock for multiplexing

// waiting queue, do not access directly
#define WAITQ_SIZE 10
FCB_ptr _waitQ[10];
int _waitQh, _waitQt; // head, tail

// shared with scheduler.o
int idx;
char arr[10000];
jmp_buf SCHEDULER;
FCB_ptr Current, Head;


void waitQ_push(FCB_ptr p) {
    LOG("push FCB %d",p->Name);
    _waitQ[_waitQt++] = p;
    if (_waitQt >= WAITQ_SIZE) _waitQt = 0;
}

FCB_ptr waitQ_pop() {
    FCB_ptr p = _waitQ[_waitQh++];
    LOG("pop FCB %d",p->Name);
    if (_waitQt >= WAITQ_SIZE) _waitQh = 0;
    return p;
}

int waitQ_count() {
    if (_waitQh <= _waitQt) return _waitQt - _waitQh;
    else return _waitQt - _waitQh + WAITQ_SIZE;
}

void funct_1(int name) {
    LOG("funct %d init", name);
    // Note
    Current = Head = malloc(sizeof(FCB));
    Current->Name = name;
    Current->Previous = Current->Next = Current;

    volatile int saved_j = 1;

    if (setjmp(Current->Environment) == 0) {
        LOG("funct %d after setjmp", name);
        funct_5(name+1);
    }
    else {
#include "funct_frag.h"
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

    volatile int saved_j = 1;

    if (setjmp(Current->Environment) == 0) {
        LOG("funct %d after setjmp", name);
        funct_5(name+1);
    }
    else {
#include "funct_frag.h"
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

    volatile int saved_j = 1;

    if (setjmp(Current->Environment) == 0) {
        LOG("funct %d after setjmp", name);
        funct_5(name+1);
    }
    else {
#include "funct_frag.h"
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

    volatile int saved_j = 1;

    if (setjmp(Current->Environment) == 0) {
        LOG("funct %d after setjmp", name);
        longjmp(MAIN, 111);// Note
    }
    else {
#include "funct_frag.h"
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

void handle_sigusr(int sig) {
    LOG("handle sigusr sig=%d", sig);

    // re-block signal
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, sig);
    sigprocmask(SIG_BLOCK, &set, NULL);

    // tell parent signal is delivered
    putchar(ACK);
    fflush(stdout);

    if (sig == SIGUSR3) {
        // print who's in queue
        LOG("SIGUSR3");
    }

    // jump back to current function
    longjmp(SCHEDULER, S_CONT);
}

void init_signal() {
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGUSR1);
    sigaddset(&sa.sa_mask, SIGUSR2);
    sigaddset(&sa.sa_mask, SIGUSR3);

    sa.sa_flags = 0;
    sa.sa_sigaction = NULL;
    sa.sa_handler = handle_sigusr;

    sigaction(SIGUSR1, &sa, NULL);;
    sigaction(SIGUSR2, &sa, NULL);;
    sigaction(SIGUSR3, &sa, NULL);;
}

int main(int argc, char** argv) {
    if (argc < 2) USAGE();

    P = strtol(argv[1], NULL, 10); if (errno != 0) USAGE();
    Q = strtol(argv[2], NULL, 10); if (errno != 0) USAGE();
    T = strtol(argv[3], NULL, 10); if (errno != 0) USAGE();
    R = strtol(argv[4], NULL, 10); if (errno != 0) USAGE();

    init_signal();

    // TODO: init link list poter to null

    mutex = 0;
//     MAIN = malloc(sizeof(jmp_buf));

    if (setjmp(MAIN) == 0)
        funct_5(1);
    else
        Scheduler();

    return 0;
}
