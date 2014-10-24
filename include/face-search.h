/*
 * face-search.h
 *
 *  Created on: Jul 7, 2014
 *      Author: namwkim85
 */

#ifndef FACE_SEARCH_H_
#define FACE_SEARCH_H_



#define RADIUS 		2
#define NEIGHBORS	8
#define GRID_X		7
#define GRID_Y		7

#define HUELBP_ON	1
#define FOREGRND_ON 0
#define UNIFORM_ON 	1

extern const float WEIGHT[49];
typedef struct _FEATURE{
	int radius;
	int neighbors;
	int grid_x;
	int grid_y;
	int num_faces;
	CvMat** histogram;
	CvMat** hue_histogram;
} FEATURE;


int MakeFeature(
		char* fileName
		);

int MakeFeatureInMem(
		IplImage* 	RGBA,
		IplImage* 	depth,
		IplImage*	mask,
		FEATURE* 	feature);

void SaveFeature(
		FEATURE* 	feature,
		char* 		fileName);

void ReleaseFeature(
		FEATURE* feature
		);

void ReadFeature(
		char* fileName,
		FEATURE* feature);

float Compare(
//		IplImage* 	RGBA,
//		IplImage* 	depth,
//		IplImage*	mask,
		FEATURE* 	feature,
//		IplImage* 	DBRGBA,
//		IplImage* 	DBDepth,
//		IplImage*	DBMask,
		FEATURE* 	DBFeature);

int SearchInMem(
		IplImage* 	RGBA,
		IplImage* 	depth,
		IplImage*	mask,
		char* DBFolderName,
		char* categoryFolders[],
		int numCategories,
		char* outputFileNames[],
		float* outputScores,
		int num);

int Search(
		char* inputFileName,
		char* DBFolderName,
		char* categoryFolders[],
		int numCategories,
		char* outputFileNames[],
		float* outputScores,
		int num);

#endif /* FACE_SEARCH_H_ */
