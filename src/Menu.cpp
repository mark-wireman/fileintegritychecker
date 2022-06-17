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

char* Menu::getDBaseInfo() {
	return this->dbasedetails;
}

void Menu::PrintMenu() {
	cout << "Usage: fileintegritychecker.exe -directory {where to start the scan} -database {location of the sqlite3 database}";
}

char* Menu::getParentDir() {
	return this->parentdir;
}

void Menu::ParseArguments(int argc, char* argv[]) {

	int foundDirectory = 0;
	int foundDatabase = 0;

	for( int argumentcount = 1; argumentcount < argc; argumentcount++ ) {

		if(strcmp(argv[argumentcount],"-directory") == 0 && foundDirectory == 0) {
			this->parentdir = argv[argumentcount+1];
			foundDirectory = 1;
			cout<<"Inside if directory..."<<this->parentdir<<endl;
		}
		if(strcmp(argv[argumentcount],"-database") == 0 && foundDatabase == 0) {
			this->dbasedetails = argv[argumentcount+1];
			foundDatabase = 1;
			cout<<"Inside if database..."<<this->dbasedetails<<endl;
		}

	}
}

