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

char* Menu::getDATABASENAME() {
	return this->dbase;
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

int Menu::getPORT() {
	return this->portno;
}

char* Menu::getPARENTDIR() {
	return this->parentdir;
}

void Menu::PrintMenu() {
	cout << "Usage: fileintegritychecker.exe -directory {starting directory} -dbasehost {MySQL IP or name} -username {MySQL username} -password {MySQL password} -dbasename {Database that contains the directories table} -port {MySQL port number} -machinename {Name of machine being scanned}";
}


void Menu::ParseArguments(int argc, char* argv[]) {
	
	for( int argumentcount = 1; argumentcount < argc; argumentcount++ ) {

		if(strcmp(argv[argumentcount],"-directory") == 0) {
			this->parentdir = argv[argumentcount+1];
		}
		if(strcmp(argv[argumentcount],"-dbasehost") == 0) {
			this->dbasehost = argv[argumentcount+1];
		}
		if(strcmp(argv[argumentcount],"-username") == 0) {
			this->uname = argv[argumentcount+1];
		}
		if(strcmp(argv[argumentcount],"-password") == 0) {
			this->pwd = argv[argumentcount+1];
		}
		if(strcmp(argv[argumentcount],"-dbasename") == 0) {
			this->dbase = argv[argumentcount+1];
		}
		if(strcmp(argv[argumentcount],"-port") == 0) {
			this->portno = atoi(argv[argumentcount+1]);
		}
		if(strcmp(argv[argumentcount],"-machinename") == 0) {
			this->mname = argv[argumentcount+1];
		}
	}
}

