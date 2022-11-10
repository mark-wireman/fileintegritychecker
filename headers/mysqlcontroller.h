
#ifndef HEADERS_MYSQLCONTROLLER_H_
#define HEADERS_MYSQLCONTROLLER_H_

#define __STDC_WANT_LIB_EXT1__ 1
/* Standard C++ includes */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
//#include <mysql.h>
#include <ctime>
//#include <chrono>
//#include <cstring>
#include <sstream>
using std::stringstream;
//#include <fstream>
//#include <vector>
#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

using namespace std;

class mysqlcontroller {
public:
    mysqlcontroller();
    mysqlcontroller(char* host,char* dbase,char* uname,char* pwd,int port,char* mname);
    void setHOSTNAME(char* host);
    void setDBASENAME(char* dbase);
    void setUSERNAME(char* uname);
    void setPASSWORD(char* pwd);
    void setPORT(int port);
    void setMACHINENAME(char* mname);
    char* getHOSTNAME();
    char* getDBASENAME();
    char* getUSERNAME();
    char* getPASSWORD();
    int getPORT();
    char* getMACHINENAME();
    void initdb();
    void saveDirectoryName(string dirname);
    void deleteDirectoryNames();
    void closedb();
    void initPreparedStatements();
    void closePreparedStatements();
private:
    //std::vector<std::string> tables;
    char* getCurrentTime();
	void addVerified(int);
	char* filterApostraphe(string);
    //void finish_with_error();
    
};

#endif
