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
#include "globals.h"
#include "types.h"
#include "errno.h"

#include "util/string.h"
#include "util/printf.h"
#include "util/debug.h"

#include "fs/dirent.h"
#include "fs/fcntl.h"
#include "fs/stat.h"
#include "fs/vfs.h"
#include "fs/vnode.h"

/* This takes a base 'dir', a 'name', its 'len', and a result vnode.
 * Most of the work should be done by the vnode's implementation
 * specific lookup() function, but you
 * may want to special case "." and/or ".." here
 * depnding on your implementation.

 *
 * If dir has no lookup(), return -ENOTDIR.
 *
 * Note: returns with the vnode refcount on *result incremented.
 */
int
lookup(vnode_t *dir, const char *name, size_t len, vnode_t **result)
{
        //NOT_YET_IMPLEMENTED("VFS: lookup");
        // precondition
        KASSERT(NULL != dir); /* the "dir" argument must be non-NULL */
        KASSERT(NULL != name); /* the "name" argument must be non-NULL */
        KASSERT(NULL != result); /* the "result" argument must be non-NULL */
        //dbg(DBG_PRINT, "(GRADING2A 2.a)\n");
        
        if(dir->vn_ops->lookup == NULL){
            //dbg(DBG_PRINT, "(GRADING2B)\n");
                return -ENOTDIR;
        }

        if(strncmp(name, ".", len) == 0){
                *result = dir;
                vget(dir->vn_fs, dir->vn_vno);
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return 0;
        }

        int retval = dir->vn_ops->lookup(dir, name, len, result);
        //dbg(DBG_PRINT, "(GRADING2A)\n");
        return retval;
}


/* When successful this function returns data in the following "out"-arguments:
 *  o res_vnode: the vnode of the parent directory of "name"
 *  o name: the `basename' (the element of the pathname)
 *  o namelen: the length of the basename
 *
 * For example: dir_namev("/s5fs/bin/ls", &namelen, &name, NULL,
 * &res_vnode) would put 2 in namelen, "ls" in name, and a pointer to the
 * vnode corresponding to "/s5fs/bin" in res_vnode.
 *
 * The "base" argument defines where we start resolving the path from:
 * A base value of NULL means to use the process's current working directory,
 * curproc->p_cwd.  If pathname[0] == '/', ignore base and start with
 * vfs_root_vn.  dir_namev() should call lookup() to take care of resolving each
 * piece of the pathname.
 *
 * Note: A successful call to this causes vnode refcount on *res_vnode to
 * be incremented.
*      o ENOENT
 *        A directory component in path does not exist.
 *      o ENOTDIR
 *        A component used as a directory in path is not, in fact, a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 */
int
dir_namev(const char *pathname, size_t *namelen, const char **name,
          vnode_t *base, vnode_t **res_vnode)
{
        // NOT_YET_IMPLEMENTED("VFS: dir_namev");
        // precondition
        KASSERT(NULL != pathname); /* the "pathname" argument must be non-NULL */
        KASSERT(NULL != namelen); /* the "namelen" argument must be non-NULL */
        KASSERT(NULL != name); /* the "name" argument must be non-NULL */
        KASSERT(NULL != res_vnode); /* the "res_vnode" argument must be non-NULL */
        //dbg(DBG_PRINT, "(GRADING2A 2.b)\n");

        //
        size_t * lenn = namelen;
        char * n = (char *)*name;
        //
        char fullname[100];
        //strcpy(fullname, pathname);
        snprintf(fullname, 100, "%s", pathname);
        vnode_t *dir_vnode = base;
        vnode_t *prev_vnode = NULL;
        if(pathname[0] == '/'){
            //dbg(DBG_PRINT, "(GRADING2A)\n");
            dir_vnode = vfs_root_vn;
        }else if(!base){
            dir_vnode = curproc->p_cwd;
            //dbg(DBG_PRINT, "(GRADING2B)\n");
        }

        if(strlen(pathname) > MAXPATHLEN){
            //dbg(DBG_PRINT, "(GRADING2B)\n");
            return -ENAMETOOLONG;
        }

        // middle
        KASSERT(NULL != dir_vnode); /* pathname resolution must start with a valid directory */
        //dbg(DBG_PRINT, "(GRADING2A 2.b)\n");

        // increment refcount of res_vnode
        vref(dir_vnode);

        char *delim = "/";
        // char fullname = strcpy(pathname);
        
        char *filename;
        char prevname[100];
        size_t len;
        int retval;

        filename = strtok(fullname, delim);


        if(filename == NULL){
            filename = "..";
            //dbg(DBG_PRINT, "(GRADING2B)\n");
        }

        len = strlen(filename);
        snprintf(prevname, NAME_LEN + 1, "%s", filename);
        if(len > NAME_LEN){
            vput(dir_vnode);
            //dbg(DBG_PRINT, "(GRADING2B)\n");
            return -ENAMETOOLONG;
        }
        prevname[len] = '\0';
        vnode_t * prevDir = NULL;

        while(filename != NULL) {
              
              prevDir = dir_vnode;
              // note, need to do special cases for "///"
              // and maybe look at "\dev", i think its good already
              
              filename = strtok(NULL, delim);

              if(filename != NULL){

                if(!S_ISDIR(dir_vnode->vn_mode)){
                    vput(dir_vnode);
                    //dbg(DBG_PRINT, "(GRADING2B)\n");
                    return -ENOTDIR;
                } 
                //dbg(DBG_VNREF, "vnref: calling lookup, prevname: %s, filename: %s\n", prevname, filename);
                prev_vnode = dir_vnode;
                
                retval = lookup(dir_vnode, prevname, len, &dir_vnode);
                //dbg(DBG_VNREF, "vnref: returning from lookup, retval: %d\n", retval);
                vput(prev_vnode);
                if(retval < 0){
                    //dbg(DBG_PRINT, "(GRADING2B)\n");
                    return retval;
                }
                //vput(dir_vnode);
                len = strlen(filename);
                if(len > NAME_LEN){
                    vput(dir_vnode);
                    //dbg(DBG_PRINT, "(GRADING2B)\n");
                    return -ENAMETOOLONG;
                }  
                snprintf(prevname, NAME_LEN + 1, "%s", filename);

                //strncpy(prevname, filename, len);
                //prevname[len] = '\0';
                //dbg(DBG_PRINT, "(GRADING2A)\n");
              }
             // dbg(DBG_PRINT, "(GRADING2A)\n");
            
        }
        if(!S_ISDIR(dir_vnode->vn_mode)){
                vput(dir_vnode);
                //dbg(DBG_PRINT, "(GRADING2B)\n");
                return -ENOTDIR;
        } 
        *namelen = len;
        strncpy(n, prevname, len);
        n[len] = '\0';
        //name = (const char **)&prevname;
        *res_vnode = prevDir;

        
        //vput(dir_vnode);
        //dbg(DBG_PRINT, "(GRADING2A)\n");
        return 0;
}

/* This returns in res_vnode the vnode requested by the other parameters.
 * It makes use of dir_namev and lookup to find the specified vnode (if it
 * exists).  flag is right out of the parameters to open(2); see
 * <weenix/fcntl.h>.  If the O_CREAT flag is specified and the file does
 * not exist, call create() in the parent directory vnode. However, if the
 * parent directory itself does not exist, this function should fail - in all
 * cases, no files or directories other than the one at the very end of the path
 * should be created.
 *
 * Note: Increments vnode refcount on *res_vnode.
 */
int
open_namev(const char *pathname, int flag, vnode_t **res_vnode, vnode_t *base)
{
        //NOT_YET_IMPLEMENTED("VFS: open_namev");
        size_t namelength;

        char name[NAME_LEN + 1];
        const char * n = (const char *)&name;
        vnode_t * parentnode = NULL;
        vnode_t *prev_vnode = NULL;
        //dbg(DBG_TEMP, "vnref: calling divnamev, filename: %s\n", pathname);
//const char *pathname, size_t *namelen, const char **name, vnode_t *base, vnode_t **res_vnode
        int retval = dir_namev(pathname, &namelength, &n, base, res_vnode);
        if(retval < 0){
            //dbg(DBG_PRINT, "(GRADING2B)\n");
                return retval;
        }
        prev_vnode = *res_vnode;
        int retval3;
        //dbg(DBG_VNREF, "vnref: calling lookup, filename: %s, name: %s\n", pathname, name);
        int retval2 = lookup(*res_vnode, n, namelength, res_vnode);
        vput(prev_vnode);
        if(retval2 < 0){
                if(retval2 == -ENOENT){
                    if(flag & O_CREAT){
                        // middle
                        /* please use TWO consecutive "conforming dbg() calls" for this since this function is not executed if you just start and stop weenix */
                             /* dir_vnode is a directory vnode of the file you are trying to open */
                        KASSERT(NULL != (*res_vnode)->vn_ops->create);
                        //dbg(DBG_PRINT, "(GRADING2A 2.c)\n");
                        //dbg(DBG_PRINT, "(GRADING2B)\n");

                        retval3 = (*res_vnode)->vn_ops->create(*res_vnode, n, namelength, res_vnode);
                        if(retval3 < 0){
                            //dbg(DBG_PRINT, "(GRADING2B)\n");
                            return retval3;
                        }
                        vref(*res_vnode);
                        //dbg(DBG_PRINT, "(GRADING2B)\n");
                    }else{
                        //dbg(DBG_PRINT, "(GRADING2B)\n");
                        return -ENOENT; 
                    }
                }else{
                    //dbg(DBG_PRINT, "(GRADING2B)\n");
                    return retval2;
                }
        }else{
            //dbg(DBG_PRINT, "(GRADING2A)\n");
            return retval2;
        }

        //dbg(DBG_VNREF, "call to lookup finished\n");
        //dbg(DBG_PRINT, "(GRADING2B)\n");
        return retval3;
}

#ifdef __GETCWD__
/* Finds the name of 'entry' in the directory 'dir'. The name is writen
 * to the given buffer. On success 0 is returned. If 'dir' does not
 * contain 'entry' then -ENOENT is returned. If the given buffer cannot
 * hold the result then it is filled with as many characters as possible
 * and a null terminator, -ERANGE is returned.
 *
 * Files can be uniquely identified within a file system by their
 * inode numbers. */
int
lookup_name(vnode_t *dir, vnode_t *entry, char *buf, size_t size)
{
        NOT_YET_IMPLEMENTED("GETCWD: lookup_name");
        return -ENOENT;
}


/* Used to find the absolute path of the directory 'dir'. Since
 * directories cannot have more than one link there is always
 * a unique solution. The path is writen to the given buffer.
 * On success 0 is returned. On error this function returns a
 * negative error code. See the man page for getcwd(3) for
 * possible errors. Even if an error code is returned the buffer
 * will be filled with a valid string which has some partial
 * information about the wanted path. */
ssize_t
lookup_dirpath(vnode_t *dir, char *buf, size_t osize)
{
        NOT_YET_IMPLEMENTED("GETCWD: lookup_dirpath");

        return -ENOENT;
}
#endif /* __GETCWD__ */
