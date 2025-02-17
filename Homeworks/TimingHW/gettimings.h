#ifndef GETTIMINGS_H
#define GETTIMINGS_H

#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

long long nsecs(void); // returns number of nanoseconds
void set_overhead(void);
long long scenario1(void); // empty function call
long long scenario2(void); // getppid() call
long long scenario3(void); // running system("/bin/true")
long long scenario4(void); // sending signal to current process and having signal handler execute
long long scenario5(void); // sending signal to other process and have signal handler send back and identify return signal
void empty();
static void sig_handler4(int signum);
static void sig_handler5(int signum);

#endif