/******************************************************************************/
/* Important Fall 2020 CSCI 402 usage information:                            */
/*                                                                            */
/* This fils is part of CSCI 402 kernel programming assignments at USC.       */
/*         53616c7465645f5fd1e93dbf35cbffa3aef28f8c01d8cf2ffc51ef62b26a       */
/*         f9bda5a68e5ed8c972b17bab0f42e24b19daa7bd408305b1f7bd6c7208c1       */
/*         0e36230e913039b3046dd5fd0ba706a624d33dbaa4d6aab02c82fe09f561       */
/*         01b0fd977b0051f0b0ce0c69f7db857b1b5e007be2db6d42894bf93de848       */
/*         806d9152bd5715e9                                                   */
/* Please understand that you are NOT permitted to distribute or publically   */
/*         display a copy of this file (or ANY PART of it) for any reason.    */
/* If anyone (including your prospective employer) asks you to post the code, */
/*         you must inform them that you do NOT have permissions to do so.    */
/* You are also NOT permitted to remove or alter this comment block.          */
/* If this comment block is removed or altered in a submitted file, 20 points */
/*         will be deducted.                                                  */
/******************************************************************************/

#include "types.h"
#include "globals.h"
#include "errno.h"

#include "util/debug.h"
#include "util/string.h"

#include "proc/proc.h"
#include "proc/kthread.h"

#include "mm/mm.h"
#include "mm/mman.h"
#include "mm/page.h"
#include "mm/pframe.h"
#include "mm/mmobj.h"
#include "mm/pagetable.h"
#include "mm/tlb.h"

#include "fs/file.h"
#include "fs/vnode.h"

#include "vm/shadow.h"
#include "vm/vmmap.h"

#include "api/exec.h"

#include "main/interrupt.h"

/* Pushes the appropriate things onto the kernel stack of a newly forked thread
 * so that it can begin execution in userland_entry.
 * regs: registers the new thread should have on execution
 * kstack: location of the new thread's kernel stack
 * Returns the new stack pointer on success. */
static uint32_t
fork_setup_stack(const regs_t *regs, void *kstack)
{
        /* Pointer argument and dummy return address, and userland dummy return
         * address */
        uint32_t esp = ((uint32_t) kstack) + DEFAULT_STACK_SIZE - (sizeof(regs_t) + 12);
        *(void **)(esp + 4) = (void *)(esp + 8); /* Set the argument to point to location of struct on stack */
        memcpy((void *)(esp + 8), regs, sizeof(regs_t)); /* Copy over struct */
        return esp;
}


/*
 * The implementation of fork(2). Once this works,
 * you're practically home free. This is what the
 * entirety of Weenix has been leading up to.
 * Go forth and conquer.
 */
int
do_fork(struct regs *regs)
{
        vmarea_t *vma, *clone_vma;
        pframe_t *pf;
        mmobj_t *to_delete, *new_shadowed;

        // NOT_YET_IMPLEMENTED("VM: do_fork");

        KASSERT(regs != NULL); /* the function argument must be non-NULL */
        KASSERT(curproc != NULL); /* the parent process, which is curproc, must be non-NULL */
        KASSERT(curproc->p_state == PROC_RUNNING); /* the parent process must be in the running state and not in the zombie state */
        dbg(DBG_PRINT, "(GRADING3A 7a)\n");
        dbg(DBG_TEMP, "\n");
        // Allocate a proc_t out of the procs structure using proc_create().
        proc_t * clone_proc = proc_create("clone");
        if(clone_proc == NULL){
            dbg(DBG_PRINT, "(GRADING3A)\n");
            panic("failed to created clone process in fork\n");
        }
        // Copy the vmmap_t from the parent process into the child using vmmap_clone(). Remember to increase the reference counts on the underlying mmobj_ts.
        vmmap_t * clone_map = vmmap_clone(curproc->p_vmmap);
        clone_map->vmm_proc = clone_proc;
        //Remember to increase the reference counts on the underlying mmobj_ts.


        vmarea_t * parent_vma = NULL;

        // For each private mapping, point the vmarea_t at the new shadow object, which in turn should point to the original mmobj_t for the vmarea_t. This is how you know that the pages corresponding to this mapping are copy-on-write. Be careful with reference counts. Also note that for shared mappings, there is no need to copy the mmobj_t.
        list_iterate_begin(&clone_map->vmm_list, clone_vma, vmarea_t, vma_plink) {
                parent_vma = vmmap_lookup(curproc->p_vmmap, clone_vma->vma_start);
                
                if(clone_vma->vma_flags & MAP_PRIVATE) {
                        mmobj_t * sha_parent_mmobj = shadow_create();
                        mmobj_t * sha_clone_mmobj = shadow_create();
                        
                        sha_parent_mmobj->mmo_shadowed = parent_vma->vma_obj;
                        sha_clone_mmobj->mmo_shadowed = clone_vma->vma_obj;
                        
                        parent_vma->vma_obj = sha_parent_mmobj;
                        clone_vma->vma_obj = sha_clone_mmobj;
                        
                        sha_parent_mmobj->mmo_un.mmo_bottom_obj = mmobj_bottom_obj(sha_parent_mmobj->mmo_shadowed);
                        sha_clone_mmobj->mmo_un.mmo_bottom_obj = mmobj_bottom_obj(sha_clone_mmobj->mmo_shadowed);
                        
                        //list_insert_tail(&sha_parent_mmobj->mmo_un.mmo_bottom_obj->mmo_un.mmo_vmas, &parent_vma->vma_olink);
                        //list_insert_tail(&sha_clone_mmobj->mmo_un.mmo_bottom_obj->mmo_un.mmo_vmas, &clone_vma->vma_olink);
                        
                        sha_parent_mmobj->mmo_shadowed->mmo_ops->ref(sha_parent_mmobj->mmo_shadowed);
                        // mmobj_bottom_obj(sha_parent_mmobj)->mmo_ops->ref(mmobj_bottom_obj(sha_parent_mmobj));
                        //mmobj_bottom_obj(sha_clone_mmobj)->mmo_ops->ref(mmobj_bottom_obj(sha_clone_mmobj));
                        dbg(DBG_PRINT, "(GRADING3A)\n");
                } else if(clone_vma->vma_flags & MAP_SHARED) {
                        //clone_vma->vma_obj = parent_vma->vma_obj;
                        //clone's obj isn't a shadow is it? we don't need to add to vmas list
                        //list_insert_head(&clone_vma->vma_obj->mmo_un.mmo_vmas, &clone_vma->vma_olink);
                        
                        parent_vma->vma_obj->mmo_ops->ref(parent_vma->vma_obj);
                        dbg(DBG_PRINT, "(GRADING3D 2)\n");
                }
                dbg(DBG_PRINT, "(GRADING3A)\n");
        } list_iterate_end();

        // Unmap the user land page table entries and flush the TLB (using pt_unmap_range() and tlb_flush_all()). This is necessary because the parent process might still have some entries marked as "writable", but since we are implementing copy-on-write we would like access to these pages to cause a trap.
        pt_unmap_range(curproc->p_pagedir, USER_MEM_LOW, USER_MEM_HIGH);
        tlb_flush_all();

        // Use kthread_clone() to copy the thread from the parent process into the child process.
        kthread_t * clone_thr = kthread_clone(curthr);
        clone_thr->kt_proc = clone_proc;

        // Set the child's working directory to point to the parent's working directory (once again, remember reference counts).
        clone_proc->p_cwd = curproc->p_cwd;
        vref(clone_proc->p_cwd);

         // Copy the file descriptor table of the parent into the child. Remember to use fref() here.
        for(int i = 0; i < NFILES; i++){
                if(curproc->p_files[i]){
                        clone_proc->p_files[i] = curproc->p_files[i];
                        fref(clone_proc->p_files[i]);
                        dbg(DBG_PRINT, "(GRADING3A)\n");
                }
                dbg(DBG_PRINT, "(GRADING3A)\n");
        }

        // Make the new thread runnable.
        sched_make_runnable(clone_thr);
        
        // Set any other fields in the new process which need to be set.
        vmmap_destroy(clone_proc->p_vmmap);
        clone_proc->p_vmmap = clone_map;
        list_insert_head(&clone_proc->p_threads, &clone_thr->kt_plink);
        clone_proc->p_brk = curproc->p_brk;
        clone_proc->p_start_brk = curproc->p_start_brk;
        clone_proc->p_status = curproc->p_status;
        // Set up the new process thread context (kt_ctx). You will need to set the following:
        //         c_pdptr - the page table pointer
        //         c_eip - function pointer for the userland_entry() function
        //         c_esp - the value returned by fork_setup_stack()
        //         c_kstack - the top of the new thread's kernel stack
        //         c_kstacksz - size of the new thread's kernel stack
        // Remember to set the return value in the child process!
        regs->r_eax = 0;
        
        clone_thr->kt_ctx.c_pdptr = clone_proc->p_pagedir;
        clone_thr->kt_ctx.c_eip = (uint32_t)userland_entry;
        //clone_thr->kt_ctx.c_ebp = clone_thr->kt_ctx.c_esp;
        clone_thr->kt_ctx.c_kstack = (uintptr_t)clone_thr->kt_kstack;
        clone_thr->kt_ctx.c_kstacksz = DEFAULT_STACK_SIZE;
        clone_thr->kt_ctx.c_esp = fork_setup_stack(regs, clone_thr->kt_kstack);
        KASSERT(clone_proc->p_state == PROC_RUNNING); /* new child process starts in the running state */
        KASSERT(clone_proc->p_pagedir != NULL); /* new child process must have a valid page table */
        KASSERT(clone_thr->kt_kstack != NULL); /* thread in the new child process must have a valid kernel stack */
        dbg(DBG_PRINT, "(GRADING3A 7a)\n");
        

        //if parent
        dbg(DBG_PRINT, "(GRADING3A)\n");
        return clone_proc->p_pid;
        
}


