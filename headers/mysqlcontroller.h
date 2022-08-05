
#ifndef HEADERS_MYSQLCONTROLLER_H_
#define HEADERS_MYSQLCONTROLLER_H_

/* Standard C++ includes */
#include <stdlib.h>
#include <iostream>
#include <mysql.h>
#include <ctime>

using namespace std;

class mysqlcontroller {
public:
    mysqlcontroller();
    mysqlcontroller(std::string,std::string,std::string,std::string,int);
    void setHOSTNAME(string host);
    void setDBASENAME(string dbase);
    void setUSERNAME(string uname);
    void setPASSWORD(string pwd);
    void setPORT(int port);
    std::string getHOSTNAME();
    std::string getDBASENAME();
    std::string getUSERNAME();
    std::string getPASSWORD();
    int getPORT();
    void initdb();
    void saveDirectoryName(std::string dirname);
    void deleteDirectoryNames();
    void closedb();
private:
    //std::vector<std::string> tables;
    std::string getCurrentTime();
	int checkIfDirectoryExists(std::string dirname);
	void addVerified(int dirid);
	void createDatabase(std::string dbasename);
	int checkIfDBaseFileExists(std::string dbasename);
	std::string filterApostraphe(std::string dirname);
};

#endif
