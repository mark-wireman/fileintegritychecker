#include "../headers/SQLFunctionsBase.h"

void SQLFunctionsBase::setHOSTNAME(char* host) {
    _hostname = host;
}

void SQLFunctionsBase::setDATABASENAME(char* dbase) {
    _databasename = dbase;
}

void SQLFunctionsBase::setUSERNAME(char* uname) {
    _username = uname;
}

void SQLFunctionsBase::setPASSWORD(char* pwd) {
    _password = pwd;
}

void SQLFunctionsBase::setPORTNUMBER(int portno) {
    _portnumber = portno;
}

void SQLFunctionsBase::setMACHINENAME(char* mname) {
	_machinename = mname;
}

void SQLFunctionsBase::setDATABASETYPE(char* dbasetype) {
	_databasetype = dbasetype;
}

char* SQLFunctionsBase::getHOSTNAME() {
    return _hostname;
}

char* SQLFunctionsBase::getDATABASENAME() {
    return _databasename;
}

char* SQLFunctionsBase::getUSERNAME() {
    return _username;
}

char* SQLFunctionsBase::getPASSWORD() {
    return _password;
}

int SQLFunctionsBase::getPORTNUMBER() {
    return _portnumber;
}

char* SQLFunctionsBase::getMACHINENAME() {
	return _machinename;
}
