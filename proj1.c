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
    // writting from the stat of each reg file into the sanp

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

   return written;
}

int compareSnaps(int fd, char* origFile)
{
    // comparing with the old snap(orig stat) the current state of the file

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

    long int* libuf;
    read(fd, libuf, sizeof(long int));
    if(*libuf!=orig_stat.st_blksize) return -7;

    read(fd, libuf,sizeof(long int));
    if(*libuf!=orig_stat.st_blocks) return -8;

    read(fd, libuf,sizeof(long int));
    if(*libuf!=orig_stat.st_mtime) return -9;
    
    read(fd, libuf,sizeof(long int));
    if(*libuf!=orig_stat.st_ctime) return -10;
    

    return 0;
}

int CheckDirOrFile(const char* path)
{
    // this checks if the files in the directories are regular files 1 or another directory 2(then we need to use iterate dir)

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
    // Making the SNAP 

    struct stat folder_stat;

    if(stat(pathname, &folder_stat)<0)
    {
        perror("stat crashed");
        return -2;
    }

    // making the name of Snap  "SNAP_FILENAME.txt", append another .txt to it so if file is idk.txt we get SNAP_idk.txt.txt
    char SnapName[70]="SNAP_";
    strcat(SnapName, strrchr(pathname, '/')+1); //copy FROM the last "/", basically just the name of the file
    strcat(SnapName, ".txt"); //add .txt
    
    char newPath[100]=""; // here i make the path of the snap(new path)
    
    strncpy(newPath,pathname,abs(strlen(pathname)-strlen(SnapName)+5+4)); //so i copy the old path - then new name and add back 5+4 (SNAP_)+(.txt), now that i write the comments i think i could have just used strrchr again till "/", well whatever...
    strcat(newPath,SnapName);                                             // adding the name

    
   // printf("%s\n", pathname);
   // printf("%s\n", SnapName);
    printf("created : %s\n\n", newPath);
    

    int fout=open(newPath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR); // create and open the SNAP
    if(fout<0)
    {
        perror(" could not create the Snap.txt file");
        return -3;
    }
    
    WriteInSnap(folder_stat, fout); // Write in the Snap the stats
    
    return 0;
}

int iterate_dir(char* pathname)
{
    // iterate the dir and checking the corupt files (no permissions)(didnt make another function, probably should have)
    struct dirent* dirent_pointer;
    DIR* dir;
    if((dir=opendir(pathname))==NULL)
    {
        perror("could not open the directory\n");
        return -1;
    }

    // the pipe that communicates with the child that checks the permisions (execlp script.sh)
    int pfd[2];

    if(pipe(pfd)<0)
    {
        perror("pipe creation failed\n");
        exit(-15);
    }

    int pid=fork();
    if(pid<0)
    {
        perror("child creation error\n");
        exit(-16);
    }
    if(pid==0) // childe process that checks the permissions (if ---------- grep the file name(from right to left everything till we get a space)) 
    {
        // i check all the files in the directory with ls -l and grep only the ones with permisions ---------

        close(pfd[0]);
        dup2(pfd[1], 1);
        execlp("./script.sh", "script", pathname, NULL);
        printf("exec error\n");
        exit(-17);
    }

    close(pfd[1]);

    char* cor_buff =(char*)malloc(sizeof(char));
    char* cor_name=(char*)malloc(sizeof(char)*50);
    /*
    char cor_buff[1];
    char cor_name[50];
    */


    
    int idk=0;

    ssize_t readCounter;
    ssize_t lastread;

    // read byte by byte as i get from the script and when i get \n i copy the corrupt name(cor_name)

    while((lastread=read(pfd[0], cor_buff, 1) /*&& lastread!=0*/))
    {
        readCounter+=lastread;

        cor_name[idk++]=cor_buff[0];
        //printf("%s\n", cor_name);

        if(cor_name[idk-1]=='\n') //so if the last char in cor_name is /n it means that grep moved to the next line so i have to remake the path for the rename move into Quarantine directory
        {
            char cor_path[100]="";
            strcat(cor_path, pathname); // this is the dirctory like dir1/dir2/dir3 and so on
            strcat(cor_path, "/");

            char without_nspace[100];
            strncpy(without_nspace, cor_name, idk-1);

            char* space_ptr;
            char quar_path[80]="Quarantine/"; // making the quarantine path 
            if((space_ptr=strrchr(without_nspace, '\n'))==NULL) // this checks if i have 2 or more files to quarantine cuz if a have a space then i have to copy the name from the last space onwards
            {
                strcat(quar_path, without_nspace);  // no space so everything good just append
                strcat(cor_path, without_nspace);
            }
            else
            {
                strcat(quar_path, space_ptr+1);  // here i have to copy from the last space (i mean the \n that appears from grep)
                strcat(cor_path, space_ptr+1);
            }
            //printf("%s\n", quar_path);
            //printf("%s\n", cor_name);
            //printf("%s\n", cor_path);
           
            printf("moved to quarantine %s\n", cor_path);
            rename(cor_path, quar_path);  // moving into Quarantine
        }
       
        //printf("%s\n", pathname);
    }

    free(cor_buff);
    free(cor_name);
    
    close(pfd[0]);

    int status;
    wait(&status);


    // here i iterate the directory and check if the file is SNAP, regfile or a directory 

    while((dirent_pointer=readdir(dir))!=NULL)
    {

        char newPath[100];
        strcpy(newPath, pathname); 
        strcat(newPath,"/");
        char* d_name=dirent_pointer->d_name;
        
        if((strcmp(d_name,".")!=0) && (strcmp(d_name, "..")!=0) && (strncmp(d_name,"SNAP",4)!=0)) // do not process the ., .., or SNAP files
        {
            //printf("%s\n", d_name);
            strcat(newPath, d_name);

            int check=CheckDirOrFile(newPath);

            //printf("it dir new path=%s\n", newPath);
            if(check<=0) printf("not a reg file or directory");
            else if(check==1) // if reg file 
            {
                //printf("%s\n", newPath); 
            
                //  here i remake the snap format to check if there already exists a snap

                char checkSnap[100]="";
                strncpy(checkSnap,newPath, abs(strlen(newPath)-strlen(d_name)));
                strcat(checkSnap, "SNAP_");
                strcat(checkSnap, d_name);
                strcat(checkSnap, ".txt");
                //printf("\n%s \n", checkSnap);

                int compare=open(checkSnap, O_RDWR, S_IRUSR | S_IWUSR); // if exists igo and compare the snap with the current stats fo the file

                if(compare!=-1)
                {
                   int result = compareSnaps(compare, newPath);
                   if(result!=0)
                   {
                        printf("there was a change\n");
                        MakeSnap(newPath);
                   }
                }
                else // if no snap make one
                {
                    MakeSnap(newPath);
                }
                
            } 
            else
            { 
                if(check==2) // if a directory iterate it
                {
                    //printf("the nnew dir path is %s \n", newPath);
                    iterate_dir(newPath);
                }
            }      
        }
    }

    return 0;
}

int child_process(char* pathname)
{
    int result=(iterate_dir(pathname));
} 



int main(int argc, char* argv[])
{

    if(argc<=1)
    {
        perror("Usage: folder1_path folder2_path ... Quarantine");
        exit(0);
    }

    // from 1 to argc-1 we have the directories that need to be checked
    for(int i=1; i<argc-1; i++)
    {
        int pid=fork();
        if(pid<0)
        {
            perror("the child was not born");
            exit(-13);
        }
        if(pid==0) // a child for each folder 
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

    // getting the status of each child
    for(int i=1; i<argc-1; i++)
    {
        int status;
        wait(&status);
    }
    
    return 0;
}
