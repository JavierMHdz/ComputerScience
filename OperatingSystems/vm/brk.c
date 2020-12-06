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
#include "util/debug.h"

#include "mm/mm.h"
#include "mm/page.h"
#include "mm/mman.h"

#include "vm/mmap.h"
#include "vm/vmmap.h"

#include "proc/proc.h"

/*
 * This function implements the brk(2) system call.
 *
 * This routine manages the calling process's "break" -- the ending address
 * of the process's "dynamic" region (often also referred to as the "heap").
 * The current value of a process's break is maintained in the 'p_brk' member
 * of the proc_t structure that represents the process in question.
 *
 * The 'p_brk' and 'p_start_brk' members of a proc_t struct are initialized
 * by the loader. 'p_start_brk' is subsequently never modified; it always
 * holds the initial value of the break. Note that the starting break is
 * not necessarily page aligned!
 *
 * 'p_start_brk' is the lower limit of 'p_brk' (that is, setting the break
 * to any value less than 'p_start_brk' should be disallowed).
 *
 * The upper limit of 'p_brk' is defined by the minimum of (1) the
 * starting address of the next occuring mapping or (2) USER_MEM_HIGH.
 * That is, growth of the process break is limited only in that it cannot
 * overlap with/expand into an existing mapping or beyond the region of
 * the address space allocated for use by userland. (note the presence of
 * the 'vmmap_is_range_empty' function).
 *
 * The dynamic region should always be represented by at most ONE vmarea.
 * Note that vmareas only have page granularity, you will need to take this
 * into account when deciding how to set the mappings if p_brk or p_start_brk
 * is not page aligned.
 *
 * You are guaranteed that the process data/bss region is non-empty.
 * That is, if the starting brk is not page-aligned, its page has
 * read/write permissions.
 *
 * If addr is NULL, you should "return" the current break. We use this to
 * implement sbrk(0) without writing a separate syscall. Look in
 * user/libc/syscall.c if you're curious.
 *
 * You should support combined use of brk and mmap in the same process.
 *
 * Note that this function "returns" the new break through the "ret" argument.
 * Return 0 on success, -errno on failure.
 */
int
do_brk(void *addr, void **ret)
{
        // NOT_YET_IMPLEMENTED("VM: do_brk");
        KASSERT(curproc->p_brk >= curproc->p_start_brk);

	int retval;

	// error checking
	if(!addr){
		*ret = curproc->p_brk;
		return 0;
	}
	
	
	if(addr < curproc->p_start_brk || (uint32_t)addr >= USER_MEM_HIGH) { // || the starting address of the next occuring mapping
		return -EFAULT;
	}

	uint32_t lopage = ADDR_TO_PN(PAGE_ALIGN_UP(curproc->p_start_brk));
	uint32_t previousEnd = ADDR_TO_PN(PAGE_ALIGN_UP(curproc->p_brk));
	uint32_t newEnd = ADDR_TO_PN(PAGE_ALIGN_UP(addr));

	uint32_t npages = previousEnd - lopage;
	int dir = VMMAP_DIR_HILO;

	if(newEnd > previousEnd && !vmmap_is_range_empty(curproc->p_vmmap, previousEnd, newEnd- previousEnd)){
		return -EFAULT;
	}
	// Initially, the heap takes up the empty space in the same vmarea as the "data+bss" region. This is the "initial" region used by the heap. In this case, there is no dynamic region vmarea.
	vmarea_t * curr_vma = NULL;
	int prot = PROT_READ | PROT_WRITE;
	int flags = MAP_PRIVATE;
	

	//case 3
	if(addr <= PAGE_ALIGN_UP(curproc->p_start_brk)){
		if(npages == 0){

		}
		else{
			retval = vmmap_remove(curproc->p_vmmap, lopage, npages);
	       	if(retval < 0) {
	        	return retval;
	        } 
    	}
	}
	//case 1
	else if(lopage == previousEnd){
		//for this case, curr_vma is new
		//vmmap_t *map, vnode_t *file, uint32_t lopage, uint32_t npages,
        //  int prot, int flags, off_t off, int dir, vmarea_t **new)
        if(vmmap_is_range_empty(curproc->p_vmmap, lopage, newEnd-lopage)){
			vmmap_map(curproc->p_vmmap, NULL, lopage, newEnd-lopage, prot, flags, 0, dir, &curr_vma);
		}else{
			return -EFAULT;
		}
	}
	//case 2
	//the condition of case 2 depends on it coming after case 1
	else if(newEnd > previousEnd){

		curr_vma = vmmap_lookup(curproc->p_vmmap, lopage);
		curr_vma->vma_end = newEnd;
		

	}
	//case 4
	else if(newEnd < previousEnd && newEnd > lopage){
		curr_vma = vmmap_lookup(curproc->p_vmmap, lopage);
		if(curr_vma == NULL){
			return -EFAULT;
		}
		
		//curr_vma->vma_end = newEnd;
		vmmap_remove(curproc->p_vmmap, newEnd, previousEnd);

	}
	//case 5
	else if(newEnd == lopage){
		if(!vmmap_is_range_empty(curproc->p_vmmap, lopage, npages)){
			retval = vmmap_remove(curproc->p_vmmap, lopage, npages);
	       	if(retval < 0) {
	        	return retval;
	        } 
    	}
    	
	}



	// When you set the brk beyond the "initial" region, you need to create a "new" vmarea and extends the heap into it! Now you have one "dynamic" region vmarea.
	

	*ret = addr;
        curproc->p_brk = *ret;

        return 0;
}

// The heap is represented by exactly 1 vmarea in weenix
// process start_brk stores an address that points somewhere inside the heap vmara and can be used to located it
// start_brk never changes
// p_brk can never be lower than p_start_brk
// increasing p_brk can be achieved by increasing the size of the vmarea's->vma_end
// decreasing p_brk can be acheived by decreasing the vma_end.
// decreasing p_brk means some pages might no longer be valid and that needs to be reflected in the mmobj's, TLB and page table
// the address passed to do_brk need not be page aligned and you need to account for that while updating the underlying data structures

//          sb                   addr           ret
// heap vma [         ][        ]



// 		[			] 
//							|addr
//      

//case 1
//  	[ bss + data  ]
//					   startbrk
//					   brk
// 								addr
//						[dynamic]


// case 2
//					   startbrk
//					   				brk
//											addr
//						[dynamic 			]

// case 3
// |	|	|	 |	|
//  [bss+data ][h]
//			  startbrk
//			  brk
//              addr						

// case 4 
//					   startbrk
//					   				brk
//							addr



// case 5
//					   startbrk
//					   				brk
//					   addr

// case other
//				|	|	|	|	|	|	|	
// 						startbrk
//							 brk
//							   addr

// 				|			|			|			|
// 		startbrk
							


//							addr	
//						    brk
//				[           ]
// 				[ 						]