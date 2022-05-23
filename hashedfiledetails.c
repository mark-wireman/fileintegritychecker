#include "headers/hashedfiledetails.h"
#include "headers/userdefinederrors.h"
#include "headers/SHA256.h"

int HashedFile_getFileHash(HashedFileDetailsPtr hashedfile) {
	if ( hashedfile == NULL) return NO_OBJECT_ERROR;
	else return hashedfile->hashvalue;
}

int HashedFile_putFileHash(HashedFileDetailsPtr hashedfile, HashedValuesPtr hashvalue) {
	if ( hashedfile == NULL) return NO_OBJECT_ERROR;
	else if (hashvalue < 1) return INVALID_HASH_NO_ERROR;
	else {
		hashedfile->hashvalue = hashvalue;
		return NO_ERROR;
	}
}

char* HashedFile_getFileDirectory(HashedFileDetailsPtr hashedfile) {
	if ( hashedfile == NULL) return NO_OBJECT_ERROR;
	else return hashedfile->directory;
}

int HashedFile_putFileDirectory(HashedFileDetailsPtr hashedfile, char* directory) {
	if ( hashedfile == NULL) return NO_OBJECT_ERROR;
	else if (directory == NULL) return INVALID_DIRECTORY_ERROR;
	else {
		hashedfile->directory = directory;
		return NO_ERROR;
	}
}

char* HashedFile_getFileName(HashedFileDetailsPtr hashedfile) {
	if ( hashedfile == NULL) return NO_OBJECT_ERROR;
	else return hashedfile->filename;
}

int HashedFile_putFileName(HashedFileDetailsPtr hashedfile, char* filename) {
	if ( hashedfile == NULL) return NO_OBJECT_ERROR;
	else if (filename == NULL) return INVALID_FILENAME_ERROR;
	else {
		hashedfile->filename = filename;
		return NO_ERROR;
	}
}
