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
#include "MySQLController.h"
#include "SQLite3Controller.h"
//#include "SQLiteHelper.h"
//#include "SQLHelper.h"

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
	char* dbasename;
	int usehashvals = 0;
	long int numberofnewdirectories = 0;
	long int numberofverifieddirectories = 0;
	
	bool verbose_output = false;
	MySQLController* mysqlctl;
	SQLite3Controller* sqlitectl;
	void get_directories_files(const string& s, int level = 0);
};
