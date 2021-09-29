#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h" 
#include "threads/vaddr.h"
#include "threads/synch.h"
#include "userprog/process.h"
#include "userprog/pagedir.h"
#include "threads/vaddr.h"
#include "unistd.h"

#define STDIN_FILENO 0
#define STDOUT_FILENO 1

#include "devices/input.h"
#include "lib/kernel/list.h"
#include "lib/user/syscall.h"
#include "threads/malloc.h"
#include <string.h>
#include "filesys/file.h"
#include "filesys/filesys.h"

static void syscall_handler (struct intr_frame *);
struct lock mem_lock;

struct file_descriptor {
  int fd;
  tid_t owner;
  struct file *file_struct;
  struct list_elem elem;
};

void
syscall_init (void) 
{
  lock_init(&mem_lock);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

bool is_Valid(void *udst){
  if( is_user_vaddr(udst) && udst != NULL){
    if((pagedir_get_page(thread_current()->pagedir, udst)) != NULL){
      return true;
    }
    else return false;
  }
  return false;
}

/* Reads a byte at user virtual address UADDR.
   UADDR must be below PHYS_BASE.
   Returns the byte value if successful, -1 if a segfault
   occurred. */
static int get_mem(const uint8_t *uaddr){
  int result;
  asm ("movl $1f, %0; movzbl %1, %0; 1:"
       : "=&a" (result) : "m" (*uaddr));
  return result;
}
 
/* Writes BYTE to user address UDST.
   UDST must be below PHYS_BASE.
   Returns true if successful, false if a segfault occurred. */
static bool write_mem(uint8_t *udst, uint8_t byte){
  int error_code;
  if(is_Valid(udst)){
      asm ("movl $1f, %0; movb %b2, %1; 1:"
        : "=&a" (error_code), "=m" (*udst) : "q" (byte));
    return error_code != -1;
  }
  else sys_exit(-1); //need to write this function

}

static void
syscall_handler (struct intr_frame *f ) //UNUSED) 
{
  // printf ("system call!\n");
  uint32_t *esp;
  esp= f->esp;

  if(!is_Valid(esp)){
    exit(-1);
  }

  switch(*(int*)f->esp)
  {
    case SYS_HALT:{
      shutdown_power_off();
      break;
    }
    case SYS_EXIT:
    {
      int num = *((int*)f->esp + 1);
      if(!is_Valid(num)){
        exit(-1);
      }
      sys_exit(num);
      break;
    }
    case SYS_CREATE:{
      char * file = *((const char*)f->esp + 1);
      unsigned size = *((unsigned*)f->esp + 2);
      if(!is_Valid(file) || !is_Valid(size)){
        exit(-1);
      }
      f->eax=sys_create (file, size);
      lock_release(&mem_lock);
      break;
    }
    case SYS_REMOVE:{
      char * file = *((const char*)f->esp + 1);
      if(!is_Valid(file)){
        exit(-1);
      }
      f->eax= sys_remove(file);
      lock_release(&mem_lock);
      break;
    }
    case SYS_FILESIZE:
    {
      int fd= *((int*)f->esp + 1);
      if(!is_Valid(fd)){
        exit(-1);
      }
      f->eax = sys_filesize(fd);
      break;
    }
    case SYS_WRITE: {
      int fd = *(int *)f->esp + 5;
      void *buffer = *(char **)(f->esp + 6);
      unsigned size = *(unsigned *)(f->esp + 7);
      if(!is_Valid(fd) || !is_Valid(buffer) || !is_Valid(size)){
        exit(-1);
      }
      f->eax = sys_write(fd, buffer, size);
      break;
    }
    case SYS_SEEK:{
      int fd = *((int*)f->esp + 1);
      unsigned position = *((unsigned*)f->esp + 2);
      if(!is_Valid(fd) || !is_Valid(position)){
        exit(-1);
      }
      sys_seek (fd, position);
      break;
    }
    case SYS_TELL:{
      int fd = *((int*)f->esp + 1);
      if(!is_Valid(fd)){
        exit(-1);
      }
      f->eax = sys_tell (fd);
      break;
    }
    case SYS_CLOSE:{
      int fd = *((int*)f->esp + 1);
      if(!is_Valid(fd)){
        exit(-1);
      }
      sys_close(fd);
      break;
    }
    case SYS_EXEC:{
      char * cmd = *((const char*)f->esp + 1);
      if(!is_Valid(cmd)){
        exit(-1);
      }
      f->eax = sys_exec(cmd);
      break;
    }
    case SYS_OPEN:{
      char * file = *((const char*)f->esp + 1);
      if(!is_Valid(file)){
        exit(-1);
      }
      sys_open(file);
      break;
    }
    case SYS_READ:{
      int fd = *((int*)f->esp + 1);
      void* buffer = (void*)(*((int*)f->esp + 2));
      unsigned size = *((unsigned*)f->esp + 3);
      if(!is_Valid(fd) || !is_Valid(buffer) || !is_Valid(size)){
        exit(-1);
      }
      sys_read (fd, buffer, size);
      break;
    }
    default:
  }
  thread_exit ();
}

void sys_exit(int status){
  if (process_current ()->info != NULL){
    process_current()->info->exit_code = status;
  }
  printf ("%s: exit(%d)\n", thread_current()->name, status);
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

int sys_exec (const char *cmd){
  lock_acquire(&mem_lock);
  int status= process_execute((const char*)cmd);
  lock_release(&mem_lock);
  return status;
}

void sys_close(int fd){

}

int sys_open (const char *file){

}

unsigned sys_tell(int fd){
  lock_acquire (&mem_lock);
  struct file *file = get_file(fd);
  if (file == NULL){
      lock_release (&mem_lock); return -1;}
  unsigned next_byte = file_tell (file);
  lock_release (&mem_lock);
  return next_byte;
}

int sys_read (int fd, void *buffer, unsigned size){

}

int sys_filesize(int fd){
  lock_acquire (&mem_lock);
  struct file *file = get_file(fd);
  if (file == NULL){
      lock_release (&mem_lock);
      return -1;
  }
  int filesize = file_length(file);
  lock_release (&mem_lock);
  return filesize;
}

void sys_seek (int fd, unsigned position){
  lock_acquire (&mem_lock);
  struct file *seek_file = get_file(fd);
  if (seek_file == NULL){
      lock_release (&mem_lock);
      return;
  }
  file_seek (seek_file, position);
  lock_release (&mem_lock);
}
