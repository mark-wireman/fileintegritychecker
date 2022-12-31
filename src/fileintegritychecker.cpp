#include "../headers/fileintegritychecker.h"

using namespace std;

namespace fs = std::filesystem;

// Tell our preprocessor to create a variable MAXCHAR with value of 100000
#define MAXCHAR 100000

template <typename TP>
std::time_t to_time_t(TP tp)
{
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
        + system_clock::now());
    return system_clock::to_time_t(sctp);
}

char* to_char_time_t(std::time_t time)
{
    string retVal;
    char* char_array;

    tm* now = localtime(&time);
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

/**
 * @brief Get the directories files object
 * 
 * @param s 
 * @param level 
 */
void fileintegritychecker::get_directories_files(const string& s, int level) {
    
    
    auto error_code = std::error_code();
    for(const auto& entry : fs::directory_iterator(s,fs::directory_options::skip_permission_denied, error_code)) {
        //cout << ".";
        if(!error_code) {
            if(!entry.is_symlink() && entry.is_directory()){
                const auto dirname = "/" + entry.path().relative_path().string();
                cout << "Dir: " << dirname << endl;
                if(strcmp(this->dbasetype,"mysql") == 0) {
                    auto dirsaveasync = std::async(std::launch::async, &mysqlcontroller::saveDirectoryName_async, this->mysqlctl, dirname);
                    dirsaveasync.wait();
                }
                if(strcmp(this->dbasetype,"sqlite") == 0) {
                    this->sqlitectl->save_dir_info(dirname.c_str());
                }    
                numberofdirectories++;
                this->get_directories_files(dirname, level+1);

            } else if (entry.is_regular_file()) {
                
                const auto dname = s;
                const auto filename = "/" +  entry.path().relative_path().string();
                const auto ftime = entry.last_write_time();
                std::time_t ctime = to_time_t(ftime);
                char* time = to_char_time_t(ctime);

                uintmax_t fsize = entry.file_size();
                numberoffiles++;
                cout << "\tFile: " << filename << endl;

                if (usehashvals == 0) {
                    if (strcmp(this->dbasetype, "sqlite") == 0) {
                        this->sqlitectl->save_file_info(filename.c_str(),dname.c_str(), time, fsize);
                    }
                }
                else {

                    FILE* file = fopen(filename.c_str(), "r");
                    
                    if (file == NULL) {
                        printf("\tUnable to open file. Moving on...\n");
                    }
                    else {
                        SHA256CPP* sha256_obj = new SHA256CPP();
                        HashValues hashVals;

                        hashVals = sha256_obj->calculateHash(file);

                        fclose(file);
                        delete sha256_obj;

                        string hashedvals = to_string(hashVals.H[0]);
                        hashedvals += to_string(hashVals.H[1]);
                        hashedvals += to_string(hashVals.H[2]);
                        hashedvals += to_string(hashVals.H[3]);
                        hashedvals += to_string(hashVals.H[4]);
                        hashedvals += to_string(hashVals.H[5]);
                        hashedvals += to_string(hashVals.H[6]);
                        hashedvals += to_string(hashVals.H[7]);

                        if (strcmp(this->dbasetype, "mysql") == 0) {
                            auto filesaveasync = std::async(std::launch::async, &mysqlcontroller::saveFileInfo_async, this->mysqlctl, dname, entry.path().filename().string(), hashedvals);
                            filesaveasync.wait();
                        }

                        if (strcmp(this->dbasetype, "sqlite") == 0) {
                            //fprintf(stderr, "Saving hashedvalue of %s", hashedvals.c_str());
                            this->sqlitectl->save_file_info(filename.c_str(), dname.c_str(), time, fsize, (char*)hashedvals.c_str());
                        }

                    }
                }
            } 
            else {
                //Do nothing
                const auto unknown = "/" +  entry.path().relative_path().string();
                cout << "\tUnknown: " << unknown << endl;
            }
        } else {
            cout << "# ERR: DirectoryIterator in " << __FILE__;
            cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
            cout << "# ERROR Code: " << error_code.category().name();
            cout << " ERROR Details: " << error_code.category().message(error_code.value()) << endl;
            cout << " ERROR accessing: " << entry.path().relative_path().string() << endl;
        }    
    }
    
}

/**
 * @brief Set the Param Vals object
 * 
 */
void fileintegritychecker::setParamVals(Menu *_menu) {
   this->parentdir = _menu->getPARENTDIR();
   this->dbasetype = _menu->getDBASETYPE();
   this->mname = _menu->getMACHINENAME();

   if(strcmp(this->dbasetype,"mysql") == 0) {
       this->dbasehost = _menu->getDATABASEHOSTNAME();
       this->uname = _menu->getUSERNAME();
       this->pwd = _menu->getPASSWORD();
       this->portno = _menu->getPORT();
   }
   
   if(strcmp(this->dbasetype,"sqlite") == 0) {
       this->dbasefilename = _menu->getDBASEFILENAME();
   }

   this->usehashvals = _menu->getPROCESSHASHVALS();

}


void fileintegritychecker::setDatabaseConnections() {
    if (strcmp(this->dbasetype, "mysql") == 0) {
        this->mysqlctl = new mysqlcontroller(this->dbasehost, "fileintegritychecker", this->uname, this->pwd, 3306, this->mname);
        this->mysqlctl->initdb();
    }

    if (strcmp(this->dbasetype, "sqlite") == 0) {
        this->sqlitectl = new sqlite3controller(this->dbasefilename, this->mname);
        this->sqlitectl->initdb();
    }
}

void fileintegritychecker::closeDatabaseConnections() {
    if (strcmp(this->dbasetype, "mysql") == 0) {
        this->mysqlctl->closedb();
    }

    if (strcmp(this->dbasetype, "sqlite") == 0) {
        this->sqlitectl->closedb();
    }
}

void fileintegritychecker::processFiles() {
    string pDir = this->parentdir;
    cout << pDir << endl;

    if (strcmp(this->dbasetype, "mysql") == 0) {
        auto dirsaveasync = std::async(std::launch::async, &mysqlcontroller::saveDirectoryName_async, this->mysqlctl, pDir.c_str());
        dirsaveasync.wait();
    }
    if (strcmp(this->dbasetype, "sqlite") == 0) {
        this->sqlitectl->save_dir_info(pDir.c_str());
    }

    numberofdirectories++;
    
    this->get_directories_files(pDir);

    
}

fileintegritychecker::fileintegritychecker() {}

fileintegritychecker::~fileintegritychecker() {}


