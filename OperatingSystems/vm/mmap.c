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

#include "globals.h"
#include "errno.h"
#include "types.h"

#include "mm/mm.h"
#include "mm/tlb.h"
#include "mm/mman.h"
#include "mm/page.h"

#include "proc/proc.h"

#include "util/string.h"
#include "util/debug.h"

#include "fs/vnode.h"
#include "fs/vfs.h"
#include "fs/file.h"

#include "vm/vmmap.h"
#include "vm/mmap.h"

/*
 * This function implements the mmap(2) syscall, but only
 * supports the MAP_SHARED, MAP_PRIVATE, MAP_FIXED, and
 * MAP_ANON flags.
 *
 * Add a mapping to the current process's address space.
 * You need to do some error checking; see the ERRORS section
 * of the manpage for the problems you should anticipate.
 * After error checking most of the work of this function is
 * done by vmmap_map(), but remember to clear the TLB.
 */
int
do_mmap(void *addr, size_t len, int prot, int flags,
        int fd, off_t off, void **ret)
{
    // NOT_YET_IMPLEMENTED("VM: do_mmap");
    // 1.flags checking
	if(!(flags & MAP_SHARED) && !(flags & MAP_PRIVATE) && !(flags & MAP_FIXED) && !(flags & MAP_ANON)){
		*ret = MAP_FAILED;
		dbg(DBG_PRINT, "(GRADING3A)\n");
		return (int)MAP_FAILED;
	}

	file_t *file = fget(fd);
	uint32_t lopage = ADDR_TO_PN(addr);
	uint32_t npages = (len % PAGE_SIZE == 0) ? len / PAGE_SIZE : len / PAGE_SIZE + 1;
	int dir = VMMAP_DIR_HILO;
	vmarea_t *new = NULL;

	// 2.errors checking

	if(!curproc->p_vmmap) {
		dbg(DBG_PRINT, "(GRADING3A)\n");
		return -EINVAL;
	}

	if(0 >= npages) {
		dbg(DBG_PRINT, "(GRADING3A)\n");
		return -EINVAL;
	}
	// access permission RO, WO, RW MMU -> mman.h vmarea->vma_prot, prot
	if(!((MAP_SHARED & flags) || (MAP_PRIVATE & flags))) {
		dbg(DBG_PRINT, "(GRADING3A)\n");
		return -EINVAL;
	}
	// in-bound? MMU
	if(lopage != 0 &&!((ADDR_TO_PN(USER_MEM_LOW) <= lopage && (ADDR_TO_PN(USER_MEM_HIGH) >= (lopage + npages))))) {
		dbg(DBG_PRINT, "(GRADING3A)\n");
		return -EFAULT;
	}
	if(!PAGE_ALIGNED(off)) {
		dbg(DBG_PRINT, "(GRADING3A)\n");
		return -EINVAL;
	}
	// 3) in user space address range? [USER_MEM_LOW - USER_MEM_HIGH)
	// 4) validity bit in MMU


	// 3.add a mapping to the current process
	int retval = vmmap_map(curproc->p_vmmap, file->f_vnode, lopage, npages, prot, flags, off, dir, &new);
    if(retval < 0) {
    	dbg(DBG_PRINT, "(GRADING3A)\n");
    	return retval;
    	
    }

    fput(file); //

    // 4. clear the TLB
    
    //pt_unmap(pagedir_t *pd, uintptr_t vaddr)
    pt_unmap_range(curproc->p_pagedir, (uintptr_t)PN_TO_ADDR(new->vma_start), (uintptr_t)PN_TO_ADDR(new->vma_end));
     tlb_flush_all();
    if(ret != NULL){
        *ret = (void *)PN_TO_ADDR(new->vma_start);
        dbg(DBG_PRINT, "(GRADING3A)\n");
    }
    // (postcondition) 
    KASSERT(NULL != curproc->p_pagedir); /* page table must be valid after a memory segment is mapped into the address space */
    dbg(DBG_PRINT, "(GRADING3A 2a)\n");
    return 0;
}


/*
 * This function implements the munmap(2) syscall.
 *
 * As with do_mmap() it should perform the required error checking,
 * before calling upon vmmap_remove() to do most of the work.
 * Remember to clear the TLB.
 */
int
do_munmap(void *addr, size_t len)
{
    // NOT_YET_IMPLEMENTED("VM: do_munmap");

    uint32_t lopage = ADDR_TO_PN(addr);
    uint32_t npages = (len % PAGE_SIZE == 0) ? len / PAGE_SIZE : len / PAGE_SIZE + 1;

    // 1.errors checking
	if(!((ADDR_TO_PN(USER_MEM_LOW) <= lopage && (ADDR_TO_PN(USER_MEM_HIGH) >= (lopage + npages))))) {
		return -EINVAL;
	}
	/*
	if(0 > npages) {
		return -EINVAL;
	}
	*/
    // 2.remove a mapping
    int retval = vmmap_remove(curproc->p_vmmap, lopage, npages);
	if(retval < 0) {
    	return retval;
    }
    // 3.clear the TLB
   

    pt_unmap_range(curproc->p_pagedir, (uintptr_t)PAGE_ALIGN_DOWN(addr), (uintptr_t)PAGE_ALIGN_UP((uintptr_t)addr + len));
    tlb_flush_all();
    return 0;
}

