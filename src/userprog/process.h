#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

tid_t process_execute (const char *file_name);
int process_wait (tid_t);

int process_wait2 (tid_t);
void process_exit (void);
void process_exit_with_status(int status);
void process_activate (void);
bool is_ELF(struct file*);

#endif /* userprog/process.h */
