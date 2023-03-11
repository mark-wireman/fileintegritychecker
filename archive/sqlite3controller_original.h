#ifndef HEADERS_SQLITE3CONTROLLER_H_
#define HEADERS_SQLITE3CONTROLLER_H_

#define __STDC_WANT_LIB_EXT1__ 1

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstdint>
#include <fstream>
#include <filesystem>
#include <string>
#include <cstring>
#include <thread>
#include <vector>
#include <dirent.h>
#include <sqlite3.h>
#include "SHA256CPP.h"
#include "SQLHelper.h"
#include "SQLFunctionsBase.h"

/**
 * @brief Sqlite3 Controller class
 * 
 * @details The class that handles the SQL functions for creating, updating, and adding files and directories attributes to a 
 * Sqlite3 database.
 */
class sqlite3controller : public virtual SQLFunctionsBase {
    public:
        /**
         * @brief Construct a new sqlite3controller::sqlite3controller object.
        */
        sqlite3controller();
        
        /**
         * @brief Construct a new sqlite3controller::sqlite3controller object. 
         * The constructor is initialized with the name of the SQLite database and the name of the machine the FileIntegrityChecker application is running on.
         * @param databasename 
         * @param machinename 
        */
        sqlite3controller(char* databasename, char* machinename);
        
        /**
         * @brief Destroy the sqlite3controller::sqlite3controller object
        */
        virtual ~sqlite3controller();
        
        
        /**
         * @brief Initializes the SQLite database with the supplied databasename.
         * The function will determine if the SQLite database already exists. If it does exist then the database is opened.
         * If it does not exist, the database will be created and then a call to createTables is made to create the tables required to store
         * the data.
         * @see createTables()
        */
        void initdb();

        /**
         * @brief Close the database after use.
        */
        void closedb();

        /**
         * @brief This is called when a new SQLite database is created.
         * The following tables are created:\n
         * <div align="center"><caption id="multi_row"><b>directories Table - used to store the details of the directories found during the scan.</b></caption></div>
         * <table><tr><th>Field name</th><th>Description</th>
         * <tr><td>id</td><td>the auto generated id for the directory entry</td>
         * <tr><td>dirname</td><td>relative path and name of the directory</td>
         * <tr><td>dateadded</td><td>the date the directory was added to the table</td>
         * <tr><td>status</td><td>Either NEW or VERIFIED. A NEW status is set when a directory is seen by the scanner the first time. VERIFIED if the scanner verifies the directory was seen again.</td>
         * <tr><td>dataverified</td><td>This is the date the VERIFIED directory was verified by the scanner.</td>
         * <tr><td>machinename</td><td>name of the machine where the scanner is running.</td></table>
         * <div><br></div>
         * <div align="center"><caption id="multi_row"><b>files Table - used to store the details of the files found during the scan.</b></caption></div>
         * <table><tr><th>Field name</th><th>Description</th>
         * <tr><td>id</td><td>the auto generated id for the file entry</td>
         * <tr><td>dirid</td><td>The ID of the Directory where the file is located.</td>
         * <tr><td>dateadded</td><td>the date the file was added to the table</td>
         * <tr><td>filename</td><td>The absolute name of the file. The relative path can be pulled together using the directory reference via the dirid.</td>
         * <tr><td>hashedvalue</td><td>The hashed value of the file if the hashed value flag is set to true. The default value is NOVAL.</td>
         * <tr><td>hashedvaluechanged</td><td>A flag to indicate the hashed value changed and all changes are added to the changes table.</td>
         * <tr><td>lastmodified</td><td>The date the file was last modified.</td>
         * <tr><td>hashedvaluechanged</td><td>A flag to indicate the last modified changed and all changes are added to the changes table.</td>
         * <tr><td>filesize</td><td>The file size of the file in bytes.</td>
         * <tr><td>filesizechanged</td><td>A flag to indicate the file size changed and all changes are added to the changes table.</td></table>
         * <div align="center"><caption id="multi_row"><b>changes Table - used to store the changed attributes for files.</b></caption></div>
         * <table><tr><th>Field name</th><th>Description</th>
         * <tr><td>id</td><td>the auto generated id for the change entry</td>
         * <tr><td>fileid</td><td>The ID of the file that has changed.</td>
         * <tr><td>datechanged</td><td>the date the change was added to the table</td>
         * <tr><td>textvalue</td><td>The text value that has changed. The default value is NULL.</td>
         * <tr><td>intvalue</td><td>The integer value that has changed. The default value is -1.</td>
         * <tr><td>attributechanged</td><td>The file attribute that changed.</td></table>
         *
        */
        void createTables();

        /**
         * @brief Save the details of the file.
         * <div>This is the main entry point to saving the file details. If the file is new then all of the required attributes are sent to SQLiteHelper::saveFileInfo.</div>
         * <div>If the file exists, then the provided attributes are sent to SQLiteHelper::saveFileInfo.</div>
         * @param fname Absolute file name. foo.bar
         * @param dirname Relative path with the directory name. Example: /foo/bar/parent
         * @param lastmodified The last modified date of the file.
         * @param filesize The size of the file in bytes.
         * @param hashval The hashed value of the file. The default value is NULL.
         * @return A 0 is returned if saved successfully. A -1 indicates an error occurred and unable to save the file information.
         * @see checkIfDirectoryExist()
         * @see SQLiteHelper::getFileId()
         * @see SQLiteHelper::saveFileInfo()
        */
        int save_file_info(const char* fname, const char* dirname, char* lastmodified, uintmax_t filesize, const char* hashval = NULL);
        
        /**
         * @brief Save the details of the directory.
         * <div>This is the main entry point to saving the directory details. A call is made to checkIfDirectory exists to determine if the directory was previously discovered.</div>
         * <div>If the directory exists, then the directory is marked as VERIFIED.</div>
         * @param dirname Relative path with the directory name. Example: /foo/bar/parent
         * @return A 0 is returned if saved successfully. A -1 indicates an error occurred and unable to save the directory information.
         * @see checkIfDirectoryExist()
        */
        int save_dir_info(const char* dirname);
    //protected:
       /**
         * @brief Checks if the directory exists in the database. 
         * @param dirname Relative name of the directory
         * @return 1 for yes, 0 for no
        */
        int checkIfDirectoryExist(const char* dirname);

        /**
         * @brief Checks if the file exists in the database.
         * @param fname Absolute name of the file name.
         * @param dirname Relative name of the directory.
         * @return 1 for yes, 0 for no
        */
        int checkIfFileExist(const char* fname, const char* dirname);

        void correctAutoIncrement(char*& _sqlStmt);

};

#endif