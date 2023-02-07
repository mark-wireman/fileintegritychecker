#include <cstdio>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "headers/Menu.h"
#include "headers/fileintegritychecker.h"

void handler(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}


int main(int argc, char* argv[])
{
    //if(argc == 2) {
        //signal(SIGSEGV, handler);   // install our handler
    //}
    
    time_t start, end;
    double time_taken = 0.0;
    int menuresult = 0;
    fileintegritychecker* fic; 
    Menu* m;

    cout << "Starting processing...." << endl;
    time(&start);

    m = new Menu();

    if (argc >= 3) {
        menuresult = m->ParseArguments(argc, argv);
        if (menuresult == -1) {
            m->PrintMenu();
            return 0;
        }
    }
    else {
        m->PrintMenu();
        return 0;
    }
    
    fic = new fileintegritychecker();
    fic->setParamVals(m);
    fic->setDatabaseConnections();
    fic->processFiles();
    fic->closeDatabaseConnections();

    time(&end);

    // Calculating total time taken by the program.
    time_taken = double(end - start);
    int n = time_taken;
    n = n % (24 * 3600);
    int hour = n / 3600;
    n %= 3600;
    int minutes = n / 60;
    n %= 60;
    int seconds = n;
    cout << "Time taken is: " << hour << " " << "hours " << minutes << " " << "minutes " << seconds << " " << "seconds " << endl;
    cout << "Total directories processed: " << fic->numberofdirectories << " ." << endl;
    cout << "Total files processed: " << fic->numberoffiles << " ." << endl;

    return 0;
}