#include <cstdio>
#include "headers/Menu.h"
#include "headers/fileintegritychecker.h"

int main(int argc, char* argv[])
{

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