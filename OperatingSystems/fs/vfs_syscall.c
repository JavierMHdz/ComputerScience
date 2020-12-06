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

/*
 *  FILE: vfs_syscall.c
 *  AUTH: mcc | jal
 *  DESC:
 *  DATE: Wed Apr  8 02:46:19 1998
 *  $Id: vfs_syscall.c,v 1.2 2018/05/27 03:57:26 cvsps Exp $
 */

#include "kernel.h"
#include "errno.h"
#include "globals.h"
#include "fs/vfs.h"
#include "fs/file.h"
#include "fs/vnode.h"
#include "fs/vfs_syscall.h"
#include "fs/open.h"
#include "fs/fcntl.h"
#include "fs/lseek.h"
#include "mm/kmalloc.h"
#include "util/string.h"
#include "util/printf.h"
#include "fs/stat.h"
#include "util/debug.h"

/*
 * Syscalls for vfs. Refer to comments or man pages for implementation.
 * Do note that you don't need to set errno, you should just return the
 * negative error code.
 */

/* To read a file:
 *      o fget(fd)
 *      o call its virtual read vn_op
 *      o update f_pos
 *      o fput() it
 *      o return the number of bytes read, or an error
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        fd is not a valid file descriptor or is not open for reading.
 *      o EISDIR
 *        fd refers to a directory.
 *
 * In all cases, be sure you do not leak file refcounts by returning before
 * you fput() a file that you fget()'ed.
 */
int
do_read(int fd, void *buf, size_t nbytes)
{
        //NOT_YET_IMPLEMENTED("VFS: do_read");
        dbg(DBG_TEMP, "fref: calling fget\n");
        file_t * files = fget(fd);
        if(files == NULL){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return -EBADF;
        }


        if(files->f_mode & 1){
                if(S_ISDIR(files->f_vnode->vn_mode)){
                        fput(files);
                        //dbg(DBG_PRINT, "(GRADING2B)\n");
                        return -EISDIR;
                }
                int transfered = files->f_vnode->vn_ops->read(files->f_vnode, files->f_pos, buf, nbytes);
                if(transfered < 0){
                        fput(files);
                        //dbg(DBG_PRINT, "(GRADING2B)\n");
                        return transfered;
                }
                files->f_pos += transfered;
                fput(files);
                //dbg(DBG_PRINT, "(GRADING2A)\n");
                return transfered;
        } else{
                fput(files);
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return -EBADF;
        }

}

/* Very similar to do_read.  Check f_mode to be sure the file is writable.  If
 * f_mode & FMODE_APPEND, do_lseek() to the end of the file, call the write
 * vn_op, and fput the file.  As always, be mindful of refcount leaks.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        fd is not a valid file descriptor or is not open for writing.
 */
int
do_write(int fd, const void *buf, size_t nbytes)
{
        //NOT_YET_IMPLEMENTED("VFS: do_write");
        if(buf == NULL){
                return -EBADF;
        }
        int retval;
        //dbg(DBG_TEMP, "curproc: %d, fref: calling fget, fd: %d, buf: %s\n", curproc->p_pid, fd, (char*)buf);
        file_t * files = fget(fd);
        if(files == NULL){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return -EBADF;
        }
        if(files->f_mode & 2){
                if(files->f_mode & 4){
                        //int fd, int offset, int whence
                        do_lseek(fd, 0, SEEK_END);
                        //dbg(DBG_PRINT, "(GRADING2B)\n");
                }
                //int (*write)(struct vnode *file, off_t offset, const void *buf, size_t count);
                if(files->f_vnode->vn_ops->write){
                        retval = files->f_vnode->vn_ops->write(files->f_vnode, files->f_pos, buf, nbytes);
                        //dbg(DBG_PRINT, "(GRADING2A)\n");
                }else{
                        fput(files);
                        //dbg(DBG_PRINT, "(GRADING2B)\n");
                        return -EBADF;
                }
                if(retval >= 0){
                        do_lseek(fd, retval, SEEK_CUR);

                        // middle
                        /* f is a pointer to the corresponding file */
                        KASSERT((S_ISCHR(files->f_vnode->vn_mode)) ||
                                         (S_ISBLK(files->f_vnode->vn_mode)) ||
                                         ((S_ISREG(files->f_vnode->vn_mode)) && (files->f_pos <= files->f_vnode->vn_len))); /* cursor must not go past end of file for these file types */
                        //dbg(DBG_PRINT, "(GRADING2A 3.a)\n");

                }
        }else{
                fput(files);
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return -EBADF;
        }
        fput(files);
        //dbg(DBG_PRINT, "(GRADING2A)\n");
        return retval;
}

/*
 * Zero curproc->p_files[fd], and fput() the file. Return 0 on success
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        fd isn't a valid open file descriptor.
 */
int
do_close(int fd)
{
        dbg(DBG_TEMP, "fref: calling fget\n");
        //NOT_YET_IMPLEMENTED("VFS: do_close");
        if(fd < 0){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return -EBADF;
        }
        
        file_t * files = fget(fd);
        if(files == NULL){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return -EBADF;
        }
        /*
        if(curproc->p_files[fd] == NULL){
                return -EBADF;
        }
        file_t * f = curproc->p_files[fd];
        fput(f);
        */
        fput(files);
        fput(files);
        curproc->p_files[fd] = NULL;
        //vput(curproc->p_cwd);
        //dbg(DBG_PRINT, "(GRADING2A)\n");
        return 0;
}

/* To dup a file:
 *      o fget(fd) to up fd's refcount
 *      o get_empty_fd()
 *      o point the new fd to the same file_t* as the given fd
 *      o return the new file descriptor
 *
 * Don't fput() the fd unless something goes wrong.  Since we are creating
 * another reference to the file_t*, we want to up the refcount.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        fd isn't an open file descriptor.
 *      o EMFILE
 *        The process already has the maximum number of file descriptors open
 *        and tried to open a new one.
 */
int
do_dup(int fd)
{
        //NOT_YET_IMPLEMENTED("VFS: do_dup");
        dbg(DBG_TEMP, "fref: calling fget\n");
        if(fd < 0){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return -EBADF;
        }
        file_t * files = fget(fd);
        if(files == NULL){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return -EBADF;
        }

        int newfd = get_empty_fd(curproc);
        if(newfd < 0){
                fput(files);
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return newfd;
        }
        curproc->p_files[newfd] = files;
        //dbg(DBG_PRINT, "(GRADING2B)\n");
        return newfd;
}

/* Same as do_dup, but insted of using get_empty_fd() to get the new fd,
 * they give it to us in 'nfd'.  If nfd is in use (and not the same as ofd)
 * do_close() it first.  Then return the new file descriptor.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        ofd isn't an open file descriptor, or nfd is out of the allowed
 *        range for file descriptors.
 */
int
do_dup2(int ofd, int nfd)
{
        //NOT_YET_IMPLEMENTED("VFS: do_dup2");
        if(ofd < 0 ){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return -EBADF;
        }
        //dbg(DBG_TEMP, "fref: calling fget\n");
        file_t * oldfiles = fget(ofd);
        if(oldfiles == NULL){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return -EBADF;
        }
        if(nfd < 0 || nfd >= NFILES){
                fput(oldfiles);
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return -EBADF;
        }
        int retval;
        if(curproc->p_files[nfd] != NULL){
                retval = do_close(nfd);
                if(retval < 0){
                        fput(oldfiles);
                        //dbg(DBG_PRINT, "(GRADING2B)\n");
                        return retval;
                }
        }

        curproc->p_files[nfd] = oldfiles;
        //dbg(DBG_PRINT, "(GRADING2B)\n");
        return nfd;
}

/*
 * This routine creates a special file of the type specified by 'mode' at
 * the location specified by 'path'. 'mode' should be one of S_IFCHR or
 * S_IFBLK (you might note that mknod(2) normally allows one to create
 * regular files as well-- for simplicity this is not the case in Weenix).
 * 'devid', as you might expect, is the device identifier of the device
 * that the new special file should represent.
 *
 * You might use a combination of dir_namev, lookup, and the fs-specific
 * mknod (that is, the containing directory's 'mknod' vnode operation).
 * Return the result of the fs-specific mknod, or an error.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EINVAL
 *        mode requested creation of something other than a device special
 *        file.
 *      o EEXIST
 *        path already exists.
 *      o ENOENT
 *        A directory component in path does not exist.
 *      o ENOTDIR
 *        A component used as a directory in path is not, in fact, a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 */
int
do_mknod(const char *path, int mode, unsigned devid)
{
        //NOT_YET_IMPLEMENTED("VFS: do_mknod");
        if(!S_ISCHR(mode) && !S_ISBLK(mode)){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return -EINVAL;
        }

        size_t namelength;
        char name[NAME_LEN + 1];
        const char * n = (const char *)&name;
        //vnode_t * vnode = (vnode_t *)kmalloc(sizeof(vnode_t));
        //vnode_t ** res_vnode = &vnode;
        vnode_t * vnode;
        vnode_t * prev_vnode;
        //dbg(DBG_TEMP, "vnref: calling divnamev, filename: %s\n", path);
        int retval = dir_namev(path, &namelength, &n, NULL, &vnode);
        if(retval < 0){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return retval;
        }
        prev_vnode = vnode;
        int retval2 = lookup(vnode, n, namelength, &vnode);
        vput(prev_vnode);
        if(retval2 >= 0){
                vput(vnode);
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return -EEXIST;
        }
        
        // middle
        /* dir_vnode is the directory vnode where you will create the target special file */
        KASSERT(NULL != vnode->vn_ops->mknod);
        //dbg(DBG_PRINT, "(GRADING2A 3.b)\n");

//struct vnode *dir, const char *name, size_t name_len, int mode, devid_t devid
        int retval3 = vnode->vn_ops->mknod(vnode, n, namelength, mode, devid);
        /*
        int retval4 = lookup(vnode, n, namelength, &vnode);
        if(retval4 < 0){
                dbg(DBG_PRINT, "(GRADING2B)\n");
                return retval4;
        }*/
        //dbg(DBG_PRINT, "(GRADING2A)\n");
        return retval3;
}

        

/* Use dir_namev() to find the vnode of the dir we want to make the new
 * directory in.  Then use lookup() to make sure it doesn't already exist.
 * Finally call the dir's mkdir vn_ops. Return what it returns.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EEXIST
 *        path already exists.
 *      o ENOENT
 *        A directory component in path does not exist.
 *      o ENOTDIR
 *        A component used as a directory in path is not, in fact, a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 */
int
do_mkdir(const char *path)
{
        //NOT_YET_IMPLEMENTED("VFS: do_mkdir");
        //no error handling for ENOENT or ENAMETOOLONG

        size_t namelength;
        char name[NAME_LEN + 1];
        const char * n = (const char *)&name;
        //vnode_t ** res_vnode = NULL;
        vnode_t * vnode = NULL;
        vnode_t * prev_vnode;
        //dbg(DBG_TEMP, "vnref: calling divnamev, filename: %s\n", path);
        int retval = dir_namev(path, &namelength, &n, NULL, &vnode);
   
        if(retval < 0){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return retval;
        }
        prev_vnode = vnode;
        //dbg(DBG_TEMP, "vnref: returned from dir_namev, vnode: %d, name: %s\n", vnode->vn_vno, path);
//vnode_t *dir, const char *name, size_t len, vnode_t **result
        int retval4 = lookup(vnode, n, namelength, &vnode);
        vput(prev_vnode);
        if(retval4 >= 0){
                vput(vnode);
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return -EEXIST;
        }

        // middle
        /* dir_vnode is the directory vnode where you will create the target directory */
        KASSERT(NULL != vnode->vn_ops->mkdir);
        //dbg(DBG_PRINT, "(GRADING2A 3.c)\n");

//struct vnode *dir,  const char *name, size_t name_len
        
        int retval2 = vnode->vn_ops->mkdir(vnode, n, namelength);
        /*
        int retval3 = lookup(vnode, n, namelength, &vnode);
        if(retval3 < 0){
                dbg(DBG_PRINT, "(GRADING2B)\n");
                return retval3;
        }*/
        //dbg(DBG_PRINT, "(GRADING2A)\n");
        return retval2;
}

/* Use dir_namev() to find the vnode of the directory containing the dir to be
 * removed. Then call the containing dir's rmdir v_op.  The rmdir v_op will
 * return an error if the dir to be removed does not exist or is not empty, so
 * you don't need to worry about that here. Return the value of the v_op,
 * or an error.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EINVAL
 *        path has "." as its final component.
 *      o ENOTEMPTY
 *        path has ".." as its fin344al component.
 *      o ENOENT
 *        A directory component in path does not exist.
 *      o ENOTDIR
 *        A component used as a directory in path is not, in fact, a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 */
int
do_rmdir(const char *path)
{
        //NOT_YET_IMPLEMENTED("VFS: do_rmdir");
        size_t namelength;
        char name[NAME_LEN + 1];
        const char * n = (const char *)&name;
        vnode_t * vnode = NULL;
        vnode_t * childnode = NULL;


//const char *pathname, int flag, vnode_t **res_vnode, vnode_t *base
        
        //dbg(DBG_TEMP, "vnref: calling dirnamev, path: %s\n", path);
        int retval = dir_namev(path, &namelength, &n, NULL, &vnode);
        //dbg(DBG_VNREF, "vnref: returned from dirnamev, retval: %d, name: %s\n", retval, n);
        if(retval < 0){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return retval;
        }
        vput(vnode);
        if(strcmp(n, ".") == 0){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return -EINVAL;
        }else if(strcmp(n, "..") == 0){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return -ENOTEMPTY;
        }
        
        // middle
        /* please use TWO consecutive "conforming dbg() calls" for this since this function is not executed if you just start and stop weenix */
                           /* dir_vnode is the directory vnode where you will remove the target directory */
        KASSERT(NULL != vnode->vn_ops->rmdir);
        //dbg(DBG_PRINT, "(GRADING2A 3.d)\n");
        //dbg(DBG_PRINT, "(GRADING2A)\n");

        int retval2 = open_namev(path, 0, &childnode, NULL);
        if(retval2 < 0){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return retval2;
        }
        vput(childnode);
        
        if(!S_ISDIR(childnode->vn_mode)){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return -ENOTDIR;
        }
        
        //struct vnode *dir,  const char *name, size_t name_len
       
        int retval3 = vnode->vn_ops->rmdir(vnode, n, namelength);
        if(retval3 < 0){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return retval3;
        }
        //vput(childnode);
        //dbg(DBG_PRINT, "(GRADING2A)\n");
        return retval3;

}

/*
 * Similar to do_rmdir, but for files.
 *
 //rmdir's comments
 Use dir_namev() to find the vnode of the directory containing the dir to be
 removed. Then call the containing dir's rmdir v_op.  The rmdir v_op will
 return an error if the dir to be removed does not exist or is not empty, so
 you don't need to worry about that here. Return the value of the v_op,
 or an error.
 //end of rmdir's comments

 * Error cases you must handle for this function at the VFS level:
 *      o EPERM
 *        path refers to a directory.
 *      o ENOENT
 *        Any component in path does not exist, including the element at the
 *        very end.
 *      o ENOTDIR
 *        A component used as a directory in path is not, in fact, a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 */
int
do_unlink(const char *path)
{
        //NOT_YET_IMPLEMENTED("VFS: do_unlink");
        size_t namelength;
        char name[NAME_LEN + 1];
        const char * n = (const char *)&name;
        vnode_t * vnode = NULL;
        vnode_t * new_vnode = NULL;
//const char *pathname, int flag, vnode_t **res_vnode, vnode_t *base

        vnode_t * prev_vnode = NULL;
        //dbg(DBG_TEMP, "vnref: calling divnamev, filename: %s\n", path);
        int retval = dir_namev(path, &namelength, &n, NULL, &vnode);
        //dbg(DBG_VNREF, "vnref: returned from dirnamev, name: %s\n", n);
        if(retval < 0){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return retval;
        }
        prev_vnode = vnode;
        int retval2 = lookup(vnode, name, namelength, &vnode);
        vput(prev_vnode);
        if(retval2 < 0){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return retval2;
        }
        
        vput(vnode);
        if(S_ISDIR(vnode->vn_mode)){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return -EPERM;
        }
        int retval3 = dir_namev(path, &namelength, &n, NULL, &new_vnode);
        if(retval3 < 0){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return retval3;
        }
        vput(new_vnode);
        // middle
        /* please use TWO consecutive "conforming dbg() calls" for this since this function is not executed if you just start and stop weenix */
                            /* dir_vnode is the directory vnode where you will unlink the target file */
        KASSERT(NULL != new_vnode->vn_ops->unlink);
        //dbg(DBG_PRINT, "(GRADING2A 3.e)\n");
        //dbg(DBG_PRINT, "(GRADING2A)\n");

//struct vnode *dir, const char *name, size_t name_len
        int retval4 = new_vnode->vn_ops->unlink(new_vnode, n, namelength);
        if(retval4 < 0){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return retval4;
        }
        vput(vnode);
        //dbg(DBG_PRINT, "(GRADING2A)\n");
        return retval4;
        
}

/* To link:
 *      o open_namev(from)
 *      o dir_namev(to)
 *      o call the destination dir's (to) link vn_ops.
 *      o return the result of link, or an error
 *
 * Remember to vput the vnodes returned from open_namev and dir_namev.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EEXIST
 *        to already exists.
 *      o ENOENT
 *        A directory component in from or to does not exist.
 *      o ENOTDIR
 *        A component used as a directory in from or to is not, in fact, a
 *        directory.
 *      o ENAMETOOLONG
 *        A component of from or to was too long.
 *      o EPERM
 *        from is a directory.
 */
int
do_link(const char *from, const char *to)
{
        //NOT_YET_IMPLEMENTED("VFS: do_link");
        vnode_t * openvnode;
        //dbg(DBG_TEMP, "vnref: calling opennamev, fromfile: %s\n", from);
        int retval1 = open_namev(from, O_RDWR, &openvnode, curproc->p_cwd);
        if(retval1 <0){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return retval1;
        }
        vput(openvnode);

        if(S_ISDIR(openvnode->vn_mode)){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return -EPERM;
        }

        size_t namelength;
        char name[NAME_LEN + 1];
        const char * n = (const char *)&name;
        vnode_t * tovnode;
        //dbg(DBG_VNREF, "vnref: calling divnamev, tofile: %s\n", to);
        int retval2 = dir_namev(to, &namelength, &n, NULL, &tovnode);
        if(retval2 < 0){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return retval2;
        }

        vput(tovnode);
        //vput(tovnode);
//struct vnode *oldvnode, struct vnode *dir, const char *name, size_t name_len
        //dbg(DBG_PRINT, "(GRADING2B)\n");
        return tovnode->vn_ops->link(openvnode, tovnode, name, namelength);

}

/*      o link newname to oldname
 *      o unlink oldname
 *      o return the value of unlink, or an error
 *
 * Note that this does not provide the same behavior as the
 * Linux system call (if unlink fails then two links to the
 * file could exist).
 */
int
do_rename(const char *oldname, const char *newname)
{
        //NOT_YET_IMPLEMENTED("VFS: do_rename");
        //dbg(DBG_TEMP, "old: %s, new:%s\n", oldname, newname);
        int retval = do_link(oldname, newname);
        if(retval < 0){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return retval;
        }
        //dbg(DBG_PRINT, "(GRADING2B)\n");
        return do_unlink(oldname);
        
}

/* Make the named directory the current process's cwd (current working
 * directory).  Don't forget to down the refcount to the old cwd (vput()) and
 * up the refcount to the new cwd (open_namev() or vget()). Return 0 on
 * success.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o ENOENT
 *        path does not exist.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 *      o ENOTDIR
 *        A component of path is not a directory.
 */
int
do_chdir(const char *path)
{
        //NOT_YET_IMPLEMENTED("VFS: do_chdir");
        
        vnode_t * vnode;
        //dbg(DBG_TEMP, "vnref: calling opennamev, path: %s\n", path);
        int retval = open_namev(path, O_RDWR, &vnode, NULL);
        if(retval < 0){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return retval;
        }
        
        if(!S_ISDIR(vnode->vn_mode)){
                vput(vnode);
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return -ENOTDIR;
        }

        vput(curproc->p_cwd);
        curproc->p_cwd = vnode;
        //dbg(DBG_PRINT, "(GRADING2B)\n");
        return 0;
}

/* Call the readdir vn_op on the given fd, filling in the given dirent_t*.
 * If the readdir vn_op is successful, it will return a positive value which
 * is the number of bytes copied to the dirent_t.  You need to increment the
 * file_t's f_pos by this amount.  As always, be aware of refcounts, check
 * the return value of the fget and the virtual function, and be sure the
 * virtual function exists (is not null) before calling it.
 *
 * Return either 0 or sizeof(dirent_t), or -errno.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        Invalid file descriptor fd.
 *      o ENOTDIR
 *        File descriptor does not refer to a directory.
 */
int
do_getdent(int fd, struct dirent *dirp)
{
        //dbg(DBG_TEMP, "fref: calling fget\n");
        //NOT_YET_IMPLEMENTED("VFS: do_getdent");
        if(fd < 0){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return -EBADF;
        }
        file_t * files = fget(fd);
        if(files == NULL){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return -EBADF;
        }
        if(files->f_mode == S_IFDIR){
                fput(files);
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return -ENOTDIR;
        }
//int (*readdir)(struct vnode *dir, off_t offset, struct dirent *d)
        if(files->f_vnode->vn_ops->readdir){
                int retval = files->f_vnode->vn_ops->readdir(files->f_vnode, files->f_pos, dirp);
                files->f_pos += retval;
                fput(files);
                if(retval == 0){
                        //dbg(DBG_PRINT, "(GRADING2B)\n");
                        return 0;
                }
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return sizeof(dirent_t);
        }
        fput(files);
        //dbg(DBG_PRINT, "(GRADING2B)\n");
        return -ENOTDIR;
}

/*
 * Modify f_pos according to offset and whence.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        fd is not an open file descriptor.
 *      o EINVAL
 *        whence is not one of SEEK_SET, SEEK_CUR, SEEK_END; or the resulting
 *        file offset would be negative.
 SEEK_SET
              The offset is set to offset bytes.

 SEEK_CUR
              The offset is set to its current location plus offset bytes.

 SEEK_END
              The offset is set to the size of the file plus offset bytes.
 */
int
do_lseek(int fd, int offset, int whence)
{
        //dbg(DBG_TEMP, "fref: calling fget, fd: %d, offset: %d\n", fd, offset);
        if(fd < 0){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return -EBADF;
        }
        //NOT_YET_IMPLEMENTED("VFS: do_lseek");
        file_t * files = fget(fd);
        if(files == NULL){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return -EBADF;
        }
        int position = files->f_pos;
        if(whence == SEEK_SET){
                position = offset;
                //dbg(DBG_PRINT, "(GRADING2B)\n");
        }else if(whence == SEEK_CUR){
                position += offset;
                //dbg(DBG_PRINT, "(GRADING2A)\n");
        }else if(whence == SEEK_END){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                position = files->f_vnode->vn_len + offset;
        }else{
                fput(files);
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return -EINVAL;
        }

        if(position < 0){
                fput(files);
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return -EINVAL;
        }
        files->f_pos = position;
        fput(files);
        //dbg(DBG_PRINT, "(GRADING2A)\n");
        return files->f_pos;
}

/*
 * Find the vnode associated with the path, and call the stat() vnode operation.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o ENOENT
 *        A component of path does not exist.
 *      o ENOTDIR
 *        A component of the path prefix of path is not a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 *      o EINVAL
 *        path is an empty string.
 */
int
do_stat(const char *path, struct stat *buf)
{
        //NOT_YET_IMPLEMENTED("VFS: do_stat");
        
        if(strlen(path) == 0){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return -EINVAL;
        }
        //if(*buf == NULL){
        //        return -EFAULT;
        //}
        const char * p;
        if(strcmp(path, "/") == 0){
                p = "/..";
                //dbg(DBG_PRINT, "(GRADING2B)\n");
        }else{
                p = path;
                //dbg(DBG_PRINT, "(GRADING2B)\n");
        }

        size_t namelength;
        char name[NAME_LEN + 1];
        const char * n = (const char *)&name;
       
        vnode_t * res_vnode = NULL;
        vnode_t * prev_vnode = NULL;
        //dbg(DBG_TEMP, "vnref: calling divnamev, filename: %s\n", p);
        int retval = dir_namev(p, &namelength, &n, NULL, &res_vnode);
        if(retval < 0){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return retval;;
        }

        prev_vnode = res_vnode;
//vnode_t *dir, const char *name, size_t len, vnode_t **result
        //dbg(DBG_VNREF, "vnref: calling lookup, name: %s\n", name);
        int retval2 = lookup(res_vnode, n, namelength, &res_vnode);
        vput(prev_vnode);
        if(retval2 < 0){
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return retval2;
        }
        //dbg(DBG_VNREF, "vnref: finished lookup");
        
//struct vnode *vnode, struct stat *buf
        // middle
        /* please use TWO consecutive "conforming dbg() calls" for this since this function is not executed if you just start and stop weenix */
                          /* dir_vnode is the directory vnode where you will perform "stat" */
        KASSERT(NULL != res_vnode->vn_ops->stat);
        //dbg(DBG_PRINT, "(GRADING2A 3.f)\n");
        //dbg(DBG_PRINT, "(GRADING2B)\n");

        //if(buf->st_ino == 37){dbg(DBG_TEMP, "crementing to vnode: %d\n", res_vnode->vn_refcount);}
        //dbg(DBG_PRINT, "(GRADING2B)\n");
        int retval3 = res_vnode->vn_ops->stat(res_vnode, buf);
        //dbg(DBG_TEMP, "buf: 0x%x\n: \n", buf);
        vput(res_vnode);
        return retval3;
        
}

#ifdef __MOUNTING__
/*
 * Implementing this function is not required and strongly discouraged unless
 * you are absolutely sure your Weenix is perfect.
 *
 * This is the syscall entry point into vfs for mounting. You will need to
 * create the fs_t struct and populate its fs_dev and fs_type fields before
 * calling vfs's mountfunc(). mountfunc() will use the fields you populated
 * in order to determine which underlying filesystem's mount function should
 * be run, then it will finish setting up the fs_t struct. At this point you
 * have a fully functioning file system, however it is not mounted on the
 * virtual file system, you will need to call vfs_mount to do this.
 *
 * There are lots of things which can go wrong here. Make sure you have good
 * error handling. Remember the fs_dev and fs_type buffers have limited size
 * so you should not write arbitrary length strings to them.
 */
int
do_mount(const char *source, const char *target, const char *type)
{
        NOT_YET_IMPLEMENTED("MOUNTING: do_mount");
        return -EINVAL;
}

/*
 * Implementing this function is not required and strongly discouraged unless
 * you are absolutley sure your Weenix is perfect.
 *
 * This function delegates all of the real work to vfs_umount. You should not worry
 * about freeing the fs_t struct here, that is done in vfs_umount. All this function
 * does is figure out which file system to pass to vfs_umount and do good error
 * checking.
 */
int
do_umount(const char *target)
{
        NOT_YET_IMPLEMENTED("MOUNTING: do_umount");
        return -EINVAL;
}
#endif
