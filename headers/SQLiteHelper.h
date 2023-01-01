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

/**
 * @brief The SQL Helper class for the SQLite database.
*/
class SQLiteHelper {
public:
    /**
     * @brief Enumerator used to set the file attribute to check for changes or to update the value for.
    */
    enum AttributeToCheck { HashedVal = 1, FileSize = 2, DateModified = 3, NONE = 4};
    
    /**
     * @brief Enumerator used to set the table to check for file attribute changes.
    */
    enum TableToCheck {Files = 1, Changes = 2};
    
    /**
     * @brief Determines if the provided attribute is in the changes table.
     * @param file_id The id of the file.
     * @param attribute The attribute to check.
     * @param db The sqlite3 database object.
     * @return 1 if is in the changes table, 0 if not
     
    */
    static int checkIfAttributeIsInChanges(const int file_id, const AttributeToCheck attribute, sqlite3 *db);
    
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
    static int checkIfAttributeHasChanged(const int file_id, const AttributeToCheck attribute, const TableToCheck table, sqlite3* db, const char* char_attr = NULL, const int int_attr = 0);

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
    
    /**
     * @brief Retrieves the id of the directory from the directories table.
     * @param db sqlite3 database object
     * @param dir_name Relative name of the directory.
     * @param machine_name The machine name where the scanner is running.
     * @return 0 if directory does not exist in directories table, otherwise the directory id will be returned.
    */
    static int getDirectoryId(sqlite3* db, char* dir_name, char* machine_name);
    
    /**
     * @brief 
     * @param db 
     * @return 
    */
    static int getFileId(sqlite3* db);
    
    /**
     * @brief 
     * @param db 
     * @param file_name 
     * @param dir_id 
     * @return 
    */
    static int getFileId(sqlite3* db, char* file_name, int dir_id);

    /**
     * @brief 
     * @param db 
     * @param dir_id 
     * @param file_name 
     * @param file_size 
     * @param date_modified 
     * @param hashed_val 
     * @param isNew 
     * @param attr_to_update 
     * @return 
     * @see saveNewFileInfo()
     * @see saveExistingFileInfo()
     * @see AttributeToCheck
    */
    static int saveFileInfo(sqlite3* db, const int dir_id, const char* file_name, const int file_size = 0, const char* date_modified = NULL, const char* hashed_val = NULL, const bool isNew = true, AttributeToCheck attr_to_update = AttributeToCheck::NONE);
    
private:
    /**
     * @brief 
     * @param hashed_val 
     * @param table 
     * @param file_id 
     * @param db 
     * @return 
    */
    static int didHashedValueChange(const char* hashed_val, TableToCheck table, const int file_id, sqlite3* db);
    
    /**
     * @brief 
     * @param file_size 
     * @param table 
     * @param file_id 
     * @param db 
     * @return 
    */
    static int didFileSizeChange(const int file_size, TableToCheck table, const int file_id, sqlite3* db);
    
    /**
     * @brief 
     * @param date_modified 
     * @param table 
     * @param file_id 
     * @param db 
     * @return 
    */
    static int didDateModifiedChange(const char* date_modified, TableToCheck table, const int file_id, sqlite3* db);
    
    /**
     * @brief 
     * @param db 
     * @return 
    */
    static int saveNewFileInfo(sqlite3* db);
    
    /**
     * @brief 
     * @param db 
     * @param attr_to_update 
     * @param text_val 
     * @param int_val 
     * @return 
    */
    static int saveExistingFileInfo(sqlite3* db, AttributeToCheck attr_to_update, const char* text_val = NULL, const int int_val = 0);
    
    /**
     * @brief 
     * @param db 
     * @param file_id 
     * @param text_val 
     * @param attr_changed 
     * @return 
    */
    static int insertTextValueToChanges(sqlite3* db, const int file_id, const char* text_val, const char* attr_changed);
    
    /**
     * @brief 
     * @param db 
     * @param file_id 
     * @param int_val 
     * @param attr_changed 
     * @return 
    */
    static int insertIntValueToChanges(sqlite3* db, const int file_id, const int int_val, const char* attr_changed);
    
    /**
     * @brief 
     * @param db 
     * @param file_id 
     * @param attr_to_change 
     * @return 
    */
    static int updateChangedInFile(sqlite3* db, int file_id, AttributeToCheck attr_to_change);

};

#endif