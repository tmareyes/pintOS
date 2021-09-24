#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
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
