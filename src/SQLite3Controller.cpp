#include "../headers/SQLite3Controller.h"

using namespace std;
namespace fs = std::filesystem;

sqlite3 *db;

SQLite3Controller::SQLite3Controller() {}

SQLite3Controller::SQLite3Controller(char* databasename, char* machinename) {
    setMACHINENAME(machinename);
    setDATABASENAME(databasename);
    setDATABASETYPE("sqlite3");
    
}


SQLite3Controller::~SQLite3Controller() {}


void SQLite3Controller::initdb() {
    char *zErrMsg = 0;
    int rc;
    
    
    /* Open database */
    rc = sqlite3_open_v2(getDATABASENAME(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
   
    if( rc ) {
        fprintf(stderr, "\nIn initdb can't open database: %s\n", sqlite3_errmsg(db));
    } else {
        fprintf(stderr, "\nOpened database successfully\n");
        createTables();
    }

}

void SQLite3Controller::closedb() {
    sqlite3_close_v2(db);
}

void SQLite3Controller::correctAutoIncrement(char*& _sqlstmt) {
    for(int i = 0; i < strlen(_sqlstmt); i++) {
        if(_sqlstmt[i] == '_') {
            _sqlstmt[i] = 'I';
            _sqlstmt[i+1] = 'N';
            _sqlstmt[i+2] = 'C';
            _sqlstmt[i+3] = 'R';
            _sqlstmt[i+4] = 'E';
            _sqlstmt[i+5] = 'M';
            _sqlstmt[i+6] = 'E';
            _sqlstmt[i+7] = 'N';
            _sqlstmt[i+8] = 'T';
            _sqlstmt[i+9] = ' ';
        }
    }
}

void SQLite3Controller::createTables() {
    int rc;

    char* dirtable_sql = SQLHelper::getCreateTableSQL(SQLHelper::TableName::Directories);
    char *filetable_sql = SQLHelper::getCreateTableSQL(SQLHelper::TableName::Files);
    char *changestable_sql = SQLHelper::getCreateTableSQL(SQLHelper::TableName::Changes);
    char *zErrMsg = 0;

    correctAutoIncrement(dirtable_sql);
    correctAutoIncrement(filetable_sql);
    correctAutoIncrement(changestable_sql);
   
    rc = sqlite3_exec(db, dirtable_sql, NULL, 0, &zErrMsg);
   
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Table created successfully\n");
    }

    rc = sqlite3_exec(db, filetable_sql, NULL, 0, &zErrMsg);
    
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Table created successfully\n");
    }

    rc = sqlite3_exec(db, changestable_sql, NULL, 0, &zErrMsg);
    
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Table created successfully\n");
    }
}

/**
 * @brief 
 * 
 * @param dirname 
 * @return int 
 */
int SQLite3Controller::checkIfDirectoryExist(const char* dirname) {
    char *zErrMsg = 0;
    int rc;
    char *sql = "SELECT id FROM directories WHERE dirname = ? AND machinename = ?; ";
    sqlite3_stmt * pStmt;
    int retVal = 0;

    rc = sqlite3_prepare_v3(db, sql, -1, 0, &pStmt, NULL);
    
    if (rc == SQLITE_OK) {        
        sqlite3_bind_text(pStmt, 1, dirname, -1, NULL);
        sqlite3_bind_text(pStmt, 2, getMACHINENAME(), -1, NULL);
    } else {
        
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    while(sqlite3_step(pStmt) == SQLITE_ROW)
    {
        retVal = sqlite3_column_int(pStmt,0);
    }

    sqlite3_finalize(pStmt);

    return retVal;

}


/**
 * @brief 
 * 
 * @param fname 
 * @param dirname 
 * @return int 
 */
int SQLite3Controller::checkIfFileExist(const char* fname, const char* dirname) {
    char *zErrMsg = 0;
    int rc;
    char *sql = "SELECT id FROM files WHERE dirid = ? AND filename = ?; ";
    sqlite3_stmt * pStmt;
    int retVal = 0;

    int dirId = checkIfDirectoryExist(dirname);

    rc = sqlite3_prepare_v3(db, sql, -1, 0, &pStmt, NULL);
    
    if (rc == SQLITE_OK) {        
        sqlite3_bind_int(pStmt, 1, dirId);
        sqlite3_bind_text(pStmt, 2, fname, -1, NULL);
        sqlite3_bind_text(pStmt, 3,getMACHINENAME(), -1, NULL);
    } else {
        
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    rc = sqlite3_step(pStmt);
    
    while(sqlite3_step(pStmt) == SQLITE_ROW)
    {
        retVal = sqlite3_column_int(pStmt,0);
    }
        
    sqlite3_finalize(pStmt);

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
int SQLite3Controller::save_file_info(const char* fname, const char* dirname, char* lastmodified, uintmax_t filesize, const char* hashval) {
    

    int dir_id = checkIfDirectoryExist(dirname);
    int fileExists = SQLHelper::getFileId(db, (char*)fname, dir_id);

    if(fileExists > 0) {
        if (lastmodified != NULL) {
            SQLHelper::saveFileInfo(db, dir_id, fname, filesize,lastmodified, NULL, false, SQLHelper::AttributeToCheck::DateModified);
        }
        if (hashval != NULL) {
            //fprintf(stderr, "\nReceived hashedvalue of %s\n", hashval);
            SQLHelper::saveFileInfo(db, dir_id, fname, filesize, NULL, hashval, false, SQLHelper::AttributeToCheck::HashedVal);
        }
        if (filesize > 0) {
            SQLHelper::saveFileInfo(db, dir_id, fname, filesize, lastmodified, NULL, false, SQLHelper::AttributeToCheck::FileSize);
        }
    } else if(fileExists == 0) {
        SQLHelper::saveFileInfo(db, dir_id, fname, filesize, lastmodified, hashval, true);
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
int SQLite3Controller::save_dir_info(const char* dirname) {
    char *zErrMsg = 0;
    int rc;
    char *diradd_sql;
    sqlite3_stmt *pStmt;
    
    int dirExists = checkIfDirectoryExist(dirname);

    if(dirExists > 1) {
        diradd_sql = "UPDATE directories set dateverified=?, status='VERIFIED' WHERE id=?; ";
    } else if(dirExists == 0) {
        diradd_sql = "INSERT INTO directories (dirname,dateadded,machinename) VALUES (?,?,? ); ";
    } else {
        return -1;
    }

    rc = sqlite3_prepare_v3(db, diradd_sql, -1, 0, &pStmt, NULL);
    
    if (rc == SQLITE_OK) {
        if(dirExists > 1) {
            sqlite3_bind_text(pStmt, 1, SQLHelper::getCurrentTime(), -1, NULL);
            sqlite3_bind_int(pStmt, 2, dirExists);
        } else {
            sqlite3_bind_text(pStmt, 1, dirname, -1, NULL);
            sqlite3_bind_text(pStmt, 2, SQLHelper::getCurrentTime(), -1, NULL);
            sqlite3_bind_text(pStmt, 3, _machinename, -1, NULL);
        }
    } else {
        
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    rc = sqlite3_step(pStmt);
    
    if (rc == SQLITE_BUSY) {
        cout << "\nDatabase is busy" << endl;
        while (rc == SQLITE_BUSY)
        {
            rc = sqlite3_step(pStmt);
        }
    }
    else if (rc == SQLITE_ERROR) {
        printf("\nIn save_dir_info execution failed: %s\n", sqlite3_errmsg(db));
    }
    else if (rc == SQLITE_MISUSE) {
        cout << "\nA MISUSE has been deteced in save_dir_info." << endl;
    }
    else if (rc == SQLITE_DONE) {
        //cout << "\nsqlite3_step executed successfully." << endl;
    }
    else {
        cout << "\nNot sure what happened." << endl;
    }
        
    sqlite3_finalize(pStmt);

    return 0;  

}


