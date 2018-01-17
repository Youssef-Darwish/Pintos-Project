#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "devices/shutdown.h"
#include "string.h"
#include "process.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f )
{
  printf ("system call!\n");
    int * esp = f->esp;
    switch (* esp){
        case SYS_HALT :
            halt(f);
            break;
//        case SYS_FILESIZE:
//            file_size(f);
//            break;
//        case SYS_CLOSE:
//            close();
//            break;
//        case SYS_CREATE:
//            create();
//            break;
        case SYS_WAIT:
            wait(0);
//            break;
//        case SYS_EXEC:
//            exec();
//            break;
//        case SYS_REMOVE:
//            remove();
//            break;
//        case SYS_OPEN:
//            open();
//            break;
//        case SYS_WRITE:
//            write();
//            break;
//        case SYS_SEEK:
//            seek();
//            break;
//        case SYS_READ:
//            read();
//            break;
//        case SYS_TELL:
//            tell();
//            break;
//        case SYS_EXIT:
//            exit(f);
//            break;
        default:
            //print error message unsupported syscall
            break;
    }

  thread_exit ();
}

void* check_addr(int * user_add) {
   if(!is_user_vaddr((void*)user_add)) {
       //exit()
       return NULL;
   }
    int* data;

    memcpy(data,user_add,4);
    return user_add;
}

void halt(struct intr_frame *f) {
    shutdown_power_off();
}


void exit ( struct intr_frame *f) {
    int *esp = f->esp + 4;
    int *arg = check_addr(esp);
    if(arg ==NULL) {
        //invalid memory access
    }
    int exit_status = *arg;
    struct thread *curr = thread_current();
    int i=0;

    struct lock * element_lock ;//= list_entry(&curr->waiting_elem,struct lock,elem);
    struct list_elem *it = list_begin(&curr->donors);
    if (it != list_end(&curr->donors)) {
        struct list_elem *e;

        for (e = list_next(it); e != list_end(&curr->donors); e = list_next(e)) {
            element_lock = list_entry(e, struct lock, elem);
            lock_release(&element_lock);

        }

    }
    tid_t parent_id = curr->parent_id;

    struct thread *parent;
    if((parent=get_thread(parent_id)) != NULL  ){
        //consider sync
        bool found = false;
        struct child * me=NULL;
        struct  list_elem *e=NULL;
        for (e = list_begin(&parent->children); e != list_end(&parent->children) && (!found); e = list_next(e)) {
            me = list_entry(e, struct child, elem);
            found|=(me->tid == curr->tid);

        }
        if(me !=NULL && me->tid==curr->tid) {
            me->exit_status = exit_status;
            me->state = 0;
            if(me->halting)
            {
                me->halting = false;
                thread_unblock(parent);
            }
        }
    }

    f->eax = exit_status;
    process_exit();
    thread_exit();
}

void wait(struct intr_frame *f) {
    int *esp = f->esp + 4;
    int *arg = check_addr(esp);
    if(arg ==NULL) {
        //invalid memory access
    }
    int child_pid = *arg;

//    if(!is_child) {
//        //exit w error
//    }

    f->eax =(uint32_t )process_wait(child_pid);

}