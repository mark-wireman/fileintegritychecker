/**
* @file SQLHelper.cpp
*/

#include "../headers/SQLHelper.h"

using namespace::std;

char*  DIRECTORYTABLE_SQL = "CREATE TABLE IF NOT EXISTS directories("  \
      "id INTEGER PRIMARY KEY AUTO_INCREMENT," \
      "dirname           TEXT," \
      "dateadded         TEXT," \
      "status        CHAR(50) DEFAULT 'NEW'," \
      "dateverified      TEXT," \ 
      "machinename       TEXT);";

char* FILETABLE_SQL = "CREATE TABLE IF NOT EXISTS files("  \
      "id INTEGER PRIMARY KEY AUTO_INCREMENT," \
      "dirid           int DEFAULT 0," \
      "dateadded         TEXT," \
      "filename      TEXT," \
      "hashedvalue      CHAR(50) DEFAULT 'NOVAL'," \
      "hashedvaluechanged      int DEFAULT 0," \
      "lastmodified      TEXT," \
      "lastmodifiedchanged      int DEFAULT 0," \
      "filesize    int," \
      "filesizechanged int DEFAULT 0); ";

char* CHANGETABLE_SQL = "CREATE TABLE IF NOT EXISTS changes(" \
      "id INTEGER PRIMARY KEY AUTO_INCREMENT," \
      "fileid int DEFAULT 0," \
      "datechanged CHAR(50) DEFAULT NULL," \
      "attributechanged TEXT," \
      "textvalue TEXT," \
      "intvalue INT DEFAULT -1);";

char* sql_check_if_filesize_changed = "select filesizechanged from files where id=?; ";
char* sql_check_if_filesize_changed_in_files = "select id from files where filesize = ? and id = ?; ";

char* sql_check_if_hashedval_changed = "select hashedvaluechanged from files where id=?; ";
char* sql_check_if_hashedval_changed_in_files = "select id from files where hashedvalue = ? and id = ?; ";

char* sql_check_if_datemodified_changed = "select lastmodifiedchanged from files where id=?; ";
char* sql_check_if_datemodified_changed_in_files = "select id from files where lastmodified = ? and id = ?; ";

char* sql_check_if_value_changed_in_changes_textval = "select id from changes where fileid = ? and textvalue = ?; ";
char* sql_check_if_value_changed_in_changes_intval = "select id from changes where fileid = ? and intvalue = ?; ";

char* sql_get_file_id = "select id from files where filename=? and dirid=?; ";
char* sql_get_directory_id = "select id from directories where dirname=? and machinename= ?; ";

char* sql_save_newfile_nohashedval = "insert into files (dirid, dateadded, filename, filesize, lastmodified) VALUES (?,?,?,?,?); ";
char* sql_save_newfile_hashedval = "insert into files (dirid, dateadded, filename, filesize, lastmodified, hashval) VALUES (?,?,?,?,?,?); ";

char* sql_save_textval_in_changes = "INSERT INTO changes (fileid,datechanged,textvalue,attributechanged) VALUES (?,?,?,?); ";
char* sql_save_intval_in_changes = "INSERT INTO changes (fileid,datechanged,intvalue,attributechanged) VALUES (?,?,?,?); ";

char* sql_set_inchanges_hashedval = "UPDATE files SET hashedvaluechanged=1 WHERE id=?; ";
char* sql_set_inchanges_filesize = "UPDATE files SET filesizechanged=1 WHERE id=?; ";
char* sql_set_inchanges_lastmodified = "UPDATE files SET lastmodifiedchanged=1 WHERE id=?; ";

char* _dirname;
char* _filename;
char* _machinename;
char* _datemodified;
char* _hashedval;
int _filesize;
int _dirid;

sqlite3_stmt* pStmt;

char* SQLHelper::getCreateTableSQL(TABLE_TO_CREATE _newtable) {
    char* retVal = NULL;

    switch(_newtable) {
        case (TABLE_TO_CREATE::CHANGES):
            retVal = (char*)malloc(strlen(CHANGETABLE_SQL) + 1);
            strcpy(retVal, CHANGETABLE_SQL);
            break;
        case (TABLE_TO_CREATE::DIRECTORIES):
            retVal = (char*)malloc(strlen(DIRECTORYTABLE_SQL) + 1);
            strcpy(retVal, DIRECTORYTABLE_SQL);
            break;
        case (TABLE_TO_CREATE::FILES):
            retVal = (char*)malloc(strlen(FILETABLE_SQL) + 1);
            strcpy(retVal, FILETABLE_SQL);
            break;
        default:
            break;
    }

    return retVal;
}


int SQLHelper::checkIfAttributeHasChanged(const int file_id, const AttributeToCheck attribute, const TableToCheck table, sqlite3* db, const char* char_attr, const int int_attr) {
    int retVal = 0;
    
    switch(attribute) {
    case 1: //Hashedval
        retVal = didHashedValueChange(char_attr, table, file_id, db);
        break;
    case 2: //Filesize
        retVal = didFileSizeChange(int_attr, table, file_id, db);
        break;
    case 3: //Datemodified
        retVal = didDateModifiedChange(char_attr, table, file_id, db);
        break;
    default:
        retVal = -1;
    }

    //fprintf(stderr, "\nReturn value from checkIfAttributeHasChanged is %d", retVal);
    
    return retVal;
}

template <class T>
int SQLHelper::checkIfAttributeIsInChanges(const int file_id, const AttributeToCheck attribute, T *connection_obj)
{
    cout << "connection_obj type name:\t";
    cout << abi::__cxa_demangle(typeid(connection_obj).name(), nullptr, nullptr, nullptr) << endl;

    switch (abi::__cxa_demangle(typeid(connection_obj).name(), nullptr, nullptr, nullptr)) {
    case "mysql*":
        break;
    case "sqlite3*":
        break;
    default:
        break;
    }
    return 0;
}



int SQLHelper::checkIfAttributeIsInChanges(const int file_id, const AttributeToCheck attribute, sqlite3* db) {
    char* zErrMsg = 0;
    int rc;
    sqlite3_stmt* pStmt;
    char* sql_stmt;
    
    int retVal = 0;

    switch (attribute) {
    case 1: //Hashedval
        rc = sqlite3_prepare_v3(db, sql_check_if_hashedval_changed, -1, 0, &pStmt, NULL);
        break;
    case 2: //Filesize
        rc = sqlite3_prepare_v3(db, sql_check_if_filesize_changed, -1, 0, &pStmt, NULL);
        break;
    case 3: //Datemodified
        rc = sqlite3_prepare_v3(db, sql_check_if_datemodified_changed, -1, 0, &pStmt, NULL);
        break;
    default:
        retVal = -1;
    }

    if (rc == SQLITE_OK) {
        sqlite3_bind_int(pStmt, 1, file_id);
    }
    else {

        fprintf(stderr, "checkIfAttributeIsInChanges::Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    while (sqlite3_step(pStmt) == SQLITE_ROW)
    {
        retVal = sqlite3_column_int(pStmt, 0);
    }

    return retVal;
}

template<class T>
int SQLHelper::didHashedValueChange(const char* hashed_val, TableToCheck table, const int file_id, T* connection_obj)
{
    return 0;
}

int SQLHelper::didHashedValueChange(const char* hashed_val, TableToCheck table, const int file_id, sqlite3* db) {
    int retVal = 0;

    char* zErrMsg = 0;
    int rc;
    sqlite3_stmt* pStmt;
    

    if (table == 1) {
        rc = sqlite3_prepare_v3(db, sql_check_if_hashedval_changed_in_files, -1, 0, &pStmt, NULL);
    }
    else if (table == 2) {
        rc = sqlite3_prepare_v3(db, sql_check_if_value_changed_in_changes_textval, -1, 0, &pStmt, NULL);
    }
    else
    {
        return -1;
    }
    

    if (rc == SQLITE_OK) {
        
        sqlite3_bind_text(pStmt, 1, hashed_val, -1, NULL);
        sqlite3_bind_int(pStmt, 2, file_id);
        
    }
    else {

        fprintf(stderr, "didHashedValueChange::Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    while (sqlite3_step(pStmt) == SQLITE_ROW)
    {
        retVal = sqlite3_column_int(pStmt, 0);
    }

    sqlite3_finalize(pStmt);

    
    return retVal;
}

int SQLHelper::didFileSizeChange(const int file_size, TableToCheck table, const int file_id, sqlite3* db) {
    int retVal = 0;

    char* zErrMsg = 0;
    int rc;
    sqlite3_stmt* pStmt;
    

    if (table == 1) {
        rc = sqlite3_prepare_v3(db, sql_check_if_filesize_changed_in_files, -1, 0, &pStmt, NULL);
    }
    else if (table == 2) {
        rc = sqlite3_prepare_v3(db, sql_check_if_value_changed_in_changes_intval, -1, 0, &pStmt, NULL);
    }
    else
    {
        return -1;
    }


    if (rc == SQLITE_OK) {
        sqlite3_bind_int(pStmt, 1, file_size);
        sqlite3_bind_int(pStmt, 2, file_id);
    }
    else {

        fprintf(stderr, "didFileSizeChange::Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    while (sqlite3_step(pStmt) == SQLITE_ROW)
    {
        retVal = sqlite3_column_int(pStmt, 0);
    }

    sqlite3_finalize(pStmt);

    return retVal;
}

int SQLHelper::didDateModifiedChange(const char* date_modified, TableToCheck table, const int file_id, sqlite3* db) {
    int retVal = 0;

    char* zErrMsg = 0;
    int rc;
    sqlite3_stmt* pStmt;
   

    if (table == 1) {
        rc = sqlite3_prepare_v3(db, sql_check_if_datemodified_changed_in_files, -1, 0, &pStmt, NULL);
    }
    else if (table == 2) {
        rc = sqlite3_prepare_v3(db, sql_check_if_value_changed_in_changes_textval, -1, 0, &pStmt, NULL);
    }
    else
    {
        return -1;
    }


    if (rc == SQLITE_OK) {
        sqlite3_bind_text(pStmt, 1, date_modified, -1, NULL);
        sqlite3_bind_int(pStmt, 2, file_id);
    }
    else {

        fprintf(stderr, "didDateModifiedChange::Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    while (sqlite3_step(pStmt) == SQLITE_ROW)
    {
        retVal = sqlite3_column_int(pStmt, 0);
    }

    sqlite3_finalize(pStmt);

    return retVal;
}


void SQLHelper::setDirectoryName(char* dir_name) {
    _dirname = (char*)malloc(strlen(dir_name) + 1);
    strcpy(_dirname, dir_name);
}

void SQLHelper::setFileName(char* file_name) {
    _filename = (char*)malloc(strlen(file_name) + 1);
    strcpy(_filename, file_name);
}

void SQLHelper::setMachineName(char* machine_name) {
    _machinename = (char*)malloc(strlen(machine_name) + 1);
    strcpy(_machinename, machine_name);
}

int SQLHelper::getDirectoryId(sqlite3* db) {
    char* zErrMsg = 0;
    int rc;
    sqlite3_stmt* pStmt;
    int retVal = 0;

    rc = sqlite3_prepare_v3(db, sql_get_directory_id, -1, 0, &pStmt, NULL);

    if (rc == SQLITE_OK) {
        sqlite3_bind_text(pStmt, 1, _dirname, -1, NULL);
        sqlite3_bind_text(pStmt, 2, _machinename, -1, NULL);
    }
    else {

        fprintf(stderr, "getDirectoryId::Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    while (sqlite3_step(pStmt) == SQLITE_ROW)
    {
        retVal = sqlite3_column_int(pStmt, 0);
    }

    sqlite3_finalize(pStmt);

    return retVal;
}

int SQLHelper::getDirectoryId(sqlite3* db, char* dir_name, char* machine_name) {
    int retVal = 0;

    _dirname = (char*)malloc(strlen(dir_name) + 1);
    strcpy(_dirname, dir_name);

    _machinename = (char*)malloc(strlen(machine_name) + 1);
    strcpy(_machinename, machine_name);

    retVal = getDirectoryId(db);

    return retVal;

}

int SQLHelper::getFileId(sqlite3* db) {
    char* zErrMsg = 0;
    int rc;
    sqlite3_stmt* pStmt;
    int retVal = 0;

    rc = sqlite3_prepare_v3(db, sql_get_file_id, -1, 0, &pStmt, NULL);

    if (rc == SQLITE_OK) {
        sqlite3_bind_text(pStmt, 1, _filename, -1, NULL);
        sqlite3_bind_int(pStmt, 2, _dirid);
    }
    else {

        fprintf(stderr, "getFileId::Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    while (sqlite3_step(pStmt) == SQLITE_ROW)
    {
        retVal = sqlite3_column_int(pStmt, 0);
    }

    sqlite3_finalize(pStmt);

    return retVal;
}

int SQLHelper::getFileId(sqlite3* db, char* file_name, int dir_id) {
    int retVal = 0;

    _filename = (char*)malloc(strlen(file_name) + 1);
    strcpy(_filename, file_name);
    _dirid = dir_id;

    retVal = getFileId(db);

    return retVal;
}


/**
 * @brief 
 * 
 * @return char* 
 */
char* SQLHelper::getCurrentTime() {
    string retVal;
	char* char_array;

	time_t t = time(0);   // get time now
    tm* now = localtime(&t);
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

int SQLHelper::saveFileInfo(sqlite3* db, const int dir_id, const char* file_name, const int file_size, const char* date_modified, const char* hashed_val, const bool isNew, AttributeToCheck attr_to_update) {
    int retVal = 0;
    int hasChanged = 0;

    _dirid = dir_id;
    _filename = (char*)malloc(strlen(file_name)+1);
    strcpy(_filename,file_name);
    _filesize = file_size;

    if (date_modified != NULL) {
        _datemodified = (char*)malloc(strlen(date_modified) + 1);
        strcpy(_datemodified, date_modified);
    }
    
    if (hashed_val != NULL) {
        //fprintf(stderr, "\nGetting ready to store hashedvalue into _hashedval with value of %s\n", _hashedval);
        _hashedval = (char*)malloc(strlen(hashed_val) + 1);
        strcpy(_hashedval, (char*)hashed_val);
    }
    
    if (isNew) {
        saveNewFileInfo(db);
    }
    else {
        switch (attr_to_update)
        {
        case 1:
            //fprintf(stderr, "\nGetting ready to save hashedvalue of %s\n", _hashedval);
            saveExistingFileInfo(db, attr_to_update,_hashedval);
            break;
        case 2:
            saveExistingFileInfo(db, attr_to_update,NULL,_filesize);
            break;
        case 3:
            saveExistingFileInfo(db, attr_to_update, _datemodified);
            break;
        default:
            break;
        }
        //saveExistingFileInfo(db, attr_to_update);
    }

    return retVal;
}

int SQLHelper::saveNewFileInfo(sqlite3* db) {
    int retVal = 0;
    char* zErrMsg = 0;
    int rc;
    sqlite3_stmt* pStmt;
    
    char* current_time = getCurrentTime();

    if (_hashedval == NULL) {
        rc = sqlite3_prepare_v3(db, sql_save_newfile_nohashedval, -1, 0, &pStmt, NULL);
    }
    else {
        rc = sqlite3_prepare_v3(db, sql_save_newfile_hashedval, -1, 0, &pStmt, NULL);
    }

    //dirid, dateadded, filename, filesize, lastmodified
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(pStmt, 1, _dirid);
        sqlite3_bind_text(pStmt, 2, current_time, -1, NULL);
        sqlite3_bind_text(pStmt, 3, _filename, -1, NULL);
        sqlite3_bind_int(pStmt, 4, _filesize);
        sqlite3_bind_text(pStmt, 5, _datemodified, -1, NULL);
        
        if (_hashedval != NULL) {
            sqlite3_bind_text(pStmt, 6, _hashedval, -1, NULL);
        }
    }
    else {

        //fprintf(stderr, "saveNewFileInfo::Failed to execute statement: %s\n", sqlite3_errmsg(db));
        retVal = -1;
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
        printf("\nIn SQLHelper::saveNewFileInfo execution failed: %s\n", sqlite3_errmsg(db));
    }
    else if (rc == SQLITE_MISUSE) {
        cout << "\nA MISUSE has been deteced in SQLHelper::saveNewFileInfo." << endl;
    }
    else if (rc == SQLITE_DONE) {
        cout << "\nsqlite3_step executed successfully in SQLHelper::saveNewFileInfo." << endl;
    }
    else {
        cout << "\nNot sure what happened in SQLHelper::saveNewFileInfo." << endl;
    }

    sqlite3_finalize(pStmt);
    
    return retVal;
}

int SQLHelper::saveExistingFileInfo(sqlite3* db, AttributeToCheck attr_to_update, const char* text_val, const int int_val) {
    int retVal = 0;

    char* zErrMsg = 0;
    int rc;
    sqlite3_stmt* pStmt;
    
    char* current_time = getCurrentTime();
    int fileId = getFileId(db);
    
    int in_changes = checkIfAttributeIsInChanges(fileId, attr_to_update, db);
    int has_changes = 0;

    switch (attr_to_update)
    {
    case 1:

        if (!in_changes) {
            has_changes = checkIfAttributeHasChanged(fileId, attr_to_update, TableToCheck::Files, db, text_val);
        }
        else {
            has_changes = checkIfAttributeHasChanged(fileId, attr_to_update, TableToCheck::Changes, db, text_val);
        }
        
        if (has_changes == 0) {
            //fprintf(stderr, "\nInside has_changes with value of %d and text value %s\n", has_changes, text_val);
            retVal = insertTextValueToChanges(db, fileId, text_val,"hashedvalue");
        }
        break;
    case 2:
        
        if (!in_changes) {
            has_changes = checkIfAttributeHasChanged(fileId, attr_to_update, TableToCheck::Files, db, NULL, int_val);
        }
        else {
            has_changes = checkIfAttributeHasChanged(fileId, attr_to_update, TableToCheck::Changes, db, NULL, int_val);
        }
        
        if (has_changes == 0) {
            //fprintf(stderr, "\nInside has_changes with value of %d and int value %d\n", has_changes, int_val);
            retVal = insertIntValueToChanges(db, fileId, int_val, "filesize");
        }
        break;
    case 3:

        if (!in_changes) {
            has_changes = checkIfAttributeHasChanged(fileId, attr_to_update, TableToCheck::Files, db, text_val);
        }
        else {
            has_changes = checkIfAttributeHasChanged(fileId, attr_to_update, TableToCheck::Changes, db, text_val);
        }
        
        if (has_changes == 0) {
            //fprintf(stderr, "\nInside has_changes with value of %d and text value %s\n", has_changes, text_val);
            retVal = insertTextValueToChanges(db, fileId, text_val,"lastmodified");
        }
        break;
    default:
        break;
    }

    if (!in_changes && has_changes == 0) {
        retVal = updateChangedInFile(db, fileId, attr_to_update);
    }

    return retVal;
}

int SQLHelper::insertTextValueToChanges(sqlite3* db, const int file_id, const char* text_val, const char* attr_changed) {
    char* zErrMsg = 0;
    int rc;
    sqlite3_stmt* pStmt;
    char* current_time = getCurrentTime();
    
    rc = sqlite3_prepare_v3(db, sql_save_textval_in_changes, -1, 0, &pStmt, NULL);

    if (rc == SQLITE_OK) {
        sqlite3_bind_int(pStmt, 1, file_id);
        sqlite3_bind_text(pStmt, 2, current_time, -1, NULL);
        sqlite3_bind_text(pStmt, 3, text_val, -1, NULL);
        sqlite3_bind_text(pStmt, 4, attr_changed, -1, NULL);
    }
    else {

        fprintf(stderr, "insertTextValueToChanges::Failed to execute statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(pStmt);
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
        printf("\nIn insertTextValuetoChanges execution failed: %s\n", sqlite3_errmsg(db));
    }
    else if (rc == SQLITE_MISUSE) {
        cout << "\nA MISUSE has been deteced in insertTextValuetoChanges." << endl;
    }
    else if (rc == SQLITE_DONE) {
        cout << "\nsqlite3_step executed successfully." << endl;
    }
    else {
        cout << "\nNot sure what happened in insertTextValuetoChanges." << endl;
    }

    sqlite3_finalize(pStmt);

    return 0;
}

int SQLHelper::insertIntValueToChanges(sqlite3* db, const int file_id, const int int_val, const char* attr_changed) {
    char* zErrMsg = 0;
    int rc;
    sqlite3_stmt* pStmt;
    char* current_time = getCurrentTime();

    rc = sqlite3_prepare_v3(db, sql_save_intval_in_changes, -1, 0, &pStmt, NULL);

    if (rc == SQLITE_OK) {
        sqlite3_bind_int(pStmt, 1, file_id);
        sqlite3_bind_text(pStmt, 2, current_time, -1, NULL);
        sqlite3_bind_int(pStmt, 3, int_val);
        sqlite3_bind_text(pStmt, 4, attr_changed, -1, NULL);
    }
    else {

        fprintf(stderr, "insertIntValueToChanges::Failed to execute statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(pStmt);
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
        printf("\nIn insertIntValueToChanges execution failed: %s\n", sqlite3_errmsg(db));
    }
    else if (rc == SQLITE_MISUSE) {
        cout << "\nA MISUSE has been deteced in insertIntValueToChanges." << endl;
    }
    else if (rc == SQLITE_DONE) {
        cout << "\nsqlite3_step executed successfully." << endl;
    }
    else {
        cout << "\nNot sure what happened in insertIntValueToChanges." << endl;
    }

    sqlite3_finalize(pStmt);

    return 0;
}

int SQLHelper::updateChangedInFile(sqlite3* db, int file_id, AttributeToCheck attr_to_change) {
    char* zErrMsg = 0;
    int rc;
    sqlite3_stmt* pStmt;
    int retVal = 0;
    char* current_time = getCurrentTime();
    char* sql_stmt;
    
    switch (attr_to_change)
    {
    case 1:
        rc = sqlite3_prepare_v3(db, sql_set_inchanges_hashedval, -1, 0, &pStmt, NULL);
        break;
    case 2:
        rc = sqlite3_prepare_v3(db, sql_set_inchanges_filesize, -1, 0, &pStmt, NULL);
        break;
    case 3:
        rc = sqlite3_prepare_v3(db, sql_set_inchanges_lastmodified, -1, 0, &pStmt, NULL);
        break;
    default:
        return -1;
        cout << "Unknown attribute. Nothing saved." << endl;
        break;
    }

    if (rc == SQLITE_OK) {
        sqlite3_bind_int(pStmt, 1, file_id);
    }
    else {
        fprintf(stderr, "updateChangedInFiles:Failed to execute statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(pStmt);
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
        printf("\nIn updateChangedInFiles execution failed: %s\n", sqlite3_errmsg(db));
    }
    else if (rc == SQLITE_MISUSE) {
        cout << "\nA MISUSE has been deteced in updateChangedInFiles." << endl;
    }
    else if (rc == SQLITE_DONE) {
        cout << "\nsqlite3_step executed successfully." << endl;
    }
    else {
        cout << "\nNot sure what happened in updateChangedInFiles." << endl;
    }

    sqlite3_finalize(pStmt);

    return 0;
}