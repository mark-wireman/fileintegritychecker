#include "../headers/mysqlcontroller.h"

#define STRING_SIZE 65535
#define SAVEDIRSTOREDPROCSTMT "call saveDirectoryDetails(?, ?, ?)"
#define SAVEFILEINFOPROCSTMT "call saveFileHashVal(?,?,?,?,?)"
#define CREATESCHEMA "CREATE DATABASE IF NOT EXISTS "

//MYSQL *connection, mysql;
char* hostname;
char* dbasename;
char* username;
char* password;
char* machinename;
int port;
int state;
//MYSQL_STMT *storedProcStmt = NULL;

sql::Driver *driver;
sql::Connection *con;
sql::Statement *stmt;
sql::ResultSet *res;
sql::PreparedStatement *savedirprepStmt;
sql::PreparedStatement *savedfileprepStmt;
sql::PreparedStatement *createDatabase;
/**
 * @brief Construct a new mysqlcontroller::mysqlcontroller object
 * 
 */
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

/**
 * @brief Destroy mysqlcontroller::mysqlcontroller object
 * 
 */
mysqlcontroller::~mysqlcontroller() {}

/**
 * @brief 
 * 
 * @param host 
 */
void mysqlcontroller::setHOSTNAME(char* host) {
    hostname = host;
}

/**
 * @brief 
 * 
 * @param dbase 
 */
void mysqlcontroller::setDBASENAME(char* dbase) {
    dbasename = dbase;
}

/**
 * @brief 
 * 
 * @param uname 
 */
void mysqlcontroller::setUSERNAME(char* uname) {
    username = uname;
}

/**
 * @brief 
 * 
 * @param pwd 
 */
void mysqlcontroller::setPASSWORD(char* pwd) {
    password = pwd;
}

/**
 * @brief 
 * 
 * @param portno 
 */
void mysqlcontroller::setPORT(int portno) {
    port = portno;
}

/**
 * @brief 
 * 
 * @param mname 
 */
void mysqlcontroller::setMACHINENAME(char* mname) {
	machinename = mname;
}

/**
 * @brief 
 * 
 * @return char* 
 */
char* mysqlcontroller::getHOSTNAME() {
    return hostname;
}

/**
 * @brief 
 * 
 * @return char* 
 */
char* mysqlcontroller::getDBASENAME() {
    return dbasename;
}

/**
 * @brief 
 * 
 * @return char* 
 */
char* mysqlcontroller::getUSERNAME() {
    return username;
}

/**
 * @brief 
 * 
 * @return char* 
 */
char* mysqlcontroller::getPASSWORD() {
    return password;
}

/**
 * @brief 
 * 
 * @return int 
 */
int mysqlcontroller::getPORT() {
    return port;
}

/**
 * @brief 
 * 
 * @return char* 
 */
char* mysqlcontroller::getMACHINENAME() {
	return machinename;
}

/**
 * @brief 
 * 
 */
void mysqlcontroller::initPreparedStatements() {

	savedirprepStmt = con->prepareStatement(SAVEDIRSTOREDPROCSTMT);
	savedfileprepStmt = con->prepareStatement(SAVEFILEINFOPROCSTMT);

}


/**
 * @brief 
 * 
 */
void mysqlcontroller::initdb() {
    try {
		cout << "Initializing connection to the database." << std::endl;

		string connstr = "tcp://" + string(hostname) + ":";
		
		if(port == 0) {
			connstr += "3306";
		}
		else {
			connstr += to_string(port);
		}

		/* Create a connection */
		driver = get_driver_instance();
		con = driver->connect(connstr, username, password);
		con->setAutoCommit(false);
		createTables();
		/* Connect to the MySQL test database */
		con->setSchema(dbasename);

		cout << "Successfully connected to database." << endl;
	} catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}

	cout << endl;
}

void mysqlcontroller::createTables() {
	
	char* createTblStmt = new char[strlen(CREATESCHEMA) + strlen(dbasename) + 1];
	strcpy(createTblStmt, CREATESCHEMA);
	strcat(createTblStmt,dbasename);
	
	sql::Statement *stmt = con->createStatement();
	stmt->execute(createTblStmt);
	con->commit();

	strcpy(createTblStmt,"USE ");
	strcat(createTblStmt,dbasename);
	stmt->execute(createTblStmt);
	con->commit();

	stmt->execute(SQLHelper::getCreateTableSQL(SQLHelper::TABLE_TO_CREATE::DIRECTORIES));
	con->commit();
	
	stmt->execute(SQLHelper::getCreateTableSQL(SQLHelper::TABLE_TO_CREATE::FILES));
	con->commit();
	
	stmt->execute(SQLHelper::getCreateTableSQL(SQLHelper::TABLE_TO_CREATE::CHANGES));
	con->commit();

	delete stmt;

}

/**
 * @brief 
 * 
 * @param dirname 
 * @return char* 
 */
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


/**
 * @brief 
 * 
 * @param dirname 
 */
void mysqlcontroller::saveDirectoryName(string dirname) {

	char* dirnameverified = new char[dirname.length() + 1]; // filterApostraphe(dirname);
	strcpy(dirnameverified,dirname.c_str());
    
	initPreparedStatements();
	char* currenttime = SQLHelper::getCurrentTime();

	try {
		con->setAutoCommit(false);
		savedirprepStmt->setString(1, dirnameverified);
		savedirprepStmt->setString(2, currenttime);
		savedirprepStmt->setString(3, machinename);
		savedirprepStmt->executeUpdate();
		con->commit();
		con->setAutoCommit(true);
	} catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}

	closePreparedStatements();

}

/**
 * @brief 
 * 
 * @param dirname 
 */
void mysqlcontroller::saveDirectoryName_async(const string &dirname) {
	string dname = dirname;
	saveDirectoryName(dname);
}

/**
 * @brief 
 * 
 * @param dirname 
 * @param fname 
 * @param hashval 
 */
void mysqlcontroller::saveFileInfo_async(const string &dirname, const string &fname, const string &hashval) {
	string dname = dirname;
	string filname = fname;
	string hval = hashval;
	string currenttime = "";
	
	std::time_t t = std::time(0);   // get time now
	std::tm* now = std::localtime(&t);
	char date_string[100];
	char time_string[100];

	strftime(date_string, 50, "%B %d, %Y ", now);
	strftime(time_string, 50, "%T", now);

	currenttime += date_string;
	currenttime += time_string;

	savedfileprepStmt = con->prepareStatement(SAVEFILEINFOPROCSTMT);

	try {
		con->setAutoCommit(false);
		savedfileprepStmt->setString(1, dirname);
		savedfileprepStmt->setString(2, currenttime);
		savedfileprepStmt->setString(3, machinename);
		savedfileprepStmt->setString(4, fname);
		savedfileprepStmt->setString(5, hashval);
		savedfileprepStmt->executeUpdate();
		con->commit();
		con->setAutoCommit(true);
	} catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}

	delete savedfileprepStmt;
}

/**
 * @brief 
 * 
 * @param dirname 
 * @param fname 
 * @param hashval 
 */
void mysqlcontroller::saveFileInfo(string dirname, string fname, string hashval) {
	
	char* currenttime = SQLHelper::getCurrentTime();

	initPreparedStatements();

	try {
		con->setAutoCommit(false);
		savedfileprepStmt->setString(1, dirname);
		savedfileprepStmt->setString(2, currenttime);
		savedfileprepStmt->setString(3, machinename);
		savedfileprepStmt->setString(4, fname);
		savedfileprepStmt->setString(5, hashval);
		savedfileprepStmt->executeUpdate();
		con->commit();
		con->setAutoCommit(true);
	} catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}

	closePreparedStatements();

}

/**
 * @brief 
 * 
 */
void mysqlcontroller::closePreparedStatements() {
	
	delete savedirprepStmt;
	delete savedfileprepStmt;
}

/**
 * @brief 
 * 
 */
void mysqlcontroller::closedb() {
	delete res;
    delete stmt;
    delete con;
}
