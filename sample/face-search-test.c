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


IplImage* getGrayScaleImg(const char* filename);
int TestCalcHistogram();
int TestFaceDetection();
int TestFileHandler();
int TestCalcLBP();
int TestCreateSubImage();
int TestFaceRecognition();
int TestDBInsert();
int TestDBDelete();
int TestDBUpdate();
int TestSearch();
int InsertFaceDB();
void AnalyzeDepthImage();
void TestAllFaceDetection();
int main()
{

//	if (TestFaceDetection()){
//		fprintf(stderr, "TestFaceDetection() Failed!\n");
//	}
// 	if (TestCalcHistogram()){
//		fprintf(stderr, "TestCalcHistogram() Failed!\n");
//	}
// 	if (TestCalcLBP()){
//		fprintf(stderr, "TestCalcHistogram() Failed!\n");
//	}
//	if (TestCreateSubImage()){
//		fprintf(stderr, "TestCreateSubImage() Failed!\n");
//	}
//	if (TestFaceRecognition()){
//		fprintf(stderr, "TestFaceRecognition() Failed!\n");
//	}
//	if (TestFileHandler()){
//		fprintf(stderr, "TestFileHandler() Failed!\n");
//	}
//	if (TestMakeFeatureInMem()){
//		fprintf(stderr, "TestMakeFeatureInMem() Failed!\n");
//	}
//	if (TestMakeFeature()){
//		fprintf(stderr, "TestMakeFeature() Failed!\n");
//	}
//	if (TestDBInsert()){
//		fprintf(stderr, "TestDBInsert() Failed!\n");
//	}
//	if (TestDBInsert()){
//		fprintf(stderr, "TestDBInsert() Failed!\n");
//	}
//	if (TestDBDelete()){
//		fprintf(stderr, "TestDBDelete() Failed!\n");
//	}
//	if (TestDBUpdate()){
//		fprintf(stderr, "TestDBUpdate() Failed!\n");
//	}
//	InsertFaceDB();
	if (TestSearch()){
		fprintf(stderr, "TestSearch() Failed!\n");
	}
//	if (TestSearchJustOne()){
//		fprintf(stderr, "TestSearch() Failed!\n");
//	}
//	AnalyzeDepthImage();
//	TestAllFaceDetection();
	return 0;
}
void TestFaceDetectionEach(char* filename){
	IplImage* frame = cvLoadImage(filename, CV_LOAD_IMAGE_COLOR);
	//printf("(%d, %d)\n", frame->depth, frame->nChannels);
	if (frame==NULL){
		fprintf(stderr, "can't load the image!");
	}
	//storage = cvCreateMemStorage(0);
	CvSeq* faces;
	int retCode = DetectFaces(frame, &faces);
	if (retCode){
		printf("ERROR: DetectFacesInImage\n" );
	}else{
		printf("%d faces detected (%s)!\n", faces->total, filename);
	}
	// Create two points to represent the face locations
	/*
	CvPoint pt1, pt2;
	for(int i = 0; i < faces->total; i++ )
	{
		// Create a new rectangle for drawing the face

		CvRect* r = (CvRect*)cvGetSeqElem( faces, i ); // Find the dimensions of the face, and scale it if necessary
		pt1.x = r->x;//*scale;
		pt2.x = (r->x+r->width);//*scale;
		pt1.y = r->y;//*scale;
		pt2.y = (r->y+r->height);//*scale;
		// Draw the rectangle in the input image
		cvRectangle( frame, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0 );
	}
	// Show the image in the window named "result"
	cvShowImage( "result", frame );
	cvWaitKey(0);
	*/
}
void TestAllFaceDetection(){
	ForAllImages("./sample/db", TestFaceDetectionEach);

}
void AnalyzeDepthImage(){
	char* filename 	= "./sample/query2/22.png";
	IplImage* gray 	= cvLoadImage(filename, CV_LOAD_IMAGE_GRAYSCALE);
	IplImage* depth = cvLoadImage("./sample/query2/22.depth.png", CV_LOAD_IMAGE_GRAYSCALE);
	IplImage* rgb 	= cvLoadImage(filename, CV_LOAD_IMAGE_COLOR);
	IplImage* show 	= cvCreateImage(cvSize(gray->width, gray->height), IPL_DEPTH_8U, 3);

	for(int i=0; i<depth->width; i++){
		for (int j=0; j<depth->height; j++){
			unsigned char dval = (unsigned char)depth->imageData[j*depth->widthStep+ i];
			if (dval<90){
				gray->imageData[j*depth->widthStep+ i] = 255;
			}
			//printf("%i, ", (unsigned char)depth->imageData[j*depth->widthStep+ i]);
		}
		//printf("\n");
	}
	//cvShowImage("adjusted for depth", gray);
	//cvWaitKey(0);
	//storage = cvCreateMemStorage(0);
	CvSeq* faces;
	int retCode = DetectFaces(gray, &faces);
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
	retCode = DetectFaces(rgb, &faces);
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
int InsertFaceDB(){
	// Insert Face Images into DB
	char srcfile[1024];
	char id[1024];
	for (int i=0; i<150; i++){
		strcpy(srcfile, "./sample/facedb/");
		sprintf(id, "%d", i);
		strcat(srcfile, id);
		int retCode = Insert(srcfile, "./sample/db", "faces");
		if (retCode>0){
			return retCode;
		}

	}
	return 0;
}

int TestSearch(){
	char* dbFolder = "./sample/db";
	char* queryFolder = "./sample/query2/";
	int testfileNums[13] = {12, 22, 32, 42, 52, 62, 72, 82, 92, 102, 112, 122, 142};

	int num = 20;
	int num_cat = 1;
	char* categories[1] = {"faces"};
	char** outputFileNames = (char**) malloc(num*sizeof(char*));
	float* outputScores = (float*)malloc(num*sizeof(float));

	char srcfile[1024];
	char num2str[1024];
	float avgAcc1 = 0.0f;
	float avgAcc2 = 0.0f;
	float avgDur  = 0.0f;
	for (int i=0; i<13; i++){
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
	printf("avg acc10: %f\n", avgAcc1/13.0f);
	printf("avg acc20: %f\n", avgAcc2/13.0f);
	printf("exec time: %f\n", avgDur/13.0f);

	return 0;
}
int TestSearchJustOne(){

	int num = 10;
	char* categories[1] = {"faces"};
	char** outputFileNames = (char**) malloc(num*sizeof(char*));
	float* outputScores = (float*)malloc(num*sizeof(float));

	int retCode = Search(
			"./sample/query2/72",
			"./sample/db",
			categories,
			1,
			outputFileNames,
			outputScores,
			num);
	char id[1024];
	for (int i=0; i<num; i++){
		printf("%s, %f\n", outputFileNames[i], outputScores[i]);
		IplImage* matched = cvLoadImage(outputFileNames[i], CV_LOAD_IMAGE_COLOR);
		sprintf(id, "Matched %d", i);
		cvShowImage(id, matched);
	}
	cvWaitKey(0);
	return retCode;
}
int TestDBDelete(){
	int retCode =  Delete("people-01", "./sample/db", "category1");
	return retCode;
}
int TestDBUpdate(){
	int retCode =  Update("./sample/db");
	return retCode;
}
int TestDBInsert(){
	int retCode = Insert("./sample/images/people-01", "./sample/db", "category1");
	return retCode;
}
int TestMakeFeature(){
	int retCode = MakeFeature("./sample/images/people-01-face1");
	return retCode;
}
int TestMakeFeatureInMem(){
	printf("Testing TestMakeFeatureInMem...\n");
	IplImage* image = cvLoadImage("./sample/images/people-01-face1.png", CV_LOAD_IMAGE_COLOR);

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
		for (int j=0; j<feature.histogram[i]->cols*feature.histogram[i]->rows; j++){
			printf("%f\n", feature.histogram[i]->data.fl[j]);
		}

	}

	ReleaseFeature(&feature);

	return retCode;
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
int TestFaceRecognition(){
	printf("Testing TestFaceRecognition...\n");
	IplImage* people = getGrayScaleImg("./sample/images/people-01.jpg");
	IplImage* person = getGrayScaleImg("./sample/images/people-01-face1.png");
	CvSeq* peopleFaces;
	int retCode = DetectFaces(people, &peopleFaces);
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
	retCode = DetectFaces(person, &personFace);
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
	IplImage* lbpQuery = CalcLBP(queryFace, 1, 8);
	CvMat* histQuery = CalcSpatialHistogram(lbpQuery, pow(2, 8), 8, 8);
	// Find Query Face in the People Faces

	float 	minScore 	= FLT_MAX;
	int		minIdx 		= -1;
	for(int i = 0; i < numPeople; i++ )
	{

		IplImage* lbp = CalcLBP(faces[i], 1, 8);
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
int TestCalcLBP(){
	printf("Testing TestCalcLBP...\n");
	IplImage* greyImg = getGrayScaleImg("./sample/query2/102.png");

	IplImage* lbp = CalcLBP(greyImg, 1, 8);
	for (int i=0; i<lbp->height; i++){
		for (int j=0; j<lbp->width; j++){
			printf("%d", (unsigned char) lbp->imageData[i*lbp->widthStep + j]);
		}
		printf("\n");
	}
	if (lbp==NULL){
		return 1;
	}
	cvShowImage( "result", lbp );
	cvWaitKey(0);
	return 0;
}

int TestCalcHistogram(){
	printf("Testing TestCalcHistogram...\n");
	IplImage* frame = getGrayScaleImg("./sample/images/people-01.jpg");
	printf("channel: %d\n", frame->nChannels);
	printf("size=%d\n", frame->width*frame->height);
	CvHistogram* hist = CalcHistogram(frame, 0, pow(2.0, 8.0)-1, 0);
	CvMatND* mat1 = (CvMatND*)(hist->bins);

	printf("dim = %d\n", mat1->dims);
	float total = 0.0f;
	for (int i=0; i<mat1->dims; i++){
		for (int j=0; j<mat1->dim[i].size; j++){
			total += mat1->data.fl[j];
			printf("%d = %f\n", j, mat1->data.fl[j]);
		}
	}

	printf("total: %f\n", total);
	return 0;
}
int TestFaceDetection(){
	IplImage* frame = getGrayScaleImg("./sample/db/faces/140.png");
	printf("(%d, %d)\n", frame->depth, frame->nChannels);
	if (frame==NULL){
		printf("can't load the image!");
		return 1;
	}
	cvShowImage( "result", frame );
	//storage = cvCreateMemStorage(0);
	CvSeq* faces;
	int retCode = DetectFaces(frame, &faces);
	if (retCode){
		printf("ERROR: DetectFacesInImage\n" );
	}else{
		printf("%d faces detected!\n", faces->total);
	}
	// Create two points to represent the face locations
	CvPoint pt1, pt2;
	for(int i = 0; i < faces->total; i++ )
	{
		// Create a new rectangle for drawing the face

		CvRect* r = (CvRect*)cvGetSeqElem( faces, i ); // Find the dimensions of the face, and scale it if necessary
		pt1.x = r->x;//*scale;
		pt2.x = (r->x+r->width);//*scale;
		pt1.y = r->y;//*scale;
		pt2.y = (r->y+r->height);//*scale;
		// Draw the rectangle in the input image
		cvRectangle( frame, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0 );
	}
	// Show the image in the window named "result"
	cvShowImage( "result", frame );
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
	IplImage* final = cvCreateImage(cvSize(frame->width, frame->height), IPL_DEPTH_8U,1);
	cvEqualizeHist(greyImg,final);
	frame = greyImg;	// Use the greyscale image.
	return frame;
}
