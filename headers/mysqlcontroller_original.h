
#ifndef HEADERS_MYSQLCONTROLLER_H_
#define HEADERS_MYSQLCONTROLLER_H_

#define __STDC_WANT_LIB_EXT1__ 1
/* Standard C++ includes */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include <ctime>
#include <sstream>
using std::stringstream;
#include "mysql_connection.h"
#include "SQLHelper.h"
#include "SQLFunctionsBase.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

using namespace std;

class mysqlcontroller : public virtual SQLFunctionsBase {
public:
    mysqlcontroller();
    mysqlcontroller(char* host,char* dbase,char* uname,char* pwd,int port,char* mname);
    virtual ~mysqlcontroller();
    
    void initdb();
    void saveDirectoryName(string dirname);    
    void saveFileInfo(string dirname, string fname, string hashval);
    void saveDirectoryName_async(const string &dirname);
    void saveFileInfo_async(const string &dirname, const string &fname, const string &hashval);
    void deleteDirectoryNames();
    void closedb();
    void initPreparedStatements();
    void closePreparedStatements();
private:
	char* filterApostraphe(string);
    void createTables();
    
};

#endif
