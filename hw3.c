#include "common.h"
#include "scheduler.h"

// status parameter for longjmp to scheduler
#define S_DONE (-2) // task is done
#define S_CTX  (1)  // context switch
#define S_OCP  (2)  // lock failed, occupied
#define S_CONT (3)  // back to current function


static jmp_buf MAIN;
// Note: declare as global variable doesn't work
// jmp_buf won't store any data after setjmp()
// reason unknown
// Update: the problem somehow fixed.
static int mutex; // lock for multiplexing

static int P, Q;
static int R;
// task 1: no use
// task 2: rounds before context switch
// task 3: pipe fd for ACK message
static int T; // which task

// waiting queue, do not access directly
#define WAITQ_SIZE 10
static bool _waitQ[WAITQ_SIZE];
static int _waitQ_sz;

// shared with scheduler.o
int idx;
char arr[10000];
jmp_buf SCHEDULER;
FCB_ptr Current, Head;


static void waitQ_push(FCB_ptr p) {
    LOG("push FCB %d",p->Name);
    ++_waitQ_sz;
    if (p->Name < WAITQ_SIZE)
        _waitQ[p->Name] = true;
}

static void waitQ_pop(FCB_ptr p) {
    LOG("pop FCB %d",p->Name);
    --_waitQ_sz;
    if (p->Name < WAITQ_SIZE)
        _waitQ[p->Name] = false;
}

static int waitQ_count() {
    return _waitQ_sz;
}

static void waitQ_show() {
    for (int i = 0; i < WAITQ_SIZE; i -=- 1)
        if (_waitQ[i])
            printf("%d ", i);
    printf("\n");
    fflush(stdout);
}

void funct_1(int name) {
    INFO("funct %d init", name);
    // Note: link list initialization
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
    INFO("funct %d init", name);
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
    INFO("funct %d init", name);
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
    INFO("funct %d init", name);
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
    INFO("funct5 name=%d", name);
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

static void handle_sigusr(int sig) {
    INFO("handle sigusr sig=%d", sig);

    // re-block signal
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, sig);
    sigprocmask(SIG_BLOCK, &set, NULL);

    if (sig == SIGUSR3) {
        // print who's in queue, don't do context switch
        LOG("hw3 -> main: waitQ");
        waitQ_show();

        // make current function continue after longjmp
        Current = Current->Previous;
    }

    // tell parent signal is delivered
    LOG("hw3 -> main: %s", ACK);
    write(R, ACK, sizeof(ACK));

    // jump back to scheduler to do a context switch
    longjmp(SCHEDULER, S_CTX);
}

static void init_signal() {
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGUSR1);
    sigaddset(&sa.sa_mask, SIGUSR2);
    sigaddset(&sa.sa_mask, SIGUSR3);

    // block as fast as possible
    sigprocmask(SIG_BLOCK, &sa.sa_mask, NULL);

    sa.sa_flags = 0;
    sa.sa_sigaction = NULL;
    sa.sa_handler = handle_sigusr;

    sigaction(SIGUSR1, &sa, NULL);;
    sigaction(SIGUSR2, &sa, NULL);;
    sigaction(SIGUSR3, &sa, NULL);;
}

int main(int argc, char** argv) {
    init_signal();

    if (argc < 5) USAGE();

    P = strtol(argv[1], NULL, 10); if (errno != 0) USAGE();
    Q = strtol(argv[2], NULL, 10); if (errno != 0) USAGE();
    T = strtol(argv[3], NULL, 10); if (errno != 0) USAGE();
    R = strtol(argv[4], NULL, 10); if (errno != 0) USAGE();

    mutex = 0;

    setbuf(stdout, NULL);
    setbuf(stdin, NULL);

    if (setjmp(MAIN) == 0)
        funct_5(1);
    else
        Scheduler();

    return 0;
}
