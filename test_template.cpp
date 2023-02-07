#include <iostream>
#include <cxxabi.h>
#include <sqlite3.h>
#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

using namespace std;

enum TABLE_TO_CREATE {
      DIRECTORIES,FILES,CHANGES,Count
    };

template <typename T> char* returnName(T x) {
     return abi::__cxa_demangle(typeid(x).name(), nullptr, nullptr, nullptr);
}

template <typename T> T myMax(T x, T y)
{
    cout << "The type of the variable x " << " is " << abi::__cxa_demangle(typeid(x).name(), nullptr, nullptr, nullptr) << endl;
    cout << "The type of the variable y " << " is " << abi::__cxa_demangle(typeid(y).name(), nullptr, nullptr, nullptr) << endl;
    return (x > y) ? x : y;
    
}

class Vehicles
{
    public:
    string make;
    string model;
    string year;
};

class Aircraft
{
    public:
    string make;
    string model;
    string year;
};

int main()
{

    Vehicles mercedes;
    Aircraft learjet;
    sqlite3* db;
    sql::Driver *driver;
    sql::Connection *con;
    sql::Statement *stmt;
    sql::ResultSet *res;
    sql::PreparedStatement *savedirprepStmt;

    cout << myMax<int>(3, 7) << endl; // Call myMax for int
    cout << myMax<double>(3.0, 7.0)
         << endl; // call myMax for double
    cout << myMax<char>('g', 'e')
         << endl; // call myMax for char
    
    cout << "The type of the object driver " << " is " << returnName(driver) << endl;
    cout << "The type of the object con " << " is " << returnName(con) << endl;
    cout << "The type of the object of db " << " is " << returnName(db) << endl;
    cout << "The type of the object stmt " << " is " << returnName(stmt) << endl;
    cout << "The type of the object of res " << " is " << returnName(res) << endl;
    cout << "The type of the object of saveddirprepStmt " << " is " << returnName(savedirprepStmt) << endl;

cout << "Number of rows in TABLE_TO_CREATE " << static_cast<int>(TABLE_TO_CREATE::Count) << endl;

for(int i = 0; i < static_cast<int>(TABLE_TO_CREATE::Count); i++) {
	TABLE_TO_CREATE tbl = {TABLE_TO_CREATE::i};
	cout << tbl << endl;
}

    return 0;
}
