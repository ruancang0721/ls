#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include<dirent.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<getopt.h>
#include<pwd.h>
#include<grp.h>
#include<time.h>
#define MAX 1024
typedef struct{
  int a;
  int l;
  int R;
  int t;
  int r;
  int i;
  int s;
}Options;
typedef struct{
  char name[1024];
  char fpath[1024];
  struct stat st;
}filei;
int comparetime1(const void*a,const void*b)
{
  return ((filei*)a)->st.st_mtime-((filei*)b)->st.st_mtime;
}
int comparetime2(const void*a,const void*b)
{
  return ((filei*)b)->st.st_mtime-((filei*)a)->st.st_mtime;
}
int compareshunxu(const void *a,const void *b)
{
  return strcmp(((filei*)a)->name,((filei*)b)->name);
}
int comparenixu(const void *a,const void *b)
{
  return -compareshunxu(a,b);
}
void  modescore(char* str,mode_t mode)
{
  if(S_ISDIR(mode))
  {
   strcat(str,"d");
  }
  else if(S_ISLNK(mode))
  {
    strcat(str,"l");
  }
  else if(S_ISBLK(mode))
  {
    strcat(str,"b");
  }
  else if(S_ISCHR(mode))
  {
    strcat(str,"c");
  }
  else if(S_ISFIFO(mode))
  {
    strcat(str,"p");
  }
  else if(S_ISSOCK(mode))
  {
    strcat(str,"s");
  }
  else
  {
    strcat(str,"-");
  }
  if(mode&S_IRUSR)
  {
    strcat(str,"r");
  }
  else
  {
    strcat(str,"-");
  }
  if(mode&S_IWUSR)
  {
    strcat(str,"w");
  }
  else{
    strcat(str,"-");
  }
  if(mode&S_IXUSR)
  {
    strcat(str,"x");
  }
  else{
    strcat(str,"-");
  }
  if(mode&S_IRGRP)
  {
    strcat(str,"r");
  }
  else{
    strcat(str,"-");
  }
  if(mode&S_IWGRP)
  {
    strcat(str,"w");
  }
  else{
    strcat(str,"-");
  }
  if(mode&S_IXGRP)
  {
    strcat(str,"x");
  }
  else{
    strcat(str,"-");
  }
  if(mode&S_IROTH)
  {
    strcat(str,"r");
  }
  else{
    strcat(str,"-");
  }
  if(mode&S_IWOTH)
  {
    strcat(str,"w");
  }
  else{
    strcat(str,"-");
  }
  if(mode&S_IXOTH)
  {
    strcat(str,"x");
  }
  else{
    strcat(str,"-");
  }

}
void lsprint(filei *file,Options* opts)
{
  if(opts->i)
  {
    printf("%ld ",file->st.st_ino);
  }
  if(opts->s)
  {
    printf("%3ld",file->st.st_blocks);
  }
  if(opts->l)
  {
    char mode[11];
    mode[0]='\0';
    modescore(mode,file->st.st_mode);
    printf(" %s",mode);
    printf("%2ld ",file->st.st_nlink);
    struct passwd * UID =getpwuid(file->st.st_uid);
    if(UID==NULL)
    {
      return;
    }

    struct group * GID =getgrgid(file->st.st_gid);
    if(GID==NULL)
    {
      return;
    }
    printf(" %s %s",UID->pw_name,GID->gr_name);
    printf("%6ld",file->st.st_size);
  struct tm* timeinfo = localtime(&file->st.st_mtime);
    printf(" %2d月  %2d  %02d:%02d",timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour,timeinfo->tm_min);
     
  }
  printf(" %s",file->name);
      if(opts->l&&S_ISLNK(file->st.st_mode))
    {
      char link[1024];
      int len = readlink(file->name,link,1023);
         link[len] = '\0';
         printf(" -> %s",link);
    }
       
      printf("\n");
}
void ls(char* path,Options* opts,int re);
 void process(char * path,Options* opts,int re)
 {

   struct stat st;
   if(lstat(path,&st)==-1)
   {
    perror("stat error");
    return;
   }
if(S_ISDIR(st.st_mode))
{
  printf("%s:\n ",path);
    ls(path,opts,1);
}



 }

void ls(char* path,Options* opts,int re)
{
    DIR * dir = opendir(path);
       if(dir == NULL)
       {
        perror("opendir error");
        
         return;
       }
   
       struct dirent *ent;
        filei* files = NULL;
        int count =0;
        int alloc=10;
        long long totalblock=0;
        files = malloc(alloc*sizeof(filei));
        if(files == NULL)
        {
          perror("malloc error");
          closedir(dir);
          return;
        }
       while((ent = readdir(dir))!=NULL)
       {
        if((opts->a!=1)&&ent->d_name[0]=='.')
        {
          continue;
        }
        
         if(count>=alloc)
         {
          alloc+=10;
          files = realloc(files,alloc*sizeof(filei));
         }
         if(files == NULL)
         {
          perror("realloc error");
          free(files);
          closedir(dir);
          return;
         
         }
        int len = strlen(path);
        if(path[len-1]!='/')
        {
          snprintf(files[count].fpath,MAX,"%s/%s",path,ent->d_name);
        }
        else
        {
            snprintf(files[count].fpath,MAX,"%s%s",path,ent->d_name);
        }
        

        strncpy(files[count].name,ent->d_name,MAX);

           
        if(lstat(files[count].fpath,&files[count].st)==-1)
        {
          perror("lstat error");
          continue;
        }
         strcpy(files[count].name,ent->d_name);
 
  
        totalblock +=files[count].st.st_blocks;
        count++;
               }
      
       closedir(dir);
              if(opts->t)
       {
        if(opts->r)
        {qsort(files,count,sizeof(filei),comparetime1);
        }
        else{
          qsort(files,count,sizeof(filei),comparetime2);
        }
       }
       else
       {
        if(opts->r)
        {
          qsort(files,count,sizeof(filei),comparenixu);
        }
        else{
          qsort(files,count,sizeof(filei),compareshunxu);
        }
       }
       if(opts->l)
       {
        printf("total %lld\n",totalblock);
       }

       for(int i=0;i<count;i++)
       {
      
        lsprint(&files[i],opts);
        
       }
    if(opts->R)
    {
      for(int i=0;i<count;i++)
      {
        if(S_ISDIR(files[i].st.st_mode)&&(strcmp(files[i].name,".")!=0)&&(strcmp(files[i].name,"..")!=0))
          {
            process(files[i].fpath,opts,1);
          }

      }
    }
   free(files);

}
int main(int argc,char* argv[])
{
     Options options = {0};
     int opt;
     while((opt=getopt(argc,argv,"alRrtsi"))!=-1)
     {
      switch(opt)
      {
        case 'a':
        options.a=1;
        break;
        case 'l':
        options.l=1;
        break;
        case 'R':
        options.R=1;
        break;
        case 't':
        options.t=1;
        break;
        case 'r':
        options.r=1;
        break;
        case 'i':
        options.i=1;
        break;
        case 's':
        options.s=1;
        break;
        default:
        perror("weizhi error");
        return 1;

      }
     }
     int dirnum = 0;
     char **dir =NULL;
     if(optind<argc)
     {
          dirnum = argc -optind;
          dir = argv + optind;
     }
     if(dirnum == 0)
     {
        process(".",&options,0);
     }
     else{
      for(int i=0;i<dirnum;i++)
      {
        if(dirnum>1)
        {
          printf("%s:\n",dir[i]);

        }
        process(dir[i],&options,0);
        if(dirnum>1&&i!=dirnum-1)
        {
          printf("\n");
        }
      }
     }



    return 0;
}