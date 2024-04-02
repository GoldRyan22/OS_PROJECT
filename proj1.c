#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

ssize_t WriteInSnap(struct stat folder_stat, int fout)
{

    ssize_t written =write(fout, &folder_stat.st_ino, sizeof(long int)); // inode
    if (written==-1) return -4;

    written =write(fout, &folder_stat.st_dev, sizeof(long int)); // dev id
    if (written==-1) return -4;

    written =write(fout, &folder_stat.st_mode, sizeof(long int)); //mode
    if (written==-1) return -4;

    written =write(fout, &folder_stat.st_nlink, sizeof(long int)); //nlinks
    if (written==-1) return -4;

    written =write(fout, &folder_stat.st_uid, sizeof(long int)); // uid
    if (written==-1) return -4;

    written =write(fout, &folder_stat.st_gid, sizeof(long int)); //gid
    if (written==-1) return -4;

    written =write(fout, &folder_stat.st_blksize, sizeof(long int)); //blksize
    if (written==-1) return -4;

    written =write(fout, &folder_stat.st_blocks, sizeof(long int)); //nr_blks
    if (written==-1) return -4;

    written =write(fout, &folder_stat.st_mtime, sizeof(long int)); //mtime
    if (written==-1) return -4;

    written =write(fout, &folder_stat.st_ctime, sizeof(long int)); //ctime
    if (written==-1) return -4;

    /*
    printf("  inode:   %li\n",   (int) folder_stat.st_ino);//
    printf(" dev id:   %li\n",   (int) folder_stat.st_dev);//
    printf("   mode:   %08x\n",       folder_stat.st_mode);//
    printf("  links:   %li\n",         folder_stat.st_nlink);//
    printf("    uid:   %li\n",   (int) folder_stat.st_uid);//
    printf("    gid:   %li\n",   (int) folder_stat.st_gid);//
    printf("   size:   %li\n",    (int) folder_stat.st_blksize);//
    printf("   blks:   %li\n",    (int) folder_stat.st_blocks);//
    printf("   mtime:   %li\n",    (int) folder_stat.st_mtime);//
    printf("   ctime:   %li\n",    (int) folder_stat.st_ctime);//
    */
   return written;
}

int MakeSnap(char* pathname)
{
    
}

int main(int argc, char* argv[])
{

    



    /*
    char* pathname=argv[1];
    
    int fin=open(pathname, O_RDONLY);
    if(fin<0) 
    {
        perror("  could not open the folder");
        return -1;
    }

    struct stat folder_stat;
    
    if(stat(pathname, &folder_stat)<0)
    {
        close(fin);
        perror("stat crashed");
        return -2;
    }

    int fout=open("Snap.txt", O_RDWR | O_CREAT);
    if(fout<0)
    {
        perror(" could not create the Snap.txt file");
        return -3;
    }

   ssize_t written=WriteInSnap(folder_stat, fout);
    if(written<0) return written;
    
    

    close(fout);
    close(fin);*/

    return 0;
}
