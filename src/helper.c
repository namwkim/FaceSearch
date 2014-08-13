/*
 * helper.c
 *
 *  Created on: Aug 11, 2014
 *      Author: namwkim85
 */

#include "cv.h"
#include "helper.h"

IplImage* CreateSubImg(IplImage* img, CvRect roiRect) {

	cvSetImageROI(img, roiRect);
	IplImage* sub_img = cvCreateImage(cvSize(roiRect.width, roiRect.height), img->depth, img->nChannels);
	cvCopy(img, sub_img, NULL);
	cvResetImageROI(img);

	return sub_img;

}
