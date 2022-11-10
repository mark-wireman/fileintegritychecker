/*
 * Menu.h
 *
 *  Created on: Jun 7, 2022
 *      Author: mark_
 */

#ifndef MENU_H_
#define MENU_H_

#include <string>
#include <cstring>
#include <iostream>

using namespace std;

class Menu {
public:
	Menu();
	virtual ~Menu();
	void ParseArguments(int argc, char *argv[]);
	void PrintMenu();
	char* getDATABASEHOSTNAME();
	char* getUSERNAME();
	char* getPASSWORD();
	char* getPORT();
	char* getMACHINENAME();
	char* getPARENTDIR();
private:
	char* parentdir;
	char* dbasehost;
	char* uname = NULL;
	char* pwd = NULL;
	char* portno;
	char* mname;
};

#endif /* MENU_H_ */
