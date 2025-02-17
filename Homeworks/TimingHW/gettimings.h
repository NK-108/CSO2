#ifndef GETTIMINGS_H
#define GETTIMINGS_H

#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

long scenario1(void); // empty function call
long scenario2(void); // getppid() call
long scenario3(void); // running system("/bin/true")
long scenario4(void); // sending signal to current process and having signal handler execute
long scenario5(void); // sending signal to other process and have signal handler send back and identify return signal

#endif