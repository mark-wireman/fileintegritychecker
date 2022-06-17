/*
 * sqlite3crud.cpp
 *
 *  Created on: Jun 4, 2022
 *      Author: mark_
 */

#include "../headers/sqlite3crud.h"

// Create a callback function
int callback(void *NotUsed, int argc, char **argv, char **azColName) {

    // int argc: holds the number of results
    // (array) azColName: holds each column returned
    // (array) argv: holds each value

	int i;
	   for(i = 0; i<argc; i++) {

	      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	   }
	   printf("\n");
	   return 0;
}

std::string sqlite3crud::getCurrentTime() {
	std::string retVal = "";

	std::time_t t = std::time(0);   // get time now
	std::tm* now = std::localtime(&t);
	char date_string[100];
	char time_string[100];

	strftime(date_string, 50, "%B %d, %Y ", now);
	strftime(time_string, 50, "%T", now);

	retVal += date_string;
	retVal += time_string;

	return retVal;
}

void sqlite3crud::initdb(std::string dbasename) {

	this->_dbasename = dbasename;
	this->opendb();
}

void sqlite3crud::opendb() {
	   char *zErrMsg = 0;
	   int rc;
	   char *sql;

	   /* Create or Open the database */
	   if(!this->checkIfDBaseFileExists(this->_dbasename)) {
		   this->createDatabase(this->_dbasename);
	   } else {
		   rc = sqlite3_open(this->_dbasename.c_str(), &this->db);
	   }

	   if( rc ) {
	      fprintf(stderr, "opendb::Can't open database: %s\n", sqlite3_errmsg(this->db));
	      throw std::invalid_argument(sqlite3_errmsg(this->db));
	      //return(0);
	   } else {
	      fprintf(stderr, "Opened database successfully\n");
	   }
}

void sqlite3crud::checkIfDBOpen() {
 int rc = sqlite3_open(this->_dbasename.c_str(), &this->db);

 if(rc) {
   this->opendb();
 }
 
}

void sqlite3crud::createDatabase(std::string dbasename) {
	std::string sql;

	sql = "CREATE TABLE directories (";
	sql += "id	INTEGER,";
	sql += "name	TEXT,";
	sql += "dateadded	TEXT,";
	sql += "status	TEXT DEFAULT ";
	sql += "'";
	sql += "NEW";
	sql += "'";
	sql += ",";
	sql += "dateverified	TEXT,";
	sql += "PRIMARY KEY(id AUTOINCREMENT));";

	 /* Open database */
	   rc = sqlite3_open(dbasename.c_str(), &this->db);

	   if( rc ) {
	      fprintf(stderr, "createDatabase::Can't open database: %s\n", sqlite3_errmsg(this->db));
	      throw std::invalid_argument(sqlite3_errmsg(this->db));
	   } else {
	      fprintf(stdout, "Opened database successfully\n");
	   }

	   /* Execute SQL statement */
	   //rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
	   rc = sqlite3_exec(this->db, sql.c_str(), NULL, 0, &zErrMsg);

	   if( rc != SQLITE_OK ){
	      fprintf(stderr, "createDatabase::SQL error: %s\n", sqlite3_errmsg(this->db));
	      sqlite3_free(zErrMsg);
	      throw std::invalid_argument(sqlite3_errmsg(this->db));
	   } else {
	      fprintf(stdout, "Table created successfully\n");
	   }

	   //this->closedb();

}

int sqlite3crud::checkIfDBaseFileExists(std::string dbasename) {
	int retVal = 0;

	FILE *file;
	if (file = fopen(dbasename.c_str(), "r")) {
	   fclose(file);
	   retVal = 1;
	}

	return retVal;

}

std::string sqlite3crud::filterApostraphe(std::string dirname) {
	std::string retVal = "";

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

	return retVal;
}

void sqlite3crud::saveDirectoryName(std::string dirname) {
	std::string dirnameverified = this->filterApostraphe(dirname);
	/* Create SQL statement */
	/*std::string sql = "INSERT INTO directories (name,dateadded) VALUES (";
	   sql+="'";
	   sql+=dirnameverified;
	   sql+="','";
	   sql+=getCurrentTime();
	   sql+="');";
	*/

	const char* sqlStmt = "INSERT INTO directories (name,dateadded) VALUES (?,?)";
	sqlite3_stmt* preparedStmt;
	int sqlerr;
	const char* currenttime = this->getCurrentTime().c_str();

	if(this->checkIfDirectoryExists(dirname) == 0) {

	  this->checkIfDBOpen();
	    

	  sqlerr = sqlite3_prepare_v2(this->db, sqlStmt, -1, &preparedStmt, NULL);

	  if(sqlerr != SQLITE_OK) {
	    printf("saveDirectory::ERROR::Prepare statement failed: %s\n",sqlite3_errmsg(this->db));
	  } else {

	    sqlite3_bind_text(preparedStmt,1,dirnameverified.c_str(),-1,SQLITE_TRANSIENT);
	    sqlite3_bind_text(preparedStmt,2,currenttime,-1,SQLITE_TRANSIENT);

	    sqlerr = sqlite3_step(preparedStmt);

	    if(sqlerr != SQLITE_DONE) {
	      printf("saveDirectory::ERROR::SQL Execution faled: %s\n",sqlite3_errmsg(this->db));
	    } else {
	       printf("Records created successfully: %s\n",sqlStmt);
	    }	   

	  }

	  sqlite3_finalize(preparedStmt);
	}

}

int sqlite3crud::checkIfDirectoryExists(std::string dirname) {
	int retVal = 0;
	std::string dirnameverified = this->filterApostraphe(dirname);
	sqlite3_stmt* stmt;
	stringstream ss;
	 /* Create SQL statement */
	 ss << "select * from directories where name = '" << dirnameverified << "';";
	 string sql(ss.str());

	 printf("Select statement: %s\n",sql.c_str());

	 // compile sql statement to binary
	 if(sqlite3_prepare_v2(this->db, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
		 printf("checkIfDirectoryExists::ERROR: while compiling sql: %s\n", sqlite3_errmsg(this->db));
	     sqlite3_finalize(stmt);
	     return retVal;
	 }

	 // execute sql statement, and while there are rows returned, print ID
	 int ret_code = 0;
	 while((ret_code = sqlite3_step(stmt)) == SQLITE_ROW) {
		 int id = sqlite3_column_int(stmt, 0);
		 printf("TEST: ID = %d\n", sqlite3_column_int(stmt, 0));
		 this->addVerified(id);
	     retVal = 1;
	 }
	 if(ret_code != SQLITE_DONE) {
		 //this error handling could be done better, but it works
		 printf("checkIfDirectoryExists::ERROR: while performing sql: %s\n", sqlite3_errmsg(this->db));
		 printf("ret_code = %d\n", ret_code);
		 //throw std::invalid_argument("SQL Error.");
	 }

	 printf("entry %s\n", retVal ? "found" : "not found");


	 //release resources
	 sqlite3_finalize(stmt);


	return retVal;
}

void sqlite3crud::addVerified(int id) {
  /*std::string sql;
	sql = "UPDATE directories set dateverified = ";
	sql+= "'";
	sql+= getCurrentTime();
	sql+= "'";
	sql+= ",status = ";
	sql+= "'";
	sql+= "VERIFIED";
	sql+= "'";
	sql+= " where ID=";
	sql+= std::to_string(id);
	sql+= ";";
  */
        const char* sqlStmt = "UPDATE directories set dateverified=? WHERE ID=?";
	sqlite3_stmt* preparedStmt;
	int sqlerr;
	const char* currenttime = this->getCurrentTime().c_str();
	//const char* data = "";

         sqlerr = sqlite3_prepare_v2(this->db, sqlStmt, -1, &preparedStmt, NULL);

	  if(sqlerr != SQLITE_OK) {
	    printf("saveDirectory::ERROR::Prepare statement failed: %s\n",sqlite3_errmsg(this->db));
	  } else {

	    sqlite3_bind_text(preparedStmt,1,currenttime,-1,SQLITE_TRANSIENT);
	    sqlite3_bind_int(preparedStmt,2,id);

	    sqlerr = sqlite3_step(preparedStmt);

	    if(sqlerr != SQLITE_DONE) {
	      printf("saveDirectory::ERROR::SQL Execution faled: %s\n",sqlite3_errmsg(this->db));
	    } else {
	       printf("Record updated successfully: %s\n",sqlStmt);
	    }	   

	  }

	  sqlite3_finalize(preparedStmt);
	
	  /*printf("SQL Statement: %s\n",sql.c_str());

	//rc = sqlite3_exec(db, sql.c_str(), callback, (void*)data, &zErrMsg);
    rc = sqlite3_exec(this->db, sql.c_str(), NULL, (void*)data, &zErrMsg);

	if( rc != SQLITE_OK ) {
		//fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(this->db));
		fprintf(stderr, "addVerified::SQL error: %s\n", zErrMsg);
	    sqlite3_free(zErrMsg);
	} else {
	    fprintf(stdout, "Operation done successfully\n");
	    }*/

}

void sqlite3crud::closedb() {
	sqlite3_close(this->db);
}


