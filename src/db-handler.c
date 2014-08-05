/*
 * db-handler.c
 *
 *  Created on: Jul 7, 2014
 *      Author: namwkim85
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "cv.h"
#include "highgui.h"
#include "math.h"
#include "db-handler.h"
#include "file-handler.h"
#include "face-search.h"

// COPY RGBA, DEPTH, FEATURE FILES INTO THE DB
// IF NO FEATURE AVAILABLE, DERIVE IT
int Insert(
		char* filename,
		char* DBFolder,
		char* categoryFolder
		){

	char srcpath[FLEN], srcfile[FLEN], dstpath[FLEN], dstfile[FLEN];
	strcpy(srcpath, filename);

	char *pch = strrchr(filename, '/')+1;
	if (pch==NULL){
		pch = filename;
	}
	strcpy(dstpath, DBFolder);
	strcat(dstpath, "/");
	strcat(dstpath, categoryFolder);
	strcat(dstpath, "/");
	strcat(dstpath, pch);

	//read and copy image file
	strcpy(srcfile, srcpath);
	strcat(srcfile, IMG_EXT);

	if (Exists(srcfile)==0){
		fprintf(stderr, "Image file does not exists!\n");
		return 1;
	}
	strcpy(dstfile, dstpath);
	strcat(dstfile, IMG_EXT);

	if (FileCopy(srcfile, dstfile)>0){
		fprintf(stderr, "FileCopy Failed!\n");
		return 3;
	}

	//read and copy depth file
	strcpy(srcfile, srcpath);
	strcat(srcfile, DEPTH_EXT);

	if (Exists(srcfile)==0){
		fprintf(stderr, "Depth file does not exists!\n");
		return 2;
	}
	strcpy(dstfile, dstpath);
	strcat(dstfile, DEPTH_EXT);

	if (FileCopy(srcfile, dstfile)>0){
		fprintf(stderr, "FileCopy Failed!\n");
		return 3;
	}

	//read and copy mask file
	strcpy(srcfile, srcpath);
	strcat(srcfile, MASK_EXT);
	strcpy(dstfile, dstpath);
	strcat(dstfile, MASK_EXT);
	if (Exists(srcfile)==0){//if not exist, create a new mask covering the whole image
		strcpy(srcfile, srcpath);
		strcat(srcfile, IMG_EXT);
		IplImage* image = cvLoadImage(srcfile, CV_LOAD_IMAGE_COLOR);
		// Fill in 255
		IplImage* mask = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U,1);
		cvSet(mask, cvScalarAll(255), NULL);
		cvSaveImage(dstfile, mask, NULL);
		cvReleaseImage(&image);
		cvReleaseImage(&mask);
	}else{
		if (FileCopy(srcfile, dstfile)>0){
			fprintf(stderr, "FileCopy Failed!\n");
			return 3;
		}
	}

	//read and copy feature file
	strcpy(srcfile, srcpath);
	strcat(srcfile, FTR_EXT);

	if (Exists(srcfile)==0){//if not exist, create a new feature file;
		int retCode = MakeFeature(dstpath);
		if (retCode){
			fprintf(stderr, "Failed to create feature\n");
		}
	}else{
		strcpy(dstfile, dstpath);
		strcat(dstfile, FTR_EXT);
		if (FileCopy(srcfile, dstfile)>0){
			fprintf(stderr, "FileCopy Failed!\n");
			return 3;
		}
	}

	return 0;
}
// DELETE RGBA, DEPTH, FEATURE FILES
int Delete(
		char* filename,
		char* DBFolder,
		char* categoryFolder
		){

	char srcpath[FLEN], srcfile[FLEN];
	strcpy(srcpath, DBFolder);
	strcat(srcpath, "/");
	strcat(srcpath, categoryFolder);
	strcat(srcpath, "/");
	strcat(srcpath, filename);

	strcpy(srcfile, srcpath);
	strcat(srcfile, IMG_EXT);

	if(Exists(srcfile) && remove(srcfile)){
		fprintf(stderr, "Failed to delete image file!\n");
		return 1;
	}

	strcpy(srcfile, srcpath);
	strcat(srcfile, DEPTH_EXT);

	if(Exists(srcfile) && remove(srcfile)){
		fprintf(stderr, "Failed to delete depth file!\n");
		return 2;
	}

	strcpy(srcfile, srcpath);
	strcat(srcfile, MASK_EXT);

	if(Exists(srcfile) && remove(srcfile)){
		fprintf(stderr, "Failed to delete mask file!\n");
		return 3;
	}

	strcpy(srcfile, srcpath);
	strcat(srcfile, FTR_EXT);

	if(Exists(srcfile) && remove(srcfile)){
		fprintf(stderr, "Failed to delete feature file!\n");
		return 4;
	}

	return 0;
}
// SCAN ALL THE CATEGORY FOLDERS
// DETECT IMAGES WHOSE FEATURES ARE MISSING
// DERIVE FEATURES AND SAVE INTO THE DB


int Update(
		char* DBFolder
		){
	char cat_dir_path[FLEN], srcpath[FLEN], srcfile[FLEN];


	DIR *dbDir = NULL;
	struct dirent *dbFile = NULL;
	struct stat buf;

	dbDir = opendir(DBFolder);
	if(!dbDir) {
		printf("ERROR\n");
	}
	while( (dbFile = readdir(dbDir)) != NULL ){
        memset(&buf, 0, sizeof(struct stat));

        strcpy(cat_dir_path, DBFolder);
		strcat(cat_dir_path,"/");
		strcat(cat_dir_path, dbFile->d_name);
		lstat(cat_dir_path, &buf);
        if(	strcmp(dbFile->d_name, ".")!=0 &&
			strcmp(dbFile->d_name, "..")!=0 &&
			S_ISDIR(buf.st_mode)){ //folder

        	DIR *catDir = NULL;
        	struct dirent *catFile = NULL;
        	catDir = opendir(cat_dir_path);
        	if(!catDir) { //directory
        		printf("ERROR\n");
        	}
        	while( (catFile = readdir(catDir)) != NULL ){
                memset(&buf, 0, sizeof(struct stat));
                strcpy(srcfile, cat_dir_path);
				strcat(srcfile,"/");
				strcat(srcfile,catFile->d_name);
                lstat(srcfile, &buf);
        		if(S_ISREG(buf.st_mode)){  //regular file
        			if (IsImageFile(catFile->d_name)){//For each Image File
        				strcpy(srcpath, cat_dir_path);
        				strcat(srcpath,"/");
        				const char* ext = GetFileExt(catFile->d_name);
        				int length = (ext-1)-catFile->d_name;
        				strncat(srcpath, catFile->d_name, length);

        				//check depth file
        				strcpy(srcfile, srcpath);
        				strcat(srcfile, DEPTH_EXT);

        				if (Exists(srcfile)==0){
        					fprintf(stderr, "Depth file does not exists!\n");
        					return 1;
        				}

        				//check mask file
        				strcpy(srcfile, srcpath);
        				strcat(srcfile, MASK_EXT);

        				if (Exists(srcfile)==0){
        					strcpy(srcfile, srcpath);
        					strcat(srcfile, IMG_EXT);
        					IplImage* image = cvLoadImage(srcfile, CV_LOAD_IMAGE_COLOR);
        					// Fill in 255
        					IplImage* mask = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U,1);
        					cvSet(mask, cvScalarAll(255), NULL);

        					strcpy(srcfile, srcpath);
        					strcat(srcfile, MASK_EXT);
        					cvSaveImage(srcfile, mask, NULL);
        					cvReleaseImage(&image);
        					cvReleaseImage(&mask);
        				}

        				//check feature file
        				strcpy(srcfile, srcpath);
        				strcat(srcfile, FTR_EXT);

        				if (Exists(srcfile)==0){
        					MakeFeature(srcpath);
        				}
        			}
        		}
        	}
        	closedir(catDir);
        }
	}
	closedir(dbDir);

	return 0;
}

