//#include <sw/redis++/redis++.h>
#include <iostream>
#include <cstdint>
#include <fstream>
#include <filesystem>
#include <string>
#include <cstring>
#include <thread>
#include <vector>
#include <sys/types.h>
#include "../headers/sha256.h"
#include "../headers/Menu.h"
#include "../headers/mysqlcontroller.h"

using namespace std;
//using namespace sw::redis;
namespace fs = std::filesystem;

char* parentdir;
char* dbasehost;
char* dbase;
char* uname;
char* pwd;
char* portno;
char* mname;
long int numberofdirectories = 0;
long int numberofnewdirectories = 0;
long int numberofverifieddirectories = 0;

// Tell our preprocessor to create a variable MAXCHAR with value of 100000
#define MAXCHAR 100000
Menu *m;

/*std::unordered_map<string, string> record = {
    {"host", ""},
    {"status", ""},
    {"dateadded", ""},
    {"dateverified", ""}
};*/

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
string getFileContents(FILE *fileForPrinting)
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

char* getCurrentTime() {
	string retVal;
	char* char_array;

	std::time_t t = std::time(0);   // get time now
	std::tm* now = std::localtime(&t);
	char date_string[100];
	char time_string[100];

	strftime(date_string, 50, "%B %d, %Y ", now);
	strftime(time_string, 50, "%T", now);

	retVal += date_string;
	retVal += time_string;

	int n = retVal.length();
	char_array = new char[n + 1];

	strcpy(char_array, retVal.c_str());
	
	return char_array;
}

/*ConnectionOptions getRedisConnStr() {
    //char* retVal;
    ConnectionOptions connection_options;
    connection_options.host = dbasehost;  // Required.
    connection_options.port = stoi(portno); // Optional. The default port is 6379.
    
    //retVal = new char[6+strlen(dbasehost)+strlen(portno)];
    //strcat(retVal,"tcp://");
    //strcat(retVal,dbasehost);
    //strcat(retVal,":");
    //strcat(retVal,portno);

    if(pwd != NULL)
    {
        connection_options.password = pwd;   // Optional. No password by default.
        //strcat(retVal," -a ");
        //strcat(retVal,pwd);
    }

    return connection_options;
}*/


/*int entryExists(string dirname, Redis *redisdbase) {
    int retVal = 0;
    string hostval = "";
    string statusval = "";
    string dateaddedval = "";


    record.clear();
    redisdbase->hgetall(dirname, std::inserter(record, record.begin()));

    if(record.size() > 0) {
        hostval = record["host"];

        if(hostval == mname) {
            record["status"] = "VERIFIED";
            record["dateverified"] = getCurrentTime();

            numberofverifieddirectories++;
        }

        retVal = 1;
    } else {
       record["host"] = (string)mname;
       record["status"] = "NEW";
       record["dateadded"] = getCurrentTime();
       record["dateverified"] = ""; 
       numberofnewdirectories++;
    }

    return retVal;

}*/



void get_directories_savetocsv(const string& s) {
    char* fname = new char[strlen(mname) + 4];

    strcat(fname,mname);
    strcat(fname,".csv");

    ofstream directorycsv(fname);

    directorycsv << "dirname,dateadded,machinename" << endl;
    
    auto d_init = filesystem::recursive_directory_iterator(s,filesystem::directory_options::skip_permission_denied);
    auto d_end = filesystem::end(d_init);
    auto error_code = std::error_code();
    for(; d_init != d_end; d_init.increment(error_code)){
        if(!error_code){
            if(!d_init->is_symlink() && d_init->is_directory()){
                cout << "Directory: " + d_init->path().string() << endl;
                directorycsv << d_init->path().string() << ",\"" << getCurrentTime() << "\"," << mname << endl;
            }
        }
    }

    directorycsv.close();
}

/*void get_directories_hmset(const string& s) {
    auto d_init = filesystem::recursive_directory_iterator(s,filesystem::directory_options::skip_permission_denied);
    auto d_end = filesystem::end(d_init);
    auto error_code = std::error_code();
    auto redis = Redis(getRedisConnStr());

    cout << "After setting Redis object with connection string. " << endl;

    for(; d_init != d_end; d_init.increment(error_code)){
        if(!error_code){
            if(!d_init->is_symlink() && d_init->is_directory()){                

                cout << "Directory: " + d_init->path().string() << endl;
                
                string dirname = d_init->path().string();
                string dateverifiedval = "";
                
                entryExists(d_init->path().string(),&redis);
                
                string indexval = "dir:" + dirname;
                redis.hmset(indexval,record.begin(),record.end());

                numberofdirectories++;
            }
        }
    }
}*/

void get_directories_mysql(const string& s, mysqlcontroller* mysql) {
   auto d_init = filesystem::recursive_directory_iterator(s,filesystem::directory_options::skip_permission_denied);
    auto d_end = filesystem::end(d_init);
    auto error_code = std::error_code();
    
    for(; d_init != d_end; d_init.increment(error_code)){
        if(!error_code){
            if(!d_init->is_symlink() && d_init->is_directory()){ 
                string dirname = d_init->path().string();
                cout << "Saving directory: " + dirname << endl;
                mysql->initPreparedStatements();
                mysql->saveDirectoryName(dirname);
                mysql->closePreparedStatements();
                numberofdirectories++;
            }
        }
    }

}

void setParamVals() {
   parentdir = m->getPARENTDIR();
   dbasehost = m->getDATABASEHOSTNAME();
   uname = m->getUSERNAME();
   pwd = m->getPASSWORD();
   portno = m->getPORT();
   mname = m->getMACHINENAME();
}

int main(int argc, char *argv[]) {
   time_t start, end;
   double time_taken = 0.0;

   time(&start);

   m = new Menu();
   
   if(argc >= 8) {
      m->ParseArguments(argc, argv);
   } else {
	   m->PrintMenu();
	   return 0;
   }
    
    setParamVals();
    
    string pDir = parentdir;
    cout << pDir << endl;
    mysqlcontroller *mysqlctl = new mysqlcontroller(dbasehost,"fileintegritychecker",uname,pwd,3306,mname);
    mysqlctl->initdb();
    get_directories_mysql(pDir,mysqlctl);
    mysqlctl->closedb();
    //get_directories_hmset(pDir);

    time(&end);

    // Calculating total time taken by the program.
    time_taken = double(end - start);
    int n = time_taken;
    n = n % (24 * 3600);
    int hour = n / 3600;
    n %= 3600;
    int minutes = n / 60 ;
    n %= 60;
    int seconds = n;
    cout << "Time taken is: " << hour << " " << "hours " << minutes << " " << "minutes " << seconds << " " << "seconds " << endl;
    cout << "Total directories: " << numberofdirectories << " - " << "new directories = " << numberofnewdirectories << " " << "verified directories = " << numberofverifieddirectories << " ." << endl;

    return(0);
}


