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
	char* getDBaseInfo();
	char* getParentDir();
	void ParseArguments(int argc, char *argv[]);
	void PrintMenu();
private:
	char* dbasedetails;
	char* parentdir;
};

#endif /* MENU_H_ */
