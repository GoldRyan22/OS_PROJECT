#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char* argv[])
{
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
        return -3;
    }

    printf("  inode:   %d\n",   (int) folder_stat.st_ino);
    printf(" dev id:   %d\n",   (int) folder_stat.st_dev);
    printf("   mode:   %08x\n",       folder_stat.st_mode);
    printf("  links:   %li\n",         folder_stat.st_nlink);
    printf("    uid:   %d\n",   (int) folder_stat.st_uid);
    printf("    gid:   %d\n",   (int) folder_stat.st_gid);



    close(fin);

    return 0;
}