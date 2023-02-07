#include <iostream>
//#include <fstream>
//#include <string>
//#include <cstring>
//#include <dirent.h>
//#include <sys/types.h>
#include "../headers/mysqlcontroller.h"

int main(int argc, char *argv[]) {
    mysqlcontroller *msc;
    msc = new mysqlcontroller();
    msc->setHOSTNAME("192.168.86.115");
    msc->setDBASENAME("mysql");
    msc->setPORT(3306);
    msc->setUSERNAME("mwireman");
    msc->setPASSWORD("shinobu5");
    msc->initdb();

    return(0);
}
