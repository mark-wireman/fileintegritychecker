#include <iostream>
#include <cstdint>
#include <fstream>
#include <filesystem>
#include <string>
#include <cstring>
#include <thread>
#include <vector>
#include <dirent.h>
#include <sys/types.h>
#include <future>
#include <chrono>
#include "SHA256CPP.h"
#include "Menu.h"
#include "mysqlcontroller.h"
#include "sqlite3controller.h"
#include "SQLiteHelper.h"

class fileintegritychecker {
public:
	fileintegritychecker();
	virtual ~fileintegritychecker();
	void setParamVals(Menu* m);
	void setDatabaseConnections();
	void processFiles();
	void closeDatabaseConnections();
	long int numberofdirectories = 0;
	long int numberoffiles = 0;

private:
	char* parentdir;
	char* dbasehost;
	char* dbase;
	char* uname;
	char* pwd;
	char* portno;
	char* mname;
	char* dbasetype;
	char* dbasefilename;
	int usehashvals = 0;
	long int numberofnewdirectories = 0;
	long int numberofverifieddirectories = 0;
	
	bool verbose_output = false;
	mysqlcontroller* mysqlctl;
	sqlite3controller* sqlitectl;
	void get_directories_files(const string& s, int level = 0);
};
