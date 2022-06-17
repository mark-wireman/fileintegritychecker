/*
 * sqlite3crud.h
 *
 *  Created on: Jun 4, 2022
 *      Author: mark_
 */

#ifndef HEADERS_SQLITE3CRUD_H_
#define HEADERS_SQLITE3CRUD_H_

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <iostream>
#include <string>
using std::string;
#include <ctime>
#include <sstream>
using std::stringstream;
#include <fstream>
#include <vector>


class sqlite3crud {
public:
	void initdb(std::string dbasename);
	void saveDirectoryName(std::string dirname);
	void deleteDirectoryNames();
	void closedb();
private:
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	std::string _dbasename;
	std::string getCurrentTime();
	int checkIfDirectoryExists(std::string dirname);
	void addVerified(int dirid);
	void createDatabase(std::string dbasename);
	int checkIfDBaseFileExists(std::string dbasename);
	std::string filterApostraphe(std::string dirname);
	void opendb();
        void checkIfDBOpen();
};

int callback(void *NotUsed, int argc, char **argv, char **azColName);

#endif /* HEADERS_SQLITE3CRUD_H_ */
