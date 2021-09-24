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

static void syscall_handler (struct intr_frame *);
struct lock mem_lock;

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
      // read memory
      sys_exit(num);
      break;
    }
    case SYS_CREATE:{
      char * file = *((const char*)f->esp + 1);
      unsigned size = *((unsigned*)f->esp + 2);
      f->eax=sys_create (file, size);
      lock_release(&mem_lock);
      break;
    }
    case SYS_REMOVE:{
      char * file = *((const char*)f->esp + 1);
      f->eax= sys_remove(file);
      lock_release(&mem_lock);
      break;
    }
    case SYS_FILESIZE:
    {
      int fd= *((int*)f->esp + 1);
      f->eax = sys_filesize(fd);
      break;
    }
    case SYS_WRITE:{
    
      int fd = *((int*)f->esp + 1);
      void* buffer = (void*)(*((int*)f->esp + 2));
      unsigned size = *((unsigned*)f->esp + 3);
      //run the syscall, a function of your own making
      //since this syscall returns a value, the return value should be stored in f->eax
      f->eax = sys_write(fd, buffer, size);
      break;
    }
    default:
    
  printf ("system call!\n");
  uint32_t *esp;
  esp = f->esp;
  printf ("am I here yet?");
  switch (*esp) {
    case SYS_EXIT:
    {
      printf("here1?");
      sys_exit(-1);
      break;
    }
    default:
      {
        printf("SHOULD NOT REACH HERE");
        sys_exit(-1);
        break;
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

int sys_filesize(int fd){

}
