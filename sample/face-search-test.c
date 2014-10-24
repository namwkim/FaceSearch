#include "cv.h"
#include "highgui.h"
#include "math.h"
#include "cxcore.h"
#include "stdio.h"
#include "time.h"
#include "string.h"
#include "float.h"
#include "face-detection.h"
#include "face-recognition.h"
#include "db-handler.h"
#include "file-handler.h"
#include "face-search.h"
#include "kmeans.h"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

///////////////// Not Part of Official Testing, but Rather experimental stuffs
void ExperimentForegroundExtraction(){
	char* filename 	= "./sample/query2/62.png";
	IplImage* gray 	= cvLoadImage(filename, CV_LOAD_IMAGE_GRAYSCALE);
	IplImage* depth = cvLoadImage("./sample/query2/62.depth.png", CV_LOAD_IMAGE_GRAYSCALE);
	IplImage* gray2 	= cvLoadImage(filename, CV_LOAD_IMAGE_COLOR);
	IplImage* show 	= cvCreateImage(cvSize(gray->width, gray->height), IPL_DEPTH_8U, 3);

	cvShowImage("depth", depth);
	cvWaitKey(0);
	// K-Means Clustering for Foreground Extraction
	int numPts = depth->width*depth->height;
	double* points = (double* )malloc(numPts*sizeof(double));

	for(int i=0; i<numPts; i++){
		points[i] = (double)(unsigned char)depth->imageData[i];
	}
	double initial_centroids[2];
	initial_centroids[0] = 255.0f/3.0f;
	initial_centroids[1] = 2.0f*255.0f/3.0f;
	int* assignments =  (int* )malloc(numPts*sizeof(int));

	kmeans(1, points, numPts, 2, initial_centroids, assignments);

	//for (int i=0; i<numPts; i++){
	//	printf("final_centroids: %f, %d\n", points[i], assignments[i]);
	//}
	for(int i=0; i<numPts; i++){
		if (assignments[i]==0){
			gray->imageData[i] = 255;
		}
	}
	free(points);
	free(assignments);
	cvShowImage("adjusted for depth", gray);
	cvWaitKey(0);

	//Face Detection with Foreground Extraction
	CvSeq* faces;
	int retCode = DetectFaces(gray, NULL,&faces, FOREGRND_ON);
	if (retCode){
	printf("ERROR: DetectFacesInImage\n" );
	}else{
	printf("%d faces detected!\n", faces->total);
	}
	// Create two points to represent the face locations
	CvPoint pt1, pt2;
	cvConvertImage(gray, show, CV_GRAY2RGB);
	for(int i = 0; i < faces->total; i++ )
	{
		// Create a new rectangle for drawing the face

		CvRect* r = (CvRect*)cvGetSeqElem( faces, i ); // Find the dimensions of the face, and scale it if necessary
		pt1.x = r->x;//*scale;
		pt2.x = (r->x+r->width);//*scale;
		pt1.y = r->y;//*scale;
		pt2.y = (r->y+r->height);//*scale;
		// Draw the rectangle in the input image
		cvRectangle( show, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0 );
	}
	cvShowImage( "result", show );
	cvWaitKey(0);

	//Face Detection without Foreground Extraction
	retCode = DetectFaces(gray2, NULL, &faces, FOREGRND_ON);
	if (retCode){
		printf("ERROR: DetectFacesInImage\n" );
	}else{
		printf("%d faces detected!\n", faces->total);
	}
	// Create two points to represent the face locations
	for(int i = 0; i < faces->total; i++ )
	{
		// Create a new rectangle for drawing the face

		CvRect* r = (CvRect*)cvGetSeqElem( faces, i ); // Find the dimensions of the face, and scale it if necessary
		pt1.x = r->x;//*scale;
		pt2.x = (r->x+r->width);//*scale;
		pt1.y = r->y;//*scale;
		pt2.y = (r->y+r->height);//*scale;
		// Draw the rectangle in the input image
		cvRectangle( show, pt1, pt2, CV_RGB(0,255,0), 3, 8, 0 );
	}
	// Show the image in the window named "result"
	cvShowImage( "result", show );
	cvWaitKey(0);
}
/*
void ExperimentHueExtraction(){
	//Load Image
	IplImage* rgb = cvLoadImage("./sample/query2/102.png", CV_LOAD_IMAGE_COLOR);
	cvShowImage( "hueLBP", rgb );
	cvWaitKey(0);
	IplImage* hsv = cvCreateImage( cvGetSize(rgb), IPL_DEPTH_8U, 3);
	//Extract Hue Channel
	cvCvtColor(rgb, hsv, CV_RGB2HSV);
	cvShowImage( "hsv", hsv );
	cvWaitKey(0);
	IplImage* h = cvCreateImage( cvGetSize(hsv), IPL_DEPTH_8U, 1 );
	IplImage* s = cvCreateImage( cvGetSize(hsv), IPL_DEPTH_8U, 1 );
	IplImage* v = cvCreateImage( cvGetSize(hsv), IPL_DEPTH_8U, 1 );
	// Split image onto the color planes
	cvSplit( hsv, h, s, v, NULL );

	//Create Hue LBP
	IplImage* hueLBP = CalcLBP(h, RADIUS, NEIGHBORS, UNIFORM_ON);
	cvShowImage( "hueLBP", hueLBP );
	cvWaitKey(0);

}




int TestFileHandler(){
	printf("Testing TestFileHandler...\n");
	if (Exists("./sample/images/people-01-face1.png")==0){
		fprintf(stderr, "Exists error");
		return 1;
	}

	const char* ext = GetFileExt("./sample/images/people-01-face1.png") ;
	if (strcmp(ext, "png")!=0){
		fprintf(stderr, "GetFileExt error");
		return 2;
	}
	printf("%s", ext);


	int retCode =  FileCopy("./sample/images/people-01-face1.png", "./sample/images/people-01-face1_copy.png");

	if (retCode){
		fprintf(stderr, "FileCopy error");
		return 3;
	}
	return 0;
}

int TestCreateSubImage(){
	printf("Testing TestCreateSubImage...\n");
	IplImage* greyImg = getGrayScaleImg("./sample/images/people-01.jpg");
	cvShowImage( "result", greyImg );
	cvWaitKey(0);
	IplImage* subImg = CreateSubImg(greyImg, cvRect(200,100,400,200));
	cvShowImage( "result", subImg );
	cvWaitKey(0);
	return 0;
}




IplImage* getGrayScaleImg(const char* filename){
	IplImage* frame; //Initialise input image pointer

	frame = cvLoadImage(filename, CV_LOAD_IMAGE_COLOR);
	//printf("(%d, %d)\n", frame->depth, frame->nChannels);
	if (frame==NULL){
		printf("can't load the image!");
		return NULL;
	}
	IplImage *greyImg = 0;
	CvSize size;

	if (frame->nChannels > 1) {
		//printf("converting to grayscale image!\n");
		size = cvSize(frame->width, frame->height);
		greyImg = cvCreateImage(size, IPL_DEPTH_8U, 1 );
		cvCvtColor( frame, greyImg, CV_BGR2GRAY );
	}
//	IplImage* final = cvCreateImage(cvSize(frame->width, frame->height), IPL_DEPTH_8U,1);
//	cvEqualizeHist(greyImg,final);
	frame = greyImg;	// Use the greyscale image.
	return frame;
}
*/
void TestDBInsert(){
	printf("TestDBInsert\n");
	assert(Insert("./sample/query1/people-01", "./sample/db", "category1")==0);
}

void TestDBUpdate(){
	printf("TestDBUpdate\n");
	assert(Update("./sample/db")==0);
	//assert(Update("./sample/public_db")==0);
}

void TestDBDelete(){
	printf("TestDBDelete\n");
	assert(Delete("people-01", "./sample/db", "category1")==0);
}

void TestFaceDetection(char* filename){
	IplImage* frame = cvLoadImage(filename, CV_LOAD_IMAGE_GRAYSCALE);
	if (frame==NULL){
		fprintf(stderr, "can't load the image!");
	}
	filename[strlen(filename)-4]='\0';
	strcat(filename, DEPTH_EXT);
	IplImage* depth = cvLoadImage(filename, CV_LOAD_IMAGE_GRAYSCALE);
	//printf("(%d, %d)\n", frame->depth, frame->nChannels);
	if (depth==NULL){
		fprintf(stderr, "can't load the depth image!\n");
	}
	//storage = cvCreateMemStorage(0);
	CvSeq* faces;
	int retCode;
	assert((retCode = DetectFaces(frame, depth, &faces, FOREGRND_ON))==0);

	if (retCode){
		printf("ERROR: DetectFacesInImage\n" );
	}else{
		printf("%d faces detected (%s)!\n", faces->total, filename);
	}
	// Create two points to represent the face locations

//	CvPoint pt1, pt2;
//	for(int i = 0; i < faces->total; i++ )
//	{
//		// Create a new rectangle for drawing the face
//
//		CvRect* r = (CvRect*)cvGetSeqElem( faces, i ); // Find the dimensions of the face, and scale it if necessary
//		pt1.x = r->x;//*scale;
//		pt2.x = (r->x+r->width);//*scale;
//		pt1.y = r->y;//*scale;
//		pt2.y = (r->y+r->height);//*scale;
//		// Draw the rectangle in the input image
//		cvRectangle( frame, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0 );
//	}
//	// Show the image in the window named "result"
//	cvShowImage( "result", frame );
//	cvWaitKey(0);

}
void TestFaceDetectionAll(){
	ForAllImages("./sample/db", TestFaceDetection);

}
void TestCalcLBP(){
	printf("Testing TestCalcLBP...\n");

	IplImage* greyImg = cvLoadImage("./sample/query2/12.png", CV_LOAD_IMAGE_GRAYSCALE);
	assert(greyImg!=NULL);
	cvShowImage( "result", greyImg );
	cvWaitKey(0);


	IplImage* lbp = CalcLBP(greyImg, 2, 8, 0);
	assert(lbp!=NULL);
	cvShowImage( "result", lbp );
	cvWaitKey(0);
}
void TestCalcHueLBP(){
	printf("Testing TestCalc Hue-LBP...\n");

	IplImage* rgbImg = cvLoadImage("./sample/query2/142.png", CV_LOAD_IMAGE_COLOR);
	assert(rgbImg!=NULL);
	cvShowImage( "result", rgbImg );
	cvWaitKey(0);

	IplImage *hsv_img, *h, *s, *v;

	// convert to hsv image
	hsv_img = cvCreateImage( cvGetSize(rgbImg), IPL_DEPTH_8U, 3);
	cvCvtColor(rgbImg, hsv_img, CV_RGB2HSV);

	h = cvCreateImage( cvGetSize(hsv_img), IPL_DEPTH_8U, 1 );
	s = cvCreateImage( cvGetSize(hsv_img), IPL_DEPTH_8U, 1 );
	v = cvCreateImage( cvGetSize(hsv_img), IPL_DEPTH_8U, 1 );

	// Split image onto the color planes
	cvSplit( hsv_img, h, s, v, NULL );

	//quantization
	int numPts = h->width*h->height;
	for(int i=0; i<numPts; i++){
		int val = (unsigned int) h->imageData[i];
		h->imageData[i] = (val/16)*10;
	}

	cvShowImage( "hue", h );
	cvWaitKey(0);
	IplImage* huelbp = CalcLBP(h, 2, 8, 0);
	assert(huelbp!=NULL);
	cvShowImage( "hue-lbp", huelbp );
	cvWaitKey(0);
}
int TestFaceRecognition(){
	printf("Testing TestFaceRecognition...\n");
	IplImage* people = cvLoadImage("./sample/query1/people-01.png", CV_LOAD_IMAGE_GRAYSCALE);//getGrayScaleImg("./sample/query1/people-01.png");
	IplImage* person = cvLoadImage("./sample/query2/people-01-face1.png", CV_LOAD_IMAGE_GRAYSCALE);//getGrayScaleImg("./sample/query1/people-01-face1.png");
	CvSeq* peopleFaces;
	int retCode = DetectFaces(people, NULL, &peopleFaces, FOREGRND_ON);
	if (retCode){
		printf("ERROR: DetectFacesInImage\n" );
	}else{
		printf("people: %d faces detected!\n", peopleFaces->total);
	}
	IplImage* faces[peopleFaces->total];
	for(int i = 0; i < peopleFaces->total; i++ )
	{
		// Create a new rectangle for drawing the face

		CvRect* r = (CvRect*)cvGetSeqElem( peopleFaces, i ); // Find the dimensions of the face, and scale it if necessary
		faces[i] = CreateSubImg(people, *r);
	}
	int numPeople = peopleFaces->total;
	// Detect Query Face
	CvSeq* personFace;
	retCode = DetectFaces(person, NULL, &personFace, FOREGRND_ON);
	IplImage* queryFace;
	if (retCode && personFace->total!=1){
		printf("ERROR: DetectFacesInImage\n" );
	}else{
		printf("person: %d faces detected!\n", personFace->total);
	}
	CvRect* r = (CvRect*)cvGetSeqElem( personFace, 0 );
	queryFace = CreateSubImg(person, *r);
//	cvShowImage( "query face", queryFace );
//	cvWaitKey(0);
	// LBP and Spatial Histogram for Query Face
	IplImage* lbpQuery = CalcLBP(queryFace, 2, 8, UNIFORM_ON);
	CvMat* histQuery = CalcSpatialHistogram(lbpQuery, pow(2, 8), 8, 8);
	// Find Query Face in the People Faces

	float 	minScore 	= FLT_MAX;
	int		minIdx 		= -1;
	for(int i = 0; i < numPeople; i++ )
	{

		IplImage* lbp = CalcLBP(faces[i], 2, 8, UNIFORM_ON);
		CvMat* hist = CalcSpatialHistogram(lbp, pow(2, 8), 8, 8);
		float score = CompareHistograms(histQuery, hist, NULL);
		if (score < minScore){
			minScore 	= score;
			minIdx 		= i;
		}
		printf("%d th score : %f\n", i, score);
		//cvShowImage( "face", faces[i] );
		//cvWaitKey(0);
	}
	cvShowImage( "face found", faces[minIdx] );
	cvWaitKey(0);
	return 0;
}
int TestMakeFeature(){
	printf("Testing TestMakeFeatureInMem...\n");
	IplImage* image = cvLoadImage("./sample/query1/people-01-face1.png", CV_LOAD_IMAGE_COLOR);

	FEATURE feature;
	int retCode = MakeFeatureInMem(image, NULL, NULL, &feature);
	if (retCode){
		return retCode;
	}
	printf("grid_x, grid_y = %d, %d\n", feature.grid_x, feature.grid_y);
	printf("radius, neighbors = %d, %d\n", feature.radius, feature.neighbors);
	printf("num of faces = %d\n", feature.num_faces);
	for (int i=0; i<feature.num_faces; i++){
		printf("Face(%d) cols, rols = %d, %d\n", i, feature.histogram[i]->cols, feature.histogram[i]->rows);
//		for (int j=0; j<feature.histogram[i]->cols*feature.histogram[i]->rows; j++){
//			printf("%f\n", feature.histogram[i]->data.fl[j]);
//		}

	}
	for (int i=0; i<feature.num_faces; i++){
		printf("Face(%d) cols, rols = %d, %d\n", i, feature.hue_histogram[i]->cols, feature.hue_histogram[i]->rows);
		for (int j=0; j<feature.hue_histogram[i]->cols*feature.hue_histogram[i]->rows; j++){
			printf("%f\n", feature.hue_histogram[i]->data.fl[j]);
		}

	}
	ReleaseFeature(&feature);

	return retCode;
}

int TestSearch(){
	char* dbFolder = "./sample/db";
	char* queryFolder = "./sample/query2/";
	int testfileNums[18] = {12, 22, 32, 42, 52, 62, 72, 82, 92, 102, 112, 122, 142, 150, 160, 170, 180, 190};

	int num = 20;
	int num_cat = 2;
	char* categories[2] = {"faces", "faces2"};
	char** outputFileNames = (char**) malloc(num*sizeof(char*));
	float* outputScores = (float*)malloc(num*sizeof(float));

	char srcfile[1024];
	char num2str[1024];
	float avgAcc1 = 0.0f;
	float avgAcc2 = 0.0f;
	float avgDur  = 0.0f;
	for (int i=0; i<18; i++){
		strcpy(srcfile, queryFolder);
		sprintf(num2str, "%d", testfileNums[i]);
		strcat(srcfile, num2str);
		printf("Testing %s...\n", srcfile);

		clock_t start = clock();
		int retCode = Search(
					srcfile,
					dbFolder,
					categories,
					num_cat,
					outputFileNames,
					outputScores,
					num);
			//char id[1024];
		clock_t end = clock();
		int found10 = 0;
		int found20 = 0;
		float exectime = ((float)(end - start))/CLOCKS_PER_SEC;
		for (int j=0; j<num; j++){
			char fileNumStr[1024];
			int	 fileNum;
			char *pch = strrchr(outputFileNames[j], '/')+1;
			strncpy(fileNumStr, pch, strlen(pch)-4);
			strcat(fileNumStr, "\0");
			fileNumStr[(pch+strlen(pch)-4)-pch]='\0';
			sscanf(fileNumStr, "%d", &fileNum);
			int diff = fileNum - testfileNums[i];
			if (diff>=0 && diff<=9){
				if (j<20) found20++;
				if (j<10) found10++;
			}
			printf("%s, %d-(%d)th %f\n", outputFileNames[j], diff, j+1, outputScores[j]);
			//IplImage* matched = cvLoadImage(outputFileNames[i], CV_LOAD_IMAGE_COLOR);
			//sprintf(id, "Matched %d", i);
			//cvShowImage(id, matched);
		}
		float accuracy10 = found10*1.0f/10.0f*100.0f;
		float accuracy20 = found20*2.0f/20.0f*100.0f;
		avgAcc1+=accuracy10;
		avgAcc2+=accuracy20;
		avgDur += exectime;
		printf("accuracy10 : %f\n", accuracy10);
		printf("accuracy20 : %f\n", accuracy20);
		printf("exec time : %f\n", exectime);

			//cvWaitKey(0);
		if (retCode) return retCode;
	}
	printf("avg acc10: %f\n", avgAcc1/18.0f);
	printf("avg acc20: %f\n", avgAcc2/18.0f);
	printf("exec time: %f\n", avgDur/18.0f);

	return 0;
}
int TestPublicSearch(){
	char* dbFolder = "./sample/public_db";
	char* queryFolder = "./sample/public_query";

	int num = 40;
	int num_cat = 1;
	char* categories[1] = {"faces94"};
	char** outputFileNames = (char**) malloc(num*sizeof(char*));
	float* outputScores = (float*)malloc(num*sizeof(float));


	float avgAcc1 = 0.0f;
	float avgAcc2 = 0.0f;
	float avgDur  = 0.0f;
	int numQueries = 0;
	char srcfile[FLEN];
	DIR *dbDir = NULL;
	struct dirent *file = NULL;
	struct stat buf;

	dbDir = opendir(queryFolder);
	if(!dbDir) {
		fprintf(stderr, "ERROR\n");
	}
	while( (file = readdir(dbDir)) != NULL ){
        memset(&buf, 0, sizeof(struct stat));

        strcpy(srcfile, queryFolder);
		strcat(srcfile,"/");
		strcat(srcfile, file->d_name);
		lstat(srcfile, &buf);
		if(S_ISREG(buf.st_mode) && IsImageFile(file->d_name)){  //For each query image
			srcfile[strlen(srcfile)-4]='\0'; //remove extension
			numQueries++;
			printf("Testing %s...\n", srcfile);
			//extract input filename
			char inputfile[FLEN];
			strncpy(inputfile, file->d_name, strlen(file->d_name)-4); // exclude extension (.png)
			inputfile[strlen(file->d_name)-4]='\0';

			// extract file number
			const char *inputnum_str = strrchr(inputfile, '.')+1;
			inputfile[inputnum_str-inputfile-1]= '\0';
			int	 inputnum;
			sscanf(inputnum_str, "%d", &inputnum);

			clock_t start = clock();
			int retCode = Search(
						srcfile,
						dbFolder,
						categories,
						num_cat,
						outputFileNames,
						outputScores,
						num);
			assert(retCode==0);
			clock_t end = clock();
			int found20 = 0;
			int found40 = 0;
			float exectime = ((float)(end - start))/CLOCKS_PER_SEC;
			for (int j=0; j<num; j++){
				// extract file name
				char outfile[FLEN];
				char *pch = strrchr(outputFileNames[j], '/')+1;
				strncpy(outfile, pch, strlen(pch)-4); // exclude extension (.png)
				outfile[strlen(pch)-4]='\0';
				//strcat(outfile, "\0");

				// extract file number
				const char *outnum_str = strrchr(outfile, '.')+1;
				outfile[outnum_str-outfile-1]= '\0';
				int	 outnum;
				sscanf(outnum_str, "%d", &outnum);

				int diff = outnum - inputnum;
				if (strcmp(inputfile, outfile) == 0 && diff>=0 && diff<=20){
					if (j<40) found40++;
					if (j<20) found20++;
				}
				printf("%s, %d-(%d)th %f\n", outputFileNames[j], diff, j+1, outputScores[j]);
				//IplImage* matched = cvLoadImage(outputFileNames[i], CV_LOAD_IMAGE_COLOR);
				//sprintf(id, "Matched %d", i);
				//cvShowImage(id, matched);
			}
			float accuracy20 = found20*1.0f/20.0f*100.0f;
			float accuracy40 = found40*2.0f/40.0f*100.0f;
			avgAcc1+=accuracy20;
			avgAcc2+=accuracy40;
			avgDur += exectime;
			printf("accuracy10 : %f\n", accuracy20);
			printf("accuracy20 : %f\n", accuracy40);
			printf("exec time : %f\n", exectime);

				//cvWaitKey(0);
			if (retCode) return retCode;
		}
	}
	printf("avg acc10: %f\n", avgAcc1/numQueries);
	printf("avg acc20: %f\n", avgAcc2/numQueries);
	printf("exec time: %f\n", avgDur/numQueries);
	closedir(dbDir);

	return (0);
}
int TestPublicSearchOne(){
	char* dbFolder = "./sample/public_db";
	char* query = "./sample/public_query/ccjame.1";

	int num = 40;
	int num_cat = 1;
	char* categories[1] = {"faces94"};
	char** outputFileNames = (char**) malloc(num*sizeof(char*));
	float* outputScores = (float*)malloc(num*sizeof(float));

	printf("Testing %s...\n", query);

	float avgAcc1 = 0.0f;
	float avgAcc2 = 0.0f;
	float avgDur  = 0.0f;
	clock_t start = clock();
	int retCode = Search(
				query,
				dbFolder,
				categories,
				num_cat,
				outputFileNames,
				outputScores,
				num);
	assert(retCode==0);
	clock_t end = clock();
	int found20 = 0;
	int found40 = 0;
	float exectime = ((float)(end - start))/CLOCKS_PER_SEC;
	for (int j=0; j<num; j++){
		// extract file name
		char outfile[FLEN];
		char *pch = strrchr(outputFileNames[j], '/')+1;
		strncpy(outfile, pch, strlen(pch)-4); // exclude extension (.png)
		outfile[strlen(pch)-4]='\0';
		//strcat(outfile, "\0");

		// extract file number
		const char *outnum_str = strrchr(outfile, '.')+1;
		outfile[outnum_str-outfile-1]= '\0';
		int	 outnum;
		sscanf(outnum_str, "%d", &outnum);

		int diff = outnum - 1;
		if (strcmp(query, outfile) == 0 && diff>=0 && diff<=20){
			if (j<40) found40++;
			if (j<20) found20++;
		}
		printf("%s, %d-(%d)th %f\n", outputFileNames[j], diff, j+1, outputScores[j]);
		//IplImage* matched = cvLoadImage(outputFileNames[i], CV_LOAD_IMAGE_COLOR);
		//sprintf(id, "Matched %d", i);
		//cvShowImage(id, matched);
	}
	float accuracy20 = found20*1.0f/20.0f*100.0f;
	float accuracy40 = found40*2.0f/40.0f*100.0f;
	avgAcc1+=accuracy20;
	avgAcc2+=accuracy40;
	avgDur += exectime;
	printf("accuracy10 : %f\n", accuracy20);
	printf("accuracy20 : %f\n", accuracy40);
	printf("exec time : %f\n", exectime);
	return 0;
}

void TextConvertToUniform(){
	int num = 255;
	int uniform = ConvertToUniform(num, 8);
	printf("%d -> %d\n", num, uniform);
}
int main()
{
	//DB Handler Test
//	TestDBInsert();
//	TestDBDelete();
//	TestDBUpdate();
//	TestFaceDetection("./sample/db/faces/53.png");
//	TestFaceDetectionAll();
//	TestFaceRecognition();
//	TestMakeFeature();
	TestSearch();
//	TestPublicSearch();
//	ExperimentForegroundExtraction();
//	TestFaceDetection("./sample/public_query/ccjame.1.png");
//	TestPublicSearchOne();
//	TestCalcLBP();
//	TestCalcHueLBP();
//	TextConvertToUniform();
	return 0;
}
