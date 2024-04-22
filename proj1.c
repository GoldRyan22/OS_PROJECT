#include<stddef.h>
#include <stdio.h>
#include <stdlib.h>
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
    ssize_t written = write(fout, &folder_stat.st_ino, sizeof(int)); // inode
    if (written==-1) return -4;

    written = write(fout, &folder_stat.st_dev, sizeof(int)); // dev id
    if (written==-1) return -4;

    written =write(fout, &folder_stat.st_mode, sizeof(int)); //mode
    if (written==-1) return -4;

    written = write(fout, &folder_stat.st_nlink, sizeof(int)); //nlinks
    if (written==-1) return -4;

    written =write(fout, &folder_stat.st_uid, sizeof(int)); // uid
    if (written==-1) return -4;

    written = write(fout, &folder_stat.st_gid, sizeof(int)); //gid
    if (written==-1) return -4;

    written =write(fout, &folder_stat.st_blksize, sizeof(long int)); //blksize
    if (written==-1) return -4;

    written =write(fout, &folder_stat.st_blocks, sizeof(long int)); //nr_blks
    if (written==-1) return -4;

    written = write(fout, &folder_stat.st_mtime, sizeof(long int)); //mtime
    if (written==-1) return -4;

    written = write(fout, &folder_stat.st_ctime, sizeof(long int)); //ctime
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

int compareSnaps(int fd, char* origFile)
{
    struct stat orig_stat;
    stat(origFile, &orig_stat);

    int* buf;
    read(fd, buf, sizeof(int));
    if(*buf!=orig_stat.st_ino) return -1;

    read(fd, buf, sizeof(int));
    if(*buf!=orig_stat.st_dev) return -2;

    read(fd, buf, sizeof(int));
     if(*buf!=orig_stat.st_mode) return -3;

    read(fd, buf, sizeof(int));
    if(*buf!=orig_stat.st_nlink) return -4;

    read(fd, buf, sizeof(int));
    if(*buf!=orig_stat.st_uid) return -5;

    read(fd, buf, sizeof(int));
    if(*buf!=orig_stat.st_gid) return -6;

    return 0;
}
int CheckDirOrFile(const char* path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    if(S_ISREG(path_stat.st_mode)) return 1; // return 1 if regular file
    else
    {
        if(S_ISDIR(path_stat.st_mode)) return 2;
        else return -1;
    }
}

int MakeSnap(char* pathname)
{

    /*int fin=open(pathname, O_RDWR,  S_IRUSR | S_IWUSR );
    printf("file path=%s\n", pathname);

    if(fin<0) 
    {
        perror("  could not open the file");
        return -1;
    }
    */

    struct stat folder_stat;
   
    
    if(stat(pathname, &folder_stat)<0)
    {
        //close(fin);
        perror("stat crashed");
        return -2;
    }
    //printf("the pathname in mksnap is=%s\n", pathname);

    char SnapName[50]="SNAP_";
    strcat(SnapName, strchr(pathname, '/')+1);
    strcat(SnapName, ".txt");
    
    //int fout=open("Snap.txt", O_RDWR | O_CREAT);

    char newPath[100];
    strncpy(newPath,pathname,abs(strlen(pathname)-strlen(SnapName))+1);
    strcat(newPath,SnapName);

    /*
    printf("%s\n", newPath);
    printf("%s\n", pathname);
    printf("%s\n", SnapName);
    */

    
    int fout=open(newPath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
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
    if((dir=opendir(pathname))==NULL)
    {
        perror("could not open the directory");
        return -1;
    }

    while((dirent_pointer=readdir(dir))!=NULL)
    {
        char newPath[100];
        strcpy(newPath, pathname); 
        strcat(newPath,"/");
        char* d_name=dirent_pointer->d_name;
        
        if((strcmp(d_name,".")!=0) && (strcmp(d_name, "..")!=0) && (strncmp(d_name,"SNAP",4)!=0))
        {
            //printf("%s\n", d_name);
            strcat(newPath, d_name);

            int check=CheckDirOrFile(newPath);
            //printf("it dir new path=%s\n", newPath);
            if(check<=0) printf("not a reg file or directory");
            else if(check==1)
            {

                //printf("%s", newPath);
                char checkSnap[100]="";
                strncpy(checkSnap,newPath, abs(strlen(newPath)-strlen(d_name)));
                strcat(checkSnap, "SNAP_");
                strcat(checkSnap, d_name);
                strcat(checkSnap, ".txt");
                //printf("\n%s \n", checkSnap);

                int compare=open(checkSnap, O_RDWR, S_IRUSR | S_IWUSR);

                if(compare!=-1)
                {
                   int result = compareSnaps(compare, newPath);
                   if(result!=0)
                   {
                        printf("there was a change\n");
                        MakeSnap(newPath);
                   }
                }
                else
                {
                    MakeSnap(newPath);
                }
                
            } //else if(check==2) iterate_dir(newPath);
        }
    }

    return 0;
}


int child_process(char* pathname)
{
    int result=(iterate_dir(pathname));
    if(result==0)
       return 0;
    else 
        return -10;
}

int main(int argc, char* argv[])
{


    //printf(" rgc==%d\n", argc);
    if(argc<=1)
    {
        perror("Usage: folder1_path folder2_path ...");
        exit(0);
    }
    
    char* pathname=argv[1];
    //printf("%d path=%s\n", argc, argv[1]);
    //printf("i dont know");
    if(argc==2)
    {
        
        
        iterate_dir(pathname);
        return 0;
    }
        
    for(int i=1; i<argc; i++)
    {
        int pid=fork();
        if(pid<0)
        {
            perror("the child was not born");
            exit(-13);
        }
        if(pid==0)
        {
            int child_return=child_process(argv[i]);
            if(child_return==0) 
                exit(0);
            else
            {
                perror("child error");
                exit(child_return);
            }
        }
    }
    

    for(int i=1; i<argc; i++)
    {
        int status;
        wait(&status);
        //printf(" status = %i \n",WEXITSTATUS(status));
    }
    
    return 0;
}
