#include "common.h"

Package* readPackage(int fd){
	// read header
	Header header;

	if(read(fd, &header, sizeof(Header))==0){
		return NULL;
	}

	// create package
	Package* package= newPackage(header.size);
	package->header= header;

	read(fd, package->data, header.size);

	return package;
}

int sendPackage(int fd, Package* package){
	write(fd, &(package->header), sizeof(Header));
	return write(fd, package->data, package->header.size);
}

int sendPackageAndFree(int fd, Package* package){
	int size= sendPackage(fd, package);
	freePackage(package);
	return size;
}

Package* newPackage(int size){
	Package* package= malloc(sizeof(Package));

	package->header.size= size;
	package->data= malloc(size);
}

void freePackage(Package* package){
	free(package->data);
	free(package);
}

int getInt(Package* package){
	if(package->header.type == INT){
		return *((int*)package->data);
	}
	else{
		debug("Package type is not INT!");
		return 0;
	}
}

char* getStr(Package* package){
	if(package->header.type == STRING){
		return package->data;
	}
	else{
		debug("Package type is not STRING!");
		return 0;
	}
}

ActionType getAction(Package* package){
	if(package->header.type == ACTION){
		return *((ActionType*)package->data);
	}
	else{
		debug("Package type is not ACTION!");
		return 0;
	}
}

Package* pack(const char* d, int size, PackageType type, int from, int to){
	Package* package= newPackage(size);
    memcpy(package->data, d, size);

    package->header.from= from;
    package->header.to= to;
    package->header.type= type;

    return package;
}

Package* intPack(int n, int from, int to){
    return pack((char*)&n, sizeof(int), INT, from, to);
}

Package* strPack(const char* str, int from, int to){
	return pack(str, strlen(str)+1, STRING, from, to);
}

Package* actionPack(ActionType action, int from, int to){
    return pack((char*)&action, sizeof(ActionType), ACTION, from, to);
}

Package* bytePack(const char* byte, int size, int from, int to){
	return pack(byte, size, BYTE, from, to);
}


// file operations
int doesFileExist(const char *filename) {
    struct stat st;
    int result = stat(filename, &st);
    return result == 0;
}