#include "../headers/MySQLController.h"

#define STRING_SIZE 65535
#define SAVEDIRSTOREDPROCSTMT "call saveDirectoryDetails(?, ?, ?)"
#define SAVEFILEINFOPROCSTMT "call saveFileHashVal(?,?,?,?,?)"
#define CREATESCHEMA "CREATE DATABASE IF NOT EXISTS "
#define UPDATEDIRINFO "UPDATE directories set dateverified=?, status='VERIFIED' WHERE id=?"
#define INSERTDIRINFO "INSERT INTO directories (dirname,dateadded,machinename) VALUES (?,?,? )"

sql::Driver *driver;
sql::Connection *con;
//sql::Statement *stmt;
//sql::ResultSet *res;


MySQLController::MySQLController() {}

MySQLController::MySQLController(char* host, char* dbase, char* uname, char* pwd, char* mname, int portno) {
    setMACHINENAME(mname);
    setDATABASENAME(dbase);
    setHOSTNAME(host);
    setUSERNAME(uname);
    setPASSWORD(pwd);
    setPORTNUMBER(portno);
    setDATABASETYPE("mysql");
    
}


MySQLController::~MySQLController() {}


void MySQLController::initdb() {
     try {
		cout << "Initializing connection to the database." << std::endl;

		string connstr = "tcp://" + string(getHOSTNAME()) + ":";
		
		if(getPORTNUMBER() == 0) {
			connstr += "3306";
		}
		else {
			connstr += to_string(getPORTNUMBER());
		}

		/* Create a connection */
		driver = get_driver_instance();
		con = driver->connect(connstr,getUSERNAME(),getPASSWORD());
		con->setAutoCommit(false);
		createTables();
		/* Connect to the MySQL test database */
		con->setSchema(getDATABASENAME());

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

void MySQLController::closedb() {
 	delete res;
    delete stmt;
    delete con;
}


void MySQLController::createTables() {
    char* createTblStmt = new char[strlen(CREATESCHEMA) + strlen(getDATABASENAME()) + 1];
	strcpy(createTblStmt, CREATESCHEMA);
	strcat(createTblStmt,getDATABASENAME());
	
	sql::Statement *stmt = con->createStatement();
	stmt->execute(createTblStmt);
	con->commit();

	strcpy(createTblStmt,"USE ");
	strcat(createTblStmt,getDATABASENAME());
	stmt->execute(createTblStmt);
	con->commit();

	stmt->execute(SQLHelper::getCreateTableSQL(SQLHelper::TableName::Directories));
	con->commit();
	
	stmt->execute(SQLHelper::getCreateTableSQL(SQLHelper::TableName::Files));
	con->commit();
	
	stmt->execute(SQLHelper::getCreateTableSQL(SQLHelper::TableName::Changes));
	con->commit();

	delete stmt;
}

/**
 * @brief 
 * 
 * @param dirname 
 * @return int 
 */
int MySQLController::checkIfDirectoryExist(const char* dirname) {
    char *sql = "SELECT id FROM directories WHERE dirname = ? AND machinename = ?";
    sql::PreparedStatement* pStmt;
    int retVal = 0;
    sql::ResultSet* rslt;

    pStmt = con->prepareStatement(sql);

    try {

		con->setAutoCommit(false);
        pStmt->setString(1,dirname);
		pStmt->setString(2, getMACHINENAME());
		rslt = pStmt->executeQuery();
		
        while(rslt->next()) {
            //printf("\n\tInside while next in checkIfDirectoryExist.\n\n");
            retVal = rslt->getInt(1);
            //printf("\n\tAfter while next in checkIfDirectoryExist with retVal %d.\n\n",retVal);
        }

		con->setAutoCommit(true);

	} catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}

    delete res;
    delete pStmt;
    
    return retVal;

}


/**
 * @brief 
 * 
 * @param fname 
 * @param dirname 
 * @return int 
 */
int MySQLController::checkIfFileExist(const char* fname, const char* dirname) {
    
    char *sql = "SELECT id FROM files WHERE dirid = ? AND filename = ?; ";
    sql::PreparedStatement* pStmt;
    int retVal = 0;
    sql::ResultSet* rslt;

    pStmt = con->prepareStatement(sql);

    int dirId = checkIfDirectoryExist(dirname);

    try {

		con->setAutoCommit(false);
        pStmt->setInt(1,dirId);
        pStmt->setString(2,dirname);
		pStmt->setString(3, getMACHINENAME());
		rslt = pStmt->executeQuery();
		
        while(rslt->next()) {
            retVal = rslt->getInt(1);
        }

		con->setAutoCommit(true);

	} catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}

    delete res;
    delete pStmt;

    return retVal;

}



/**
 * @brief 
 * 
 * @param fname 
 * @param dirname 
 * @param lastmodified 
 * @param filesize 
 * @param hashval 
 * @return int 
 */
int MySQLController::save_file_info(const char* fname, const char* dirname, char* lastmodified, uintmax_t filesize, const char* hashval) {
    //printf("Variables provided are: \n\t%s \n\t %s \n\t %s \n\t %d \n\t %s \n",fname,dirname,lastmodified,filesize,hashval);

    int dir_id = checkIfDirectoryExist(dirname);
    int fileExists = SQLHelper::getFileId(con, (char*)fname, dir_id);

    if(fileExists > 0) {
        if (lastmodified != NULL) {
            SQLHelper::saveFileInfo(con, dir_id, fname, filesize,lastmodified, NULL, false, SQLHelper::AttributeToCheck::DateModified);
        }
        if (hashval != NULL) {
            //fprintf(stderr, "\nReceived hashedvalue of %s\n", hashval);
            SQLHelper::saveFileInfo(con, dir_id, fname, filesize, NULL, hashval, false, SQLHelper::AttributeToCheck::HashedVal);
        }
        if (filesize > 0) {
            SQLHelper::saveFileInfo(con, dir_id, fname, filesize, NULL, NULL, false, SQLHelper::AttributeToCheck::FileSize);
        }
    } else if(fileExists == 0) {
        SQLHelper::saveFileInfo(con, dir_id, fname, filesize, lastmodified, hashval, true);
    } else {
        return -1;
    }

    return 0;

}

/**
 * @brief 
 * 
 * @param dirname 
 * @return int 
 */
int MySQLController::save_dir_info(const char* dirname) {
    char *diradd_sql;
    sql::PreparedStatement *pStmt;
    char* current_time = SQLHelper::getCurrentTime();
    int dirExists = checkIfDirectoryExist(dirname);

    if(dirExists > 1) {
        pStmt = con->prepareStatement(UPDATEDIRINFO);
    } else if(dirExists == 0) {
        pStmt = con->prepareStatement(INSERTDIRINFO);
    } else {
        return -1;
    }

    try {

		con->setAutoCommit(false);

        if(dirExists > 0) {
            pStmt->setString(1,current_time);
            pStmt->setInt(2,dirExists);
            pStmt->executeQuery();
        } else {
            pStmt->setString(1,dirname);
            pStmt->setString(2,current_time);
            pStmt->setString(3,getMACHINENAME());
            pStmt->executeQuery();
        }
		
		
        con->setAutoCommit(true);

	} catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}

    delete res;
    delete pStmt;

    return 0;  

}