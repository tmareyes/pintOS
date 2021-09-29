#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include <stdbool.h>

void syscall_init (void);
void sys_exit(int exit_sys);
bool sys_create (const char *file, unsigned initial_size);
bool sys_remove(const char *file);
int sys_filesize(int fd);
void sys_exit(int status);
int sys_read (int fd, void *buffer, unsigned size);
int sys_open (const char *file);
void sys_close(int fd);
int sys_exec (const char *cmd);
void sys_seek (int fd, unsigned position);
unsigned sys_tell(int fd);
#endif /* userprog/syscall.h */
