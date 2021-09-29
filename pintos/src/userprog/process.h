#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

typedef int pid_t;
#define PID_ERROR (pid_t -1) 

struct process{
    struct process *parent;         
    struct file *exec_file;         
    struct list child_list;          
    struct list file_list;          
    struct process_info *info;     
    int fd_next;
    pid_t pid;                     
};
struct process_info{
    struct process *process;        
    int status;                      
    int exit_code;                  
    bool wait;                 
    struct list_elem elem;  
    pid_t pid;        
};
struct process_file{
    int fd;        
    struct file *file;              
    struct list_elem elem;          
};

tid_t process_execute (const char *file_name);

int process_wait (tid_t);
struct process *process_current (void);
struct file * get_file (int fd);
void process_exit (void);
void process_activate (void);

int process_write(int fd, const void *buffer, unsigned size);

#endif /* userprog/process.h */
