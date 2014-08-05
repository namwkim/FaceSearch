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

static CvMemStorage* storage = 0;
static CvHaarClassifierCascade* cascade = 0;
const char* cascade_name =
		"./resource/haarcascade_frontalface_alt2.xml";
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
	// Clear the memory storage which was used before
	if (storage == NULL){
		storage = cvCreateMemStorage(0);
		if( !storage )	return 1;
	}
	cvClearMemStorage( storage );
	// Find whether the cascade is loaded, to find the faces. If yes, then:
	if( cascade == NULL){
		cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );
		if( !cascade )	return 2;
	}
	// There can be more than one face in an image. So create a growable sequence of faces.
	// Detect the objects and store them in the sequence
	*faces = cvHaarDetectObjects( img, cascade, storage, 1.1, 2, CV_HAAR_DO_CANNY_PRUNING, cvSize(40, 40), cvSize(0,0));
	// Loop the number of faces found.
	if (!faces)	return 3;

	// Release the temp image created.
	//if (greyImg)
	//		cvReleaseImage( &greyImg );
	//cvReleaseImage( &scaledImg );
	return 0;
}
