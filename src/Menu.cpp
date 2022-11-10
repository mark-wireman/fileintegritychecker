/*
 * Menu.cpp
 *
 *  Created on: Jun 7, 2022
 *      Author: mark_
 */

#include "../headers/Menu.h"

Menu::Menu() {
	// TODO Auto-generated constructor stub

}

Menu::~Menu() {
	// TODO Auto-generated destructor stub
}

char* Menu::getDATABASEHOSTNAME() {
	return this->dbasehost;
}

char* Menu::getMACHINENAME() {
	return this->mname;
}

char* Menu::getPASSWORD() {
	return this->pwd;
}

char* Menu::getUSERNAME() {
	return this->uname;
}

char* Menu::getPORT() {
	return this->portno;
}

char* Menu::getPARENTDIR() {
	return this->parentdir;
}

void Menu::PrintMenu() {
	char menutext[] = R"(
    
    Usage: fileintegritychecker [-dir -host -port -machine] optional: [-user -pwd]

            -dir The directory to start the search
            -host Redis server name or IP
            -port Port number for the Redis server (Most Redis servers default to 6379)
            -machine Name of machine that is being searched
            
	Optional:

        	-user Username for the Redis server
		-pwd  Password of the Redis server user
    
	Example:
		fileintegritychecker -dir /home -host 127.0.0.1 -port 6379 -machine MyServerName

    )";
	
	cout << menutext;
}


void Menu::ParseArguments(int argc, char* argv[]) {
		
	for( int argumentcount = 1; argumentcount < argc; argumentcount++ ) {

		if(strcmp(argv[argumentcount],"-dir") == 0) {
			this->parentdir = new char[strlen(argv[argumentcount+1])];
			this->parentdir = argv[argumentcount+1];
		}
		if(strcmp(argv[argumentcount],"-host") == 0) {
			this->dbasehost = new char[strlen(argv[argumentcount+1])];
			this->dbasehost = argv[argumentcount+1];
		}
		if(strcmp(argv[argumentcount],"-username") == 0) {
			this->uname = new char[strlen(argv[argumentcount+1])];
			this->uname = argv[argumentcount+1];
		}
		if(strcmp(argv[argumentcount],"-password") == 0) {
			this->pwd = (char*)malloc(sizeof(char)*strlen(argv[argumentcount+1]));
			this->pwd = new char[strlen(argv[argumentcount+1])];
			this->pwd = argv[argumentcount+1];
		}
		if(strcmp(argv[argumentcount],"-port") == 0) {
			this->portno = new char[strlen(argv[argumentcount+1])];
			this->portno = argv[argumentcount+1];
		}
		if(strcmp(argv[argumentcount],"-machine") == 0) {
			this->mname = new char[strlen(argv[argumentcount+1])];
			this->mname = argv[argumentcount+1];
		}
	}
}

