/*
 * Menu.cpp
 *
 *  Created on: Jun 7, 2022
 *      Author: mark_
 */

#include "../headers/Menu.h"

/**
 * @brief Construct a new Menu:: Menu object
 * 
 */
Menu::Menu() {
	
}

/**
 * @brief Destroy the Menu:: Menu object
 * 
 */
Menu::~Menu() {
	
}

/**
 * @brief 
 * 
 * @return char* 
 */
char* Menu::getDATABASEHOSTNAME() {
	return this->dbasehost;
}

/**
 * @brief 
 * 
 * @return char* 
 */
char* Menu::getMACHINENAME() {
	return this->mname;
}

/**
 * @brief 
 * 
 * @return char* 
 */
char* Menu::getPASSWORD() {
	return this->pwd;
}

/**
 * @brief 
 * 
 * @return char* 
 */
char* Menu::getUSERNAME() {
	return this->uname;
}

/**
 * @brief 
 * 
 * @return char* 
 */
char* Menu::getPORT() {
	return this->portno;
}

/**
 * @brief 
 * 
 * @return char* 
 */
char* Menu::getPARENTDIR() {
	return this->parentdir;
}

/**
 * @brief 
 * 
 * @return char* 
 */
char* Menu::getDBASETYPE() {
	return this->dbasetype;
}

/**
 * @brief 
 * 
 * @return char* 
 */
char* Menu::getDBASEFILENAME() {
	return this->dbasefilename;
}

/**
 * @brief 
 * 
 * @return int 
 */
int Menu::getPROCESSHASHVALS() {
	return this->processhashvals;
}

char* Menu::getDBASENAME() {
	return this->dbasename;
}

/**
 * @brief 
 * 
 */
void Menu::PrintMenu() {
	
	cout << "Usage: fileintegritychecker [-dir -dbasetype -machine] optional: [-dbasefilename -host -port -user -pwd]" << endl;
	cout << "\t-dir The directory to start the search" << endl;
	cout << "\t-dbasetype Either mysql or sqlite." << endl; 
	cout << "\t\tIf mysql the -host, -port, -user, and -pwd are required." << endl;
	cout << "\t\tIf sqlite the -dbasefilename is required." << endl;
    cout << "\t-machine Name of machine that is being searched." << endl;
    cout << endl;        
	cout << "Optional:" << endl;
	cout << "\t-usehashvals Calculate hashedvalues to detect changes. NOTE: This will significantly increase processing time. Default is set to not use hashedvalues." << endl;
	cout << "\t-dbasefilename Location and Database file name for the SQLite3 file." << endl;
	cout << "\t-host MySQL server name or IP." << endl;
    cout << "\t-port Port number for the MySQL server (Most MySQL servers default to 3306)." << endl;
    cout << "\t-user Username for the MySQL server." << endl;
	cout << "\t-pwd  Password of the MySQL server user." << endl;
    cout << endl;
	cout << "Example:" << endl;
	cout << "\tFor mysql dbasetype -" << endl;
	cout << "\t\tfileintegritychecker -dir /home -dbasetype mysql -machine MyMachineName -host 127.0.0.1 -port 3306 -user user -pwd password" << endl;
	cout << endl;
	cout << "\tFor sqlite dbasetype -" << endl;
	cout << "\t\tfileintegritychecker -dir /home -dbasetype sqlite -machine MyMachineName -dbasefilename dirname/dbasefilename.db" << endl;

}

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int Menu::ParseArguments(int argc, char* argv[]) {
	int retVal = 0;
	int dbaseset = -1;

	for( int argumentcount = 1; argumentcount < argc; argumentcount++ ) {

		if(strcmp(argv[argumentcount],"-dir") == 0) {
			this->parentdir = new char[strlen(argv[argumentcount+1])];
			this->parentdir = argv[argumentcount+1];
		}
		if(strcmp(argv[argumentcount],"-host") == 0) {
			this->dbasehost = new char[strlen(argv[argumentcount+1])];
			this->dbasehost = argv[argumentcount+1];
			cout << "host is: " << this->dbasehost << endl;
		}
		if(strcmp(argv[argumentcount],"-username") == 0) {
			this->uname = new char[strlen(argv[argumentcount+1])];
			this->uname = argv[argumentcount+1];
			cout << "username is set." << endl;
		}
		if(strcmp(argv[argumentcount],"-password") == 0) {
			this->pwd = (char*)malloc(sizeof(char)*strlen(argv[argumentcount+1]));
			this->pwd = new char[strlen(argv[argumentcount+1])];
			this->pwd = argv[argumentcount+1];
			cout << "username is set." << endl;
		}
		if(strcmp(argv[argumentcount],"-port") == 0) {
			this->portno = new char[strlen(argv[argumentcount+1])];
			this->portno = argv[argumentcount+1];
		}
		if(strcmp(argv[argumentcount],"-machine") == 0) {
			this->mname = new char[strlen(argv[argumentcount+1])];
			this->mname = argv[argumentcount+1];
		}
		if(strcmp(argv[argumentcount],"-dbasetype") == 0) {
			this->dbasetype = (char*)malloc(sizeof(char)*strlen(argv[argumentcount+1]));
			this->dbasetype = new char[strlen(argv[argumentcount+1])];
			this->dbasetype = argv[argumentcount+1];
			cout << "dbasetype is: " << this->dbasetype << endl;
		}
		if (strcmp(argv[argumentcount], "-dbasefilename") == 0) {
			this->dbasefilename = (char*)malloc(sizeof(char) * strlen(argv[argumentcount + 1]));
			this->dbasefilename = new char[strlen(argv[argumentcount + 1])];
			this->dbasefilename = argv[argumentcount + 1];
			cout << "dbasefilename is: " << this->dbasefilename << endl;
		}
		if (strcmp(argv[argumentcount], "-dbasename") == 0) {
			this->dbasename = (char*)malloc(sizeof(char) * strlen(argv[argumentcount + 1]));
			this->dbasename = new char[strlen(argv[argumentcount + 1])];
			this->dbasename = argv[argumentcount + 1];
			cout << "dbasename is: " << this->dbasename << endl;
		}
		if(strcmp(argv[argumentcount],"-usehashvals") == 0) {
			this->processhashvals = 1;
		}
	}

	if(strcmp(this->dbasetype,"mysql") == 0) {
		cout << "MySQL database set." << endl;
		if(this->dbasehost == NULL || this->portno == NULL || this->uname == NULL || this->pwd == NULL) {
			cout << "Missing either host, port, username, or passoword for the mysql database type. Please try again" << endl;
			retVal = -1;
		}
		dbaseset = 0;

	}

	if(strcmp(this->dbasetype,"sqlite") == 0) {
		cout << "Sqlite3 database set." << endl;
		if(this->dbasefilename == NULL) {
			cout << "Missing dbasefilename. Please try again" << endl;
			retVal = -1;
		}
		dbaseset = 0;
	}

	if(dbaseset == -1)
	{
		retVal = dbaseset;
	}
	
	return retVal;
}

