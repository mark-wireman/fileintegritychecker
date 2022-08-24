#include "../headers/mysqlcontroller.h"

#define STRING_SIZE 65535
#define STOREDPROCSTMT "call saveDirectoryDetails(?, ?, ?)"

MYSQL *connection, mysql;
char* hostname;
char* dbasename;
char* username;
char* password;
char* machinename;
int port;
int state;
MYSQL_STMT *storedProcStmt = NULL;

mysqlcontroller::mysqlcontroller() {}

mysqlcontroller::mysqlcontroller(char* host, char* dbase, char* uname, char* pwd, int portno, char* mname)
{
    hostname = host;
    dbasename = dbase;
    username= uname;
    password = pwd;
    port = portno;
	machinename = mname;
}

void mysqlcontroller::setHOSTNAME(char* host) {
    hostname = host;
}

void mysqlcontroller::setDBASENAME(char* dbase) {
    dbasename = dbase;
}

void mysqlcontroller::setUSERNAME(char* uname) {
    username = uname;
}

void mysqlcontroller::setPASSWORD(char* pwd) {
    password = pwd;
}

void mysqlcontroller::setPORT(int portno) {
    port = portno;
}

void mysqlcontroller::setMACHINENAME(char* mname) {
	machinename = mname;
}

char* mysqlcontroller::getHOSTNAME() {
    return hostname;
}

char* mysqlcontroller::getDBASENAME() {
    return dbasename;
}

char* mysqlcontroller::getUSERNAME() {
    return username;
}

char* mysqlcontroller::getPASSWORD() {
    return password;
}

int mysqlcontroller::getPORT() {
    return port;
}

char* mysqlcontroller::getMACHINENAME() {
	return machinename;
}

void mysqlcontroller::initPreparedStatements() {
	int result; 
    int count; 

    storedProcStmt = mysql_stmt_init(connection); 
    if (storedProcStmt == NULL) { 
		cout << "init of insert stmt failed" << "\n";
		exit(1);
	} 
    
	result = mysql_stmt_prepare(storedProcStmt, STOREDPROCSTMT, strlen(STOREDPROCSTMT));  
    if (result != 0) { 
		cout << "prepare of insert stmt failed" << "\n";
		exit(1);
	} 

    count = mysql_stmt_param_count(storedProcStmt);
    printf("there are %d parameters in the insert statements\n", count); 

}

void mysqlcontroller::finish_with_error()
{
	string mysqlerr = mysql_error(&mysql);
	fprintf(stderr, "MySQL Error: %s\n", mysqlerr.c_str());
	mysql_close(connection);
	throw invalid_argument(mysqlerr);
	exit(1);
}

char* mysqlcontroller::getCurrentTime() {
	string retVal;
	char* char_array;

	std::time_t t = std::time(0);   // get time now
	std::tm* now = std::localtime(&t);
	char date_string[100];
	char time_string[100];

	strftime(date_string, 50, "%B %d, %Y ", now);
	strftime(time_string, 50, "%T", now);

	retVal += date_string;
	retVal += time_string;

	int n = retVal.length();
	char_array = new char[n + 1];

	strcpy(char_array, retVal.c_str());
	
	return char_array;
}

void mysqlcontroller::initdb() {
    mysql_init(&mysql);
  
	cout << "mysqlcontroller::Before call to connection\n";
	cout << hostname << " - " << username << " - " << password << " - " << dbasename << "\n";
    connection = mysql_real_connect(&mysql,hostname,username,password,dbasename,0,NULL,0);
	cout << "mysqlcontroller::After call to connection\n";

    if (connection == NULL)
    {
        cout << "mysqlcontroller::initdb() Error - " << mysql_error(connection) << std::endl;
		fprintf(stderr, "mysqlcontroller::Failed to connect to database: Error: %s\n",
          mysql_error(&mysql));
		this->finish_with_error();
    }

    cout << "Successfully connected to database." << std::endl;
}

char* mysqlcontroller::filterApostraphe(string dirname) {
	string retVal = "";
    char* char_array;

	std::vector<string> result;
	std::stringstream s_stream(dirname); //create string stream from the string

	while(s_stream.good()) {
	   string substr;
	   getline(s_stream, substr, '\''); //get first string delimited by comma
	   result.push_back(substr);
	}

	if(result.size() > 1) {
		retVal+=result.at(0);

		for(int i = 1; i<result.size(); i++) {    //print all splitted strings
			retVal+="'";
			retVal+="'";
			retVal+=result.at(i);
		}
	} else {
		retVal = dirname;
	}
		
	int n = retVal.length();
	char_array = new char[n + 1];

	strcpy(char_array, retVal.c_str());

	return char_array;
}

void mysqlcontroller::saveDirectoryName(string dirname) {

	char* dirnameverified = new char[dirname.length() + 1]; // filterApostraphe(dirname);
	strcpy(dirnameverified,dirname.c_str());
    
	cout << "Before checkIfDirectoryExists.\n";

	cout << "saveDirectoryName::Inside ifexists.\n";

	MYSQL_BIND    bind[3];
	char* currenttime = getCurrentTime();
	unsigned long sizeofdirname = strlen(dirnameverified);
	unsigned long sizeoftime = strlen(currenttime);
	unsigned long sizeofmname = strlen(machinename);
	unsigned int cursor_type = CURSOR_TYPE_NO_CURSOR;
	int result = 0;

	cout << "Current time is " << currenttime << "\n";
	
	cout << "Before setting up bind.\n";
	memset(bind, 0, sizeof(MYSQL_BIND)*3);

	/* STRING PARAM */
	bind[0].buffer_type= MYSQL_TYPE_STRING;
	bind[0].buffer= dirnameverified;
	bind[0].buffer_length= strlen(dirnameverified);
	bind[0].length= &sizeofdirname;

	/* STRING PARAM */
	bind[1].buffer_type= MYSQL_TYPE_STRING;
	bind[1].buffer= currenttime;
	bind[1].buffer_length= strlen(currenttime);
	bind[1].length= &sizeoftime;

	/* STRING PARAM */
	bind[2].buffer_type= MYSQL_TYPE_STRING;
	bind[2].buffer= machinename;
	bind[2].buffer_length= strlen(machinename);
	bind[2].length= &sizeofmname;

	mysql_stmt_attr_set(storedProcStmt, STMT_ATTR_CURSOR_TYPE, &cursor_type);

	result = mysql_stmt_bind_param(storedProcStmt, bind);
	
	/* Bind the buffers */
	if (result != 0)
	{
  		fprintf(stderr, " saveDirectoryName::mysql_stmt_bind_param() failed\n");
  		finish_with_error();
	}

	cout << "saveDirectoryName::After stmt bind.\n";
		
	result = mysql_stmt_execute(storedProcStmt);

	/* Execute the INSERT statement - 1*/
	if (result != 0)
	{
  		fprintf(stderr, " saveDirectoryName::mysql_stmt_execute(), 1 failed\n");
		cout << "saveDirectoryName::MySQL Error - " << mysql_error(connection) << endl;
		finish_with_error();
  			
	}

	cout << "saveDirectoryName::After stmt execute.\n";
}

void mysqlcontroller::closePreparedStatements() {
	
    mysql_stmt_close(storedProcStmt);
}

void mysqlcontroller::closedb() {
	mysql_close(connection); 
}
