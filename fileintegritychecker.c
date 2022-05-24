/*
http://staff.washington.edu/gmobus/Academics/TCES202/Moodle/OO-ProgrammingInC.html
https://www.tutorialspoint.com/data_structures_algorithms/linked_list_program_in_c.htm

*/

#include "headers/fileintegritychecker.h"

int main(int argc, char* argv[] ){
   printf("Directory name to search: %s\n",argv[1]);
   //DIR *dir = opendir(argv[1]);

   if (argv[1] == NULL){
      printf("Directory must be provided." );
      return 0;
   }
   
   hashfiles(argv[1]);

   return 0;
}

int isDir(char* filename) {
   struct stat path;
   stat(filename, &path);
   return S_ISREG(path.st_mode);
}

void hashfiles(char* startdirectory) {
   struct dirent *dir;
   DIR *d;

   d = opendir(startdirectory);

   while( (dir = readdir(d)) != NULL )
   {
      struct stat buf;
      //stat(files->d_name,&filestat);
      //if ((files->d_name, &sb) == -1) {
      //         perror("lstat");
      //         exit(1);
      //}

      /*printf("ID of containing device:  [%jx,%jx]\n\n",
                   (uintmax_t) major(sb.st_dev),
                   (uintmax_t) minor(sb.st_dev));

      switch (sb.st_mode & S_IFMT) {
	case S_IFDIR: printf("Directory %s\n",files->d_name); break;
        case S_IFREG: printf("\tFile %s\n",files->d_name); break;
        default: printf("%s",files->d_name); break;
      }*/
      int ret = isDir(dir->d_name);

      printf("Return value for %s is %d\n\n",dir->d_name,ret);

      if(dir->d_type == DT_DIR) {
         //if(checkifrootorparent(files->d_name) == 0 ) {
	 printf("Found a directory %s. Moving on....\n\n",dir->d_name);
         //}
      }
      else {   
         FILE *file;
         file = fopen(dir->d_name, "r");
         HashedValuesPtr hashedvalue = (HashedValuesPtr) malloc (sizeof(HashedValues));
         hashedvalue = calculateHash(file);

         printf("HASHING %s\n",dir->d_name);
          // Print the results
         printf("\t\n=================== HASH OUTPUT from fileintegritychecker.c ======\n\n");
         printf("\t%08llx", hashedvalue->HASHVALUES[0]);
         printf("\t%08llx", hashedvalue->HASHVALUES[1]);
         printf("\t%08llx", hashedvalue->HASHVALUES[2]);
         printf("\t%08llx", hashedvalue->HASHVALUES[3]);
         printf("\t%08llx", hashedvalue->HASHVALUES[4]);
         printf("\t%08llx", hashedvalue->HASHVALUES[5]);
         printf("\t%08llx", hashedvalue->HASHVALUES[6]);
         printf("\t%08llx", hashedvalue->HASHVALUES[7]);
         
         printf("\t\n\n==================================================================\n\n");

	free(hashedvalue);
        //fclose(file);

      }
      /*stat(files->d_name,&filestat);
      if( S_ISDIR(filestat.st_mode) ) {
         if(checkifrootorparent(files->d_name) == 0 ) {
            DIR *newdir = opendir(files->d_name);
            printf("%4s: %s\n","Dir",files->d_name);
            hashfiles(newdir);
         }
      }
      else
              printf("%4s: %s\n","File",files->d_name);
      */
   }

   closedir(d);

}

int checkifrootorparent(char* dirname) {

    int retVal = 0;

    if(checkifrootdir(dirname) == 0) {
       retVal = 1;
    }
    else if(checkifparentdir(dirname) == 0) {
       retVal = 1;
    }
    else {
       retVal = 0;
    }

    return retVal;
}

int checkifrootdir(char* dirname) {
    return strcmp(dirname,".");
}

int checkifparentdir(char* dirname) {
   return strcmp(dirname,"..");
}
