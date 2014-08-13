/*
 * face-recognition.h
 *
 *  Created on: Jul 16, 2014
 *      Author: namwkim85
 */

#ifndef FACE_RECOGNITION_H_
#define FACE_RECOGNITION_H_


IplImage* CalcLBP(IplImage* src, int radius, int neighbors);
CvHistogram* CalcHistogram(IplImage* src, int minVal, int maxVal, int normed);
CvMat*  CalcSpatialHistogram(IplImage* src, int numPatterns, int grid_x, int grid_y);
IplImage* CreateSubImg(IplImage* img, CvRect roiRect);
float CompareHistograms(CvMat* hist1, CvMat* hist2, const float* weight);

#endif /* FACE_RECOGNITION_H_ */
