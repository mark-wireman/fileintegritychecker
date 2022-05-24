/*
http://staff.washington.edu/gmobus/Academics/TCES202/Moodle/OO-ProgrammingInC.html
https://www.tutorialspoint.com/data_structures_algorithms/linked_list_program_in_c.htm

*/

#include "headers/fileintegritychecker.h"

int main(int argc, char* argv[] ){
   printf("Directory name to search: %s\n",argv[1]);
   DIR *dir = opendir(argv[1]);

   if (dir == NULL){
      printf("Directory cannot be opened!" );
      return 0;
   }
   
   hashfiles(dir);

   return 0;
}

void hashfiles(DIR *currentlocation) {
   struct dirent *files;
   struct stat filestat;
   char* dirname;

   while( files=readdir(currentlocation) )
   {
      stat(files->d_name,&filestat);
      //if( S_ISDIR(filestat.st_mode) ) {
      if(checkifrootorparent(files->d_name) != 0 ) {   
         FILE *file;
         file = fopen(files->d_name, "r");
         HashedValuesPtr hashedvalue = (HashedValuesPtr) malloc (sizeof(HashedValues));
         hashedvalue = calculateHash(file);

          // Print the results
         printf("\n=================== HASH OUTPUT from fileintegritychecker.c ======\n\n");
         printf("%08llx", hashedvalue->HASHVALUES[0]);
         printf("%08llx", hashedvalue->HASHVALUES[1]);
         printf("%08llx", hashedvalue->HASHVALUES[2]);
         printf("%08llx", hashedvalue->HASHVALUES[3]);
         printf("%08llx", hashedvalue->HASHVALUES[4]);
         printf("%08llx", hashedvalue->HASHVALUES[5]);
         printf("%08llx", hashedvalue->HASHVALUES[6]);
         printf("%08llx", hashedvalue->HASHVALUES[7]);
         
         printf("\n\n==================================================================\n\n");

	free(hashedvalue);

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

   closedir(currentlocation);

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
