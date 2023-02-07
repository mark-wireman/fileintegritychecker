/*
 * Menu.h
 *
 *  Created on: Jun 7, 2022
 *      Author: mark_
 */

#ifndef HEADERS_MENU_H_
#define HEADERS_MENU_H_

#include <string>
#include <cstring>
#include <iostream>

using namespace std;

class Menu {
public:
	Menu();
	virtual ~Menu();
	int ParseArguments(int argc, char *argv[]);
	void PrintMenu();
	char* getDATABASEHOSTNAME();
	char* getUSERNAME();
	char* getPASSWORD();
	char* getPORT();
	char* getMACHINENAME();
	char* getPARENTDIR();
	char* getDBASETYPE();
	char* getDBASEFILENAME();
	char* getDBASENAME();
	int getPROCESSHASHVALS();
private:
	char* parentdir = NULL;
	char* dbasehost = NULL;
	char* uname = NULL;
	char* pwd = NULL;
	char* portno = NULL;
	char* mname = NULL;
	char* dbasetype = NULL;
	char* dbasefilename = NULL;
	char* dbasename = NULL;
	int processhashvals = 0;
};

#endif /* MENU_H_ */
