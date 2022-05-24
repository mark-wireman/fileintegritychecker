#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <sys/sysmacros.h>
#include "SHA256.h"

int isDir(char* filename);
int main(int argc, char* argv[] );
void hashfiles(char* startdirectory);
int checkifrootorparent(char* dirname);
int checkifrootdir(char* dirname);
int checkifparentdir(char* dirname);
