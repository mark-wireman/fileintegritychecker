
#ifndef HEADERS_MYSQLCONTROLLER_H_
#define HEADERS_MYSQLCONTROLLER_H_

/* Standard C++ includes */
#include <stdlib.h>
#include <iostream>
#include <mysql.h>

using namespace std;

class mysqlcontroller {
public:
    mysqlcontroller();
    mysqlcontroller(std::string,std::string,std::string,std::string,int);
    void setHOSTNAME(std::string);
    void setDBASENAME(std::string);
    void setUSERNAME(std::string);
    void setPASSWORD(std::string);
    void setPORT(int);
    std::string getHOSTNAME();
    std::string getDBASENAME(std::string);
    std::string getUSERNAME(std::string);
    std::string getPASSWORD(std::string);
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