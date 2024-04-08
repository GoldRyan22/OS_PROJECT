
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

ssize_t WriteInSnap(struct stat folder_stat, int fout)
{

    ssize_t written= write(fout, &folder_stat.st_ino, sizeof(long int)); // inode
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

int CheckDirOrFile(const char* path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    if(S_ISREG(path_stat.st_mode)) return 1; // return 1 if regular file
    else
    {
        if(S_ISDIR(path.st_mode)) return 2;
        else return -1;
    }
}

int MakeSnap(char* pathname)
{
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

    char* SnapName=malloc(sizeof(pathname)+5);
    

    //int fout=open("Snap.txt", O_RDWR | O_CREAT);
    int fout=open(strcat(SnapName, ".txt"), O_RDWR | O_CREAT);
    if(fout<0)
    {
        perror(" could not create the Snap.txt file");
        return -3;
    }

    WriteInSnap(folder_stat, fout);

    return 0;
}

int iterate_dir(char* pathname)
{
    struct dirent* dirent_pointer;
    DIR* dir;


    

    if((dir=opendir(dirent_pointer))==NULL)
    {
        perror("could not open the directory");
        return -1;
    }

    while((dirent_pointer=readdir(dir))!=NULL)
    {
        char* newPath;
        strcpy(newPath, pathname); 
        strcat(newPath,"/");
        strcat(newPath, dirent_pointer.d_name);

        int check=CheckDirOrFile(newPath);

        if(check<=0) printf("not a reg file or directory");
        else if(check==1) MakeSnap(newPath);
        else if(check==2) iterate_dir(newPath);

    }
}

int main(int argc, char* argv[])
{
    printf("Hello Word\n");
    
    char* pathname=argv[1];

    iterate_dir(pathname);



    
    
    /*
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
