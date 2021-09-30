#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

/* Moved process definitions to thread.h to avoid conflicts */

tid_t process_execute (const char *file_name);

int process_wait (tid_t);
struct process *process_current (void);
struct file * get_file (int fd);
void process_exit (void);
void process_activate (void);

int process_write(int fd, const void *buffer, unsigned size);

#endif /* userprog/process.h */
