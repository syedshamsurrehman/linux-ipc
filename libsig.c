#include "sig.h"

int signal_register(int signum, void (*fun)(int), struct sigaction *oldact, 
                        sigset_t *sa_mask) 
{
    struct sigaction new_action;
    new_action.sa_handler = fun;
    new_action.sa_mask = *sa_mask;
    new_action.sa_flags = 0;
    if(sigaction(signum, &new_action, oldact) < 0) {
        return 1;
    }
    // TODO 1: Register the signal handler and save the current behaviour in oldact
    return 0;
}

int signal_deregister(int signum) 
{
    // TODO 2: Reset the behaviour to SIG_DFL
    struct sigaction new_action;
    new_action.sa_handler = SIG_DFL;
    new_action.sa_flags = 0;
    if(sigaction(signum, &new_action, NULL) < 0) {
        return 1;
    }
    return 0;
}

int signal_restore(int signum, struct sigaction *sa)
{
    if(sigaction(signum, sa, NULL) < 0) {
        return 1;
    }

    // TODO 3: Restore the behaviour as per sa

    return 0;
}

int signal_ignore(int signum) 
{
    struct sigaction new_action;
    new_action.sa_handler = SIG_IGN;
    new_action.sa_flags = 0;
    if(sigaction(signum, &new_action, NULL) < 0) {
        return 1;
    }
    // TODO 4: Ignore the signal
    return 0;
}

int signal_send(pid_t pid, int signum)
{
    // TODO 5: Send the signal to the process 
    return (kill (pid, signum));
}
