/**
* @file SQLHelper.h
*/

#ifndef HEADERS_SQLHelper_H_
#define HEADERS_SQLHelper_H_

#define __STDC_WANT_LIB_EXT1__ 1

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstdint>
#include <fstream>
#include <cstring>
#include <sqlite3.h>
#include <cxxabi.h>
#include <map>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

/**
 * @brief The SQL Helper class for the SQLite database.
*/
class SQLHelper {
public:
    //enum TableToCreate {Directories = 1, Files = 2, Changes = 3};
    enum TableName {Directories = 1, Files = 2, Changes = 3};
    template <typename T> char* returnName(T x);

    static char* getCreateTableSQL(TableName _newtable);

    /**
     * @brief Enumerator used to set the file attribute to check for changes or to update the value for.
    */
    enum AttributeToCheck { HashedVal = 1, FileSize = 2, DateModified = 3, NONE = 4};
    
    /**
     * @brief Enumerator used to set the table to check for file attribute changes.
    */
    //enum TableToCheck {Files = 1, Changes = 2};
    
    /**
     * @brief Determines if the provided attribute is in the changes table.
     * @param file_id The id of the file.
     * @param attribute The attribute to check.
     * @param db The sqlite3 database object.
     * @return 1 if is in the changes table, 0 if not
     
    */
    static int checkIfAttributeIsInChanges(const int file_id, const AttributeToCheck attribute, sqlite3 *db);
    static int checkIfAttributeIsInChanges(const int file_id, const AttributeToCheck attribute, sql::Connection *conn);
    
    
    /**
     * @brief Determines if the provided attribute has changed its value.
     * <div>The logic in the function will first determine the attribute to check using the attribute parameter and then will call the appropriate function to check if the attribute did change.</div>
     * @param file_id The id of the file.
     * @param attribute The attribute to check.
     * @param table The table to check based on whether the attribute is in the changes or the files table.
     * @param db The sqlite3 database object.
     * @param char_attr The character value attribute to check. The default is NULL.
     * @param int_attr The integer value attribute to check. The default is 0
     * @see AttributeToCheck
     * @see TableToCheck
     * @see didHashedValueChange()
     * @see didFileSizeChange()
     * @see didDateModifiedChange()
     * @return 1 if changed, 0 if not
    */
    static int checkIfAttributeHasChanged(const int file_id, const AttributeToCheck attribute, const TableName table, sqlite3* db, const char* char_attr = NULL, const int int_attr = 0);
    static int checkIfAttributeHasChanged(const int file_id, const AttributeToCheck attribute, const TableName table, sql::Connection* conn, const char* char_attr = NULL, const int int_attr = 0);

    /**
     * @brief Will retrieve the current time.
     * @return Current time in %B %d, %Y %T format. Example: December 12, 2020 20:10:05
    */
    static char* getCurrentTime();

    /**
     * @brief Sets the relative directory name.
     * @param dir_name 
    */
    static void setDirectoryName(char* dir_name);
    
    /**
     * @brief Sets the absolute file name.
     * @param file_name 
    */
    static void setFileName(char* file_name);
    
    /**
     * @brief Sets the machine name where the scanner is running.
     * @param machine_name 
    */
    static void setMachineName(char* machine_name);

    /**
     * @brief Retrieves the id of the directory from the directories table.
     * <div><b>PRECONDITION:</b> Call to setDirectoryName() to set the directory to check.</div>
     * @param db sqlite3 database object
     * @return 0 if directory does not exist in directories table, otherwise the directory id will be returned.
    */
    static int getDirectoryId(sqlite3* db);
    static int getDirectoryId(sql::Connection* conn);
    
    /**
     * @brief Retrieves the id of the directory from the directories table.
     * @param db sqlite3 database object
     * @param dir_name Relative name of the directory.
     * @param machine_name The machine name where the scanner is running.
     * @return 0 if directory does not exist in directories table, otherwise the directory id will be returned.
    */
    static int getDirectoryId(sqlite3* db, char* dir_name, char* machine_name);
    static int getDirectoryId(sql::Connection* conn, char* dir_name, char* machine_name);
    
    /**
     * @brief Returns the id of the file if the file exists in the files table.
     * @param db The sqlite3 database object.
     * @return 0 if file does not exist in files table, otherwise the file id will be returned.
    */
    static int getFileId(sqlite3* db);
    static int getFileId(sql::Connection* conn);
    
    /**
     * @brief Returns the id of the file if the file exists in the files table.
     * <div><b>NOTE:</b> Will call the getFileId(sqlite3* db) method after setting the directory id and file name at the class level.</div>
     * @param db sqlite3 database object
     * @param file_name Absolute name of the file
     * @param dir_id Id of the Directory where the file is located
     * @return 0 if file does not exist in files table, otherwise the file id will be returned.
    */
    static int getFileId(sqlite3* db, char* file_name, int dir_id);
    static int getFileId(sql::Connection* conn, char* file_name, int dir_id);

    /**
     * @brief Entry point to saving the file information.
     * <div></div>
     * @param db sqlite3 database object.
     * @param dir_id Id of the directory from the directories table.
     * @param file_name Absolute name of the file.
     * @param file_size File size of the file in bytes. Default value is 0.
     * @param date_modified Date the file was last modified. Default value is NULL.
     * @param hashed_val 
     * @param isNew Determines if the file information being added is for a new file or an existing file. Default value is true. 
     * @param attr_to_update The attribute to update if the isNew = false. Default value is NONE.
     * @return 
     * @see saveNewFileInfo()
     * @see saveExistingFileInfo()
     * @see AttributeToCheck
    */
    static int saveFileInfo(sqlite3* db, const int dir_id, const char* file_name, const int file_size = 0, const char* date_modified = NULL, const char* hashed_val = NULL, const bool isNew = true, AttributeToCheck attr_to_update = AttributeToCheck::NONE);
    static int saveFileInfo(sql::Connection* conn, const int dir_id, const char* file_name, const int file_size = 0, const char* date_modified = NULL, const char* hashed_val = NULL, const bool isNew = true, AttributeToCheck attr_to_update = AttributeToCheck::NONE);
    
private:
    /**
     * @brief 
     * @param hashed_val 
     * @param table 
     * @param file_id 
     * @param db 
     * @return 
    */
    static int didHashedValueChange(const char* hashed_val, TableName table, const int file_id, sqlite3* db);
    static int didHashedValueChange(const char* hashed_val, TableName table, const int file_id, sql::Connection* conn);
    
    /**
     * @brief 
     * @param file_size 
     * @param table 
     * @param file_id 
     * @param db 
     * @return 
    */
    static int didFileSizeChange(const int file_size, TableName table, const int file_id, sqlite3* db);
    static int didFileSizeChange(const int file_size, TableName table, const int file_id, sql::Connection* conn);
    
    /**
     * @brief 
     * @param date_modified 
     * @param table 
     * @param file_id 
     * @param db 
     * @return 
    */
    static int didDateModifiedChange(const char* date_modified, TableName table, const int file_id, sqlite3* db);
    static int didDateModifiedChange(const char* date_modified, TableName table, const int file_id, sql::Connection* conn);
    
    /**
     * @brief 
     * @param db 
     * @return 
    */
    static int saveNewFileInfo(sqlite3* db);
    static int saveNewFileInfo(sql::Connection* conn);
    
    /**
     * @brief 
     * @param db 
     * @param attr_to_update 
     * @param text_val 
     * @param int_val 
     * @return 
    */
    static int saveExistingFileInfo(sqlite3* db, AttributeToCheck attr_to_update, const char* text_val = NULL, const int int_val = 0);
    static int saveExistingFileInfo(sql::Connection* conn, AttributeToCheck attr_to_update, const char* text_val = NULL, const int int_val = 0);
    
    /**
     * @brief 
     * @param db 
     * @param file_id 
     * @param text_val 
     * @param attr_changed 
     * @return 
    */
    static int insertTextValueToChanges(sqlite3* db, const int file_id, const char* text_val, const char* attr_changed);
    static int insertTextValueToChanges(sql::Connection* conn, const int file_id, const char* text_val, const char* attr_changed);
    
    /**
     * @brief 
     * @param db 
     * @param file_id 
     * @param int_val 
     * @param attr_changed 
     * @return 
    */
    static int insertIntValueToChanges(sqlite3* db, const int file_id, const int int_val, const char* attr_changed);
    static int insertIntValueToChanges(sql::Connection* conn, const int file_id, const int int_val, const char* attr_changed);
    
    /**
     * @brief 
     * @param db 
     * @param file_id 
     * @param attr_to_change 
     * @return 
    */
    static int updateChangedInFile(sqlite3* db, int file_id, AttributeToCheck attr_to_change);
    static int updateChangedInFile(sql::Connection* conn, int file_id, AttributeToCheck attr_to_change);

};

#endif


