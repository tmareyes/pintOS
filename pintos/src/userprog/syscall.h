#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include <stdbool.h>

void syscall_init (void);

void sys_exit(int exit_sys);
bool sys_create (const char *file, unsigned initial_size);
bool sys_remove(const char *file);
int sys_filesize(int fd);

void sys_exit(int);


#endif /* userprog/syscall.h */
