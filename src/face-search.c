/*
 * face-search.c
 *
 *  Created on: Jul 7, 2014
 *      Author: namwkim85
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "cv.h"
#include "highgui.h"
#include "math.h"
#include "float.h"
#include "file-handler.h"
#include "face-detection.h"
#include "face-recognition.h"
#include "face-search.h"

#define FACE_THRESHOLD 100.0
typedef struct _FileWithScore{
	char* img_file;
	float score;
} FileWithScore;

const float WEIGHT[49] = //7*7=49
		{	2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f,
			2.0f, 4.0f, 4.0f, 1.0f, 4.0f, 4.0f, 2.0f,
			1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
			0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 1.0f, 2.0f, 1.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f
		};

void ReleaseFeature(FEATURE* feature);
void SaveFeature(FEATURE* feature, char* fileName);

static int CompFileWithScore( const void* _a, const void* _b, void* userdata )
{
	FileWithScore* a = (FileWithScore*)_a;
	FileWithScore* b = (FileWithScore*)_b;
    float diff = a->score - b->score;
    if (fabs(diff)<FLT_EPSILON){
    	return 0;
    }else if (diff>0.0f){
    	return -1;
    }
    return 1;
}

int MakeFeature(
		char* fileName
		){
	char srcpath[FLEN], srcfile[FLEN];

	//read image
	strcpy(srcpath, fileName);

	strcpy(srcfile, srcpath);
	strcat(srcfile, IMG_EXT);

	IplImage* image = cvLoadImage(srcfile, CV_LOAD_IMAGE_COLOR);

	//read depth image
	strcpy(srcfile, srcpath);
	strcat(srcfile, DEPTH_EXT);

	IplImage* depth = cvLoadImage(srcfile, CV_LOAD_IMAGE_GRAYSCALE);

	//read mask image
	strcpy(srcfile, srcpath);
	strcat(srcfile, MASK_EXT);

	IplImage* mask = cvLoadImage(srcfile, CV_LOAD_IMAGE_GRAYSCALE);

	// calc features
	FEATURE feature;
	int retCode = MakeFeatureInMem(image, depth, mask, &feature);

	if (retCode){
		return retCode;
	}
	// save features
	strcpy(srcfile, srcpath);
	strcat(srcfile, FTR_EXT);
	SaveFeature(&feature, srcfile);

	ReleaseFeature(&feature);

	return retCode;
}

int MakeFeatureInMem(
		IplImage* 	RGBA,
		IplImage* 	depth,
		IplImage*	mask,
		FEATURE* 	feature){
	if (RGBA==NULL){
		fprintf(stderr, "image file is required to create feature set!");
		return 1;
	}
	// convert to grayscale-image
	IplImage* gray_img = RGBA;
	if (gray_img->nChannels > 1) {
		gray_img = cvCreateImage(cvGetSize(RGBA), IPL_DEPTH_8U, 1 );
		cvCvtColor( RGBA, gray_img, CV_BGR2GRAY );
	}
//	cvEqualizeHist(gray_img,gray_img);

	feature->grid_x 	= GRID_X;
	feature->grid_y 	= GRID_Y;
	feature->radius 	= RADIUS;
	feature->neighbors	= NEIGHBORS;

	//detect faces
	CvSeq* faces;
	int retCode = DetectFaces(gray_img, &faces);
	if (retCode){//no faces found
		feature->histogram 	= NULL;
		feature->num_faces 	= 0;
		return 0;
	}else{
		//calculate features
		feature->num_faces 	= faces->total;
		feature->histogram = (CvMat**) malloc(faces->total*sizeof(CvMat*));
		for(int i = 0; i < faces->total; i++ )
		{
			// Create a new rectangle for drawing the face
			CvRect* r = (CvRect*)cvGetSeqElem( faces, i ); // Find the dimensions of the face, and scale it if necessary
			IplImage* face_img = CreateSubImg(gray_img, *r);
			IplImage* lbp_img =  CalcLBP(face_img, RADIUS, NEIGHBORS);

			if (lbp_img==NULL){
				fprintf(stderr, "failed to create lbp image!\n");
				return 1;
			}
			feature->histogram[i] = CalcSpatialHistogram(lbp_img, pow(2.0, NEIGHBORS), GRID_X, GRID_Y);
			if (feature->histogram[i]==NULL){
				fprintf(stderr, "failed to create spatial histogram!\n");
				return 2;
			}
			cvReleaseImage(&face_img);
			cvReleaseImage(&lbp_img);
		}
	}

	return 0;

}
void ReleaseFeature(FEATURE* feature){
	for(int i=0; i<feature->num_faces; i++){
		cvReleaseMat(&feature->histogram[i]);
	}
	if (feature->histogram)
		free(feature->histogram);
}
void SaveFeature(
		FEATURE* 	feature,
		char* 		fileName){
	CvFileStorage* fs = cvOpenFileStorage( fileName, 0, CV_STORAGE_WRITE, NULL);
	if (fs==NULL){
		fprintf(stderr, "can't open feature file\n");
		return;
	}
	cvWriteInt(fs, "radius", feature->radius);
	cvWriteInt(fs, "neighbors", feature->neighbors);
	cvWriteInt(fs, "grid_x", feature->grid_x);
	cvWriteInt(fs, "grid_y", feature->grid_y);
	cvWriteInt(fs, "num_faces", feature->num_faces);

	char faceid[1024];
	char id[1024];

	for (int i=0; i<feature->num_faces; i++){
		strcpy(faceid, "face-");
		sprintf(id, "%d", i);
		strcat(faceid, id);
		cvWrite( fs, faceid, feature->histogram[i], cvAttrList(0,0) );
	}
	cvReleaseFileStorage( &fs );
}

void ReadFeature(
		char* fileName,
		FEATURE* feature){
	CvFileStorage* fs = cvOpenFileStorage( fileName, 0, CV_STORAGE_READ, NULL);
	if (fs==NULL){
		fprintf(stderr, "can't open feature file!\n");
		return;
	}
	feature->radius 	= cvReadIntByName(fs, NULL, "radius", RADIUS);
	feature->neighbors	= cvReadIntByName(fs, NULL, "neighbors", NEIGHBORS);
	feature->grid_x		= cvReadIntByName(fs, NULL, "grid_x", GRID_X);
	feature->grid_y		= cvReadIntByName(fs, NULL, "grid_y", GRID_Y);
	feature->num_faces	= cvReadIntByName(fs, NULL, "num_faces", 0);

	char faceid[1024];
	char id[20];
	feature->histogram = (CvMat**) malloc(feature->num_faces*sizeof(CvMat*));
	for (int i=0; i<feature->num_faces; i++){
		strcpy(faceid, "face-");
		sprintf(id, "%d", i);
		strcat(faceid, id);
		feature->histogram[i] = cvReadByName(fs, NULL, faceid, NULL);
	}
	cvReleaseFileStorage( &fs );

}

float Compare(
		//IplImage* 	RGBA,
		//IplImage* 	depth,
		//IplImage*	mask,
		FEATURE* 	feature,
		//IplImage* 	DBRGBA,
		//IplImage* 	DBDepth,
		//IplImage*	DBMask,
		FEATURE* 	DBFeature){
	float queryScore = 0.0;
	for (int i=0; i<feature->num_faces; i++){
		//check whether the input face exists in the db image
		float minDist = FLT_MAX;
		int faceidx = -1;
		for (int j=0; j<DBFeature->num_faces; j++){
			float dist = CompareHistograms(feature->histogram[i], DBFeature->histogram[j], WEIGHT);
			if (dist<minDist){// && dist<FACE_THRESHOLD){
				minDist = dist;
				faceidx = j;
			}
		}
		if (faceidx!=-1){
			queryScore+=(-1.0f*(minDist/FACE_THRESHOLD)) + 1.0f; //[-inf, 1.0];
		}
	}
	float DBScore = 0.0;
	for (int i=0; i<DBFeature->num_faces; i++){
		//check whether the input face exists in the db image
		float minDist = FLT_MAX;
		int faceidx = -1;
		for (int j=0; j<feature->num_faces; j++){
			float dist = CompareHistograms(DBFeature->histogram[i], feature->histogram[j], WEIGHT);
			if (dist<minDist){// && dist<FACE_THRESHOLD){
				minDist = dist;
				faceidx = j;
			}
		}
		if (faceidx!=-1){
			DBScore+=(-1.0f*(minDist/FACE_THRESHOLD)) + 1.0f; //[-inf, 1.0];
		}
	}
	float totalScore = (feature->num_faces==0? 0 : queryScore/feature->num_faces) + (DBFeature->num_faces==0? 0:DBScore/DBFeature->num_faces);
	return totalScore;// max score  = 1.0 + 1.0 = 2.0
}


int SearchInMem(
		IplImage* 	RGBA,
		IplImage* 	depth,
		IplImage*	mask,
		char* DBFolderName,
		char* categoryFolders[],
		int numCategories,
		char* outputFileNames[],
		float* outputScores,
		int num){


	char cat_dir_path[FLEN], srcpath[FLEN], srcfile[FLEN];
	DIR *catDir = NULL;
	struct dirent *catFile = NULL;
	struct stat buf;

	// calculate for the input files
	FEATURE feature;
	MakeFeatureInMem(RGBA, depth, mask, &feature);

	// loop through the files in the category folders in DB.


	//temporary storage for files
	CvMemStorage* storage = cvCreateMemStorage(0);

	CvSeq* scores = cvCreateSeq(0, sizeof(CvSeq), sizeof(FileWithScore), storage);

	for (int i=0; i<numCategories; i++){
		strcpy(cat_dir_path, DBFolderName);
		strcat(cat_dir_path,"/");
		strcat(cat_dir_path, categoryFolders[i]);


		catDir = opendir(cat_dir_path);
		if(!catDir) { //directory
			printf("ERROR\n");
		}
		while( (catFile = readdir(catDir)) != NULL ){
			memset(&buf, 0, sizeof(struct stat));

	        strcpy(srcfile, cat_dir_path);
			strcat(srcfile,"/");
			strcat(srcfile, catFile->d_name);
			lstat(srcfile, &buf);
			if(S_ISREG(buf.st_mode)){  //regular file

				if (IsImageFile(catFile->d_name)){//For each Image File
					strcpy(srcpath, cat_dir_path);
					strcat(srcpath,"/");
					const char * ext = GetFileExt(catFile->d_name);
					int length = (ext-1)-catFile->d_name;
					strncat(srcpath, catFile->d_name, length);

					//check feature file
					strcpy(srcfile, srcpath);
					strcat(srcfile, FTR_EXT);

					FileWithScore score;
					if (Exists(srcfile)==0){
						fprintf(stderr, "feature file does not exists!\n");
						strcpy(srcfile, srcpath);
						strcat(srcfile, IMG_EXT);
						score.img_file = (char*)malloc(strlen(srcfile));
						strcpy(score.img_file, srcfile);
						score.score = -1.0f;
						cvSeqPush(scores, &score);
						continue;
					}
					FEATURE DBFeature;
					ReadFeature(srcfile, &DBFeature);

					strcpy(srcfile, srcpath);
					strcat(srcfile, IMG_EXT);
					score.img_file = (char*)malloc(strlen(srcfile));
					strcpy(score.img_file, srcfile); //save db filename
					score.score = Compare(&feature,&DBFeature);

					cvSeqPush(scores, &score);

				}
			}
		}
		closedir(catDir);
	}

	//SORT
	cvSeqSort(scores, CompFileWithScore, NULL);

	//Release scores & strings
	for (int i=0; i<scores->total; i++){
		FileWithScore* sc = (FileWithScore*) cvGetSeqElem(scores, i);
		if (i<num){
			outputFileNames[i] = (char*)malloc(strlen(sc->img_file));
			strcpy(outputFileNames[i], sc->img_file);
			outputScores[i] = sc->score;
		}
		free(sc->img_file);
	}
	for (int i=scores->total; i<num; i++){
		outputFileNames[i] 	= NULL;
		outputScores[i]		= -1.0;
	}
	cvClearSeq(scores);
	cvReleaseMemStorage( &storage );
	ReleaseFeature(&feature);

	return 0;
}

int Search(
		char* inputFileName,
		char* DBFolderName,
		char* categoryFolders[],
		int numCategories,
		char* outputFileNames[],
		float* outputScores,
		int num){
	char srcfile[FLEN];

	//read image
	strcpy(srcfile, inputFileName);
	strcat(srcfile, IMG_EXT);

	if (Exists(srcfile)==0){
		fprintf(stderr, "Can't Find the Query Image!\n");
		return 1;
	}

	IplImage* image = cvLoadImage(srcfile, CV_LOAD_IMAGE_COLOR);



	//read depth image
	strcpy(srcfile, inputFileName);
	strcat(srcfile, DEPTH_EXT);

	IplImage* depth;
	if (Exists(srcfile)==0){
		fprintf(stderr, "WARNING: No Depth File Found!\n");
		depth = NULL;
	}else{
		depth = cvLoadImage(srcfile, CV_LOAD_IMAGE_GRAYSCALE);
	}


	//read mask image
	strcpy(srcfile, inputFileName);
	strcat(srcfile, MASK_EXT);

	IplImage* mask;
	if (Exists(srcfile)==0){
		fprintf(stderr, "WARNING: No Mask File Found!\n");
		mask = NULL;
	}else{
		mask = cvLoadImage(srcfile, CV_LOAD_IMAGE_GRAYSCALE);
	}

	int retCode = SearchInMem(image, depth, mask, DBFolderName, categoryFolders,
			numCategories, outputFileNames, outputScores, num);

	return retCode;

}
