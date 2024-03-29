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

#include "kernel.h"
#include "errno.h"
#include "globals.h"

#include "vm/vmmap.h"
#include "vm/shadow.h"
#include "vm/anon.h"

#include "proc/proc.h"

#include "util/debug.h"
#include "util/list.h"
#include "util/string.h"
#include "util/printf.h"

#include "fs/vnode.h"
#include "fs/file.h"
#include "fs/fcntl.h"
#include "fs/vfs_syscall.h"

#include "mm/slab.h"
#include "mm/page.h"
#include "mm/mm.h"
#include "mm/mman.h"
#include "mm/mmobj.h"

static slab_allocator_t *vmmap_allocator;
static slab_allocator_t *vmarea_allocator;

void
vmmap_init(void)
{
        vmmap_allocator = slab_allocator_create("vmmap", sizeof(vmmap_t));
        KASSERT(NULL != vmmap_allocator && "failed to create vmmap allocator!");
        vmarea_allocator = slab_allocator_create("vmarea", sizeof(vmarea_t));
        KASSERT(NULL != vmarea_allocator && "failed to create vmarea allocator!");
}

vmarea_t *
vmarea_alloc(void)
{
        vmarea_t *newvma = (vmarea_t *) slab_obj_alloc(vmarea_allocator);
        if (newvma) {
                newvma->vma_vmmap = NULL;
        }
        return newvma;
}

void
vmarea_free(vmarea_t *vma)
{
        KASSERT(NULL != vma);
        slab_obj_free(vmarea_allocator, vma);
}

/* a debugging routine: dumps the mappings of the given address space. */
size_t
vmmap_mapping_info(const void *vmmap, char *buf, size_t osize)
{
        KASSERT(0 < osize);
        KASSERT(NULL != buf);
        KASSERT(NULL != vmmap);

        vmmap_t *map = (vmmap_t *)vmmap;
        vmarea_t *vma;
        ssize_t size = (ssize_t)osize;

        int len = snprintf(buf, size, "%21s %5s %7s %8s %10s %12s\n",
                           "VADDR RANGE", "PROT", "FLAGS", "MMOBJ", "OFFSET",
                           "VFN RANGE");

        list_iterate_begin(&map->vmm_list, vma, vmarea_t, vma_plink) {
                size -= len;
                buf += len;
                if (0 >= size) {
                        goto end;
                }

                len = snprintf(buf, size,
                               "%#.8x-%#.8x  %c%c%c  %7s 0x%p %#.5x %#.5x-%#.5x\n",
                               vma->vma_start << PAGE_SHIFT,
                               vma->vma_end << PAGE_SHIFT,
                               (vma->vma_prot & PROT_READ ? 'r' : '-'),
                               (vma->vma_prot & PROT_WRITE ? 'w' : '-'),
                               (vma->vma_prot & PROT_EXEC ? 'x' : '-'),
                               (vma->vma_flags & MAP_SHARED ? " SHARED" : "PRIVATE"),
                               vma->vma_obj, vma->vma_off, vma->vma_start, vma->vma_end);
        } list_iterate_end();

end:
        if (size <= 0) {
                size = osize;
                buf[osize - 1] = '\0';
        }
        /*
        KASSERT(0 <= size);
        if (0 == size) {
                size++;
                buf--;
                buf[0] = '\0';
        }
        */
        return osize - size;
}

/* Create a new vmmap, which has no vmareas and does
 * not refer to a process. */
vmmap_t *
vmmap_create(void)
{
    //NOT_YET_IMPLEMENTED("VM: vmmap_create");
    vmmap_t * obj = (vmmap_t *)slab_obj_alloc(vmmap_allocator);
    list_init(&obj->vmm_list);
    obj->vmm_proc = NULL;
    return obj;
}

/* Removes all vmareas from the address space and frees the
 * vmmap struct. */
void
vmmap_destroy(vmmap_t *map)
{
    //NOT_YET_IMPLEMENTED("VM: vmmap_destroy");
    KASSERT(NULL != map); /* function argument must not be NULL */
    dbg(DBG_PRINT, "(GRADING3A 3a)\n");
    vmarea_t * curr = NULL;
    list_iterate_begin(&map->vmm_list, curr, vmarea_t, vma_plink) {
        
        curr->vma_obj->mmo_ops->put(curr->vma_obj);
        list_remove(&curr->vma_plink);
        list_remove(&curr->vma_olink);
        vmarea_free(curr);
        dbg(DBG_PRINT, "(GRADING3A)\n");
    } list_iterate_end();
    slab_obj_free(vmmap_allocator, map);
    dbg(DBG_PRINT, "(GRADING3A)\n");
}

/* Add a vmarea to an address space. Assumes (i.e. asserts to some extent)
 * the vmarea is valid.  This involves finding where to put it in the list
 * of VM areas, and adding it. Don't forget to set the vma_vmmap for the
 * area. */
void
vmmap_insert(vmmap_t *map, vmarea_t *newvma)
{
    //NOT_YET_IMPLEMENTED("VM: vmmap_insert");
    //kasserts needed
    KASSERT(NULL != map && NULL != newvma); /* both function arguments must not be NULL */
    KASSERT(NULL == newvma->vma_vmmap); /* newvma must be newly create and must not be part of any existing vmmap */
    KASSERT(newvma->vma_start < newvma->vma_end); /* newvma must not be empty */
    KASSERT(ADDR_TO_PN(USER_MEM_LOW) <= newvma->vma_start && ADDR_TO_PN(USER_MEM_HIGH) >= newvma->vma_end);
                                  /* addresses in this memory segment must lie completely within the user space */
    dbg(DBG_PRINT, "(GRADING3A 3b)\n");
    vmarea_t * curr = NULL;
    list_iterate_begin(&map->vmm_list, curr, vmarea_t, vma_plink) {
        if(newvma->vma_start < curr->vma_start){
            list_insert_before(&curr->vma_plink, &newvma->vma_plink);
            newvma->vma_vmmap = map;
            dbg(DBG_PRINT, "(GRADING3A)\n");
            return;
        }
        dbg(DBG_PRINT, "(GRADING3A)\n");
    } list_iterate_end();

    list_insert_tail(&map->vmm_list, &newvma->vma_plink);
    newvma->vma_vmmap = map;
    dbg(DBG_PRINT, "(GRADING3A)\n");
}

/* Find a contiguous range of free virtual pages of length npages in
 * the given address space. Returns starting vfn for the range,
 * without altering the map. Returns -1 if no such range exists.
 *
 * Your algorithm should be first fit. If dir is VMMAP_DIR_HILO, you
 * should find a gap as high in the address space as possible; if dir
 * is VMMAP_DIR_LOHI, the gap should be as low as possible. */
int
vmmap_find_range(vmmap_t *map, uint32_t npages, int dir)
{
        //NOT_YET_IMPLEMENTED("VM: vmmap_find_range");
    //NEED TO FIGURE OUT WHAT THIS IS
    int MAXVFN = ADDR_TO_PN(USER_MEM_HIGH);
    vmarea_t * curr = NULL;
    vmarea_t * prev = NULL;
    if(dir == VMMAP_DIR_HILO){
        list_iterate_reverse(&map->vmm_list, curr, vmarea_t, vma_plink) {
            if(prev == NULL){
                if((MAXVFN - curr->vma_end) >= npages){
                    dbg(DBG_PRINT, "(GRADING3A)\n");
                    return MAXVFN-npages;
                }
            }else{
                if((prev->vma_start - curr->vma_end) >= npages){
                    dbg(DBG_PRINT, "(GRADING3D 2)\n");
                    return prev->vma_start - npages;
                }
            }
            prev = curr;
        } list_iterate_end();
        if(prev->vma_start - ADDR_TO_PN(USER_MEM_LOW) >= npages){
            dbg(DBG_PRINT, "(GRADING3A)\n");
            return prev->vma_start - npages;
        }
    }else{
        list_iterate_begin(&map->vmm_list, curr, vmarea_t, vma_plink) {
            if(prev == NULL){
                if(curr->vma_start - USER_MEM_LOW >= npages){
                    dbg(DBG_PRINT, "(GRADING3A)\n");
                    return 0;
                }
            }else{
                if((curr->vma_start - prev->vma_end) >= npages){
                    dbg(DBG_PRINT, "(GRADING3A)\n");
                    return curr->vma_start;
                }
            }
            prev = curr;
        } list_iterate_end();
        if((USER_MEM_HIGH - prev->vma_end) >= npages){
            dbg(DBG_PRINT, "(GRADING3A)\n");
            return curr->vma_start;
        }
    }
    dbg(DBG_PRINT, "(GRADING3D 2)\n");
    return -1;
}

/* Find the vm_area that vfn lies in. Simply scan the address space
 * looking for a vma whose range covers vfn. If the page is unmapped,
 * return NULL. */
vmarea_t *
vmmap_lookup(vmmap_t *map, uint32_t vfn)
{
    //NOT_YET_IMPLEMENTED("VM: vmmap_lookup");
    KASSERT(NULL != map); /* the first function argument must not be NULL */
    dbg(DBG_PRINT, "(GRADING3A 3c)\n");
    vmarea_t * curr = NULL;
    list_iterate_begin(&map->vmm_list, curr, vmarea_t, vma_plink) {
        if(curr->vma_start <= vfn && curr->vma_end > vfn){
            dbg(DBG_PRINT, "(GRADING3A)\n");
            return curr;
        }
        dbg(DBG_PRINT, "(GRADING3A)\n");
    } list_iterate_end();
    dbg(DBG_PRINT, "(GRADING3A)\n");
    return NULL;
}
///                     5000        5050
//                        vfn     5000

/* Allocates a new vmmap containing a new vmarea for each area in the
 * given map. The areas should have no mmobjs set yet. Returns pointer
 * to the new vmmap on success, NULL on failure. This function is
 * called when implementing fork(2). */
vmmap_t *
vmmap_clone(vmmap_t *map)
{
        //NOT_YET_IMPLEMENTED("VM: vmmap_clone");
    vmmap_t * obj = vmmap_create();
    vmarea_t * curr = NULL;
    vmarea_t * new_vma = NULL;
    list_iterate_begin(&map->vmm_list, curr, vmarea_t, vma_plink) {
        new_vma = vmarea_alloc();
        new_vma->vma_start = curr->vma_start;
        new_vma->vma_end = curr->vma_end;
        new_vma->vma_off = curr->vma_off;
        new_vma->vma_prot = curr->vma_prot;
        new_vma->vma_flags = curr->vma_flags;
        new_vma->vma_obj = curr->vma_obj;
        
        list_link_init(&new_vma->vma_plink);
        list_link_init(&new_vma->vma_olink);
        vmmap_insert(obj, new_vma);

        list_insert_tail(mmobj_bottom_vmas(curr->vma_obj), &(new_vma->vma_olink));
        // list_insert_tail(&obj->vmm_list, &new_vma->vma_plink);
        
        // new_vma->vma_obj->mmo_ops->ref(new_vma->vma_obj);
        //vma_olink
        dbg(DBG_PRINT, "(GRADING3A)\n");
    } list_iterate_end();
    obj->vmm_proc = map->vmm_proc;
    dbg(DBG_PRINT, "(GRADING3A)\n");
    return obj;
}

/* Insert a mapping into the map starting at lopage for npages pages.
 * If lopage is zero, we will find a range of virtual addresses in the
 * process that is big enough, by using vmmap_find_range with the same
 * dir argument.  If lopage is non-zero and the specified region
 * contains another mapping that mapping should be unmapped.
 *
 * If file is NULL an anon mmobj will be used to create a mapping
 * of 0's.  If file is non-null that vnode's file will be mapped in
 * for the given range.  Use the vnode's mmap operation to get the
 * mmobj for the file; do not assume it is file->vn_obj. Make sure all
 * of the area's fields except for vma_obj have been set before
 * calling mmap.
 *
 * If MAP_PRIVATE is specified set up a shadow object for the mmobj.
 *
 * All of the input to this function should be valid (KASSERT!).
 * See mmap(2) for for description of legal input.
 * Note that off should be page aligned.
 *
 * Be very careful about the order operations are performed in here. Some
 * operation are impossible to undo and should be saved until there
 * is no chance of failure.
 *
 * If 'new' is non-NULL a pointer to the new vmarea_t should be stored in it.
 */
int
vmmap_map(vmmap_t *map, vnode_t *file, uint32_t lopage, uint32_t npages,
          int prot, int flags, off_t off, int dir, vmarea_t **new)
{
    //NOT_YET_IMPLEMENTED("VM: vmmap_map");
    // precondition
    KASSERT(NULL != map); /* must not add a memory segment into a non-existing vmmap */
    KASSERT(0 < npages); /* number of pages of this memory segment cannot be 0 */
    KASSERT((MAP_SHARED & flags) || (MAP_PRIVATE & flags)); /* must specify whether the memory segment is shared or private */
    KASSERT((0 == lopage) || (ADDR_TO_PN(USER_MEM_LOW) <= lopage)); /* if lopage is not zero, it must be a user space vpn */
    KASSERT((0 == lopage) || (ADDR_TO_PN(USER_MEM_HIGH) >= (lopage + npages)));
    /* if lopage is not zero, the specified page range must lie completely within the user space */
    KASSERT(PAGE_ALIGNED(off)); /* the off argument must be page aligned */
    dbg(DBG_PRINT, "(GRADING3A 3d)\n");
    dbg(DBG_TEMP, "lopage %d, npages %d, off %d\n", lopage, npages, off);   
    //kasserts for the inputs
    int start;
    if(lopage == 0){
        start = vmmap_find_range(map, npages, dir);
        if(start < 0){
            dbg(DBG_PRINT, "(GRADING3A)\n");
            return start;
        }

    }else{
        if(vmmap_is_range_empty(map, lopage, npages)){
            
            start = lopage;
            dbg(DBG_PRINT, "(GRADING3A)\n");
        }else{
            vmmap_remove(map, lopage, npages);
            start = lopage;
            dbg(DBG_PRINT, "(GRADING3A)\n");
        }
    }
    vmarea_t * new_vma = vmarea_alloc();
    new_vma->vma_start = start;
    new_vma->vma_end = start + npages;
    new_vma->vma_off = off / PAGE_SIZE;
    new_vma->vma_prot = prot;
    list_link_init(&new_vma->vma_plink);
    list_link_init(&new_vma->vma_olink);
/*
    if(MAP_SHARED & flags){
        new_vma->vma_flags = MAP_SHARED;
    }else if(MAP_PRIVATE & flags){
         new_vma->vma_flags = MAP_PRIVATE;
    }
*/
    new_vma->vma_flags = flags;


    if(file == NULL){
        //anon mmobj?
        new_vma->vma_obj = anon_create();
        //new_vma->vma_obj->mmo_ops->ref(new_vma->vma_obj);
        dbg(DBG_PRINT, "(GRADING3A)\n");
    }else{
        file->vn_ops->mmap(file, new_vma, &new_vma->vma_obj);
        dbg(DBG_PRINT, "(GRADING3D 2)\n");
    }

    
    list_insert_tail(mmobj_bottom_vmas(new_vma->vma_obj), &(new_vma->vma_olink));
    //list_insert_head(&new_vma->vma_obj->mmo_un.mmo_vmas, &new_vma->vma_olink);
    mmobj_t* shadowobj;
    
    if(flags & MAP_PRIVATE){
        //set up shadow object
        shadowobj = shadow_create();
        shadowobj->mmo_shadowed = new_vma->vma_obj;
        shadowobj->mmo_un.mmo_bottom_obj = new_vma->vma_obj;

        //new_vma->vma_obj->mmo_ops->ref(new_vma->vma_obj);
        new_vma->vma_obj = shadowobj;
        dbg(DBG_PRINT, "(GRADING3A)\n");
        //list_insert_tail(mmobj_bottom_vmas(new_vma->vma_obj), &(new_vma->vma_olink));
    }
    vmmap_insert(map, new_vma);

    if(new != NULL){
        *new = new_vma;
        dbg(DBG_PRINT, "(GRADING3A)\n");
    }
    dbg(DBG_PRINT, "(GRADING3A)\n");
    return 0;
}

/*
 * We have no guarantee that the region of the address space being
 * unmapped will play nicely with our list of vmareas.
 *
 * You must iterate over each vmarea that is partially or wholly covered
 * by the address range [addr ... addr+len). The vm-area will fall into one
 * of four cases, as illustrated below:
 *
 * key:
 *          [             ]   Existing VM Area
 *        *******             Region to be unmapped
 *
 * Case 1:  [   ******    ]
 * The region to be unmapped lies completely inside the vmarea. We need to
 * split the old vmarea into two vmareas. be sure to increment the
 * reference count to the file associated with the vmarea.
 *
 * Case 2:  [      *******]**
 * The region overlaps the end of the vmarea. Just shorten the length of
 * the mapping.
 *
 * Case 3: *[*****        ]
 * The region overlaps the beginning of the vmarea. Move the beginning of
 * the mapping (remember to update vma_off), and shorten its length.
 *
 * Case 4: *[*************]**
 * The region completely contains the vmarea. Remove the vmarea from the
 *
 * Case 5: [   ]  *****
 * Case 6: **** [    ]
 * list.
 *          10, 11, 12, 13, 14
 *          10      15 
 *          [       ]
         15 20
 */
int
vmmap_remove(vmmap_t *map, uint32_t lopage, uint32_t npages)
{
    // NOT_YET_IMPLEMENTED("VM: vmmap_remove");

    vmarea_t *curr_vma = NULL;
    list_iterate_begin(&map->vmm_list, curr_vma, vmarea_t, vma_plink) {
        if(curr_vma->vma_start < lopage && curr_vma->vma_end - 1 >= lopage + npages){
            vmarea_t * new_vma = NULL;

            new_vma = vmarea_alloc();
            new_vma->vma_start = lopage + npages;
            new_vma->vma_end = curr_vma->vma_end;
            new_vma->vma_off = curr_vma->vma_end - curr_vma->vma_start + curr_vma->vma_off + lopage + npages; //
            new_vma->vma_prot = curr_vma->vma_prot;
            new_vma->vma_flags = curr_vma->vma_flags;
            
            new_vma->vma_obj = curr_vma->vma_obj;
            curr_vma->vma_end = lopage;
            list_link_init(&new_vma->vma_plink);
            list_link_init(&new_vma->vma_olink);
            vmmap_insert(map, new_vma);
            list_insert_tail(mmobj_bottom_vmas(new_vma->vma_obj), &new_vma->vma_olink);
            new_vma->vma_obj->mmo_ops->ref(curr_vma->vma_obj);
            dbg(DBG_PRINT, "(GRADING3D 2)\n");
        }else if(curr_vma->vma_start < lopage && curr_vma->vma_end -1 < lopage + npages && curr_vma->vma_end > lopage){
            curr_vma->vma_end = lopage;
            dbg(DBG_PRINT, "(GRADING3D 2)\n");
        }else if(curr_vma->vma_start >= lopage && curr_vma->vma_end -1 >= lopage + npages && curr_vma->vma_start <= lopage + npages){
            curr_vma->vma_start = lopage + npages;
            curr_vma->vma_off += lopage + npages - curr_vma->vma_start; 
            dbg(DBG_PRINT, "(GRADING3D 2)\n");
        }else if(curr_vma->vma_start >= lopage && curr_vma->vma_end - 1 < lopage + npages){
            
            curr_vma->vma_obj->mmo_ops->put(curr_vma->vma_obj);
            list_remove(&curr_vma->vma_plink);
            //curr_vma->vma_vmmap = NULL;
            list_remove(&curr_vma->vma_olink);
            vmarea_free(curr_vma);
            dbg(DBG_PRINT, "(GRADING3A)\n");
        }
        dbg(DBG_PRINT, "(GRADING3A)\n");
    } list_iterate_end();
    dbg(DBG_PRINT, "(GRADING3A)\n");
    return 0;
}

/*
 * Returns 1 if the given address space has no mappings for the
 * given range, 0 otherwise.
 *      [        ]
 *               *******
 */
int
vmmap_is_range_empty(vmmap_t *map, uint32_t startvfn, uint32_t npages)
{
    // NOT_YET_IMPLEMENTED("VM: vmmap_is_range_empty");
    KASSERT((startvfn < startvfn+npages) && (ADDR_TO_PN(USER_MEM_LOW) <= startvfn) && (ADDR_TO_PN(USER_MEM_HIGH) >= startvfn+npages));
                                  /* the specified page range must not be empty and lie completely within the user space */
    dbg(DBG_PRINT, "(GRADING3A 3e)\n");
    vmarea_t * curr_vma = NULL;
    list_iterate_begin(&map->vmm_list, curr_vma, vmarea_t, vma_plink) {
        if(!(curr_vma->vma_start >= startvfn + npages || curr_vma->vma_end <= startvfn)){
            dbg(DBG_PRINT, "(GRADING3A)\n");
            return 0;
        }
        dbg(DBG_PRINT, "(GRADING3A)\n");
    } list_iterate_end();
    dbg(DBG_PRINT, "(GRADING3A)\n");
    return 1;
}

/* Read into 'buf' from the virtual address space of 'map' starting at
 * 'vaddr' for size 'count'. To do so, you will want to find the vmareas
 * to read from, then find the pframes within those vmareas corresponding
 * to the virtual addresses you want to read, and then read from the
 * physical memory that pframe points to. You should not check permissions
 * of the areas. Assume (KASSERT) that all the areas you are accessing exist.
 * Returns 0 on success, -errno on error.
 */
int
vmmap_read(vmmap_t *map, const void *vaddr, void *buf, size_t count)
{
    // NOT_YET_IMPLEMENTED("VM: vmmap_read");
    int curr_pos  = count;
    uint32_t curr_addr = 0;

    // find the vmarea
    uint32_t vfn = ADDR_TO_PN(vaddr);
    uintptr_t off = PAGE_OFFSET(vaddr);
    dbg(DBG_TEMP, "curproc: %d, vaddr 0x%x, count %d, off 0x%x\n", curproc->p_pid, (uintptr_t)vaddr, count, (uintptr_t) off);
    //vmmap_lookup(vmmap_t *map, uint32_t vfn)
    vmarea_t* curr_vma = vmmap_lookup(map, vfn);
    pframe_t *result = NULL;    

    void *curr_buf = buf;
    int retval = 0;

    uint32_t pagenum = vfn - curr_vma->vma_start + curr_vma->vma_off;
    int currcpy = 0;
    while(curr_pos > 0){

        if(vfn >= curr_vma->vma_end){
            curr_vma = vmmap_lookup(map, vfn);
            pagenum = vfn - curr_vma->vma_start + curr_vma->vma_off;
            dbg(DBG_PRINT, "(GRADING3A)\n");
        }

        //pfn = pt_virt_to_phys(vfn);
        // find the pframe
        retval = pframe_lookup(curr_vma->vma_obj, pagenum, 0, &result);
        if((int)(0x1000 - off) < curr_pos){
            currcpy = 0x1000 - off;
            dbg(DBG_PRINT, "(GRADING3A)\n");
        }else{
            currcpy = curr_pos;
            dbg(DBG_PRINT, "(GRADING3A)\n");
        }
        // read from the physical memory
        if(curr_addr == 0){
            memcpy(buf, (void *)(((uint32_t)result->pf_addr) + off), currcpy);
            curr_addr += currcpy;
            curr_pos -= currcpy;
            dbg(DBG_PRINT, "(GRADING3A)\n");
        }else if(0x1000 > curr_pos){
            memcpy((void*)((uint32_t)buf+curr_addr), result->pf_addr, curr_pos);
            curr_addr += curr_pos;
            curr_pos -= curr_pos;
            dbg(DBG_PRINT, "(GRADING3A)\n");
        }else{
            memcpy((void*)((uint32_t)buf+curr_addr), result->pf_addr, 0x1000);
            curr_addr += 0x1000;
            curr_pos -= 0x1000;
            dbg(DBG_PRINT, "(GRADING3A)\n");
        }

        // range to copy from is from vaddr to vaddr + count
        
        vfn++;
        pagenum++;
        result = NULL;
        dbg(DBG_PRINT, "(GRADING3A)\n");
    }
    dbg(DBG_PRINT, "(GRADING3A)\n");
    return retval;
}
      
/* Write from 'buf' into the virtual address space of 'map' starting at
 * 'vaddr' for size 'count'. To do this, you will need to find the correct
 * vmareas to write into, then find the correct pframes within those vmareas,
 * and finally write into the physical addresses that those pframes correspond
 * to. You should not check permissions of the areas you use. Assume (KASSERT)
 * that all the areas you are accessing exist. Remember to dirty pages!
 * Returns 0 on success, -errno on error.
 */
int
vmmap_write(vmmap_t *map, void *vaddr, const void *buf, size_t count)
{
    // NOT_YET_IMPLEMENTED("VM: vmmap_write");
    
    int curr_pos  = count;
    uint32_t curr_addr = 0;

    // find the vmarea
    uint32_t vfn = ADDR_TO_PN(vaddr); //0x12345
    uintptr_t off = PAGE_OFFSET(vaddr); //678
    dbg(DBG_TEMP, "curproc: %d, vaddr 0x%x, count %d, off 0x%x\n", curproc->p_pid, (uintptr_t)vaddr, count, (uintptr_t)off);
    vmarea_t* curr_vma = vmmap_lookup(map, vfn);
    pframe_t *result = NULL;    

    int retval = 0;
    uint32_t pagenum = vfn - curr_vma->vma_start + curr_vma->vma_off;

    int currcpy = 0;
    while(curr_pos > 0){

        if(vfn >= curr_vma->vma_end){
            curr_vma = vmmap_lookup(map, vfn);
            pagenum = vfn - curr_vma->vma_start + curr_vma->vma_off;
            dbg(DBG_PRINT, "(GRADING3A)\n");
        }

        // pagenum = vfn - vma_start + vma_off
        // find the pframe 
        retval = pframe_lookup(curr_vma->vma_obj, pagenum, 1, &result);
        
        if(retval < 0){
            dbg(DBG_PRINT, "(GRADING3A)\n");
            return retval;
        }
        if((int)(0x1000 - off) < curr_pos){
            currcpy = 0x1000 - off;
            dbg(DBG_PRINT, "(GRADING3A)\n");
        }else{
            currcpy = curr_pos;
            dbg(DBG_PRINT, "(GRADING3A)\n");
        }

        pframe_dirty(result);

        // read from the physical memory
        if(curr_addr == 0){
            memcpy((void *)((uint32_t)result->pf_addr + off), buf, currcpy);
            curr_addr += currcpy;
            curr_pos -= currcpy;
            dbg(DBG_PRINT, "(GRADING3A)\n");
        }else if(0x1000 > curr_pos){
            memcpy(result->pf_addr, (const void*)((uint32_t)buf+curr_addr), curr_pos);
            curr_addr += curr_pos;
            curr_pos -= curr_pos;
            dbg(DBG_PRINT, "(GRADING3A)\n");
        }else{
            memcpy(result->pf_addr, (const void*)((uint32_t)buf+curr_addr), 0x1000);
            curr_addr += 0x1000;
            curr_pos -= 0x1000;
            dbg(DBG_PRINT, "(GRADING3A)\n");
        }

        //pframe_clean(result);

        // range to copy from is from vaddr to vaddr + count
        
        vfn++;
        pagenum++;
        result = NULL;
        dbg(DBG_PRINT, "(GRADING3A)\n");
    }
dbg(DBG_PRINT, "(GRADING3A)\n");
    return retval;
}
