#ifndef HEADERS_SQLITEHELPER_H_
#define HEADERS_SQLITEHELPER_H_

#define __STDC_WANT_LIB_EXT1__ 1

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstdint>
#include <fstream>
#include <cstring>
#include <sqlite3.h>

class SQLiteHelper {
public:
    enum AttributeToCheck { HashedVal = 1, FileSize = 2, DateModified = 3, NONE = 4};
    enum TableToCheck {Files = 1, Changes = 2};
    
    static int checkIfAttributeIsInChanges(const int file_id, const AttributeToCheck attribute, sqlite3 *db);
    static int checkIfAttributeHasChanged(const int file_id, const AttributeToCheck attribute, const TableToCheck table, sqlite3* db, const char* char_attr = NULL, const int int_attr = 0);

    static char* getCurrentTime();

    static void setDirectoryName(char* dir_name);
    static void setFileName(char* file_name);
    static void setMachineName(char* machine_name);

    static int getDirectoryId(sqlite3* db);
    static int getDirectoryId(sqlite3* db, char* dir_name, char* machine_name);
    static int getFileId(sqlite3* db);
    static int getFileId(sqlite3* db, char* file_name, int dir_id);

    static int saveFileInfo(sqlite3* db, const int dir_id, const char* file_name, const int file_size = 0, const char* date_modified = NULL, const char* hashed_val = NULL, const bool isNew = true, AttributeToCheck attr_to_update = AttributeToCheck::NONE);
    
private:
    static int didHashedValueChange(const char* hashed_val, TableToCheck table, const int file_id, sqlite3* db);
    static int didFileSizeChange(const int file_size, TableToCheck table, const int file_id, sqlite3* db);
    static int didDateModifiedChange(const char* date_modified, TableToCheck table, const int file_id, sqlite3* db);
    static int saveNewFileInfo(sqlite3* db);
    static int saveExistingFileInfo(sqlite3* db, AttributeToCheck attr_to_update, const char* text_val = NULL, const int int_val = 0);
    static int insertTextValueToChanges(sqlite3* db, const int file_id, const char* text_val);
    static int insertIntValueToChanges(sqlite3* db, const int file_id, const int int_val);
    static int updateChangedInFile(sqlite3* db, int file_id, AttributeToCheck attr_to_change);

    
    
};

#endif