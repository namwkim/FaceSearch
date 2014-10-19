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
	}else{
		strcpy(dstfile, dstpath);
		strcat(dstfile, IMG_EXT);

		if (FileCopy(srcfile, dstfile)>0){
			fprintf(stderr, "FileCopy Failed!\n");
			return 3;
		}
	}

	//read and copy depth file
	strcpy(srcfile, srcpath);
	strcat(srcfile, DEPTH_EXT);

	if (Exists(srcfile)==0){
		fprintf(stderr, "Depth file does not exists!\n");
		//return 2;
	}else{
		strcpy(dstfile, dstpath);
		strcat(dstfile, DEPTH_EXT);

		if (FileCopy(srcfile, dstfile)>0){
			fprintf(stderr, "FileCopy Failed!\n");
			return 3;
		}
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
		if (image==NULL){
			fprintf(stderr, "Failed to Load an image file\n");
		}
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
		//return 1;
	}

	strcpy(srcfile, srcpath);
	strcat(srcfile, DEPTH_EXT);

	if(Exists(srcfile) && remove(srcfile)){
		fprintf(stderr, "Failed to delete depth file!\n");
		//return 2;
	}

	strcpy(srcfile, srcpath);
	strcat(srcfile, MASK_EXT);

	if(Exists(srcfile) && remove(srcfile)){
		fprintf(stderr, "Failed to delete mask file!\n");
		//return 3;
	}

	strcpy(srcfile, srcpath);
	strcat(srcfile, FTR_EXT);

	if(Exists(srcfile) && remove(srcfile)){
		fprintf(stderr, "Failed to delete feature file!\n");
		//return 4;
	}

	return 0;
}
// SCAN ALL THE CATEGORY FOLDERS
// DETECT IMAGES WHOSE FEATURES ARE MISSING
// DERIVE FEATURES AND SAVE INTO THE DB

void UpdateFile(char* imageFile){
    printf("updating %s\n", imageFile);
    char path[FLEN], file[FLEN];
//    const char* ext = GetFileExt(imageFile);

    int length = strlen(imageFile)-4;
    strncpy(path, imageFile, length);
    path[length]='\0';
    //check depth file
    strcpy(file, path);
    strcat(file, DEPTH_EXT);

    if (Exists(file)==0){
		fprintf(stderr, "Depth file does not exists!\n");
		//return 1;
    }

    //check mask file
    strcpy(file, path);
    strcat(file, MASK_EXT);

    if (Exists(file)==0){
    	strcpy(file, path);
		strcat(file, IMG_EXT);
		IplImage* image = cvLoadImage(file, CV_LOAD_IMAGE_COLOR);
		if (image==NULL){
			fprintf(stderr, "Failed to Load an image file\n");
		}
		// Fill in 255
		IplImage* mask = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U,1);
		cvSet(mask, cvScalarAll(255), NULL);

		strcpy(file, path);
		strcat(file, MASK_EXT);
		cvSaveImage(file, mask, NULL);
		cvReleaseImage(&image);
		cvReleaseImage(&mask);
    }

    //check feature file
    strcpy(file, path);
    strcat(file, MASK_EXT);

    //if (Exists(srcfile)==0){
    //always update features
    MakeFeature(path);
    //}
}

int Update(
		char* folder
		){
	ForAllImages(folder, UpdateFile);
	return 0;
}

