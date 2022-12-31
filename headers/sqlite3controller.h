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
#include "SQLiteHelper.h"

/**
 * @brief Sqlite3 Controller class
 * 
 * @details The class that handles the SQL functions for creating, updating, and adding files and directories to a 
 * Sqlite3 database.
 */
class sqlite3controller {
    public:
        sqlite3controller();
        sqlite3controller(char* databasename, char* machinename);
        virtual ~sqlite3controller();
        /**
         * @brief Initializes the SQLite database with the supplied databasename.
         * The function will determine if the SQLite database already exists. If it does exist then the database is opened.
         * If it does not exist, the database will be created and then a call to createTables is made to create the tables required to store
         * the data.
         * @see createTables()
        */
        void initdb();
        void closedb();
        void setDATABASENAME(char* dbasename);
        void setMACHINENAME(char* machinename);
        char* getDATABASENAME();
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
         * <tr><td>id</td><td>the auto generated id for the directory entry</td>
         * <tr><td>dirid</td><td>The ID of the Directory where the file is located.</td>
         * <tr><td>dateadded</td><td>the date the directory was added to the table</td>
         * <tr><td>filename</td><td>The absolute name of the file. The relative path can be pulled together using the directory reference via the dirid.</td>
         * <tr><td>hashedvalue</td><td>The hashed value of the file if the hashed value flag is set to true. The default value is NOVAL.</td>
         * <tr><td>hashedvaluechanged</td><td>A flag to indicate the hashed value changed and all changes are added to the changes table.</td>
         * <tr><td>lastmodified</td><td>The date the file was last modified.</td>
         * <tr><td>hashedvaluechanged</td><td>A flag to indicate the last modified changed and all changes are added to the changes table.</td>
         * <tr><td>filesize</td><td>The file size of the file in bytes.</td>
         * <tr><td>filesizechanged</td><td>A flag to indicate the file size changed and all changes are added to the changes table.</td></table>
         * @see Menu.h
        */
        void createTables();
        int save_file_info(const char* fname, const char* dirname, char* lastmodified, uintmax_t filesize, const char* hashval = NULL);
        int save_dir_info(const char* dirname);
    protected:
        char* getPreviousHashval(const int fileId);
        int checkIfDirectoryExist(const char* dirname);
        int checkIfFileExist(const char* fname, const char* dirname);

};

#endif