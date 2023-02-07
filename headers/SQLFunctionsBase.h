#ifndef SQLFUNCTIONSBASE_H_
#define SQLFUNCTIONSBASE_H_

#include <iostream>

using namespace std;

class SQLFunctionsBase {
public:
    SQLFunctionsBase() {};
    virtual void setDATABASENAME(char* dbasename);
    virtual void setMACHINENAME(char* machinename);
    virtual void setHOSTNAME(char* host);
    virtual void setUSERNAME(char* uname);
    virtual void setPASSWORD(char* pwd);
    virtual void setPORTNUMBER(int port);
    virtual void setDATABASETYPE(char* dbasetype);
    virtual char* getDATABASENAME();
    virtual char* getMACHINENAME();
    virtual char* getHOSTNAME();
    virtual char* getUSERNAME();
    virtual char* getPASSWORD();
    virtual int getPORTNUMBER();
    
protected:
    char* _databasename;
    char* _machinename;
    char* _username;
    char* _password;
    char* _hostname;
    int _portnumber;
    char* _databasetype;


};

#endif