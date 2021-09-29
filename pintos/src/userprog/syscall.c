#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
<<<<<<< Updated upstream
=======
#include "devices/shutdown.h" 
#include "threads/vaddr.h"
#include "threads/synch.h"
#include "userprog/process.h"
#include "userprog/pagedir.h"
#include "threads/vaddr.h"
#include "unistd.h"

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
>>>>>>> Stashed changes

static void syscall_handler (struct intr_frame *);

struct file_descriptor {
  int fd;
  tid_t owner;
  struct file *file_struct;
  struct list_elem elem;
};

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
<<<<<<< Updated upstream
syscall_handler (struct intr_frame *f UNUSED) 
{
  printf ("system call!\n");
  thread_exit ();
}
=======
syscall_handler (struct intr_frame *f) //UNUSED) 
{
  printf ("system call!\n");
  int sys_call = (*(int *)f->esp);

  if(!is_Valid(sys_call)){
    sys_exit(-1);
  }

  switch(sys_call)
  {
    case SYS_HALT:{
      shutdown_power_off();
      break;
    }
    case SYS_WRITE: {
      int fd = *(int *)f->esp + 5;
      void *buffer = *(char **)(f->esp + 6);
      unsigned size = *(unsigned *)(f->esp + 7);
      f->eax = sys_write(fd, buffer, size);
      break;
    }
    // case SYS_EXIT:
    // {
    //   int num = *((int*)f->esp + 1);
    //   // read memory
    //   sys_exit(num);
    //   break;
    // }
    // case SYS_CREATE:{
    //   char * file = *((const char*)f->esp + 1);
    //   unsigned size = *((unsigned*)f->esp + 2);
    //   f->eax=sys_create (file, size);
    //   lock_release(&mem_lock);
    //   break;
    // }
    // case SYS_REMOVE:{
    //   char * file = *((const char*)f->esp + 1);
    //   f->eax= sys_remove(file);
    //   lock_release(&mem_lock);
    //   break;
    // }
    // case SYS_FILESIZE:
    // {
    //   int fd= *((int*)f->esp + 1);
    //   f->eax = sys_filesize(fd);
    //   break;
    // }
    // case SYS_WRITE:{
    
    //   int fd = *((int*)f->esp + 1);
    //   void* buffer = (void*)(*((int*)f->esp + 2));
    //   unsigned size = *((unsigned*)f->esp + 3);
    //   //run the syscall, a function of your own making
    //   //since this syscall returns a value, the return value should be stored in f->eax
    //   f->eax = sys_write(fd, buffer, size);
    //   break;
    // }
    default: {
      sys_exit(-1);
    }
  }
  thread_exit ();
}

void sys_exit(int exit_sys){

  printf ("%s: exit(%d)\n", thread_current()->name, exit_sys);
  thread_exit();
}

bool sys_create(const char *file, unsigned initial_size){
  lock_acquire(&mem_lock);
  return filesys_create((const char*)file,initial_size);
}

bool sys_remove(const char *file){
  lock_acquire(&mem_lock);
  return filesys_remove(file);
}

int sys_write(int fd, const void *buffer, unsigned size) {
  printf("I AM IN WRITE");
  struct file_descriptor *fd_struct;
  int bytes_written = 0;
  lock_acquire(&mem_lock);

  if (fd = STDIN_FILENO) {
    return -1;
  }
  if (fd = STDOUT_FILENO) {
    putbuf(buffer, size);
    return size;
  }

  fd_struct = get_file_from_list(fd);
  bytes_written = file_write(fd_struct->file_struct, buffer, size);

  lock_release(&mem_lock);
  return bytes_written;
}

struct file_descriptor *get_file_from_list(int fd) {
  
}

// int sys_filesize(int fd){

// }
>>>>>>> Stashed changes
