# Makefile for compiling File Integrity Checker
 
# *****************************************************
# Variables to control Makefile operation
 
CC = g++
STDLIB = c++17
OBJECTS = SQLFunctionsBase.o SQLHelper.o SHA256CPP.o Menu.o mysqlcontroller.o sqlite3controller.o fileintegritychecker.o
CFLAGS = -Wl,--copy-dt-needed-entries
LIBS =  -pthread -lssl -lcrypto -lsqlite3
MYSQLLIBS = -pthread -lssl -lcrypto
INCLUDES = -I/usr/local/include -DSTATIC_CONCPP -I/usr/include
MYSQLINCLUDES = -DSTATIC_CONCPP -I/usr/include /usr/lib/x86_64-linux-gnu/libmysqlcppconn-static.a /usr/lib/x86_64-linux-gnu/libmysqlclient.a
DEPS = /usr/lib/x86_64-linux-gnu/libmysqlcppconn-static.a /usr/lib/x86_64-linux-gnu/libmysqlclient.a
SRC_DIR = src
SOURCEFILE = main.cpp
EXENAME = fileintegritychecker.exe
STACK_TRACE = -g -rdynamic

all: SQLFunctionsBase.o SQLHelper.o SHA256CPP.o Menu.o MySQLController.o SQLite3Controller.o fileintegritychecker.o
	$(CC) -std=$(STDLIB) $(OBJECTS) $(INCLUDES) $(SOURCEFILE) $(DEPS) $(CFLAGS) -o $(EXENAME) $(LIBS)

SQLFunctionsBase.o:
	$(CC) -c -std=$(STDLIB) $(SRC_DIR)/SQLFunctionsBase.cpp

SQLiteHelper.o:
	$(CC) -c -std=$(STDLIB) $(SRC_DIR)/SQLiteHelper.cpp $(MYSQLINCLUDES) $(LIBS)

SQLHelper.o:
	$(CC) -c -std=$(STDLIB) $(SRC_DIR)/SQLHelper.cpp $(LIBS)

SHA256CPP.o:
	$(CC) -c -std=$(STDLIB) $(SRC_DIR)/SHA256CPP.cpp
 
Menu.o:
	$(CC) -c -std=$(STDLIB) $(SRC_DIR)/Menu.cpp

# g++ -c -std=c++17 -DSTATIC_CONCPP -I/usr/include src/mysqlcontroller.cpp /usr/lib/x86_64-linux-gnu/libmysqlcppconn-static.a /usr/lib/x86_64-linux-gnu/libmysqlclient.a -lssl -lcrypto -lpthread
MySQLController.o:
	$(CC) -c -std=$(STDLIB) SQLFunctionsBase.o SQLHelper.o $(SRC_DIR)/MySQLController.cpp $(MYSQLINCLUDES) $(MYSQLLIBS)

# g++ -std=c++17 -c src/sqlite3controller.cpp -I/usr/local/include -DSTATIC_CONCPP -I/usr/include -Wl,--copy-dt-needed-entries -pthread -lssl -lcrypto -lsqlite3
SQLite3Controller.o: SQLHelper.o
	$(CC) -c -std=$(STDLIB) SQLFunctionsBase.o SQLHelper.o $(SRC_DIR)/SQLite3Controller.cpp $(INCLUDES) $(CFLAGS) $(LIBS)

fileintegritychecker.o: SHA256CPP.o Menu.o SQLFunctionsBase.o SQLHelper.o MySQLController.o SQLite3Controller.o
	$(CC) -c -std=$(STDLIB) SHA256CPP.o Menu.o MySQLController.o SQLite3Controller.o $(SRC_DIR)/fileintegritychecker.cpp $(INCLUDES) $(CFLAGS) $(LIBS)
#	$(CC) -c -std=$(STDLIB) SHA256CPP.o Menu.o SQLiteHelper.o mysqlcontroller.o sqlite3controller.o $(SRC_DIR)/fileintegritychecker.cpp $(INCLUDES) $(DEPS) $(CFLAGS) $(LIBS)

# *****
# g++ -std=c++17 SHA256CPP.o Menu.o mysqlcontroller.o sqlite3controller.o -I/usr/local/include -DSTATIC_CONCPP -I/usr/include src/fileintegritychecker.cpp /usr/lib/x86_64-linux-gnu/libmysqlcppconn-static.a /usr/lib/x86_64-linux-gnu/libmysqlclient.a -Wl,--copy-dt-needed-entries -o fileintegritychecker.exe -pthread -lssl -lcrypto -lsqlite3
#
clean:
	rm *.o
	rm fileintegritychecker.exe
