#include "../headers/sqlite3controller.h"

using namespace std;
namespace fs = std::filesystem;

sqlite3 *db;
char* sqlite3_dbasename;
char* sqlite3_mname;

sqlite3controller::sqlite3controller() {}

sqlite3controller::sqlite3controller(char* databasename, char* machinename) {
    sqlite3_mname = machinename;
    sqlite3_dbasename = databasename;
}


sqlite3controller::~sqlite3controller() {}


void sqlite3controller::setDATABASENAME(char* databasename) {
    sqlite3_dbasename = databasename;
}


void sqlite3controller::setMACHINENAME(char* machinename) {
    sqlite3_mname = machinename;
}


char* sqlite3controller::getDATABASENAME() {
    return sqlite3_dbasename;
}



void sqlite3controller::initdb() {
    char *zErrMsg = 0;
    int rc;
   
    /* Open database */
    rc = sqlite3_open_v2(sqlite3_dbasename, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
   
    if( rc ) {
        fprintf(stderr, "\nIn initdb can't open database: %s\n", sqlite3_errmsg(db));
    } else {
        fprintf(stderr, "\nOpened database successfully\n");
        createTables();
    }

}

void sqlite3controller::closedb() {
    sqlite3_close_v2(db);
}


void sqlite3controller::createTables() {
    int rc;
    char *dirtable_sql;
    char *filetable_sql;
    char *changestable_sql;
    char *zErrMsg = 0;

    dirtable_sql = "CREATE TABLE IF NOT EXISTS directories("  \
      "id INTEGER PRIMARY KEY AUTOINCREMENT," \
      "dirname           TEXT," \
      "dateadded         TEXT," \
      "status        CHAR(50) DEFAULT 'NEW'," \
      "dateverified      TEXT," \ 
      "machinename       TEXT);";

    filetable_sql = "CREATE TABLE IF NOT EXISTS files("  \
      "id INTEGER PRIMARY KEY AUTOINCREMENT," \
      "dirid           int DEFAULT 0," \
      "dateadded         TEXT," \
      "filename      TEXT," \
      "hashedvalue      TEXT DEFAULT 'NOVAL'," \
      "hashedvaluechanged      int DEFAULT 0," \
      "lastmodified      TEXT," \
      "lastmodifiedchanged      int DEFAULT 0," \
      "filesize    int," \
      "filesizechanged int DEFAULT 0); ";

    changestable_sql = "CREATE TABLE IF NOT EXISTS changes(" \
      "id INTEGER PRIMARY KEY AUTOINCREMENT," \
      "fileid int DEFAULT 0," \
      "datechanged TEXT DEFAULT NULL," \
      "attributechanged TEXT DEFAULT NULL," \
      "textvalue TEXT DEFAULT NULL," \
      "intvalue INT DEFAULT -1);";

    /* Execute SQL statement */
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
int sqlite3controller::checkIfDirectoryExist(const char* dirname) {
    char *zErrMsg = 0;
    int rc;
    char *sql = "SELECT id FROM directories WHERE dirname = ? AND machinename = ?; ";
    sqlite3_stmt * pStmt;
    int retVal = 0;

    rc = sqlite3_prepare_v3(db, sql, -1, 0, &pStmt, NULL);
    
    if (rc == SQLITE_OK) {        
        sqlite3_bind_text(pStmt, 1, dirname, -1, NULL);
        sqlite3_bind_text(pStmt, 2, sqlite3_mname, -1, NULL);
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
int sqlite3controller::checkIfFileExist(const char* fname, const char* dirname) {
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
        sqlite3_bind_text(pStmt, 3, sqlite3_mname, -1, NULL);
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
int sqlite3controller::save_file_info(const char* fname, const char* dirname, char* lastmodified, uintmax_t filesize, const char* hashval) {
    

    int dir_id = checkIfDirectoryExist(dirname);
    int fileExists = SQLiteHelper::getFileId(db, (char*)fname, dir_id);

    if(fileExists > 0) {
        if (lastmodified != NULL) {
            SQLiteHelper::saveFileInfo(db, dir_id, fname, filesize,lastmodified, NULL, false, SQLiteHelper::AttributeToCheck::DateModified);
        }
        if (hashval != NULL) {
            //fprintf(stderr, "\nReceived hashedvalue of %s\n", hashval);
            SQLiteHelper::saveFileInfo(db, dir_id, fname, filesize, NULL, hashval, false, SQLiteHelper::AttributeToCheck::HashedVal);
        }
        if (filesize > 0) {
            SQLiteHelper::saveFileInfo(db, dir_id, fname, filesize, lastmodified, NULL, false, SQLiteHelper::AttributeToCheck::FileSize);
        }
    } else if(fileExists == 0) {
        SQLiteHelper::saveFileInfo(db, dir_id, fname, filesize, lastmodified, hashval, true);
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
int sqlite3controller::save_dir_info(const char* dirname) {
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
            sqlite3_bind_text(pStmt, 1, SQLiteHelper::getCurrentTime(), -1, NULL);
            sqlite3_bind_int(pStmt, 2, dirExists);
        } else {
            sqlite3_bind_text(pStmt, 1, dirname, -1, NULL);
            sqlite3_bind_text(pStmt, 2, SQLiteHelper::getCurrentTime(), -1, NULL);
            sqlite3_bind_text(pStmt, 3, sqlite3_mname, -1, NULL);
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
        cout << "\nsqlite3_step executed successfully." << endl;
    }
    else {
        cout << "\nNot sure what happened." << endl;
    }
        
    sqlite3_finalize(pStmt);

    return 0;  

}


