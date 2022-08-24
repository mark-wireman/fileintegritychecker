#include <iostream>
#include <cstdint>
#include <fstream>
#include <filesystem>
#include <string>
#include <cstring>
#include <thread>
#include <dirent.h>
#include <sys/types.h>
#include "../headers/sha256.h"
#include "../headers/mysqlcontroller.h"
#include "../headers/Menu.h"

using namespace std;
namespace fs = std::filesystem;

class dbase_thread_obj {
public:
    void operator()(char* dbasesettings[6], const char* dirname)
    {

        char* dbasehost = dbasesettings[0];
        char* dbasename = dbasesettings[1];
        char* dbaseuser = dbasesettings[2];
        char* dbasepwd = dbasesettings[3];
        int dbaseport = atoi(dbasesettings[4]);
        char* machine = dbasesettings[5];

        cout << "Current directory value inside thread: " << dirname << endl;

        mysqlcontroller *mysqlctlthread;
        mysqlctlthread = new mysqlcontroller(dbasehost,dbasename,dbaseuser,dbasepwd,dbaseport,machine);
        mysqlctlthread->initdb();
        mysqlctlthread->initPreparedStatements();
        mysqlctlthread->saveDirectoryName(dirname);
        mysqlctlthread->closePreparedStatements();
        mysqlctlthread->closedb();
    }
};

DIR *dr;
struct dirent *en;
char* parentdir;
char* dbasehost;
char* dbase;
char* uname;
char* pwd;
int portno;
char* mname;

// Tell our preprocessor to create a variable MAXCHAR with value of 100000
#define MAXCHAR 100000
//sqlite3crud sql3 = sqlite3crud();
mysqlcontroller *mysqlctl;
Menu *m;

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

void traverseDirectoryTree(DIR* parentdir, const char* dirpath) {

	DIR *dr;
    struct dirent *en;
    //cout<<"\nInside traversDirectoryTree...";
	if (parentdir) {
	  //cout<<"\nInside if(parentdir)...";
      while ((en = readdir(parentdir)) != NULL) {
         //cout<<"\nInside while....";
         cout << "\t\t" << en->d_name << endl;
      	 //if(en->d_type == DT_DIR && strcmp(en->d_name,".") != 0 && strcmp(en->d_name,"..") != 0) {
        if(en->d_type == DT_DIR) {
            if(strcmp(en->d_name,".") != 0 && strcmp(en->d_name,"..") != 0) {
                std::string dirname = dirpath;
                dirname+= en->d_name;
                dirname+="/";
                cout<<" \nFound Directory: "<<dirname<<endl; //print directory name
                dr = opendir(dirname.c_str());
                mysqlctl->initPreparedStatements();
                mysqlctl->saveDirectoryName(dirname);
                mysqlctl->closePreparedStatements();  		
                traverseDirectoryTree(dr,dirname.c_str());
                //dirname = "";
            }
         }
         
      }
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

void get_directories(const string& s) {
    auto d_init = filesystem::recursive_directory_iterator(s,filesystem::directory_options::skip_permission_denied);
    auto d_end = filesystem::end(d_init);
    auto error_code = std::error_code();
    for(; d_init != d_end; d_init.increment(error_code)){
        if(!error_code){
            if(!d_init->is_symlink() && d_init->is_directory()){
                cout << "Directory: " + d_init->path().string() << endl;
                //const char* currentdir = d_init->path().string().c_str();
                //cout << "Current directory value before thread: " << currentdir << endl;
                //char* dbaseinfo[6] = {dbasehost,dbase,uname,pwd,const_cast<char*>(to_string(portno).c_str()),mname};
                
                //thread mysql_thread(dbase_thread_obj(), dbaseinfo, currentdir);
                //if (mysql_thread.joinable()) {
                //    mysql_thread.join();
                //}
                mysqlctl->initPreparedStatements();
                mysqlctl->saveDirectoryName(d_init->path().string().c_str());
                mysqlctl->closePreparedStatements();
            }
        }
    }
}

/*std::vector<std::string> get_directories(const std::string& s)
{
    cout << "Directory passed in is " << s << endl;
     
    std::vector<std::string> r;
    for(auto& p : fs::recursive_directory_iterator(s, fs::directory_options::skip_permission_denied)) {
        if (p.is_directory()) {
            r.push_back(p.path().string());
            cout << p.path().string() << endl;
        }
    }
    
    return r;
}*/

void setParamVals() {
   parentdir = m->getPARENTDIR();
   dbasehost = m->getDATABASEHOSTNAME();
   dbase = m->getDATABASENAME();
   uname = m->getUSERNAME();
   pwd = m->getPASSWORD();
   portno = m->getPORT();
   mname = m->getMACHINENAME();
}

void setDBaseConnection() {
   mysqlctl = new mysqlcontroller();
    mysqlctl->setHOSTNAME(dbasehost);
    mysqlctl->setDBASENAME(dbase);
    mysqlctl->setPORT(portno);
    mysqlctl->setUSERNAME(uname);
    mysqlctl->setPASSWORD(pwd);
    mysqlctl->setMACHINENAME(mname);
    cout << "fileintegritychecker::Before call to initdb()\n";
    mysqlctl->initdb();
    cout << "fileintegritychecker::After call to initdb()\n";
}

int main(int argc, char *argv[]) {
   
   m = new Menu();
   cout << "Number of arguments: " << argc << "\n";
   if(argc >= 15) {
        cout << "fileintegritychecker::Before calling ParseArguments.\n";
	   m->ParseArguments(argc, argv);
       cout << "fileintegritychecker::After calling ParseArguments.\n";
   } else {
	   m->PrintMenu();
	   return 0;
   }
   
   cout << "Before calling get_directories.\n";
    
    setParamVals();
    setDBaseConnection();
    
    
    
    string pDir = parentdir;
    cout << pDir << endl;
    
    get_directories_savetocsv(pDir);
    //get_directories(pDir);

    //dr = opendir(parentdir); //open all directory
    //string pDir = parentdir;
    //pDir+="/";
    //traverseDirectoryTree(dr, pDir.c_str());
    //mysqlctl->closedb();
   
   return(0);
}


