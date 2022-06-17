#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <dirent.h>
#include <sys/types.h>
#include "../headers/sha256.h"
#include "../headers/sqlite3crud.h"
#include "../headers/Menu.h"

using namespace std;

// Tell our preprocessor to create a variable MAXCHAR with value of 100000
#define MAXCHAR 100000
sqlite3crud sql3 = sqlite3crud();

// Simple function that calcuates the size of a file
int calcFileSize(FILE *file)
{
    int prev=ftell(file);
    fseek(file, 0L, SEEK_END);
    int size=ftell(file);
    fseek(file,prev,SEEK_SET); 
    return size;
}

// This function is used to read the contents of the file and return them as an array of chars
std::string getFileContents(FILE *fileForPrinting)
{
    // Variables
    char fileContents[MAXCHAR];
    char fileContentsAsString[MAXCHAR];
    long fileSize;
    std::string retVal = "";

    // First check to make sure the file could be found
    if (fileForPrinting == NULL){
        retVal = "Could not open file";
    }
    else
    {
        // Calculate the size of the file
        fileSize = calcFileSize(fileForPrinting);

        // While there is still stuff to read from the file
        while(fgets(fileContents, MAXCHAR, fileForPrinting) != NULL)
        {
            // Print the contents of the file
            retVal+=fileContents;
            //printf(" %s\n", fileContents);
        };
        
        fclose(fileForPrinting);

        // Close the file 
        return retVal;
    }
    
}

void traverseDirectoryTree(DIR* parentdir, const char* dirpath) {

	DIR *dr;
    struct dirent *en;
    //cout<<"\nInside traversDirectoryTree...";
	if (parentdir) {
	  //cout<<"\nInside if(parentdir)...";
      while ((en = readdir(parentdir)) != NULL) {
         //cout<<"\nInside while....";
      	 if(en->d_type == DT_DIR && strcmp(en->d_name,".") != 0 && strcmp(en->d_name,"..") != 0) {
      	    std::string dirname = dirpath;
      	    dirname+= en->d_name;
      	    dirname+="/";
     		cout<<" \nFound Directory: "<<dirname<<endl; //print directory name
     		dr = opendir(dirname.c_str());
     		sql3.saveDirectoryName(dirname);  		
     		traverseDirectoryTree(dr,dirname.c_str());
         }
      }
	}
}

int main(int argc, char *argv[]) {
   DIR *dr;
   struct dirent *en;
   char* parentdir;
   char* dbase;
   Menu m;
   cout << "Number of arguments: " << argc << "\n";
   if(argc >= 5) {
	   m.ParseArguments(argc, argv);
	   parentdir = m.getParentDir();
	   dbase = m.getDBaseInfo();
   	//parentdir = argv[1];
   	cout<<"Starting directory: "<<parentdir<<"\n";
   } else {
	   m.PrintMenu();
	   return 0;
	   //throw std::invalid_argument("Arguments missing: Must include a directory and database argument.");
   }
   
   /*dr = opendir(parentdir); //open all directory
   if (dr) {
      while ((en = readdir(dr)) != NULL) {
      	 if(en->d_type == DT_DIR) {
     		cout<<" \nFound Directory: "<<en->d_name; //print directory name
         }
         else {
         	std::string filename = parentdir;
         	filename+="/";
         	filename+=en->d_name;
         	cout<<" \n Found File: "<<en->d_name; //print file name
         	std::string filecontents;
         	FILE *file;
         	file = fopen(filename.c_str(), "r");
         	filecontents = getFileContents(file);
         	//cout<<"\n\t\tFile Contents: "<<filecontents;
         	std::string output1 = sha256(filecontents);
         	cout<<"\n\t\tHashed value: "<<output1;
         }
      }
      
      closedir(dr); //close all directory
      
   }
   else {
   	cout<<"Unable to open directory.";
   	throw std::invalid_argument("Unable to open directory");
   }*/
   
   
   sql3.initdb(dbase);
   dr = opendir(parentdir); //open all directory
   std::string pDir = parentdir;
   pDir+="/";
   traverseDirectoryTree(dr, pDir.c_str());
   sql3.closedb();
   
   return(0);
}


