/*
 * face-detection.c
 *
 *  Created on: Jul 16, 2014
 *      Author: namwkim85
 */

#include "cv.h"
#include "highgui.h"
#include "math.h"
#include "cxcore.h"
#include "face-detection.h"
#include "helper.h"
#include "kmeans.h"

static CvMemStorage* face_storage = 0;
//static CvMemStorage* eye_storage = 0;
static CvHaarClassifierCascade* face_cascade = 0;
//static CvHaarClassifierCascade* eye_cascade = 0;
const char* cascade_face_name =
		"./resource/haarcascade_frontalface_alt2.xml";
//const char* cascade_eye_name =
//		"./resource/haarcascade_mcs_eyepair_small.xml";
int DetectFaces( IplImage* img, IplImage* depth, CvSeq** faces, int foregrnd_on) {


	IplImage *greyImg = 0;
	CvSize size = cvSize(img->width, img->height);

	if (img->nChannels > 1) {
		greyImg = cvCreateImage(size, IPL_DEPTH_8U, 1 );
		cvCvtColor( img, greyImg, CV_RGB2GRAY );
	}else{
		greyImg = img;
	}

	if (foregrnd_on && depth!=NULL){
		//FOREGROUND EXTRACTION USING K-MEAN CLUSTERING
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
				greyImg->imageData[i] = 255;
			}
		}
		free(points);
		free(assignments);
//		cvShowImage("adjusted for depth", greyImg);
//		cvWaitKey(0);
	}
	IplImage* final = cvCreateImage(cvSize(greyImg->width, greyImg->height), IPL_DEPTH_8U,1);
	cvEqualizeHist(greyImg,final);

	// Clear the memory storages which was used before
	if (face_storage == NULL){
		face_storage = cvCreateMemStorage(0);
		if( !face_storage )	return 1;
	}
//	if (eye_storage == NULL){
//		eye_storage = cvCreateMemStorage(0);
//		if( !eye_storage )	return 1;
//	}
	cvClearMemStorage( face_storage );
//	cvClearMemStorage( eye_storage );

	// Find whether the cascade is loaded:
	if( face_cascade == NULL){
		face_cascade = (CvHaarClassifierCascade*)cvLoad( cascade_face_name, 0, 0, 0 );
		if( !face_cascade )	return 2;
	}
//	if( eye_cascade == NULL){
//		eye_cascade = (CvHaarClassifierCascade*)cvLoad( cascade_eye_name, 0, 0, 0 );
//		if( !eye_cascade )	return 2;
//	}
	// There can be more than one face in an image. So create a growable sequence of faces.
	// Detect the objects and store them in the sequence
	CvSeq* candidates = cvHaarDetectObjects( final, face_cascade, face_storage, 1.3, 4, CV_HAAR_DO_CANNY_PRUNING, cvSize(20, 20), cvSize(0,0));
	if (!candidates)	return 3;

	*faces = candidates;

	// Release the temp image created.
	if (img->nChannels > 1)
		cvReleaseImage( &greyImg );
	cvReleaseImage( &final );
	//cvReleaseImage( &scaledImg );
	return 0;
}
