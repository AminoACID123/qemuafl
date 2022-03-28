#ifndef AFL_H
#define AFL_H

#include "../../debug.h"
#include <sys/shm.h>

#define CTRL_FD (FORKSRV_FD)
#define STUS_FD (FORKSRV_FD + 1)

#define FUZZ_DEV "USBFUZZ_"

#define TEST_START 0x52
#define TEST_CRASH 0x51
#define TEST_NORMAL_EXIT 0x50

extern void start_test(void* opaque);
extern void stop_test(int val);
extern void reply_forksrv_handshake(void);
extern bool in_afl;
extern unsigned char* shm_ptr;
extern char* inputFile;
extern pid_t __pid;

#endif