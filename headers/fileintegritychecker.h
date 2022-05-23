#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include "SHA256.h"

void hashfiles(DIR *currentlocation);
int checkifrootorparent(char* dirname);
int checkifrootdir(char* dirname);
int checkifparentdir(char* dirname);
