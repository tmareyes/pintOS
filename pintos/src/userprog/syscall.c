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
#include "devices/input.h"
#include "lib/kernel/list.h"
#include "lib/user/syscall.h"
#include "threads/malloc.h"
#include <string.h>
#include "filesys/file.h"
#include "filesys/filesys.h"

#define STDIN_FILENO 0
#define STDOUT_FILENO 1

static void syscall_handler (struct intr_frame *);
struct file_descriptor *get_file_from_list(int fd);
bool is_Valid(void *udst);
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
  // lock_init(&mem_lock);
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
  // uint32_t *esp;
  // esp= f->esp;

  // if(!is_Valid(esp)){
  //   sys_exit(-1);
  // }
  // int *p = f->esp;
  // int system_call = *p;
  // switch(system_call)
  printf("how many times");
  switch(*(int*)f->esp)
  {
    case SYS_HALT:{
      shutdown_power_off();
      break;
    }
    case SYS_EXIT:
    {
      int num = *((int*)(f->esp + 1));
      sys_exit(num);
      // if (!is_Valid((void *)(esp + 1))) {
      //   sys_exit(-1);
      // }
      // sys_exit((int) *(esp + 1));
      break;
    }
    case SYS_WRITE:{
      // printf("DID I REACH WRITE?\n");
      int fd = *((int*)f->esp + 5);
      printf("fd is: %d\n", fd);
      void* buffer = (void*)(*((int*)f->esp + 6));
      unsigned size = *((unsigned*)f->esp + 7);
      printf("size is: %d\n", size);
      // int fd = ((int*)(esp + 5));
      // void* buffer = (void*)(*(esp + 6));
      // unsigned size = ((unsigned*)(esp + 7));
      //run the syscall, a function of your own making
      //since this syscall returns a value, the return value should be stored in f->eax
      f->eax = (uint32_t) sys_write(fd, buffer, size);
      // sys_exit(-1);
      printf("HELLO\n");
      break;
    }
    default: {
      printf("SHOULD NOT REACH");
      break;
    }
  }
  thread_exit ();
}

void sys_exit(int status){
  printf("DID I REACH EXIT?\n");
  struct thread *cur = thread_current();
  printf ("%s: exit(%d)\n", cur->name, status);
  thread_exit();
}

int sys_write(int fd, const void *buffer, unsigned size) {
  printf("WRITE: fd = %d, size = %d\n", fd, size);
  struct file_descriptor *fd_struct;
  int bytes_written = 0;
  lock_acquire(&mem_lock);

  if (fd = STDIN_FILENO) {
    lock_release(&mem_lock);
    return -1;
  }
  if (fd = STDOUT_FILENO) {
    printf("STDOUT?\n");
    putbuf(buffer, size);
    lock_release(&mem_lock);
    return size;
  }
  // printf("AM I BREAKING?");
  fd_struct = get_file_from_list(fd);
  bytes_written = file_write(fd_struct->file_struct, buffer, size);

  // lock_release(&mem_lock);
  printf("bytes written: %d\n", bytes_written);
  return bytes_written;
}

// struct file_descriptor *get_file_from_list(int fd) {
//   struct list_elem *list_element;
//   struct file_descriptor *fd_struct;
//   printf("get_file");

//   for (list_element = list_head(&thread_current()->thread_files); 
//   list_element != list_tail(&thread_current()->thread_files);
//   list_element = list_next(list_element)) {
//     fd_struct = list_entry(list_element, struct file_descriptor, elem);
//     if (fd_struct->fd == fd) {
//       return fd_struct;
//     }
//   }
//   return NULL;
// }
