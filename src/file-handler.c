/*
 * file-handler.c
 *
 *  Created on: Jul 25, 2014
 *      Author: namwkim85
 */

#include "file-handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MSIZE 16777216  // 16mega memory buffer

int IsImageFile(char* filename){
	char temp[1024];
	const char*ext = GetFileExt(filename);
	if (strcmp(IMG_EXT+1, ext)==0){
		strncpy(temp, filename, strlen(filename)-4);
		temp[strlen(filename)-4]='\0';
		ext = GetFileExt(temp);
		if (strcmp(temp+(strlen(temp)-5), "depth")==0 || strcmp("depth", ext)==0 || strcmp("mask", ext)==0){
			return 0;
		}
		return 1;
	}
	return 0;
}

const char *GetFileExt(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

int FileCopy(const char* src, const char* dst) {
  FILE *in, *out;
  char* buf;
  size_t len;

  if (!strcmp(src, dst)) return 4; // if src == dst

  if ((in  = fopen(src, "rb")) == NULL) return 1;
  if ((out = fopen(dst, "wb")) == NULL) { fclose(in); return 2; }

  if ((buf = (char *) malloc(MSIZE)) == NULL) { fclose(in); fclose(out); return 10; } // 버퍼 메모리 할당

  while ( (len = fread(buf, sizeof(char), sizeof(buf), in)) != NULL )
    if (fwrite(buf, sizeof(char), len, out) == 0) {
      fclose(in); fclose(out);
      free(buf);
      remove(dst);
      return 3;
    }

  fclose(in); fclose(out);
  free(buf);

  return 0;
}

int Exists(const char *fname){
    FILE *file= fopen(fname, "r");
    if (file)
    {
        fclose(file);
        return 1;
    }
    return 0;
}


void ForAllImages(char* folder, void (*f)(char*)){
	char dir_path[FLEN];
	DIR *dbDir = NULL;
	struct dirent *file = NULL;
	struct stat buf;

	dbDir = opendir(folder);
	if(!dbDir) {
		fprintf(stderr, "ERROR\n");
	}
	while( (file = readdir(dbDir)) != NULL ){
        memset(&buf, 0, sizeof(struct stat));

        strcpy(dir_path, folder);
		strcat(dir_path,"/");
		strcat(dir_path, file->d_name);
		lstat(dir_path, &buf);
        if(	strcmp(file->d_name, ".")!=0 &&
			strcmp(file->d_name, "..")!=0 &&
			S_ISDIR(buf.st_mode)){ //folder
        	ForAllImages(dir_path, f);

        }else if(S_ISREG(buf.st_mode)){  //regular file
            if (IsImageFile(file->d_name)){//For each Image File
            	(*f)(dir_path);
            }
        }
	}
	closedir(dbDir);
}
