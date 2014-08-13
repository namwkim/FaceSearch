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

static CvMemStorage* face_storage = 0;
static CvMemStorage* eye_storage = 0;
static CvHaarClassifierCascade* face_cascade = 0;
static CvHaarClassifierCascade* eye_cascade = 0;
const char* cascade_face_name =
		"./resource/haarcascade_frontalface_alt2.xml";
const char* cascade_eye_name =
		"./resource/haarcascade_mcs_eyepair_small.xml";
int DetectFaces( IplImage* img, CvSeq** faces) {
	//int scale = 1; // Create a new image based on the input image
	/*IplImage* scaledImg = cvCreateImage( cvSize(img->width/scale,img->height/scale), 8, 3 );
	// If the image is color, use a greyscale copy of the image.
	IplImage* detectImg = (IplImage*)scaledImg;

	IplImage *greyImg = 0;
	CvSize size;

	if (scaledImg->nChannels > 1) {
		printf("converting to grayscale image!\n");
		size = cvSize(scaledImg->width, scaledImg->height);
		greyImg = cvCreateImage(size, IPL_DEPTH_8U, 1 );
		cvCvtColor( scaledImg, greyImg, CV_BGR2GRAY );
		detectImg = greyImg;	// Use the greyscale image.
	}*/
	IplImage *greyImg = 0;
	CvSize size = cvSize(img->width, img->height);

	if (img->nChannels > 1) {
		greyImg = cvCreateImage(size, IPL_DEPTH_8U, 1 );
		cvCvtColor( img, greyImg, CV_BGR2GRAY );
	}else{
		greyImg = img;
	}
	IplImage* final = cvCreateImage(cvSize(greyImg->width, greyImg->height), IPL_DEPTH_8U,1);
	cvEqualizeHist(greyImg,final);

	// Clear the memory storages which was used before
	if (face_storage == NULL){
		face_storage = cvCreateMemStorage(0);
		if( !face_storage )	return 1;
	}
	if (eye_storage == NULL){
		eye_storage = cvCreateMemStorage(0);
		if( !eye_storage )	return 1;
	}
	cvClearMemStorage( face_storage );
	cvClearMemStorage( eye_storage );

	// Find whether the cascade is loaded:
	if( face_cascade == NULL){
		face_cascade = (CvHaarClassifierCascade*)cvLoad( cascade_face_name, 0, 0, 0 );
		if( !face_cascade )	return 2;
	}
	if( eye_cascade == NULL){
		eye_cascade = (CvHaarClassifierCascade*)cvLoad( cascade_eye_name, 0, 0, 0 );
		if( !eye_cascade )	return 2;
	}
	// There can be more than one face in an image. So create a growable sequence of faces.
	// Detect the objects and store them in the sequence
	CvSeq* candidates = cvHaarDetectObjects( final, face_cascade, face_storage, 1.3, 4, CV_HAAR_DO_CANNY_PRUNING, cvSize(20, 20), cvSize(0,0));
	if (!candidates)	return 3;
	/*
	// Check the validity of the face by detecting eyes within
	int i = 0;
	while (i<candidates->total){
		CvRect* r = (CvRect*)cvGetSeqElem( candidates, i );
		IplImage* face_img = CreateSubImg(greyImg, *r);
		IplImage* scaledImg = cvCreateImage( cvSize(face_img->width*2,face_img->height*2), IPL_DEPTH_8U, 1 );
		cvResize(face_img, scaledImg, CV_INTER_LINEAR);
		//detect eyes within the eyes
		CvSeq* eyes = cvHaarDetectObjects( scaledImg, eye_cascade, eye_storage, 1.1, 4, CV_HAAR_DO_CANNY_PRUNING, cvSize(22, 5), cvSize(0,0));

		// Create two points to represent the face locations
		CvPoint pt1, pt2;
		for(int i = 0; i < eyes->total; i++ )
		{
			// Create a new rectangle for drawing the face

			CvRect* r = (CvRect*)cvGetSeqElem( eyes, i ); // Find the dimensions of the face, and scale it if necessary
			pt1.x = r->x;
			pt2.x = (r->x+r->width);
			pt1.y = r->y;
			pt2.y = (r->y+r->height);
			// Draw the rectangle in the input image
			cvRectangle( scaledImg, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0 );
		}
		cvShowImage( "DETECTED FACE", scaledImg );
		cvWaitKey(0);
		cvReleaseImage( &face_img );
		cvReleaseImage( &scaledImg );

		// a single face has to have only one eye pair, no Cyclops!
		if (eyes->total!=1){
			cvSeqRemove(candidates, i);
			continue;
		}
		i++;
	}
	*/
	*faces = candidates;

	// Release the temp image created.
	if (img->nChannels > 1)
		cvReleaseImage( &greyImg );
	cvReleaseImage( &final );
	//cvReleaseImage( &scaledImg );
	return 0;
}
