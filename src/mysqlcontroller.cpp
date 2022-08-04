#include "../headers/mysqlcontroller.h"

MYSQL_RES *result;
MYSQL_ROW row;
MYSQL *connection, mysql;
std::string hostname;
std::string dbasename;
std::string username;
std::string password;
static int port = 3306;
int state;

mysqlcontroller::mysqlcontroller() {}

mysqlcontroller::mysqlcontroller(std::string host, std::string dbase, std::string uname, std::string pwd, int portno)
{
    hostname = host;
    dbasename = dbase;
    username= uname;
    password = pwd;
    port = portno;
}

void setHOSTNAME(std::string host) {
    hostname = host;
}

void setDBASENAME(std::string dbase) {
    dbasename = dbase;
}

void setUSERNAME(std::string uname) {
    username = uname;
}

void setPASSWORD(std::string pwd) {
    password = pwd;
}

void setPORT(int portno) {
    port = portno;
}

std::string getHOSTNAME() {
    return hostname;
}

std::string getDBASENAME() {
    return dbasename;
}

std::string getUSERNAME() {
    return username;
}

std::string getPASSWORD() {
    return password;
}

int getPORT() {
    return port;
}

std::string mysqlcontroller::getCurrentTime() {
	std::string retVal = "";

	std::time_t t = std::time(0);   // get time now
	std::tm* now = std::localtime(&t);
	char date_string[100];
	char time_string[100];

	strftime(date_string, 50, "%B %d, %Y ", now);
	strftime(time_string, 50, "%T", now);

	retVal += date_string;
	retVal += time_string;

	return retVal;
}

void mysqlcontroller::initdb() {
    mysql_init(&mysql);
  
    connection = mysql_real_connect(&mysql,hostname.c_str(),username.c_str(),password.c_str(),dbasename.c_str(),0,0,0);
  
    // cout << connection;
    if (connection == NULL)
    {
        std::cout << mysql_error(&mysql) << std::endl;
        throw std::invalid_argument(mysql_error(&mysql));
    }

    cout << "Successfully connected to database." << std::endl;
}
