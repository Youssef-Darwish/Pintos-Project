#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "devices/shutdown.h"
#include "string.h"
#include "process.h"
#include "devices/input.h"
#include "filesys/file.h"
#include "threads/malloc.h"
#include "filesys/filesys.h"

static void syscall_handler(struct intr_frame *);

void
syscall_init(void) {
    intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
}

int *check_addr(int *);

static void
syscall_handler(struct intr_frame *f) {
//    printf("system call!\n");
    int *esp = f->esp;
    esp = check_addr(esp);
    if (esp == NULL) {
        f->eax = -1;
        return;

    }
    switch (*esp) {
        case SYS_HALT :
            halt(f);
            break;
        case SYS_FILESIZE:
            file_size(f);
            break;
        case SYS_CLOSE:
            close(f);
            break;
        case SYS_CREATE:
            create(f);
            break;
        case SYS_WAIT:
            wait(f);
            break;
        case SYS_EXEC:
             exec(f);
            break;
        case SYS_REMOVE:
            remove(f);
            break;
        case SYS_OPEN:
            open(f);
            break;
        case SYS_WRITE:
            write(f);
            break;
        case SYS_SEEK:
            seek(f);
            break;
        case SYS_READ:
            read(f);
            break;
        case SYS_TELL:
            tell(f);
            break;
        case SYS_EXIT:
            exit(f);
            break;
        default:
            //print error message unsupported syscall
            break;
    }

    //
    // thread_exit();
}

void exec(struct intr_frame *f) {
    int *esp = f->esp;
    esp=check_addr(esp+1);
    int *arg = *esp;
    arg = check_addr(arg);
    int pid = process_execute(arg);
    f->eax = pid;

}
/* Reads a byte at user virtual address UADDR.
   UADDR must be below PHYS_BASE.
   Returns the byte value if successful, -1 if a segfault
   occurred. */
static int
get_user(const uint8_t *uaddr) {
    int result;
    asm ("movl $1f, %0; movzbl %1, %0; 1:"
    : "=&a" (result) : "m" (*uaddr));
    return result;
}

/* Writes BYTE to user address UDST.
   UDST must be below PHYS_BASE.
   Returns true if successful, false if a segfault occurred. */
static bool
put_user(uint8_t *udst, uint8_t byte) {
    int error_code;
    asm ("movl $1f, %0; movb %b2, %1; 1:"
    : "=&a" (error_code), "=m" (*udst) : "q" (byte));
    return error_code != -1;
}

int *check_addr(int *user_add) {
    if ((user_add) >= PHYS_BASE) {
        process_exit_with_status(-1);
        thread_exit();
        return NULL;
    }
    //int dummy = *user_add;
    int dummy = get_user(user_add);
    if (dummy == -1) {
        process_exit_with_status(-1);
        thread_exit();
    }//return NULL;
    return user_add;
}

void halt(struct intr_frame *f) {
    shutdown_power_off();
}


void exit(struct intr_frame *f) {
    int *arg = f->esp;
    arg = check_addr(arg + 1);
    int exit_status = *arg;

    f->eax = exit_status;
    process_exit_with_status(exit_status);
    thread_exit();
}

void seek(struct intr_frame *f) {
    int *esp = f->esp;

    esp = check_addr(esp + 1);
    int fd = *esp;
    esp = check_addr(esp + 1);
    unsigned pos = *esp;
    struct thread *curr = thread_current();
    struct list_elem *e;
    struct file_resource *me;
    bool found = false;
    for (e = list_begin(&curr->files); e != list_end(&curr->files) && (!found); e = list_next(e)) {
        me = list_entry(e, struct file_resource, elem);
        found |= (me->fd == fd);
    }
    if (!found) {
        return;
    }
    file_seek(me->res, pos);
    me->index = pos;
    return;

}
void file_size(struct intr_frame *f) {
    int *esp = f->esp;
    esp = check_addr(esp + 1);
    int fd = *esp;
    struct thread *curr = thread_current();
    struct list_elem *e;
    struct file_resource *me;
    bool found = false;
    for (e = list_begin(&curr->files); e != list_end(&curr->files) && (!found); e = list_next(e)) {
        me = list_entry(e, struct file_resource, elem);
        found |= (me->fd == fd);
    }
    if (!found) {
        f->eax=-1;
        return;
    }
    f->eax=file_length(me->res);
    return;
}
void remove(struct intr_frame *f) {

}

void wait(struct intr_frame *f) {
    int *arg = f->esp;
    arg++;
    int child_pid = *arg;
    //
    // printf("waiting for %d\n",child_pid);
//    if(!is_child) {
//        //exit w error
//    }

    f->eax = (uint32_t) process_wait(child_pid);

}

void open(struct intr_frame *f) {
    int *esp = f->esp;
    esp = check_addr(esp + 1);
    int *arg0 = esp;
    arg0 = check_addr(*arg0);
    if (arg0 == NULL) {
        return;
    }
    struct file *fil = filesys_open((char *) arg0);
    if (fil == NULL) {

        f->eax = -1;
        return;
    }
    struct file_resource *nres;
    nres = malloc(sizeof(struct file_resource));
    nres->res = fil;
    nres->fd = thread_current()->fir_fid;
    nres->index = 0;
    list_push_back(&thread_current()->files, &nres->elem);
    thread_current()->fir_fid++;
    f->eax = nres->fd;
    return;

}

void tell(struct intr_frame *f) {
    int *esp = f->esp;
    esp = check_addr(esp + 1);
    int fd = *esp;
    struct thread *curr = thread_current();
    struct list_elem *e;
    struct file_resource *me;
    bool found = false;
    for (e = list_begin(&curr->files); e != list_end(&curr->files) && (!found); e = list_next(e)) {
        me = list_entry(e, struct file_resource, elem);
        found |= (me->fd == fd);
    }

    f->eax = (found) ? me->index : -1;
    return;


}

void close(struct intr_frame *f) {
    int *esp = f->esp;
    esp = check_addr(esp + 1);
    int fd = *esp;
    struct thread *curr = thread_current();
    struct list_elem *e;
    struct file_resource *me;
    bool found = false;
    for (e = list_begin(&curr->files); e != list_end(&curr->files) && (!found); e = list_next(e)) {
        me = list_entry(e, struct file_resource, elem);
        found |= (me->fd == fd);
    }
    if (!found)
        return;
    file_close(me->res);
    list_remove(&me->elem);


}

void read(struct intr_frame *f) {
    int *esp = f->esp;
    esp = check_addr(esp + 1);
    int fd = *esp;
    esp = check_addr(esp + 1);
    int *arg1 = esp;
    arg1 = check_addr(*arg1);
    if (arg1 == NULL) {
        f->eax = -1;
        return;
    }
    char *buff = arg1;
    esp = check_addr(esp + 1);
    int size = *esp;
    if (size == 0) {
        f->eax = 0;
        return;
    }
    if (fd == 0) {
        char *tbuff = (char *) buff;
        *tbuff = input_getc();
        *(tbuff + 1) = 0;
        f->eax = 1;
        return;
    } else if (fd > 1) {
        struct list_elem *e = NULL;
        struct file_resource *me;
        bool found = false;
        struct thread *curr = thread_current();
        for (e = list_begin(&curr->files); e != list_end(&curr->files) && (!found); e = list_next(e)) {
            me = list_entry(e, struct file_resource, elem);
            found |= (me->fd == fd);
        }
        if (!found) {
            f->eax = -1;
            return;
        }
        int b = file_read_at(me->res, buff, size, me->index);
        me->index += b;
        f->eax = b;
        return;
    }
}

void create(struct intr_frame *f) {
    int *esp = f->esp;
    esp = check_addr(esp + 1);
    int *arg0 = esp;
    esp = check_addr(esp + 1);
    int arg1 = *esp;
    arg0 = check_addr(*arg0);
    if (arg0 == NULL) {
        f->eax = -1;
        return;
    }
    //printf("file name is %s",arg0);
    bool succ = filesys_create((char *) arg0, arg1);
    if (!succ) {
        f->eax = 0;
        return;
    }
    f->eax = 1;
    return;

}

void write(struct intr_frame *f) {
    int *esp = f->esp;
    esp = check_addr(esp + 1);
    int fd = *esp;
    esp = check_addr(esp + 1);
    int *arg1 = esp;

    arg1 = check_addr(*arg1);
    esp = check_addr(esp + 1);
    unsigned size = *esp;
    int *end = check_addr(arg1 + size);
    if (arg1 == NULL) {
        f->eax = -1;
        return;
    }

    char *buff = (char *) arg1;
    if (fd == 1) {

        putbuf(buff, size);
        f->eax = size;

        return;
    } else if (fd > 1) {
        struct list_elem *e = NULL;
        struct file_resource *me;
        bool found = false;
        struct thread *curr = thread_current();
        for (e = list_begin(&curr->files); e != list_end(&curr->files) && (!found); e = list_next(e)) {
            me = list_entry(e, struct file_resource, elem);
            found |= (me->fd == fd);

        }
        if (!found) {
            f->eax = -1;
            return;
        }
        int b = file_write_at(me->res, buff, size, me->index);
        me->index += b;
        f->eax = b;

        return;
    }

}